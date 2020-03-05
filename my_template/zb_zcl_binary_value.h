/* ZBOSS Zigbee 3.0 */

#if !defined ZB_ZCL_BINARY_VALUE_H
#define ZB_ZCL_BINARY_VALUE_H

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"

/** @cond DOXYGEN_ZCL_SECTION */

/** @addtogroup ZB_ZCL_BINARY_VALUE
 *  @{
 *    @details
 *    According to ZCL spec, subclause 3.14.7, Binary Value (Basic) cluster has no cluster-specific
 *    commands. Cluster attributes could be queried with
 *    @ref ZB_ZCL_COMMANDS "general ZCL commands".
 */

/* ZB_ZCL_CLUSTER_ID_BINARY_VALUE = 0x0011 defined in zb_zcl_common.h ZCL spec 3.14.7 */

/*! @name Binary Value cluster attributes
    @{
*/

/** @brief Binary Value cluster attribute identifiers. */
enum zb_zcl_binary_value_attr_e
{
  /** This attribute, of type Character string, MAY be used to hold a human readable
   *  description of the ACTIVE state of a binary PresentValue. */
  ZB_ZCL_ATTR_BINARY_VALUE_ACTIVE_TEXT_ID               = 0x0004,
  /** The Description attribute, of type Character string, MAY be used to hold a
   *  description of the usage of the value, value or value, as appropriate
   *  to the cluster. */
  ZB_ZCL_ATTR_BINARY_VALUE_DESCRIPTION_ID               = 0x001C,
  /** This attribute, of type Character string, MAY be used to hold a human readable
   *  description of the INACTIVE state of a binary PresentValue. */
  ZB_ZCL_ATTR_BINARY_VALUE_INACTIVE_TEXT_ID             = 0x002E,
  /** This property, of type 32-bit unsigned integer, represents the minimum number of 
   * seconds that a binary PresentValue SHALL remain in the INACTIVE (0) state after a 
   * write to PresentValue causes it to assume the INACTIVE state. */
  ZB_ZCL_ARRT_BINARY_VALUE_MINUMUM_OFF_TIME_ID          = 0x0042,
  /** This property, of type 32-bit unsigned integer, represents the minimum number of 
   * seconds that a binary PresentValue SHALL remain in the ACTIVE (1) state after a write 
   * to PresentValue causes it to assume the ACTIVE state. */
  ZB_ZCL_ARRT_BINARY_VALUE_MINUMUM_ON_TIME_ID           = 0x0043,
  /** @brief The OutOfService attribute, of type Boolean, indicates whether (TRUE) or not (FALSE) 
   * the physical value, value or value that the cluster represents is not in service.  */
  ZB_ZCL_ATTR_BINARY_VALUE_OUT_OF_SERVICE_ID            = 0x0051,
  /** @brief The PresentValue attribute indicates the current value of the output, output or value, 
   * as appropriate for the cluster. */
  ZB_ZCL_ATTR_BINARY_VALUE_PRESENT_VALUE_ID             = 0x0055,
  /* The PriorityArray attribute is an array of 16 structures. The first element of 
   * each structure is a Boolean, and the second element is of the same type as the 
   * PresentValue attribute of the corresponding cluster. */
  ZB_ZCL_ARRT_BINARY_VALUE_PRIORITY_ARRAY_ID            = 0x0057,
  /** The Reliability attribute, of type 8-bit enumeration, provides an indication
   *  of whether the PresentValue or the operation of the physical value,
   *  value or value in question (as appropriate for the cluster) is reliable
   *  as far as can be determined and, if not, why not. */
  ZB_ZCL_ATTR_BINARY_VALUE_RELIABILITY_ID               = 0x0067,
  /* The RelinquishDefault attribute is the default value to be used for 
   * the PresentValue attribute when all elements of the PriorityArray attribute 
   * are marked as invalid. */
  ZB_ZCL_ARRT_BINARY_VALUE_RELINQUISH_DEFAULT_ID        = 0x0068,
  /** @brief This attribute, of type bitmap, represents four Boolean flags that indicate 
   * the general "health" of the analog sensor. */
  ZB_ZCL_ATTR_BINARY_VALUE_STATUS_FLAGS_ID              = 0x006F,
  /** The ApplicationType attribute is an unsigned 32-bit integer that indicates
   *  the specific application usage for this cluster. */
  ZB_ZCL_ATTR_BINARY_VALUE_APPLICATION_TYPE_ID          = 0x0100,
};

/**
 *  @brief StatusFlag attribute values.
 *  @see ZCL spec 3.14.10.3.
 */
enum zb_zcl_binary_value_status_flag_value_e
{
  ZB_ZCL_BINARY_VALUE_STATUS_FLAG_NORMAL         = 0x00,  /**< Normal (default) state. */
  ZB_ZCL_BINARY_VALUE_STATUS_FLAG_IN_ALARM       = 0x01,  /**< In alarm bit. */
  ZB_ZCL_BINARY_VALUE_STATUS_FLAG_FAULT          = 0x02,  /**< Fault bit. */
  ZB_ZCL_BINARY_VALUE_STATUS_FLAG_OVERRIDEN      = 0x04,  /**< Overriden bit. */
  ZB_ZCL_BINARY_VALUE_STATUS_FLAG_OUT_OF_SERVICE = 0x08,  /**< Out of service bit. */
};

/*! @brief Default value for ActiveText attribute */
#define ZB_ZCL_BINARY_VALUE_ACTIVE_TEXT_DEFAULT_VALUE {0}

/*! @brief Default value for Description attribute */
#define ZB_ZCL_BINARY_VALUE_DESCRIPTION_DEFAULT_VALUE {0}

/*! @brief Default value for InactiveText attribute */
#define ZB_ZCL_BINARY_VALUE_INACTIVE_TEXT_DEFAULT_VALUE {0}

/*! @brief Default value for MinimumOffTime attribute */
#define ZB_ZCL_BINARY_VALUE_MINIMUM_OFF_TIME_DEFAULT_VALUE 0xffffffff

/*! @brief Default value for MinimumOnTime attribute */
#define ZB_ZCL_BINARY_VALUE_MINIMUM_ON_TIME_DEFAULT_VALUE 0xffffffff

/*! @brief OutOfService attribute default value */
#define ZB_ZCL_BINARY_VALUE_OUT_OF_SERVICE_DEFAULT_VALUE ZB_FALSE

/** @brief Default value for Reliability attribute */
#define ZB_ZCL_BINARY_VALUE_RELIABILITY_DEFAULT_VALUE ((zb_uint8_t)0x00)

/*! @brief StatusFlag attribute default value */
#define ZB_ZCL_BINARY_VALUE_STATUS_FLAG_DEFAULT_VALUE ZB_ZCL_BINARY_VALUE_STATUS_FLAG_NORMAL

/*! @brief StatusFlag attribute minimum value */
#define ZB_ZCL_BINARY_VALUE_STATUS_FLAG_MIN_VALUE 0

/*! @brief StatusFlag attribute maximum value */
#define ZB_ZCL_BINARY_VALUE_STATUS_FLAG_MAX_VALUE 0x0F

/** @brief Declare attribute list for Binary Value cluster
    @param attr_list - attribute list name
    @param out_of_service - pointer to variable to store OutOfService attribute value
    @param present_value -  pointer to variable to store PresentValue attribute value
    @param status_flag -  pointer to variable to store StatusFlag attribute value
*/
#define ZB_ZCL_DECLARE_BINARY_VALUE_ATTRIB_LIST(                                     \
    attr_list, out_of_service, present_value, status_flag)                           \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list)                                        \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BINARY_VALUE_OUT_OF_SERVICE_ID, (out_of_service)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BINARY_VALUE_PRESENT_VALUE_ID, (present_value))   \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BINARY_VALUE_STATUS_FLAG_ID, (status_flag))       \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/*! @} */ /* Binary Value cluster attributes */

/*! @name Binary value attribute value manipulation API
    @{
*/

/** @brief Set normal operating mode
    @param ep - endpoint number
*/
#define ZB_ZCL_BINARY_VALUE_SET_NORMAL_MODE(ep)                          \
{                                                                        \
  zb_uint8_t val;                                                        \
                                                                         \
  val = ZB_FALSE;                                                        \
  ZB_ZCL_SET_ATTRIBUTE(ep, ZB_ZCL_CLUSTER_ID_BINARY_VALUE,               \
                       ZB_ZCL_CLUSTER_SERVER_ROLE,                       \
                       ZB_ZCL_ATTR_BINARY_VALUE_OUT_OF_SERVICE_ID,       \
                       &val, ZB_FALSE);                                  \
                                                                         \
  val = ZB_ZCL_BINARY_VALUE_STATUS_FLAG_NORMAL;                          \
  ZB_ZCL_SET_ATTRIBUTE(ep, ZB_ZCL_CLUSTER_ID_BINARY_VALUE,               \
                       ZB_ZCL_CLUSTER_SERVER_ROLE,                       \
                       ZB_ZCL_ATTR_BINARY_VALUE_STATUS_FLAG_ID,          \
                       &val, ZB_FALSE);                                  \
}

/** @brief Set Out of service operating mode
    @param ep - endpoint number
*/
#define ZB_ZCL_BINARY_VALUE_SET_OUT_OF_SERVICE(ep)                                          \
{                                                                                           \
  zb_zcl_attr_t *attr_desc;                                                                 \
  zb_uint8_t val;                                                                           \
                                                                                            \
  val = ZB_TRUE;                                                                            \
  ZB_ZCL_SET_ATTRIBUTE(ep, ZB_ZCL_CLUSTER_ID_BINARY_VALUE,                                  \
                       ZB_ZCL_CLUSTER_SERVER_ROLE,                                          \
                       ZB_ZCL_ATTR_BINARY_VALUE_OUT_OF_SERVICE_ID,                          \
                       &val, ZB_FALSE);                                                     \
                                                                                            \
  attr_desc = zb_zcl_get_attr_desc_a(ep, ZB_ZCL_CLUSTER_ID_BINARY_VALUE,                    \
                                     ZB_ZCL_CLUSTER_SERVER_ROLE,                            \
                                     ZB_ZCL_ATTR_BINARY_VALUE_STATUS_FLAG_ID);              \
  if (attr_desc)                                                                            \
  {                                                                                         \
    val = *(zb_uint8_t*)attr_desc->data_p | ZB_ZCL_BINARY_VALUE_STATUS_FLAG_OUT_OF_SERVICE; \
    ZB_ZCL_SET_ATTRIBUTE(ep, ZB_ZCL_CLUSTER_ID_BINARY_VALUE,                                \
                         ZB_ZCL_CLUSTER_SERVER_ROLE,                                        \
                         ZB_ZCL_ATTR_BINARY_VALUE_STATUS_FLAG_ID,                           \
                         &val, ZB_FALSE);                                                   \
  }                                                                                         \
}

/** @brief Set overriden operating mode
   @param ep - endpoint number
*/
#define ZB_ZCL_BINARY_VALUE_SET_OVERRIDEN_MODE(ep)                                     \
{                                                                                      \
  zb_zcl_attr_t *attr_desc;                                                            \
  zb_uint8_t val;                                                                      \
                                                                                       \
  attr_desc = zb_zcl_get_attr_desc_a(ep, ZB_ZCL_CLUSTER_ID_BINARY_VALUE,               \
                                     ZB_ZCL_CLUSTER_SERVER_ROLE,                       \
                         ZB_ZCL_ATTR_BINARY_VALUE_STATUS_FLAG_ID);                     \
  if (attr_desc)                                                                       \
  {                                                                                    \
    val = *(zb_uint8_t*)attr_desc->data_p | ZB_ZCL_BINARY_VALUE_STATUS_FLAG_OVERRIDEN; \
    ZB_ZCL_SET_ATTRIBUTE(ep, ZB_ZCL_CLUSTER_ID_BINARY_VALUE,                           \
                         ZB_ZCL_CLUSTER_SERVER_ROLE,                                   \
                         ZB_ZCL_ATTR_BINARY_VALUE_STATUS_FLAG_ID,                      \
                         &val, ZB_FALSE);                                              \
  }                                                                                    \
}

/*! @} */ /* Binary value cluster commands */

/** @cond internals_doc */
/** @name Binary Value cluster internals
    Internal structures for Binary Value cluster
    @{
*/

/* Optionally, access to this attribute may be changed to READ_WRITE */
#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_BINARY_VALUE_OUT_OF_SERVICE_ID(data_ptr) \
{                                                                                   \
  ZB_ZCL_ATTR_BINARY_VALUE_OUT_OF_SERVICE_ID,                                       \
  ZB_ZCL_ATTR_TYPE_BOOL,                                                            \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_WRITE_OPTIONAL,                 \
  (zb_voidp_t) data_ptr                                                             \
}

/* Optionally, access to this attribute may be changed to READ_WRITE */
#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_BINARY_VALUE_PRESENT_VALUE_ID(data_ptr)                 \
{                                                                                                  \
  ZB_ZCL_ATTR_BINARY_VALUE_PRESENT_VALUE_ID,                                                       \
  ZB_ZCL_ATTR_TYPE_BOOL,                                                                           \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_WRITE_OPTIONAL | ZB_ZCL_ATTR_ACCESS_REPORTING, \
  (zb_voidp_t) data_ptr                                                                            \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_BINARY_VALUE_STATUS_FLAG_ID(data_ptr) \
{                                                                                \
  ZB_ZCL_ATTR_BINARY_VALUE_STATUS_FLAGS_ID,                                      \
  ZB_ZCL_ATTR_TYPE_8BITMAP,                                                      \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                   \
  (zb_voidp_t) data_ptr                                                          \
}

/*! Number of attributes mandatory for reporting in Binary Value cluster */
#define ZB_ZCL_BINARY_VALUE_REPORT_ATTR_COUNT 2


/** @brief Binary Output cluster attribute type. */
typedef struct zb_zcl_binary_value_attrs_s
{
  /** The OutOfService attribute, of type Boolean, indicates whether (TRUE) or not (FALSE) 
   * the physical output, output or value that the cluster represents is not in service.  */
  zb_bool_t out_of_service;

  /** The PresentValue attribute indicates the current value of the output, output or value, 
   * as appropriate for the cluster. For Analog clusters it is of type single precision. */
  zb_bool_t present_value;

  /** This attribute, of type bitmap, represents four Boolean flags that indicate 
   * the general "health" of the analog sensor. */
  zb_uint8_t status_flags;

} zb_zcl_binary_value_attrs_t;

/*! @}
 *  @endcond */ /* Binary Value cluster internals */


/*! @} */ /* ZCL Binary Value cluster definitions */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

zb_void_t zb_zcl_binary_value_init_server(void);
zb_void_t zb_zcl_binary_value_init_client(void);
#define ZB_ZCL_CLUSTER_ID_BINARY_VALUE_SERVER_ROLE_INIT zb_zcl_binary_value_init_server
#define ZB_ZCL_CLUSTER_ID_BINARY_VALUE_CLIENT_ROLE_INIT zb_zcl_binary_value_init_client

#endif /* ZB_ZCL_BINARY_VALUE_H */
