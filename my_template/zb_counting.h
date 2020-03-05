#ifndef ZB_COUNTING_H_
#define ZB_COUNTING_H_

#include "zboss_api.h"
#include "zboss_api_addons.h"

#include "C:\nRF_SDK\nRF5_SDK_for_Thread_and_Zigbee_v4.0.0_dc7186b\examples\zigbee\light_control\my_template\zb_zcl_analog_input.h"
#include "C:\nRF_SDK\nRF5_SDK_for_Thread_and_Zigbee_v4.0.0_dc7186b\examples\zigbee\light_control\my_template\zb_zcl_multistate_value.h"
#include "C:\nRF_SDK\nRF5_SDK_for_Thread_and_Zigbee_v4.0.0_dc7186b\examples\zigbee\light_control\my_template\zb_zcl_binary_value.h"
#include "C:\nRF_SDK\nRF5_SDK_for_Thread_and_Zigbee_v4.0.0_dc7186b\examples\zigbee\light_control\my_template\zb_zcl_multistate_input.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ZB_DEVICE_VER                        1

/* COUNTER EndPoint - для работы счетчика */
#define ZB_COUNTER_EP                        10                  /**< Device endpoint. */
#define ZB_COUNTER_EP_REPORT_ATTR_COUNT      4                   /**< Number of attributes mandatory for reporting in cluster. */
#define ZB_COUNTER_EP_IN_CLUSTER_NUM         4                   /**< Number of the input (server) clusters in the multisensor device. */
#define ZB_COUNTER_EP_OUT_CLUSTER_NUM        0                   /**< Number of the output (client) clusters in the multisensor device. */

/* DUTY EndPoint - для настроек устройства */
#define ZB_DUTY_EP                           11                  /**< Device endpoint. */
#define ZB_DUTY_EP_REPORT_ATTR_COUNT         2                   /**< Number of attributes mandatory for reporting in cluster. */
#define ZB_DUTY_EP_IN_CLUSTER_NUM            3                   /**< Number of the input (server) clusters in the multisensor device. */
#define ZB_DUTY_EP_OUT_CLUSTER_NUM           0                   /**< Number of the output (client) clusters in the multisensor device. */

/* LEDS EndPoint - для светодиодов */
#define ZB_LEDS_EP                           12                  /**< Device endpoint. */
#define ZB_LEDS_EP_REPORT_ATTR_COUNT         4                   /**< Number of attributes mandatory for reporting in cluster. */
#define ZB_LEDS_EP_IN_CLUSTER_NUM            5                   /**< Number of the input (server) clusters in the multisensor device. */
#define ZB_LEDS_EP_OUT_CLUSTER_NUM           0                   /**< Number of the output (client) clusters in the multisensor device. */
#define ZB_LEDS_EP_LIGHT_CVC_ATTR_COUNT      1

#define ZB_SUM_REPORTING_ATTR_COUNT           \
          (ZB_COUNTER_EP_REPORT_ATTR_COUNT + ZB_DUTY_EP_REPORT_ATTR_COUNT + ZB_LEDS_EP_REPORT_ATTR_COUNT)

/* Main application customizable context. Stores all settings and static values. */
typedef struct
{
    zb_zcl_basic_attrs_ext_t         basic_attr_counter;            /**< Базовый атрибут с информацией об устройстве */
    zb_zcl_basic_attrs_ext_t         basic_attr_duty;            /**< Базовый атрибут с информацией об устройстве */
    zb_zcl_basic_attrs_ext_t         basic_attr_leds;            /**< Базовый атрибут с информацией об устройстве */
    
    zb_zcl_identify_attrs_t          identify_attr_counter;         /**< */
    zb_zcl_identify_attrs_t          identify_attr_duty;         /**< */
    zb_zcl_identify_attrs_t          identify_attr_leds;         /**< */

    zb_zcl_analog_input_attrs_t      counter_value;         /**< Значение счетчика в десятках ватт в час */
    zb_zcl_multi_value_attrs_t       counter_pulse_in_kwh;  /**< Кол-во импульсов в одном кВт*ч */
    zb_zcl_multi_input_attrs_t       duty_time_live_in_hour;/**< Время работы устройства до перезагрузки в часах */
    zb_zcl_on_off_attrs_ext_t        leds_on_off;           /**< */
    zb_zcl_level_control_attrs_t     leds_level_control;    /**< */
    zb_zcl_binary_value_attrs_t      leds_inversion;        /**< Флаг инверсии светодиода импульса */

} counting_device_ctx_t;

/* Counter struct param */
typedef struct counter_s
{
    zb_uint16_t num_pulse_in_kwh;       /**< Кол-во импульсов в 1 кВт*ч */
    double      cur_counter;            /**< Текущее насчитанное значение счетчика */
    double      prev_counter;           /**< Предыдущее насчитанное значение счетчика */
    double      watt_in_one_pulse;      /**< Кол-во ватт в 1 импульсе */
} counter_param_t;

typedef struct system_param_s
{
    zb_bool_t       start_join_timeout;
    zb_bool_t       connected;
    zb_ext_pan_id_t extended_pan_id;
    zb_uint16_t     pan_id;
    counter_param_t counter_param;

} system_param_t;


system_param_t system_param_default = 
{
  .start_join_timeout = ZB_FALSE,
  .connected          = ZB_FALSE,
  .extended_pan_id    = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  .pan_id             = 0x0000,
  .counter_param      = 
  {
      .num_pulse_in_kwh  = 3200,
      .cur_counter       = 0,
      .prev_counter      = 0,
      .watt_in_one_pulse = 1000.0 / 3200,
  }
};

/* -------------------------------------------- COUNTER ENDPOINT DECLARE ---------------------------------------------------------- */

/** @brief Declares cluster list for the COUNTER EndPoint.
 *
 *  @param cluster_list_name            Cluster list variable name.
 *  @param basic_attr_list              Attribute list for the Basic cluster.
 *  @param identify_attr_list           Attribute list for the Identify cluster.
 *  @param counter_attr_list            Attribute list for the Analog Input cluster.
 *  @param pulse_in_kwh_attr_list       Attribute list for the Multistate Value cluster.
 */
#define ZB_DECLARE_COUNTER_CLUSTER_LIST(                            \
      cluster_list_name,                                            \
      basic_attr_list,                                              \
      identify_attr_list,                                           \
      counter_attr_list,                                            \
      pulse_in_kwh_attr_list)                                       \
      zb_zcl_cluster_desc_t cluster_list_name[] =                   \
      {                                                             \
        ZB_ZCL_CLUSTER_DESC(                                        \
          ZB_ZCL_CLUSTER_ID_BASIC,                                  \
          ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),        \
          (basic_attr_list),                                        \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                               \
          ZB_ZCL_MANUF_CODE_INVALID                                 \
        ),                                                          \
        ZB_ZCL_CLUSTER_DESC(                                        \
          ZB_ZCL_CLUSTER_ID_IDENTIFY,                               \
          ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t),     \
          (identify_attr_list),                                     \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                               \
          ZB_ZCL_MANUF_CODE_INVALID                                 \
        ),                                                          \
        ZB_ZCL_CLUSTER_DESC(                                        \
          ZB_ZCL_CLUSTER_ID_ANALOG_INPUT,                           \
          ZB_ZCL_ARRAY_SIZE(counter_attr_list, zb_zcl_attr_t),      \
          (counter_attr_list),                                      \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                               \
          ZB_ZCL_MANUF_CODE_INVALID                                 \
        ),                                                          \
        ZB_ZCL_CLUSTER_DESC(                                        \
          ZB_ZCL_CLUSTER_ID_MULTI_VALUE,                            \
          ZB_ZCL_ARRAY_SIZE(pulse_in_kwh_attr_list, zb_zcl_attr_t), \
          (pulse_in_kwh_attr_list),                                 \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                               \
          ZB_ZCL_MANUF_CODE_INVALID                                 \
        )                                                           \
      }

/** @brief Declares simple descriptor for the COUNTER EndPoint.
 *  
 *  @param ep_name          Endpoint variable name.
 *  @param ep_id            Endpoint ID.
 *  @param in_clust_num     Number of the supported input clusters.
 *  @param out_clust_num    Number of the supported output clusters.
 */
#define ZB_DECLARE_COUNTER_DESC(ep_name, ep_id, in_clust_num, out_clust_num)               \
  ZB_DECLARE_SIMPLE_DESC_VA(in_clust_num, out_clust_num, ep_name);                         \
  ZB_AF_SIMPLE_DESC_TYPE_VA(in_clust_num, out_clust_num, ep_name) simple_desc_## ep_name = \
  {                                                                                        \
    ep_id,                                                                                 \
    ZB_AF_HA_PROFILE_ID,                                                                   \
    ZB_HA_CONSUMPTION_AWARENESS_DEVICE_ID,                                                 \
    ZB_DEVICE_VER,                                                                         \
    0,                                                                                     \
    in_clust_num,                                                                          \
    out_clust_num,                                                                         \
    {                                                                                      \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                             \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                          \
      ZB_ZCL_CLUSTER_ID_ANALOG_INPUT,                                                      \
      ZB_ZCL_CLUSTER_ID_MULTI_VALUE,                                                       \
    }                                                                                      \
  }           

/** @brief Declares endpoint for the COUNTER EndPoint.
 *   
 *  @param ep_name          Endpoint variable name.
 *  @param ep_id            Endpoint ID.
 *  @param cluster_list     Endpoint cluster list.
 */
#define ZB_DECLARE_COUNTER_EP(ep_name, ep_id, cluster_list)                           \
  ZB_DECLARE_COUNTER_DESC(ep_name,                                                    \
                          ep_id,                                                      \
                          ZB_COUNTER_EP_IN_CLUSTER_NUM,                               \
                          ZB_COUNTER_EP_OUT_CLUSTER_NUM);                             \
                                                                                      \
  ZBOSS_DEVICE_DECLARE_REPORTING_CTX(reporting_info_counter,                          \
                                     (ZB_COUNTER_EP_REPORT_ATTR_COUNT));              \
                                                                                      \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name,                                                \
                              ep_id,                                                  \
                              ZB_AF_HA_PROFILE_ID,                                    \
                              0,                                                      \
                              NULL,                                                   \
                              ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), \
                              cluster_list,                                           \
                              (zb_af_simple_desc_1_1_t*)&simple_desc_## ep_name,      \
                              ZB_COUNTER_EP_REPORT_ATTR_COUNT,                        \
                              reporting_info_counter,                                 \
                              0,                                                      \
                              NULL)


/* -------------------------------------------- DUTY ENDPOINT DECLARE ---------------------------------------------------------- */

/** @brief Declares cluster list for the DUTY EndPoint.
 *
 *  @param cluster_list_name            Cluster list variable name.
 *  @param basic_attr_list              Attribute list for the Basic cluster.
 *  @param identify_attr_list           Attribute list for the Identify cluster.
 *  @param time_live_in_hour_attr_list  Attribute list for the Multistate Input cluster.
 */
#define ZB_DECLARE_DUTY_CLUSTER_LIST(                                     \
      cluster_list_name,                                                  \
      basic_attr_list,                                                    \
      identify_attr_list,                                                 \
      time_live_in_hour_attr_list)                                        \
      zb_zcl_cluster_desc_t cluster_list_name[] =                         \
      {                                                                   \
        ZB_ZCL_CLUSTER_DESC(                                              \
          ZB_ZCL_CLUSTER_ID_BASIC,                                        \
          ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),              \
          (basic_attr_list),                                              \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                                     \
          ZB_ZCL_MANUF_CODE_INVALID                                       \
        ),                                                                \
        ZB_ZCL_CLUSTER_DESC(                                              \
          ZB_ZCL_CLUSTER_ID_IDENTIFY,                                     \
          ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t),           \
          (identify_attr_list),                                           \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                                     \
          ZB_ZCL_MANUF_CODE_INVALID                                       \
        ),                                                                \
        ZB_ZCL_CLUSTER_DESC(                                              \
          ZB_ZCL_CLUSTER_ID_MULTI_INPUT,                                  \
          ZB_ZCL_ARRAY_SIZE(time_live_in_hour_attr_list, zb_zcl_attr_t),  \
          (time_live_in_hour_attr_list),                                  \
          ZB_ZCL_CLUSTER_SERVER_ROLE,                                     \
          ZB_ZCL_MANUF_CODE_INVALID                                       \
        )                                                                 \
      }

/** @brief Declares simple descriptor for the DUTY EndPoint.
 *  
 *  @param ep_name          Endpoint variable name.
 *  @param ep_id            Endpoint ID.
 *  @param in_clust_num     Number of the supported input clusters.
 *  @param out_clust_num    Number of the supported output clusters.
 */
#define ZB_DECLARE_DUTY_DESC(ep_name, ep_id, in_clust_num, out_clust_num)                  \
  ZB_DECLARE_SIMPLE_DESC_VA(in_clust_num, out_clust_num, ep_name);                         \
  ZB_AF_SIMPLE_DESC_TYPE_VA(in_clust_num, out_clust_num, ep_name) simple_desc_## ep_name = \
  {                                                                                        \
    ep_id,                                                                                 \
    ZB_AF_HA_PROFILE_ID,                                                                   \
    ZB_HA_CONFIGURATION_TOOL_DEVICE_ID,                                                    \
    ZB_DEVICE_VER,                                                                         \
    0,                                                                                     \
    in_clust_num,                                                                          \
    out_clust_num,                                                                         \
    {                                                                                      \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                             \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                          \
      ZB_ZCL_CLUSTER_ID_MULTI_INPUT,                                                       \
    }                                                                                      \
  }    

/** @brief Declares endpoint for the DUTY EndPoint.
 *   
 *  @param ep_name          Endpoint variable name.
 *  @param ep_id            Endpoint ID.
 *  @param cluster_list     Endpoint cluster list.
 */
#define ZB_DECLARE_DUTY_EP(ep_name, ep_id, cluster_list)                              \
  ZB_DECLARE_DUTY_DESC(ep_name,                                                       \
                       ep_id,                                                         \
                       ZB_DUTY_EP_IN_CLUSTER_NUM,                                     \
                       ZB_DUTY_EP_OUT_CLUSTER_NUM);                                   \
                                                                                      \
  ZBOSS_DEVICE_DECLARE_REPORTING_CTX(reporting_info_duty,                        \
                                     (ZB_DUTY_EP_REPORT_ATTR_COUNT));                 \
                                                                                      \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name,                                                \
                              ep_id,                                                  \
                              ZB_AF_HA_PROFILE_ID,                                    \
                              0,                                                      \
                              NULL,                                                   \
                              ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), \
                              cluster_list,                                           \
                              (zb_af_simple_desc_1_1_t*)&simple_desc_## ep_name,      \
                              ZB_DUTY_EP_REPORT_ATTR_COUNT,                           \
                              reporting_info_duty,                               \
                              0,                                                      \
                              NULL)                                                   \
                              

/* -------------------------------------------- LEDS ENDPOINT DECLARE ---------------------------------------------------------- */

/*!
  @brief Declare cluster list for Dimmable Light device
  @param cluster_list_name - cluster list variable name
  @param basic_attr_list - attribute list for Basic cluster
  @param identify_attr_list - attribute list for Identify cluster
  @param on_off_attr_list - attribute list for On/Off cluster
  @param level_control_attr_list - attribute list for Level Control cluster
 */
#define ZB_DECLARE_LEDS_CLUSTER_LIST(                            \
  cluster_list_name,                                             \
  basic_attr_list,                                               \
  identify_attr_list,                                            \
  on_off_attr_list,                                              \
  level_control_attr_list,                                       \
  inversion_attr_list)                                           \
  zb_zcl_cluster_desc_t cluster_list_name[] =                    \
  {                                                              \
    ZB_ZCL_CLUSTER_DESC(                                         \
      ZB_ZCL_CLUSTER_ID_BASIC,                                   \
      ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),         \
      (basic_attr_list),                                         \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                \
      ZB_ZCL_MANUF_CODE_INVALID                                  \
    ),                                                           \
    ZB_ZCL_CLUSTER_DESC(                                         \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                \
      ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t),      \
      (identify_attr_list),                                      \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                \
      ZB_ZCL_MANUF_CODE_INVALID                                  \
    ),                                                           \
    ZB_ZCL_CLUSTER_DESC(                                         \
      ZB_ZCL_CLUSTER_ID_ON_OFF,                                  \
      ZB_ZCL_ARRAY_SIZE(on_off_attr_list, zb_zcl_attr_t),        \
      (on_off_attr_list),                                        \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                \
      ZB_ZCL_MANUF_CODE_INVALID                                  \
    ),                                                           \
    ZB_ZCL_CLUSTER_DESC(                                         \
      ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,                           \
      ZB_ZCL_ARRAY_SIZE(level_control_attr_list, zb_zcl_attr_t), \
      (level_control_attr_list),                                 \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                \
      ZB_ZCL_MANUF_CODE_INVALID                                  \
    ),                                                           \
    ZB_ZCL_CLUSTER_DESC(                                         \
      ZB_ZCL_CLUSTER_ID_BINARY_VALUE,                            \
      ZB_ZCL_ARRAY_SIZE(inversion_attr_list, zb_zcl_attr_t),     \
      (inversion_attr_list),                                     \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                \
      ZB_ZCL_MANUF_CODE_INVALID                                  \
    )                                                            \
  }


/*!
  @brief Declare simple descriptor for Dimmable Light device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param in_clust_num - number of supported input clusters
  @param out_clust_num - number of supported output clusters
*/
#define ZB_DECLARE_LEDS_SIMPLE_DESC(ep_name, ep_id, in_clust_num, out_clust_num)           \
  ZB_DECLARE_SIMPLE_DESC_VA(in_clust_num, out_clust_num, ep_name);                         \
  ZB_AF_SIMPLE_DESC_TYPE_VA(in_clust_num, out_clust_num, ep_name) simple_desc_## ep_name = \
  {                                                                                        \
    ep_id,                                                                                 \
    ZB_AF_HA_PROFILE_ID,                                                                   \
    ZB_HA_DIMMABLE_LIGHT_DEVICE_ID,                                                        \
    ZB_DEVICE_VER,                                                                         \
    0,                                                                                     \
    in_clust_num,                                                                          \
    out_clust_num,                                                                         \
    {                                                                                      \
      ZB_ZCL_CLUSTER_ID_BASIC,                                                             \
      ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                          \
      ZB_ZCL_CLUSTER_ID_ON_OFF,                                                            \
      ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL,                                                     \
      ZB_ZCL_CLUSTER_ID_BINARY_VALUE                                                       \
    }                                                                                      \
  }   

/*!
  @brief Declare endpoint for Dimmable Light device
  @param ep_name - endpoint variable name
  @param ep_id - endpoint ID
  @param cluster_list - endpoint cluster list
 */
#define ZB_DECLARE_LEDS_EP(ep_name, ep_id, cluster_list)                              \
  ZB_DECLARE_LEDS_SIMPLE_DESC(ep_name,                                                \
                              ep_id,                                                  \
                              ZB_LEDS_EP_IN_CLUSTER_NUM,                              \
                              ZB_LEDS_EP_OUT_CLUSTER_NUM);                            \
                                                                                      \
  ZBOSS_DEVICE_DECLARE_REPORTING_CTX(reporting_info_leds,                             \
                                     (ZB_LEDS_EP_REPORT_ATTR_COUNT));                 \
                                                                                      \
  ZBOSS_DEVICE_DECLARE_LEVEL_CONTROL_CTX(cvc_alarm_info## ep_name,                    \
                                         ZB_LEDS_EP_LIGHT_CVC_ATTR_COUNT);            \
                                                                                      \
  ZB_AF_DECLARE_ENDPOINT_DESC(ep_name,                                                \
                              ep_id,                                                  \
                              ZB_AF_HA_PROFILE_ID,                                    \
                              0,                                                      \
                              NULL,                                                   \
                              ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), \
                              cluster_list,                                           \
                              (zb_af_simple_desc_1_1_t*)&simple_desc_## ep_name,      \
                              ZB_LEDS_EP_REPORT_ATTR_COUNT,                           \
                              reporting_info_leds,                                    \
                              ZB_LEDS_EP_LIGHT_CVC_ATTR_COUNT,                        \
                              cvc_alarm_info## ep_name)

#ifdef __cplusplus
}
#endif
#endif // ZB_COUNTING_H_
