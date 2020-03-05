#include "zboss_api.h"
#include "zb_zcl_multistate_value.h"
#include "app_error.h"
#include "nrf_log.h"

#define ZB_ZCL_GET_ATTR_BOOL(p_value) (*(zb_bool_t*)p_value)
#define ZB_ZCL_GET_ATTR_MAP8(p_value) (*(zb_uint8_t*)p_value)

/**@brief Function which pre-validates the value of attributes before they are written.
 * 
 * @param [in] attr_id  Attribute ID
 * @param [in] endpoint Endpoint
 * @param [in] p_value  Pointer to the value of the attribute which is to be validated
 * 
 * @return ZB_TRUE if the value is valid, ZB_FALSE otherwise.
 */
static zb_ret_t check_value_multi_value(zb_uint16_t attr_id, zb_uint8_t endpoint, zb_uint8_t * p_value)
{
    zb_ret_t ret = ZB_FALSE;

    switch(attr_id)
    {
        case ZB_ZCL_ATTR_MULTI_VALUE_OUT_OF_SERVICE_ID:
        {   
            zb_bool_t val = ZB_ZCL_GET_ATTR_BOOL(p_value);

            if (val != ZB_TRUE && 
                val != ZB_FALSE) 
            {
                NRF_LOG_INFO("ERR: Check value ZB_ZCL_ATTR_MULTI_VALUE_OUT_OF_SERVICE_ID - fail");
                break;
            }

            ret = ZB_TRUE;
        }   break;

        case ZB_ZCL_ATTR_MULTI_VALUE_PRESENT_VALUE_ID:
        {
            zb_uint16_t val = ZB_ZCL_ATTR_GET16(p_value);
            NRF_LOG_DEBUG("Validating value %d of Multistate Value %d", val, attr_id);
            
            ret = ZB_TRUE;
        }   break;

        case ZB_ZCL_ATTR_MULTI_VALUE_STATUS_FLAGS_ID:
        {    
            zb_uint8_t val = ZB_ZCL_GET_ATTR_MAP8(p_value);

            if (val > 0x0F) 
            {
                NRF_LOG_INFO("ERR: Check value ZB_ZCL_ATTR_MULTI_VALUE_STATUS_FLAGS_ID - fail");
                break;
            }

            ret = ZB_TRUE;
        }   break;

        default:
           break;
    }

    return ret;
}

/**@brief Hook which is being called whenever a new value of attribute is being written. / Hook, который вызывается всякий раз, когда пишется новое значение атрибута.
 * 
 * @param [in] endpoint Endpoint
 * @param [in] attr_id Attribute ID
 * @param [in] new_value Pointer to the new value of the attribute
 */
static zb_void_t zb_zcl_multi_value_write_attr_hook(zb_uint8_t endpoint, zb_uint16_t attr_id, zb_uint8_t * new_value)
{
    UNUSED_PARAMETER(new_value);
    NRF_LOG_DEBUG("Writing attribute %d of Multistate Value Cluster on endpoint %d", attr_id, endpoint);

    if (attr_id == ZB_ZCL_ATTR_MULTI_VALUE_PRESENT_VALUE_ID)
    {
	      /* Implement your own write attributes hook if needed. */
    }
}

/**@brief Function which initialises the server side of Multistate Value Cluster. */
zb_void_t zb_zcl_multi_value_init_server(void) 
{
    zb_ret_t ret = zb_zcl_add_cluster_handlers(ZB_ZCL_CLUSTER_ID_MULTI_VALUE,
                                               ZB_ZCL_CLUSTER_SERVER_ROLE,
                                               check_value_multi_value,
                                               zb_zcl_multi_value_write_attr_hook,
                                               (zb_zcl_cluster_handler_t)NULL);
    ASSERT(ret == RET_OK);
}

/**@brief Function which initialises the client side of Multistate Value Cluster. */
zb_void_t zb_zcl_multi_value_init_client(void) 
{
    zb_ret_t ret = zb_zcl_add_cluster_handlers(ZB_ZCL_CLUSTER_ID_MULTI_VALUE,
                                               ZB_ZCL_CLUSTER_CLIENT_ROLE,
                                               check_value_multi_value,
                                               zb_zcl_multi_value_write_attr_hook,
                                               (zb_zcl_cluster_handler_t)NULL);
    ASSERT(ret == RET_OK);
}
