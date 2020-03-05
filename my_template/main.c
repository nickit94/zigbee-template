/**
 * Copyright (c) 2018 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/** @file
 *
 * @defgroup zigbee_examples_light_bulb main.c
 * @{
 * @ingroup zigbee_examples
 * @brief Dimmable light sample (HA profile)
 */

#include "sdk_config.h"
#include "zboss_api.h"
#include "zboss_api_addons.h"
#include "zb_mem_config_med.h"
#include "zb_ha_dimmable_light.h"
#include "zb_error_handler.h"
#include "zb_nrf52_internal.h"
#include "zigbee_helpers.h"

#include "bsp.h"
#include "boards.h"
#include "app_pwm.h"
#include "app_timer.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrf_drv_gpiote.h"
#include "nrf_802154.h"


#include "C:\nRF_SDK\nRF5_SDK_for_Thread_and_Zigbee_v4.0.0_dc7186b\examples\zigbee\light_control\my_template\zb_counting.h"
#include "C:\nRF_SDK\nRF5_SDK_for_Thread_and_Zigbee_v4.0.0_dc7186b\external\zboss\include\zboss_api_zcl.h"


#define MAX_CHILDREN                      10                                    /**< The maximum amount of connected devices. Setting this value to 0 disables association to this device.  */
#define ZIGBEE_CHANNEL                    17                                    /**< 802.15.4 channel used by Zigbee */
#define IEEE_CHANNEL_MASK                 (1l << ZIGBEE_CHANNEL)                /**< Scan only one, predefined channel to find the coordinator. */
#define ERASE_PERSISTENT_CONFIG           ZB_FALSE                              /**< Do not erase NVRAM to save the network parameters after device reboot or power-off. */

/* Basic cluster attributes initial values. */
#define BULB_INIT_BASIC_APP_VERSION       01                                    /**< Version of the application software (1 byte). */
#define BULB_INIT_BASIC_STACK_VERSION     10                                    /**< Version of the implementation of the Zigbee stack (1 byte). */
#define BULB_INIT_BASIC_HW_VERSION        01                                    /**< Version of the hardware of the device (1 byte). */
#define BULB_INIT_BASIC_MANUF_NAME        "Hohobo"                              /**< Manufacturer name (32 bytes). */
#define BULB_INIT_BASIC_MODEL_ID          "uCounter HO-DC1010"                  /**< Model number assigned by manufacturer (32-bytes long string). */
#define BULB_INIT_BASIC_DATE_CODE         "20200212"                            /**< First 8 bytes specify the date of manufacturer of the device in ISO 8601 format (YYYYMMDD). The rest (8 bytes) are manufacturer specific. */
#define BULB_INIT_BASIC_LOCATION_DESC     "DIN RAIL"                            /**< Describes the physical location of the device (16 bytes). May be modified during commisioning process. */
#define BULB_INIT_BASIC_POWER_SOURCE      ZB_ZCL_BASIC_POWER_SOURCE_DC_SOURCE   /**< Type of power sources available for the device. For possible values see section 3.2.2.2.8 of ZCL specification. */
#define BULB_INIT_BASIC_PH_ENV            ZB_ZCL_BASIC_ENV_UNSPECIFIED          /**< Describes the type of physical environment. For possible values see section 3.2.2.2.10 of ZCL specification. */

#define IDENTIFY_MODE_BSP_EVT             BSP_EVENT_KEY_1                       /**< Button event used to enter the Bulb into the Identify mode. */
#define ZIGBEE_NETWORK_STATE_LED          BSP_BOARD_LED_0                       /**< LED indicating that light switch successfully joind Zigbee network. */
#define PULSE_STATE_LED                   BSP_BOARD_LED_3

#define PIN_INPUT_PULSE                   23                                    /**< Pin for pulse interrupt */
#define ACCURACY_COUNTER_IN_KW    (double)10
#define TIME_ONE_SEC                     (1000)                     
#define TIME_LACK_PULSE_SEC               (1 * 10 * ZB_TIME_ONE_SECOND)          /**< Если в течение этого времени не было ни одного импульса - ошибка */
#define TIME_JOIN_PRESS_BUTTON_SEC        (1 * 10 * ZB_TIME_ONE_SECOND)          /**< Время попытки подключиться к координатору по нажатии кнопки*/

#define TIME_PRESS_BUT_SHORT_CLICK        ZB_MILLISECONDS_TO_BEACON_INTERVAL(100)
#define TIME_PRESS_BUT_LONG_CLICK         ZB_MILLISECONDS_TO_BEACON_INTERVAL(4000)
#define TIMEOUT_PARENT_IS_REACHABLE       ZB_MILLISECONDS_TO_BEACON_INTERVAL(16000)

#define LEDS_PWM_PIN (32+14)

APP_TIMER_DEF(zb_app_timer);
APP_TIMER_DEF(long_click_button_timer);
APP_PWM_INSTANCE(LEDS_PWM, 2);

static counting_device_ctx_t m_dev_ctx;
static system_param_t        system_param;

static zb_bool_t reset_button_press         = ZB_FALSE;
static zb_time_t reset_button_time_start    = 0;

/* COMMON clusters */
ZB_ZCL_DECLARE_IDENTIFY_ATTRIB_LIST(identify_attr_list, &m_dev_ctx.identify_attr.identify_time);

ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST_EXT(basic_attr_list,
                                     &m_dev_ctx.basic_attr.zcl_version,
                                     &m_dev_ctx.basic_attr.app_version,
                                     &m_dev_ctx.basic_attr.stack_version,
                                     &m_dev_ctx.basic_attr.hw_version,
                                     m_dev_ctx.basic_attr.mf_name,
                                     m_dev_ctx.basic_attr.model_id,
                                     m_dev_ctx.basic_attr.date_code,
                                     &m_dev_ctx.basic_attr.power_source,
                                     m_dev_ctx.basic_attr.location_id,
                                     &m_dev_ctx.basic_attr.ph_env,
                                     m_dev_ctx.basic_attr.sw_ver);

/* COUNTER clusters */
ZB_ZCL_DECLARE_ANALOG_INPUT_ATTRIB_LIST(counter_attr_list,
                                        &m_dev_ctx.counter_value.out_of_service,
                                        &m_dev_ctx.counter_value.present_value,
                                        &m_dev_ctx.counter_value.status_flags);

ZB_ZCL_DECLARE_MULTI_VALUE_ATTRIB_LIST(pulse_in_kwh_attr_list,
                                       &m_dev_ctx.counter_pulse_in_kwh.number_of_states,
                                       &m_dev_ctx.counter_pulse_in_kwh.out_of_service,
                                       &m_dev_ctx.counter_pulse_in_kwh.present_value,
                                       &m_dev_ctx.counter_pulse_in_kwh.status_flags);

/* DUTYT clusters */
ZB_ZCL_DECLARE_MULTI_INPUT_ATTRIB_LIST(time_live_in_hour_attr_list,
                                       &m_dev_ctx.duty_time_live_in_hour.number_of_states,
                                       &m_dev_ctx.duty_time_live_in_hour.out_of_service,
                                       &m_dev_ctx.duty_time_live_in_hour.present_value,
                                       &m_dev_ctx.duty_time_live_in_hour.status_flags);

/* LEDS clusters */
ZB_ZCL_DECLARE_ON_OFF_ATTRIB_LIST_EXT(on_off_attr_list,
                                      &m_dev_ctx.leds_on_off.on_off,
                                      &m_dev_ctx.leds_on_off.global_scene_ctrl,
                                      &m_dev_ctx.leds_on_off.on_time,
                                      &m_dev_ctx.leds_on_off.off_wait_time);

ZB_ZCL_DECLARE_LEVEL_CONTROL_ATTRIB_LIST(level_control_attr_list,
                                         &m_dev_ctx.leds_level_control.current_level,
                                         &m_dev_ctx.leds_level_control.remaining_time);

ZB_ZCL_DECLARE_BINARY_VALUE_ATTRIB_LIST(led_inversion_attr_list,
                                        &m_dev_ctx.leds_inversion.out_of_service,
                                        &m_dev_ctx.leds_inversion.present_value,
                                        &m_dev_ctx.leds_inversion.status_flags); 

/* EP1 - COUNTER */
ZB_DECLARE_COUNTER_CLUSTER_LIST(counter_cluster_list, basic_attr_list, identify_attr_list, counter_attr_list, pulse_in_kwh_attr_list);
ZB_DECLARE_COUNTER_EP(counter_ep, ZB_COUNTER_EP, counter_cluster_list); 

/* EP2 - DUTY */
ZB_DECLARE_DUTY_CLUSTER_LIST(duty_cluster_list, basic_attr_list, identify_attr_list, time_live_in_hour_attr_list);
ZB_DECLARE_DUTY_EP(duty_ep, ZB_DUTY_EP, duty_cluster_list);

/* EP3 - LEDS */
ZB_DECLARE_LEDS_CLUSTER_LIST(leds_cluster_list, basic_attr_list, identify_attr_list, on_off_attr_list, level_control_attr_list, led_inversion_attr_list);
ZB_DECLARE_LEDS_EP(leds_ep, ZB_LEDS_EP, leds_cluster_list);

/* CTX */
ZBOSS_DECLARE_DEVICE_CTX_3_EP(ucounter_ctx, counter_ep, duty_ep, leds_ep);


typedef enum button_event_e 
{
    BUTTON_EVENT_NONE,
    BUTTON_EVENT_SHORT_CLICK,
    BUTTON_EVENT_LONG_CLICK
} button_event_t;


///////////////// PWM /////////////////////////////


/**@brief Sets brightness of on-board LED
 *
 * @param[in] brightness_level Brightness level, allowed values 0 ... 255, 0 - turn off, 255 - full brightness
 */
static void leds_set_brightness(zb_uint8_t brightness_level)
{
    app_pwm_duty_t app_pwm_duty;

    /* Scale level value: APP_PWM uses 0-100 scale, but Zigbee level control cluster uses values from 0 up to 255. */
    app_pwm_duty = (brightness_level * 100U) / 255U;

    /* Set the duty cycle - keep trying until PWM is ready. */
    while (app_pwm_channel_duty_set(&LEDS_PWM, 0, app_pwm_duty) == NRF_ERROR_BUSY)
    {
    }
}

////////////////////////////////////////

/** @brief Function-handler for disconnect event */
static void zb_network_disconnected(zb_uint8_t param)
{
    NRF_LOG_INFO(" -- zb_network_disconnected -- ");

    system_param.connected = ZB_FALSE;
    bsp_board_led_off(ZIGBEE_NETWORK_STATE_LED);
    nrf_gpio_pin_write(3, 1);
}

/** @brief Function-handler for disconnect event */
static void zb_network_connected(zb_uint8_t param)
{
    ret_code_t err_code;
    NRF_LOG_INFO(" -- zb_network_connected -- ");

    system_param.connected = ZB_TRUE;
    bsp_board_led_on(ZIGBEE_NETWORK_STATE_LED);
    nrf_gpio_pin_write(3, 0);

    /* Временный таймер для обновления значений атрибутов по времени, раз в минуту */
    err_code = app_timer_start(zb_app_timer, APP_TIMER_TICKS(60000), NULL);
    APP_ERROR_CHECK(err_code);

    /* Change Long Pull interval (3.16.2) */
    zb_zdo_pim_set_long_poll_interval(60000);
}

/** @brief Perform local operation - leave network.
 *
 * @param[in]   bufid   Reference to Zigbee stack buffer that will be used to construct leave request.
 */
static void device_leave_nwk(zb_bufid_t bufid)
{
    zb_ret_t zb_err_code;

    /* We are going to leave */
    if (bufid)
    {
        zb_zdo_mgmt_leave_param_t * p_req_param;

        p_req_param = ZB_BUF_GET_PARAM(bufid, zb_zdo_mgmt_leave_param_t);
        UNUSED_RETURN_VALUE(ZB_BZERO(p_req_param, sizeof(zb_zdo_mgmt_leave_param_t)));

        /* Set dst_addr == local address for local leave */
        p_req_param->dst_addr = ZB_PIBCACHE_NETWORK_ADDRESS();
        p_req_param->rejoin   = ZB_FALSE;
        UNUSED_RETURN_VALUE(zdo_mgmt_leave_req(bufid, NULL));
    }
    else
    {
        zb_err_code = zb_buf_get_out_delayed(device_leave_nwk);
        ZB_ERROR_CHECK(zb_err_code);
    }
}

/**@brief Function for starting join/rejoin procedure.
 *
 * param[in]   leave_type   Type of leave request (with or without rejoin).
 */
static zb_void_t device_retry_join(zb_uint8_t leave_type)
{
    NRF_LOG_INFO(" --device_retry_join-- ");
    zb_bool_t comm_status;

    if (leave_type == ZB_NWK_LEAVE_TYPE_RESET)
    {
        comm_status = bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING);
        ZB_COMM_STATUS_CHECK(comm_status);
    }
}

/**@brief Function for leaving current network and starting join procedure afterwards.
 *
 * @param[in]   bufid   Optional reference to Zigbee stack buffer to be reused by leave and join procedure.
 */
static zb_void_t device_leave_and_join(zb_bufid_t bufid)
{
    NRF_LOG_INFO("--device_leave_and_join--");
    if (ZB_JOINED())
    {
        /* Leave network. Joining procedure will be initiated inisde Zigbee stack signal handler. */
        device_leave_nwk(bufid);
    }
    else
    {
        /* Already left network. Start joining procedure. */
        device_retry_join(ZB_NWK_LEAVE_TYPE_RESET);

        if (bufid)
        {
            zb_buf_free(bufid);
        }
    }
}

/** @brief Handler join button timeout events */
static void join_timeout_handler(zb_uint8_t param)
{
    system_param.start_join_timeout = ZB_FALSE;
}

/** @brief Handler factory reset events */
static void factory_reset_handler(zb_uint8_t param)
{
    zb_bdb_reset_via_local_action(0U);
    app_timer_stop(zb_app_timer);

    //device_leave_nwk(0); //!~!!!!!!!!!!!!!!!!!!

    UNUSED_RETURN_VALUE(ZB_MEMSET(&system_param, 0, sizeof(system_param)));
    UNUSED_RETURN_VALUE(ZB_MEMCPY(&system_param, &system_param_default, sizeof(system_param_t)));

    NRF_LOG_INFO("FACTORY RESET DONE");
}

/** @brief Handler button events */
static void zb_user_button_handler(zb_uint8_t param)
{
    zb_bool_t comm_status;

    switch(param)
    {
        case BUTTON_EVENT_SHORT_CLICK:
            NRF_LOG_INFO(" -- BUTTON_EVENT_SHORT_CLICK, Connect = %d, Join Timeout = %d --", system_param.connected, system_param.start_join_timeout);
            
            if (!system_param.connected && !system_param.start_join_timeout) 
            {
                system_param.start_join_timeout = ZB_TRUE;
                ZB_ERROR_CHECK(ZB_SCHEDULE_APP_ALARM(join_timeout_handler, 0, TIME_JOIN_PRESS_BUTTON_SEC));

                comm_status = bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING);
                ZB_COMM_STATUS_CHECK(comm_status);
            }

            /* Если таймер был уже запущен - обновить таймер */
            if (system_param.start_join_timeout)
            {
                /* Пересброс таймера */
                UNUSED_RETURN_VALUE(ZB_SCHEDULE_APP_ALARM_CANCEL(join_timeout_handler, 0));
                ZB_ERROR_CHECK(ZB_SCHEDULE_APP_ALARM(join_timeout_handler, 0, TIME_JOIN_PRESS_BUTTON_SEC));
            }
            break;

        case BUTTON_EVENT_LONG_CLICK:
            NRF_LOG_INFO(" -- BUTTON_EVENT_LONG_CLICK --");
            ZB_SCHEDULE_APP_CALLBACK(factory_reset_handler, NULL);
            break;

        default: 
            break;
    }
}

/** @brief  Function for check long press button */
static void check_long_click(void * param)
{
    if (ZB_TIME_SUBTRACT(ZB_TIMER_GET(), reset_button_time_start) >= TIME_PRESS_BUT_LONG_CLICK)
    {
        ZB_SCHEDULE_APP_CALLBACK(zb_user_button_handler, BUTTON_EVENT_LONG_CLICK);
        
        ret_code_t err_code = app_timer_stop(long_click_button_timer);
        ZB_ERROR_CHECK(err_code);
    }     
}

/** @brief Function for convert double counter value to uint32 */
static zb_uint32_t convert_counter_value(double value)
{
    /* 2 знака после запятой */
    return (zb_uint32_t)(value / 10.0);     
}

/** @brief Function for update COUNTER VALUE cluster attributes */
static void update_counter_value(zb_uint32_t new_value)
{
    if (!system_param.connected) return;

    zb_zcl_status_t zcl_status = zb_zcl_set_attr_val(ZB_COUNTER_EP, 
                                                     ZB_ZCL_CLUSTER_ID_ANALOG_INPUT, 
                                                     ZB_ZCL_CLUSTER_SERVER_ROLE, 
                                                     ZB_ZCL_ATTR_ANALOG_INPUT_PRESENT_VALUE_ID, 
                                                     (zb_uint8_t *)&new_value,
                                                     ZB_FALSE);

    if (zcl_status == ZB_ZCL_STATUS_SUCCESS) { NRF_LOG_INFO("Set new value in COUNTER VALUE success! Value: %ld", (uint32_t)new_value);       }
    else                                     { NRF_LOG_INFO("Set new value in COUNTER VALUE is fail. zcl_status: %d", zcl_status); }
}

/** @brief Function for update COUNTER PULSE_IN_KW cluster attributes */
static void update_counter_pulse_in_kwh(zb_int16_t new_value)
{
    if (!system_param.connected) return;

    zb_zcl_status_t zcl_status = zb_zcl_set_attr_val(ZB_COUNTER_EP, 
                                                     ZB_ZCL_CLUSTER_ID_MULTI_VALUE, 
                                                     ZB_ZCL_CLUSTER_SERVER_ROLE, 
                                                     ZB_ZCL_ATTR_MULTI_VALUE_PRESENT_VALUE_ID, 
                                                     (zb_uint8_t *)&new_value,
                                                     ZB_FALSE);

    if (zcl_status == ZB_ZCL_STATUS_SUCCESS) { NRF_LOG_INFO("Set new value in COUNTER PULSE_IN_KWH success! Value: %d", new_value);       }
    else                                     { NRF_LOG_INFO("Set new value in COUNTER PULSE_IN_KWH is fail. zcl_status: %d", zcl_status); }
}

/** @brief Function for update DUTY TIME_LIVE_IN_HOUR cluster attributes */
static void update_duty_time_live_in_hour(zb_uint16_t new_value)
{
    if (!system_param.connected) return;

    zb_zcl_status_t zcl_status = zb_zcl_set_attr_val(ZB_DUTY_EP, 
                                                     ZB_ZCL_CLUSTER_ID_MULTI_INPUT, 
                                                     ZB_ZCL_CLUSTER_SERVER_ROLE, 
                                                     ZB_ZCL_ATTR_MULTI_INPUT_PRESENT_VALUE_ID, 
                                                     (zb_uint8_t *)&new_value,
                                                     ZB_FALSE);

    if (zcl_status == ZB_ZCL_STATUS_SUCCESS) { NRF_LOG_INFO("Set new value in DUTY TIME_LIVE_IN_HOUR success! Value: %d", new_value);       }
    else                                     { NRF_LOG_INFO("Set new value in DUTY TIME_LIVE_IN_HOUR is fail. zcl_status: %d", zcl_status); }
  
}

/** @brief Function for update LEDS INVERSION cluster attributes */
static void update_leds_inversion(zb_bool_t new_value)
{
    if (!system_param.connected) return;

    zb_zcl_status_t zcl_status = zb_zcl_set_attr_val(ZB_LEDS_EP, 
                                                     ZB_ZCL_CLUSTER_ID_BINARY_VALUE, 
                                                     ZB_ZCL_CLUSTER_SERVER_ROLE, 
                                                     ZB_ZCL_ATTR_BINARY_VALUE_PRESENT_VALUE_ID, 
                                                     (zb_uint8_t *)&new_value,
                                                     ZB_FALSE);

    if (zcl_status == ZB_ZCL_STATUS_SUCCESS) { NRF_LOG_INFO("Set new value in LEDS INVERSION success! Value: %d", new_value);       }
    else                                     { NRF_LOG_INFO("Set new value in LEDS INVERSION is fail. zcl_status: %d", zcl_status); }
  
}

/** @brief Function for update LEDS LEVEL cluster attributes */
static void update_leds_level_control(zb_uint16_t new_value)
{
    if (!system_param.connected) return;

    zb_zcl_status_t zcl_status;
    NRF_LOG_INFO("Set level value: %i", new_value);

    zcl_status = zb_zcl_set_attr_val(ZB_LEDS_EP,                                       
                                     ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,            
                                     ZB_ZCL_CLUSTER_SERVER_ROLE,                 
                                     ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID, 
                                     (zb_uint8_t *)&new_value,                                       
                                     ZB_FALSE);                                  
 
    if (zcl_status == ZB_ZCL_STATUS_SUCCESS) { NRF_LOG_INFO("Set new value in LEDS LEVEL success! Value: %d", new_value);               }
    else                                     { NRF_LOG_INFO("Set new value in LEDS LEVEL is fail. zcl_status: %d", zcl_status); return; }

    /* According to the table 7.3 of Home Automation Profile Specification v 1.2 rev 29, chapter 7.1.3. */
    if (new_value == 0)
    {
        zb_uint8_t value = ZB_FALSE;
        zcl_status = zb_zcl_set_attr_val(ZB_LEDS_EP, 
                                         ZB_ZCL_CLUSTER_ID_ON_OFF,    
                                         ZB_ZCL_CLUSTER_SERVER_ROLE,  
                                         ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                                         &value,                        
                                         ZB_FALSE);

        if (zcl_status == ZB_ZCL_STATUS_SUCCESS) { NRF_LOG_INFO("Set new value in LEDS ON_OFF success! Value: %d", new_value);       }
        else                                     { NRF_LOG_INFO("Set new value in LEDS ON_OFF is fail. zcl_status: %d", zcl_status); }        
    }
    else
    {
        zb_uint8_t value = ZB_TRUE;
        zcl_status = zb_zcl_set_attr_val(ZB_LEDS_EP, 
                                        ZB_ZCL_CLUSTER_ID_ON_OFF,    
                                        ZB_ZCL_CLUSTER_SERVER_ROLE,  
                                        ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                                        &value,                        
                                        ZB_FALSE);

        if (zcl_status == ZB_ZCL_STATUS_SUCCESS) { NRF_LOG_INFO("Set new value in LEDS ON_OFF success! Value: %d", new_value);       }
        else                                     { NRF_LOG_INFO("Set new value in LEDS ON_OFF is fail. zcl_status: %d", zcl_status); }

    }
}

/** @brief Function for update LEDS ON_OFF cluster attributes */
static void update_leds_on_off(zb_bool_t new_value)
{
    if (!system_param.connected) return;
    
    NRF_LOG_INFO("Set ON/OFF value: %i", new_value);

    zb_zcl_status_t zcl_status = zb_zcl_set_attr_val(ZB_LEDS_EP, 
                                                     ZB_ZCL_CLUSTER_ID_ON_OFF,    
                                                     ZB_ZCL_CLUSTER_SERVER_ROLE,  
                                                     ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                                                     (zb_uint8_t *)&new_value,                        
                                                     ZB_FALSE);

    if (zcl_status == ZB_ZCL_STATUS_SUCCESS) { NRF_LOG_INFO("Set new value in LEDS ON_OFF success! Value: %d", new_value);               }
    else                                     { NRF_LOG_INFO("Set new value in LEDS ON_OFF is fail. zcl_status: %d", zcl_status); return; }

    if (new_value)
    {
        update_leds_level_control(m_dev_ctx.leds_level_control.current_level);
    }
}


/** @brief Function for configure reporting NOT USE */
static void configure_reporting_counter_value(void)
{
  zb_zcl_reporting_info_t rep_info;
  memset(&rep_info, 0, sizeof(rep_info));

  rep_info.direction      = ZB_ZCL_CONFIGURE_REPORTING_SEND_REPORT;
  rep_info.ep             = ZB_COUNTER_EP;
  rep_info.cluster_id     = ZB_ZCL_CLUSTER_ID_ANALOG_INPUT;
  rep_info.cluster_role   = ZB_ZCL_CLUSTER_SERVER_ROLE;
  rep_info.attr_id        = ZB_ZCL_ATTR_ANALOG_INPUT_PRESENT_VALUE_ID; 
  rep_info.dst.profile_id = ZB_AF_HA_PROFILE_ID;

  rep_info.u.send_info.min_interval = 1;
  rep_info.u.send_info.max_interval = 0;
  rep_info.u.send_info.delta.u32    = 1;

  zb_zcl_put_reporting_info(&rep_info, ZB_TRUE);
} 

/**@brief Function for initializing the application timer.
 */
static void timer_init(void)
{
    uint32_t error_code = app_timer_init();
    APP_ERROR_CHECK(error_code);
}

/**@brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

/**@brief Function for handling nrf app timer.
 * 
 * @param[IN]   context   Void pointer to context function is called with.
 * 
 * @details Function is called with pointer to sensor_device_ep_ctx_t as argument.
 */
static void zb_app_timer_handler(void * context)
{
    //NRF_LOG_INFO("CUR LEVEL: %d", m_dev_ctx.leds_level_control.current_level); 
    update_counter_value(convert_counter_value(system_param.counter_param.cur_counter));
}

/**@brief Callback for button events.
 *
 * @param[in]   evt      Incoming event from the BSP subsystem.
 */
static void buttons_handler(bsp_event_t evt)
{
    zb_ret_t err_code;

    switch(evt)
    {
        case IDENTIFY_MODE_BSP_EVT:
            /* Check if endpoint is in identifying mode, if not put desired endpoint in identifying mode. */
            if (m_dev_ctx.identify_attr.identify_time == ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE)
            {
                NRF_LOG_INFO("Device put in identifying mode");
                err_code = zb_bdb_finding_binding_target(ZB_COUNTER_EP);
                ZB_ERROR_CHECK(err_code);
            }
            else
            {
                NRF_LOG_INFO("Cancel F&B target procedure");
                zb_bdb_finding_binding_target_cancel();
            }
            break;

        case BSP_EVENT_RESET:
            
            /* Смена состояния */
            reset_button_press = !reset_button_press;

            if (reset_button_press) 
            { 
                reset_button_time_start = ZB_TIMER_GET();

                /* Таймер для проверки длинного нажатия TIME_PRESS_BUT_LONG_CLICK */
                err_code = app_timer_start(long_click_button_timer, APP_TIMER_TICKS(500), NULL);
                ZB_ERROR_CHECK(err_code);
            }
            else
            {
                err_code = app_timer_stop(long_click_button_timer);
                ZB_ERROR_CHECK(err_code);

                /* Если время нажатия кнопки было больше TIME_PRESS_BUT_SHORT_CLICK - короткое нажатие */
                if (ZB_TIME_SUBTRACT(ZB_TIMER_GET(), reset_button_time_start) >= TIME_PRESS_BUT_SHORT_CLICK &&
                    ZB_TIME_SUBTRACT(ZB_TIMER_GET(), reset_button_time_start) <  TIME_PRESS_BUT_LONG_CLICK)
                {
                    ZB_SCHEDULE_APP_CALLBACK(zb_user_button_handler, BUTTON_EVENT_SHORT_CLICK);
                }     
            } 
            break;

        default:
            NRF_LOG_INFO("Unhandled BSP Event received: %d", evt);
            break;
    }
}

static const uint8_t m_board_led_list[LEDS_NUMBER] = LEDS_LIST;

/**@brief Function for initializing LEDs and a single PWM channel.
 */
static void leds_buttons_init(void)
{
    /* Initialize all LEDs and buttons. */
    ret_code_t       err_code;
    app_pwm_config_t pwm_cfg = APP_PWM_DEFAULT_CONFIG_1CH(5000L, LEDS_PWM_PIN);//bsp_board_led_idx_to_pin(BULB_LED));
    
    err_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS, buttons_handler);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_event_to_button_action_assign(BSP_BOARD_BUTTON_0, BSP_BUTTON_ACTION_PUSH, BSP_EVENT_RESET);
	APP_ERROR_CHECK(err_code);

    err_code = bsp_event_to_button_action_assign(BSP_BOARD_BUTTON_0, BSP_BUTTON_ACTION_RELEASE, BSP_EVENT_RESET);
	APP_ERROR_CHECK(err_code);

    /* Initialize PWM running on timer 1 in order to control dimmable light bulb. */
    err_code = app_pwm_init(&LEDS_PWM, &pwm_cfg, NULL);
    APP_ERROR_CHECK(err_code);

    app_pwm_enable(&LEDS_PWM);

    while (app_pwm_channel_duty_set(&LEDS_PWM, 0, 99) == NRF_ERROR_BUSY)
    {
    }
}

/**@brief Function for initializing all clusters attributes.
 */
static void clusters_attr_init(void)
{
    /* Basic cluster attributes data */
    m_dev_ctx.basic_attr.zcl_version   = ZB_ZCL_VERSION;
    m_dev_ctx.basic_attr.app_version   = BULB_INIT_BASIC_APP_VERSION;
    m_dev_ctx.basic_attr.stack_version = BULB_INIT_BASIC_STACK_VERSION;
    m_dev_ctx.basic_attr.hw_version    = BULB_INIT_BASIC_HW_VERSION;
    m_dev_ctx.basic_attr.power_source  = BULB_INIT_BASIC_POWER_SOURCE;
    m_dev_ctx.basic_attr.ph_env        = BULB_INIT_BASIC_PH_ENV;

    ZB_ZCL_SET_STRING_VAL(m_dev_ctx.basic_attr.mf_name,
                          BULB_INIT_BASIC_MANUF_NAME,
                          ZB_ZCL_STRING_CONST_SIZE(BULB_INIT_BASIC_MANUF_NAME));

    ZB_ZCL_SET_STRING_VAL(m_dev_ctx.basic_attr.model_id,
                          BULB_INIT_BASIC_MODEL_ID,
                          ZB_ZCL_STRING_CONST_SIZE(BULB_INIT_BASIC_MODEL_ID));

    ZB_ZCL_SET_STRING_VAL(m_dev_ctx.basic_attr.date_code,
                          BULB_INIT_BASIC_DATE_CODE,
                          ZB_ZCL_STRING_CONST_SIZE(BULB_INIT_BASIC_DATE_CODE));

    ZB_ZCL_SET_STRING_VAL(m_dev_ctx.basic_attr.location_id,
                          BULB_INIT_BASIC_LOCATION_DESC,
                          ZB_ZCL_STRING_CONST_SIZE(BULB_INIT_BASIC_LOCATION_DESC));

    /* Identify cluster attributes data */
    m_dev_ctx.identify_attr.identify_time = ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE;               

    /* Users clusters attributes data */
    m_dev_ctx.duty_time_live_in_hour.number_of_states  = ZB_ZCL_MULTI_INPUT_NUMBER_OF_STATES_MAX_VALUE;
    m_dev_ctx.counter_pulse_in_kwh.number_of_states    = ZB_ZCL_MULTI_INPUT_NUMBER_OF_STATES_MAX_VALUE;
    m_dev_ctx.leds_on_off.on_off                       = (zb_bool_t)ZB_ZCL_ON_OFF_IS_ON;
    m_dev_ctx.leds_level_control.current_level         = ZB_ZCL_LEVEL_CONTROL_LEVEL_MAX_VALUE;
    m_dev_ctx.leds_level_control.remaining_time        = ZB_ZCL_LEVEL_CONTROL_REMAINING_TIME_DEFAULT_VALUE;

    ZB_ZCL_SET_ATTRIBUTE(ZB_LEDS_EP, 
                         ZB_ZCL_CLUSTER_ID_ON_OFF,    
                         ZB_ZCL_CLUSTER_SERVER_ROLE,  
                         ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                         (zb_uint8_t *)&m_dev_ctx.leds_on_off.on_off,                        
                         ZB_FALSE);                   

    ZB_ZCL_SET_ATTRIBUTE(ZB_LEDS_EP,                                       
                         ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,            
                         ZB_ZCL_CLUSTER_SERVER_ROLE,                 
                         ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID, 
                         (zb_uint8_t *)&m_dev_ctx.leds_level_control.current_level,                                       
                         ZB_FALSE);  

    ZB_ZCL_SET_ATTRIBUTE(ZB_DUTY_EP,                                       
                         ZB_ZCL_CLUSTER_ID_MULTI_INPUT,            
                         ZB_ZCL_CLUSTER_SERVER_ROLE,                 
                         ZB_ZCL_ATTR_MULTI_INPUT_NUMBER_OF_STATES_ID, 
                         (zb_uint8_t *)&m_dev_ctx.duty_time_live_in_hour.number_of_states,                                     
                         ZB_FALSE);

    ZB_ZCL_SET_ATTRIBUTE(ZB_COUNTER_EP,                                       
                         ZB_ZCL_CLUSTER_ID_MULTI_VALUE,            
                         ZB_ZCL_CLUSTER_SERVER_ROLE,                 
                         ZB_ZCL_ATTR_MULTI_VALUE_NUMBER_OF_STATES_ID, 
                         (zb_uint8_t *)&m_dev_ctx.counter_pulse_in_kwh.number_of_states,                                      
                         ZB_FALSE);             
}

/**@brief Function which tries to sleep down the MCU 
 *
 * Function which sleeps the MCU on the non-sleepy End Devices to optimize the power saving.
 * The weak definition inside the OSIF layer provides some minimal working template
 */
zb_void_t zb_osif_go_idle(zb_void_t)
{
    //TODO: implement your own logic if needed
    zb_osif_wait_for_event();
}

/**@brief Callback function for handling ZCL commands.
 *
 * @param[in]   bufid   Reference to Zigbee stack buffer used to pass received data.
 */
static zb_void_t zcl_device_cb(zb_bufid_t bufid)
{
    zb_uint8_t                       cluster_id;
    zb_uint8_t                       attr_id;
    zb_zcl_device_callback_param_t * p_device_cb_param = ZB_BUF_GET_PARAM(bufid, zb_zcl_device_callback_param_t);

    NRF_LOG_INFO("zcl_device_cb id %hd", p_device_cb_param->device_cb_id);

    /* Set default response value. */
    p_device_cb_param->status = RET_OK;

    switch (p_device_cb_param->device_cb_id)
    {
          case ZB_ZCL_LEVEL_CONTROL_SET_VALUE_CB_ID:
            NRF_LOG_INFO("Level control setting to %d", p_device_cb_param->cb_param.level_control_set_value_param.new_value);
            update_leds_level_control(p_device_cb_param->cb_param.level_control_set_value_param.new_value);
            break;

        case ZB_ZCL_SET_ATTR_VALUE_CB_ID:
            cluster_id = p_device_cb_param->cb_param.set_attr_value_param.cluster_id;
            attr_id    = p_device_cb_param->cb_param.set_attr_value_param.attr_id;

            if (cluster_id == ZB_ZCL_CLUSTER_ID_ON_OFF)
            {
                zb_uint8_t value = p_device_cb_param->cb_param.set_attr_value_param.values.data8;

                NRF_LOG_INFO("on/off attribute setting to %hd", value);
                if (attr_id == ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID)
                {
                    update_leds_on_off((zb_bool_t) value);
                }
            }
            else if (cluster_id == ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL)
            {
                zb_uint16_t value = p_device_cb_param->cb_param.set_attr_value_param.values.data16;

                NRF_LOG_INFO("level control attribute setting to %hd", value);
                if (attr_id == ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID)
                {
                    update_leds_level_control(value);
                }
            }
            else
            {
                /* Other clusters can be processed here */
                NRF_LOG_INFO("Unhandled cluster attribute id: %d", cluster_id);
            }
            break;

        default:
            p_device_cb_param->status = RET_ERROR;
            break;
    }

    NRF_LOG_INFO("zcl_device_cb status: %hd", p_device_cb_param->status);
}

/**@brief Zigbee stack event handler.
 *
 * @param[in]   bufid   Reference to the Zigbee stack buffer used to pass signal.
 */
void zboss_signal_handler(zb_bufid_t bufid)
{
    zb_zdo_app_signal_hdr_t  * p_sg_p      = NULL;
    zb_zdo_app_signal_type_t   sig         = zb_get_app_signal(bufid, &p_sg_p);
    zb_ret_t                   status      = ZB_GET_APP_SIGNAL_STATUS(bufid);
    zb_bool_t                  comm_status;
    ret_code_t                 err_code;

    /* Update network status LED */
    //zigbee_led_status_update(bufid, ZIGBEE_NETWORK_STATE_LED);

    switch (sig)
    {
        /*case ZB_ZDO_SIGNAL_SKIP_STARTUP:
            NRF_LOG_INFO("ZB_ZDO_SIGNAL_SKIP_STARTUP");
            break;*/

        case ZB_BDB_SIGNAL_DEVICE_FIRST_START:
            NRF_LOG_INFO("ZB_BDB_SIGNAL_DEVICE_FIRST_START");
            nrf_802154_channel_set(ZIGBEE_CHANNEL);
            nrf_802154_tx_power_set(NRF_RADIO_TXPOWER_NEG40DBM );
            nrf_radio_txpower_set(NRF_RADIO_TXPOWER_NEG40DBM );
            nrf_802154_init();
            break;

        case ZB_BDB_SIGNAL_DEVICE_REBOOT:
            /* fall-through */
        case ZB_BDB_SIGNAL_STEERING:
            if (status == RET_OK)
            { 
                char ieee_addr_buf[17] = {0};
                int  addr_len;

                /* Сброс таймера */
                UNUSED_RETURN_VALUE(ZB_SCHEDULE_APP_ALARM_CANCEL(join_timeout_handler, 0));

                /* Установка события подключения к сети в TRUE */
                system_param.connected = ZB_TRUE;
                system_param.pan_id = ZB_PIBCACHE_PAN_ID();
                zb_get_extended_pan_id(system_param.extended_pan_id);

                addr_len = ieee_addr_to_str(ieee_addr_buf, sizeof(ieee_addr_buf), system_param.extended_pan_id);
                if (addr_len < 0) strcpy(ieee_addr_buf, "unknown");
                NRF_LOG_INFO("Joined network successfully (Extended PAN ID: %s, PAN ID: 0x%04hx)", NRF_LOG_PUSH(ieee_addr_buf), system_param.pan_id);

                /* User handler */
                ZB_SCHEDULE_APP_CALLBACK(zb_network_connected, NULL);
            }
            else
            {
                NRF_LOG_ERROR("Failed to join network. Status: %d, Start_Join_Timeout: %d", status, system_param.start_join_timeout);
                
                /* Пока открыто подключение - делать попытки подключится к координатору */
                if (system_param.start_join_timeout)
                {
                    comm_status = bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING);
                    ZB_COMM_STATUS_CHECK(comm_status);
                }
            }
            break;

        /** Событие, уведомляющее о том, что устройство само вышло из сети. */
        case ZB_ZDO_SIGNAL_LEAVE:
            if (status == RET_OK)
            {
                zb_zdo_signal_leave_params_t * p_leave_params = ZB_ZDO_SIGNAL_GET_PARAMS(p_sg_p, zb_zdo_signal_leave_params_t);
                NRF_LOG_INFO("Network left. Leave type: %d, Connected = %d", p_leave_params->leave_type, system_param.connected);

                /* handle leave parameters leave_params */
                ZB_SCHEDULE_APP_CALLBACK(zb_network_disconnected, NULL);
            }
            else
            {
                NRF_LOG_ERROR("Unable to leave network. Status: %d", status);
            }
            break;

        /** Уведомляет приложение об окончании срока действия таблицы соседей (Neighbor Table). Это означает, что соседние устройства 
         *  не отправляли сообщения о состоянии соединения в течение (ZB_NWK_ROUTER_AGE_LIMIT * ZB_NWK_LINK_STATUS_PERIOD) секунд. */
        case ZB_NWK_SIGNAL_NO_ACTIVE_LINKS_LEFT:

            NRF_LOG_WARNING("Parent is unreachable");
            ZB_SCHEDULE_APP_CALLBACK(zb_network_disconnected, NULL);

            /* Сброс таймера для определения события, что родитель снова в сети. Как только перестанет приходить это событие - флаг выставится автоматически*/
            UNUSED_RETURN_VALUE(ZB_SCHEDULE_APP_ALARM_CANCEL(zb_network_connected, 0));
            ZB_ERROR_CHECK(ZB_SCHEDULE_APP_ALARM(zb_network_connected, 0, TIMEOUT_PARENT_IS_REACHABLE));
            break;

        /** Уведомляет приложение о том, что определенная часть рабочей конфигурации приложения была найдена и восстановлена, 
         * и предоставляет приложению эти данные. В этот момент стек Zigbee попытался загрузить производственную конфигурацию из NVRAM. 
         * Этот шаг выполняется каждый раз при инициализации стека.*/
        case ZB_ZDO_SIGNAL_PRODUCTION_CONFIG_READY:
            if (status != RET_OK)
            {
                NRF_LOG_INFO("Production configuration is not present or invalid (status: %d)", status);
            }
            break;

        case ZB_BDB_SIGNAL_WWAH_REJOIN_STARTED:
            NRF_LOG_INFO("  --- ZB_BDB_SIGNAL_WWAH_REJOIN_STARTED");
            break;

        case ZB_ZGP_SIGNAL_COMMISSIONING:
            NRF_LOG_INFO("ZB_ZGP_SIGNAL_COMMISSIONING");
            break;

        default:
            /* Call default signal handler. */
            ZB_ERROR_CHECK(zigbee_default_signal_handler(bufid));
            break;
    }

    if (bufid)
    {
        zb_buf_free(bufid);
    }
}

static void not_pulse(zb_uint8_t param)
{
    ZB_ZCL_ANALOG_INPUT_SET_OUT_OF_SERVICE(ZB_COUNTER_EP);
    NRF_LOG_INFO(" -- NOT PULSE -- ");
    NRF_LOG_INFO(" -- FLAGS = %d, OUT OF SERVICE = %d -- ", m_dev_ctx.counter_value.status_flags, m_dev_ctx.counter_value.out_of_service); 
}

/** @brief Function for check pulse value */
static void check_change_counter_value(zb_uint8_t param)
{
    /* Проверка на изменение значения в ACCURACY_COUNTER_IN_KW */
    if ((double)(system_param.counter_param.cur_counter - system_param.counter_param.prev_counter) >= ACCURACY_COUNTER_IN_KW)
    {
        system_param.counter_param.prev_counter = system_param.counter_param.cur_counter;
        update_counter_value(convert_counter_value(system_param.counter_param.cur_counter));  
    }

    /* Сброс таймера */
    UNUSED_RETURN_VALUE(ZB_SCHEDULE_APP_ALARM_CANCEL(not_pulse, 0));
    ZB_ERROR_CHECK(ZB_SCHEDULE_APP_ALARM(not_pulse, 0, TIME_LACK_PULSE_SEC));

    if (m_dev_ctx.counter_value.out_of_service)
    {
        ZB_ZCL_ANALOG_INPUT_SET_NORMAL_MODE(ZB_COUNTER_EP);
    }
}

/** @brief Function for interrupt pulse */
static void pulse_interrupt_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    system_param.counter_param.cur_counter += system_param.counter_param.watt_in_one_pulse;
    bsp_board_led_invert(PULSE_STATE_LED);
    nrf_drv_gpiote_out_toggle(32+15);
    ZB_SCHEDULE_APP_CALLBACK(check_change_counter_value, NULL);
}

/** @brief Function for enable pulse interrupt */
static void gpio_interrupt_enable(void)
{
	nrf_drv_gpiote_in_event_enable(PIN_INPUT_PULSE, true);
}

/** @brief Function for disable pulse interrupt */
static void gpio_interrupt_disable(void)
{
	nrf_drv_gpiote_in_event_disable(PIN_INPUT_PULSE);
}

/**
 * @brief Function for configuring: PIN_INPUT_PULSE pin for input, PIN_OUT pin for output,
 * and configures GPIOTE to give an interrupt on pin change.
 */
static void gpio_interrupt_init(void)
{
    ret_code_t err_code;

    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
    in_config.pull                       = NRF_GPIO_PIN_PULLUP;
    in_config.hi_accuracy                = true;

    err_code = nrf_drv_gpiote_in_init(PIN_INPUT_PULSE, &in_config, pulse_interrupt_handler);
    APP_ERROR_CHECK(err_code);

    gpio_interrupt_enable();    // TODO: убрать, активировать только после настройки
}

/** @brief Function for application main entry.
 */
int main(void)
{
    zb_ret_t       zb_err_code;
    zb_ieee_addr_t ieee_addr;

    /* Initialize timer, logging system and GPIOs. */
    timer_init();
    log_init();
    leds_buttons_init();
    gpio_interrupt_init();

    /* Create Timer for reporting attribute */
    zb_err_code = app_timer_create(&zb_app_timer, APP_TIMER_MODE_REPEATED, zb_app_timer_handler);
    ZB_ERROR_CHECK(zb_err_code);

    zb_err_code = app_timer_create(&long_click_button_timer, APP_TIMER_MODE_REPEATED, check_long_click);
    ZB_ERROR_CHECK(zb_err_code);

    /* Set Zigbee stack logging level and traffic dump subsystem. */
    ZB_SET_TRACE_LEVEL(ZIGBEE_TRACE_LEVEL);
    ZB_SET_TRACE_MASK(ZIGBEE_TRACE_MASK);
    ZB_SET_TRAF_DUMP_OFF();

    /* Initialize Zigbee stack. */
    ZB_INIT("ucounter");

    /* Set device address to the value read from FICR registers. */
    zb_osif_get_ieee_eui64(ieee_addr);
    zb_set_long_address(ieee_addr);

    /* Set static long IEEE address. */
    zb_set_network_router_role(IEEE_CHANNEL_MASK);
    zb_set_max_children(MAX_CHILDREN);
    zigbee_erase_persistent_storage(ERASE_PERSISTENT_CONFIG);
    zb_set_keepalive_timeout(ZB_MILLISECONDS_TO_BEACON_INTERVAL(3000));

    /* Initialize application context structure. */
    UNUSED_RETURN_VALUE(ZB_MEMSET(&m_dev_ctx, 0, sizeof(m_dev_ctx)));

    /* ------- */
    ZB_ERROR_CHECK(ZB_SCHEDULE_APP_ALARM(not_pulse, 0, TIME_LACK_PULSE_SEC));

    UNUSED_RETURN_VALUE(ZB_MEMSET(&system_param, 0, sizeof(system_param)));
    system_param.counter_param.num_pulse_in_kwh  = 3200;
    system_param.counter_param.watt_in_one_pulse = 1000.0 / system_param.counter_param.num_pulse_in_kwh;

    nrf_gpio_cfg_output(32+14);
    nrf_gpio_cfg_output(32+15);
    nrf_gpio_cfg_output(3);

    nrf_gpio_pin_write(32+14, 1);
    nrf_gpio_pin_write(32+15, 1);
    nrf_gpio_pin_write(3, 1);
    
    /* ------- */

    /* Register callback for handling ZCL commands. */
    ZB_ZCL_REGISTER_DEVICE_CB(zcl_device_cb);

    /* Register dimmer switch device context (endpoints). */
    ZB_AF_REGISTER_DEVICE_CTX(&ucounter_ctx);

    /* Init */
    clusters_attr_init();
    update_leds_level_control(m_dev_ctx.leds_level_control.current_level);

    leds_set_brightness(128);

    /** Start Zigbee Stack. */
    zb_err_code = zboss_start_no_autostart();
    ZB_ERROR_CHECK(zb_err_code);

    while(1)
    {
        zboss_main_loop_iteration();
        UNUSED_RETURN_VALUE(NRF_LOG_PROCESS());
    }
}

//bsp_led_indication
/**
 * @}
 */