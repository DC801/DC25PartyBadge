/*
 * @file ble.h
 *
 * @date Jul 19, 2017
 * @author hamster
 *
 */

#ifndef BLE_H_
#define BLE_H_

/**
 * @brief Variable length data encapsulation in terms of length and pointer to data.
 */
typedef struct
{
    uint8_t  * p_data;   /**< Pointer to data. */
    uint16_t   data_len; /**< Length of data. */
} data_t;

typedef struct{
	uint16_t appearance;
	uint16_t manu;
	uint8_t manu_data[10];
	char *short_name;
	char *long_name;
} ADVERTISEMENT;

void advertising_init(void);
void advertising_start(void);
void ble_stack_init(void);
void gap_params_init(void);
void gatt_init(void);
uint32_t adv_report_parse(uint8_t type, data_t * p_advdata, data_t * p_typedata);
void scan_start(void);
bool ble_evt_is_advertising_timeout(ble_evt_t * p_ble_evt);
void on_ble_peripheral_evt(ble_evt_t * p_ble_evt);
void ble_evt_dispatch(ble_evt_t * p_ble_evt);
void sys_evt_dispatch(uint32_t sys_evt);
void advertising_setVector(char *vector);
void advertising_setUser(char *user);
void advertising_setDieRoll(uint8_t die);
void handle801Badge(unsigned char *data);
void advertising_setFlag(int flag);

void parseAdvertisementData(uint8_t *data, uint8_t len, ADVERTISEMENT *adv);

char * getManuNameString(uint16_t group);

// Not all of these IDs are registered with the bluetooth standard
#define MANU_CRYPTO						0x0C97
#define MANU_QUEERCON					0x04D3
#define MANU_DC801						0x0801
#define MANU_DC503						0x0503
#define MANU_BENDER						0x049E
#define MANU_B_D						0xFFFE
#define MANU_DCZIA						0x5050
#define MANU_DCZIA_SHOTBOT				0x5051

#define APP_COMPANY_IDENTIFIER_DC801    MANU_DC801
#define APP_APPEARANCE_TYPE_DC25        0x19DC

#define BLOCK_TYPE_APPEARANCE	0x19
#define BLOCK_TYPE_MANU_INFO	0xFF
#define BLOCK_TYPE_LONG_NAME	0x09
#define BLOCK_TYPE_SHORT_NAME	0x08

#define COMMAND_PARTY	0x0142
#define COMMAND_RESET	0x0011
#define COMMAND_NONE	0x0000

#define COMMAND_KEYWORD "HELGABAH"

#define CENTRAL_LINK_COUNT              0
#define PERIPHERAL_LINK_COUNT           1
#define APP_CONN_CFG_TAG                1

#define DEVICE_NAME                     "DC801   "                 				/**< Name of device. Will be included in the advertising data. */
#define APP_CFG_NON_CONN_ADV_TIMEOUT    0                                 		/**< Time for which the device must be advertising in
																					non-connectable mode (in seconds). 0 disables timeout. */
#define NON_CONNECTABLE_ADV_INTERVAL    MSEC_TO_UNITS(100, UNIT_0_625_MS) 		/**< The advertising interval for non-connectable advertisement (100 ms).
																					This value can vary between 100ms to 10.24s). */

#define SCAN_INTERVAL                   MSEC_TO_UNITS(100, UNIT_0_625_MS)    	/**< Determines scan interval in units of 0.625 millisecond. */
#define SCAN_WINDOW                     MSEC_TO_UNITS(50, UNIT_0_625_MS)    	/**< Determines scan window in units of 0.625 millisecond. */
#define SCAN_TIMEOUT                    0

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(100, UNIT_1_25_MS)        /**< Minimum acceptable connection interval (0.1 seconds). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(200, UNIT_1_25_MS)        /**< Maximum acceptable connection interval (0.2 second). */
#define SLAVE_LATENCY                   0                                       /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)         /**< Connection supervisory timeout (4 seconds). */

#define ADV_INTERVAL    				MSEC_TO_UNITS(500, UNIT_0_625_MS)		/**< How often to advertise.  500ms to not blast too often */


#endif /* BLE_H_ */
