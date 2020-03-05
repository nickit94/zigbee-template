/* ZBOSS Zigbee 3.0 */

#if !defined ZB_ZCL_ANALOG_INPUT_H
#define ZB_ZCL_ANALOG_INPUT_H

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"

/** @cond DOXYGEN_ZCL_SECTION */

/* ZB_ZCL_CLUSTER_ID_ANALOG_INPUT = 0x000c defined in zb_zcl_common.h ZCL spec 3.14.2 */

/*! @name Analog Input cluster attributes
    @{
*/

/** @brief Analog Input cluster attribute identifiers. */
enum zb_zcl_analog_input_attr_e
{
  /** The Description attribute, of type Character string, MAY be used to hold a description 
   * of the usage of the input, output or value, as appropriate to the cluster. */
  ZB_ZCL_ATTR_ANALOG_INPUT_DESCRIPTION_ID         = 0x001C,
  /** The MaxPresentValue attribute, of type Single precision, indicates the highest value
   *  that can be reliably obtained for the PresentValue attribute of an Analog Input cluster,
   *  or which can reliably be used for the PresentValue attribute of an Analog Output
   *  or Analog Value cluster. */
  ZB_ZCL_ATTR_ANALOG_INPUT_MAX_PRESENT_VALUE_ID   = 0x0041,
  /** The MinPresentValue attribute, of type Single precision, indicates the lowest value 
   * that can be reliably obtained for the PresentValue attribute of an Analog Input cluster,
   * or which can reliably be used for the PresentValue attribute of an Analog Output or 
   * Analog Value cluster. */
  ZB_ZCL_ATTR_ANALOG_INPUT_MIN_PRESENT_VALUE_ID   = 0x0045,
  /** @brief The OutOfService attribute, of type Boolean, indicates whether (TRUE) or not (FALSE) 
   * the physical input, output or value that the cluster represents is not in service.  */
  ZB_ZCL_ATTR_ANALOG_INPUT_OUT_OF_SERVICE_ID      = 0x0051,
  /** @brief The PresentValue attribute indicates the current value of the input, output or value, 
   * as appropriate for the cluster. For Analog clusters it is of type single precision. */
  ZB_ZCL_ATTR_ANALOG_INPUT_PRESENT_VALUE_ID       = 0x0055,
  /** The Reliability attribute, of type 8-bit enumeration, provides an indication of 
   * whether the PresentValue or the operation of the physical input, output or value in 
   * question (as appropriate for the cluster) is “reliable” as far as can be determined and, 
   * if not, why not. */
  ZB_ZCL_ATTR_ANALOG_INPUT_RELIABILITY_ID         = 0x0067,
  /** This attribute, of type Single precision, indicates the smallest 
   * recognizable change to  PresentValue. */
  ZB_ZCL_ATTR_ANALOG_INPUT_RESOLUTION_ID          = 0x006A,
  /** @brief This attribute, of type bitmap, represents four Boolean flags that indicate 
   * the general "health" of the analog sensor. */
  ZB_ZCL_ATTR_ANALOG_INPUT_STATUS_FLAGS_ID        = 0x006F,
  /** The EngineeringUnits attribute indicates the physical units associated with
   *  the value of the PresentValue attribute of an Analog cluster. */
  ZB_ZCL_ATTR_ANALOG_INPUT_ENGINEERING_UNITS_ID   = 0x0075,
  /** The ApplicationType attribute is an unsigned 32 bit integer that indicates the 
   * specific application usage for this cluster. */
  ZB_ZCL_ATTR_ANALOG_INPUT_APPLICATION_TYPE_ID    = 0x0100,
};

/**
 *  @brief StatusFlag attribute values.
 *  @see ZCL spec 3.14.11.3.
 */
enum zb_zcl_analog_input_status_flag_value_e
{
  ZB_ZCL_ANALOG_INPUT_STATUS_FLAG_NORMAL          = 0x00,  /**< Normal (default) state. */
  ZB_ZCL_ANALOG_INPUT_STATUS_FLAG_IN_ALARM        = 0x01,  /**< In alarm bit. */
  ZB_ZCL_ANALOG_INPUT_STATUS_FLAG_FAULT           = 0x02,  /**< Fault bit. */
  ZB_ZCL_ANALOG_INPUT_STATUS_FLAG_OVERRIDEN       = 0x04,  /**< Overriden bit. */
  ZB_ZCL_ANALOG_INPUT_STATUS_FLAG_OUT_OF_SERVICE  = 0x08,  /**< Out of service bit. */
};

/** @brief Default value for Description attribute */
#define ZB_ZCL_ANALOG_INPUT_DESCRIPTION_DEFAULT_VALUE {0}

/*! @brief OutOfService attribute default value */
#define ZB_ZCL_ANALOG_INPUT_OUT_OF_SERVICE_DEFAULT_VALUE ZB_FALSE

/** @brief Default value for Reliability attribute */
#define ZB_ZCL_ANALOG_INPUT_RELIABILITY_DEFAULT_VALUE ((zb_uint8_t)0x00)

/*! @brief StatusFlag attribute default value */
#define ZB_ZCL_ANALOG_INPUT_STATUS_FLAG_DEFAULT_VALUE ZB_ZCL_ANALOG_INPUT_STATUS_FLAG_NORMAL

/*! @brief StatusFlag attribute minimum value */
#define ZB_ZCL_ANALOG_INPUT_STATUS_FLAG_MIN_VALUE 0

/*! @brief StatusFlag attribute maximum value */
#define ZB_ZCL_ANALOG_INPUT_STATUS_FLAG_MAX_VALUE 0x0F


/** @brief Declare attribute list for Analog Input cluster
    @param attr_list - attribute list name
    @param out_of_service - pointer to variable to store OutOfService attribute value
    @param present_value  - pointer to variable to store PresentValue attribute value
    @param status_flag    - pointer to variable to store StatusFlag attribute value
*/
#define ZB_ZCL_DECLARE_ANALOG_INPUT_ATTRIB_LIST(                                     \
    attr_list, out_of_service, present_value, status_flag)                           \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list)                                        \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ANALOG_INPUT_OUT_OF_SERVICE_ID, (out_of_service)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ANALOG_INPUT_PRESENT_VALUE_ID, (present_value))   \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ANALOG_INPUT_STATUS_FLAG_ID, (status_flag))       \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/*! @} */ /* Analog Input cluster attributes */

/*! @name Analog input attribute value manipulation API
    @{
*/

/** @brief Set normal operating mode
    @param ep - endpoint number
*/
#define ZB_ZCL_ANALOG_INPUT_SET_NORMAL_MODE(ep)                           \
{                                                                         \
  zb_uint8_t val;                                                         \
                                                                          \
  val = ZB_FALSE;                                                         \
  ZB_ZCL_SET_ATTRIBUTE(ep, ZB_ZCL_CLUSTER_ID_ANALOG_INPUT,                \
                       ZB_ZCL_CLUSTER_SERVER_ROLE,                        \
                       ZB_ZCL_ATTR_ANALOG_INPUT_OUT_OF_SERVICE_ID,        \
                       &val, ZB_FALSE);                                   \
                                                                          \
  val = ZB_ZCL_ANALOG_INPUT_STATUS_FLAG_NORMAL;                           \
  ZB_ZCL_SET_ATTRIBUTE(ep, ZB_ZCL_CLUSTER_ID_ANALOG_INPUT,                \
                       ZB_ZCL_CLUSTER_SERVER_ROLE,                        \
                       ZB_ZCL_ATTR_ANALOG_INPUT_STATUS_FLAGS_ID,          \
                       &val, ZB_FALSE);                                   \
}

/** @brief Set Out of service operating mode
    @param ep - endpoint number
*/
#define ZB_ZCL_ANALOG_INPUT_SET_OUT_OF_SERVICE(ep)                                          \
{                                                                                           \
  zb_zcl_attr_t *attr_desc;                                                                 \
  zb_uint8_t val;                                                                           \
                                                                                            \
  val = ZB_TRUE;                                                                            \
  ZB_ZCL_SET_ATTRIBUTE(ep, ZB_ZCL_CLUSTER_ID_ANALOG_INPUT,                                  \
                       ZB_ZCL_CLUSTER_SERVER_ROLE,                                          \
                       ZB_ZCL_ATTR_ANALOG_INPUT_OUT_OF_SERVICE_ID,                          \
                       &val, ZB_FALSE);                                                     \
                                                                                            \
  attr_desc = zb_zcl_get_attr_desc_a(ep, ZB_ZCL_CLUSTER_ID_ANALOG_INPUT,                    \
                                     ZB_ZCL_CLUSTER_SERVER_ROLE,                            \
                                     ZB_ZCL_ATTR_ANALOG_INPUT_STATUS_FLAGS_ID);             \
  if (attr_desc)                                                                            \
  {                                                                                         \
    val = *(zb_uint8_t*)attr_desc->data_p | ZB_ZCL_ANALOG_INPUT_STATUS_FLAG_OUT_OF_SERVICE; \
    ZB_ZCL_SET_ATTRIBUTE(ep, ZB_ZCL_CLUSTER_ID_ANALOG_INPUT,                                \
                         ZB_ZCL_CLUSTER_SERVER_ROLE,                                        \
                         ZB_ZCL_ATTR_ANALOG_INPUT_STATUS_FLAGS_ID,                          \
                         &val, ZB_FALSE);                                                   \
  }                                                                                         \
}

/** @brief Set overriden operating mode
   @param ep - endpoint number
*/
#define ZB_ZCL_ANALOG_INPUT_SET_OVERRIDEN_MODE(ep)                                     \
{                                                                                      \
  zb_zcl_attr_t *attr_desc;                                                            \
  zb_uint8_t val;                                                                      \
                                                                                       \
  attr_desc = zb_zcl_get_attr_desc_a(ep, ZB_ZCL_CLUSTER_ID_ANALOG_INPUT,               \
                                     ZB_ZCL_CLUSTER_SERVER_ROLE,                       \
                                     ZB_ZCL_ATTR_ANALOG_INPUT_STATUS_FLAGS_ID);        \
  if (attr_desc)                                                                       \
  {                                                                                    \
    val = *(zb_uint8_t*)attr_desc->data_p | ZB_ZCL_ANALOG_INPUT_STATUS_FLAG_OVERRIDEN; \
    ZB_ZCL_SET_ATTRIBUTE(ep, ZB_ZCL_CLUSTER_ID_ANALOG_INPUT,                           \
                         ZB_ZCL_CLUSTER_SERVER_ROLE,                                   \
                         ZB_ZCL_ATTR_ANALOG_INPUT_STATUS_FLAGS_ID,                     \
                         &val, ZB_FALSE);                                              \
  }                                                                                    \
}

/*! @} */ /* Analog input cluster commands */

/** @cond internals_doc */
/** @name Analog Input cluster internals
    Internal structures for Analog Input cluster
    @{
*/

/* Optionally, access to this attribute may be changed to READ_WRITE */
#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ANALOG_INPUT_OUT_OF_SERVICE_ID(data_ptr) \
{                                                                                   \
  ZB_ZCL_ATTR_ANALOG_INPUT_OUT_OF_SERVICE_ID,                                       \
  ZB_ZCL_ATTR_TYPE_BOOL,                                                            \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_WRITE_OPTIONAL,                 \
  (zb_voidp_t) data_ptr                                                             \
}

/* Optionally, access to this attribute may be changed to READ_WRITE - ZB_ZCL_ATTR_TYPE_SINGLE */
#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ANALOG_INPUT_PRESENT_VALUE_ID(data_ptr)             \
{                                                                                              \
  ZB_ZCL_ATTR_ANALOG_INPUT_PRESENT_VALUE_ID,                                                   \
  ZB_ZCL_ATTR_TYPE_SINGLE,                                                                     \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_WRITE_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING, \
  (zb_voidp_t) data_ptr                                                                        \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ANALOG_INPUT_STATUS_FLAG_ID(data_ptr) \
{                                                                                \
  ZB_ZCL_ATTR_ANALOG_INPUT_STATUS_FLAGS_ID,                                      \
  ZB_ZCL_ATTR_TYPE_8BITMAP,                                                      \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                   \
  (zb_voidp_t) data_ptr                                                          \
}

/*! Number of attributes mandatory for reporting in Analog Input cluster */
#define ZB_ZCL_ANALOG_INPUT_REPORT_ATTR_COUNT 2


/** @brief Analog Input cluster attribute type. */
typedef struct zb_zcl_analog_input_attrs_s
{
  /** The OutOfService attribute, of type Boolean, indicates whether (TRUE) or not (FALSE) 
   * the physical input, output or value that the cluster represents is not in service.  */
  zb_bool_t out_of_service;

  /** The PresentValue attribute indicates the current value of the input, output or value, 
   * as appropriate for the cluster. For Analog clusters it is of type single precision. */
  zb_uint32_t present_value;

  /** This attribute, of type bitmap, represents four Boolean flags that indicate 
   * the general "health" of the analog sensor. */
  zb_uint8_t status_flags;

} zb_zcl_analog_input_attrs_t;

/*! @}
 *  @endcond */ /* Analog Input cluster internals */


/*! @} */ /* ZCL Analog Input cluster definitions */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

zb_void_t zb_zcl_analog_input_init_server(void);
zb_void_t zb_zcl_analog_input_init_client(void);
#define ZB_ZCL_CLUSTER_ID_ANALOG_INPUT_SERVER_ROLE_INIT zb_zcl_analog_input_init_server
#define ZB_ZCL_CLUSTER_ID_ANALOG_INPUT_CLIENT_ROLE_INIT zb_zcl_analog_input_init_client

#endif /* ZB_ZCL_ANALOG_INPUT_H */
