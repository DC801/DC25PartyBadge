/*
 * @file ble.c
 *
 * @date Jul 19, 2017
 * @author hamster
 *
 * Bluetooth configuration and data handling
 *
 */


#include "system.h"
#include "ble.h"

#include "ble_advdata.h"
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "bsp.h"
#include "ble_conn_state.h"
#include "ble_conn_params.h"
#include "ble_db_discovery.h"
#include "peer_manager.h"
#include "app_timer.h"
#include "ble_advertising.h"
#include "nrf_ble_gatt.h"

static ble_gap_adv_params_t m_adv_params;       /**< Parameters to be passed to the
													 stack when starting advertising. */
static nrf_ble_gatt_t m_gatt;                   /**< GATT module instance. */


// This is the data we stuff into the manufacturer block after the ID
// The first 8 bytes is the ASCII Offensive Vector
// The new two blocks differ depending on the OV:
// - If the OV is COMMAND_KEYWORD, then bytes 9 and 10 are a command
// - Otherwise, byte 9 is ignored, and byte 10 is the random number
static uint8_t m_dc801_beacon_info[10] =
{
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00
};

/**
 * @brief Parameters used when scanning.
 */
static ble_gap_scan_params_t const m_scan_params =
{
    .active   = 1,
    .interval = SCAN_INTERVAL,
    .window   = SCAN_WINDOW,
    .timeout  = SCAN_TIMEOUT,
    #if (NRF_SD_BLE_API_VERSION <= 2)
        .selective   = 0,
        .p_whitelist = NULL,
    #endif
    #if (NRF_SD_BLE_API_VERSION >= 3)
        .use_whitelist = 0,
    #endif
};


/**
 * @brief Function for initiating scanning.
 */
void scan_start(void){

    sd_ble_gap_scan_stop();
    sd_ble_gap_scan_start(&m_scan_params);

}


/**
 * @brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details This function is called from the scheduler in the main loop after a BLE stack event has
 * been received.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
void ble_evt_dispatch(ble_evt_t * p_ble_evt){

	static ADVERTISEMENT adv = {0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, NULL, NULL};

    ble_conn_state_on_ble_evt(p_ble_evt);

    // Did we get an advertising message?
    if (p_ble_evt->header.evt_id == BLE_GAP_EVT_ADV_REPORT){

        const ble_gap_evt_t   * const p_gap_evt = &p_ble_evt->evt.gap_evt;

        // Was the advertisement more than 0 bytes long?
    	if(p_ble_evt->evt.gap_evt.params.adv_report.dlen > 0){

//				printf("adv report: %d bytes: ", p_gap_evt->params.adv_report.dlen);
//				for(int i = 0; i < p_gap_evt->params.adv_report.dlen; i++){
//					printf("%02X ", p_gap_evt->params.adv_report.data[i]);
//				}
//				printf("\n");

    		// Grab a copy of it and pass that to be parsed
			uint8_t data[BLE_GAP_ADV_MAX_SIZE];
			memcpy(data, p_gap_evt->params.adv_report.data, BLE_GAP_ADV_MAX_SIZE);

//			printf("%s\n", p_gap_evt->params.adv_report.scan_rsp ? "Response" : "");

			if(!p_gap_evt->params.adv_report.scan_rsp){
				// This is not a scan response, so clear the struct
				memset(&adv, 0, sizeof(adv));
			}

			parseAdvertisementData(data, p_gap_evt->params.adv_report.dlen, &adv);


			// Is this a DC25 badge?
			if(adv.appearance == APP_APPEARANCE_TYPE_DC25){
				// Yes
//				printf("Adding seen type %04X\n", adv.manu);
				addSeenManufacturer(adv.manu);

				// Is this a dc801 badge?
				// Appearance should have been set by the time we get here for DC801 badges
				if(adv.manu == MANU_DC801  && adv.appearance == APP_APPEARANCE_TYPE_DC25 ){
					// Sweet jesus, it's a 801 badge!
//					printf("801 handle!\n");
					handle801Badge(adv.manu_data);
				}

			}

    	}

    }


}



/**
 * @brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
void gap_params_init(void){

    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    // Set the connection mode
    // We're not actually offering any services, so this will
    // just trip up anyone connecting to us, for funsies.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    // Set a default device short name
    // The app will probably override this pretty quickly
    sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *)DEVICE_NAME, strlen(DEVICE_NAME));

    // Set our appearance as a DC25 badge
    sd_ble_gap_appearance_set(APP_APPEARANCE_TYPE_DC25);

    // Finally, setup the gap parameters for broadcast interval
    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    sd_ble_gap_ppcp_set(&gap_conn_params);

}


/**
 * @brief Function for initializing the Advertising functionality.
 *
 * @details Encodes the required advertising data and passes it to the stack.
 *          Also builds a structure to be passed to the stack when starting advertising.
 */
void advertising_init(void){
	ble_advdata_t advdata;
	uint8_t       flags = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED | BLE_GAP_ADV_FLAG_LE_GENERAL_DISC_MODE;

	// Set the manufacturer data to be a DC801 badge
	ble_advdata_manuf_data_t manuf_specific_data;
	manuf_specific_data.company_identifier = APP_COMPANY_IDENTIFIER_DC801;
	manuf_specific_data.data.p_data = (uint8_t *) m_dc801_beacon_info;
	manuf_specific_data.data.size   = 10;

	// Build and set advertising data.
	memset(&advdata, 0, sizeof(advdata));
	advdata.name_type             = BLE_ADVDATA_FULL_NAME;
	advdata.short_name_len        = 8;
	advdata.include_appearance    = true;
	advdata.flags                 = flags;
	advdata.p_manuf_specific_data = &manuf_specific_data;

	ble_advdata_set(&advdata, NULL);

}


/**
 * @brief Function for starting advertising.
*/
void advertising_start(void){


	// Initialize advertising parameters (used when starting advertising).
	memset(&m_adv_params, 0, sizeof(m_adv_params));

	m_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_NONCONN_IND;
	m_adv_params.p_peer_addr = NULL;                             // Undirected advertisement.
	m_adv_params.fp          = BLE_GAP_ADV_FP_ANY;
	m_adv_params.interval    = ADV_INTERVAL;
	m_adv_params.timeout     = APP_CFG_NON_CONN_ADV_TIMEOUT;

	sd_ble_gap_adv_start(&m_adv_params, BLE_CONN_CFG_TAG_DEFAULT);

	bsp_indication_set(BSP_INDICATE_ADVERTISING);

}


/**
 * @brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
void ble_stack_init(void){

	// Very important, set the clock source.  We're using the internal RC clock
    nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;

    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);

    // Fetch the starting address of the application ram. This is needed by the upcoming SoftDevice calls.
    uint32_t ram_start = 0;
    softdevice_app_ram_start_get(&ram_start);

    // Overwrite some of the default configurations for the BLE stack.
    ble_cfg_t ble_cfg;

    // Configure the number of custom UUIDS.
    memset(&ble_cfg, 0, sizeof(ble_cfg));
    ble_cfg.common_cfg.vs_uuid_cfg.vs_uuid_count = 0;
    sd_ble_cfg_set(BLE_COMMON_CFG_VS_UUID, &ble_cfg, ram_start);

    // Configure the maximum number of connections, which is 0 since we are just a periph
    memset(&ble_cfg, 0, sizeof(ble_cfg));
    ble_cfg.gap_cfg.role_count_cfg.periph_role_count  = BLE_GAP_ROLE_COUNT_PERIPH_DEFAULT;
    ble_cfg.gap_cfg.role_count_cfg.central_role_count = 0;
    ble_cfg.gap_cfg.role_count_cfg.central_sec_count  = 0;
    sd_ble_cfg_set(BLE_GAP_CFG_ROLE_COUNT, &ble_cfg, ram_start);

    // Enable BLE stack.
    softdevice_enable(&ram_start);

    // Register with the SoftDevice handler module for BLE events.
    softdevice_ble_evt_handler_set(ble_evt_dispatch);

    // Register for system events
    softdevice_sys_evt_handler_set(sys_evt_dispatch);

}

/**
 * @brief Dispatch any system events
 */
void sys_evt_dispatch(uint32_t sys_evt){
	// Dispatch the system event to the fstorage module, where it will be
	// dispatched to the Flash Data Storage (FDS) module.
	fs_sys_event_handler(sys_evt);

}

/**
 * @brief Parse an advertisement packet
 */
void parseAdvertisementData(uint8_t *data, uint8_t len, ADVERTISEMENT *adv){

	// Parse the data
	// The data is structured in blocks, each block starts with
	// the number of bytes in the block, then the block type, then the block data

	uint8_t data_left = len;
	uint8_t loc = 0;

	while(data_left > 0){

//		printf("Block %02X is %d bytes: ", data[loc + 1], data[loc]);
//		for(int i = 0; i < data[loc]; i++){
//			printf("%02X ", data[loc + 1 + i]);
//		}
//		printf("\n");

		uint8_t block_type = 0;
		uint8_t block_data[BLE_GAP_ADV_MAX_SIZE];
		memset(block_data, 0, sizeof(block_data));

		uint8_t block_size = data[loc++];

		if(block_size > data_left){
			// Block is bigger than data left, that's not good
//			printf("Block %02X of size %d exceeds data left of %d\n", data[loc], block_size, data_left);
			data_left = 0;
			break;
		}

		uint8_t j = 0;
		for(int i = 0; i < block_size; i++){
			if(i == 0){
				block_type = data[loc];
			}
			else{
				block_data[j++] = data[loc+i];
			}
		}

		loc = loc + block_size;

		data_left = data_left - block_size - 1;

//		printf("Bytes left: %d\n", data_left);

		// Figure out what to do with the block
		switch(block_type){
			case BLOCK_TYPE_APPEARANCE:
			{
				uint16_t id;
				id = ((uint16_t)block_data[1] << 8) | block_data[0];
				if(id == APP_APPEARANCE_TYPE_DC25){
//					printf("Hey a DC25 badge\n");
				}
				else{
//					printf("Appearance %02X\n", id);
				}
				adv->appearance = id;
				break;
			}
			case BLOCK_TYPE_MANU_INFO:
			{
				uint16_t id = ((uint16_t)block_data[1] << 8) | block_data[0];
//				printf("Manu is %02X\n", id);
				adv->manu = id;

				// Save 10 bytes of the manu data if it is a DC801 badge
				if(id == MANU_DC801 && block_size == 13){
					memcpy(adv->manu_data, &block_data[2], 10);
				}

				break;
			}
			case BLOCK_TYPE_LONG_NAME:
			{
//				printf("Long name: %s\n", (char *)block_data);
				adv->long_name = (char *)block_data;
				break;
			}
			case BLOCK_TYPE_SHORT_NAME:
			{
//				printf("Short name: %s\n", (char*)block_data);
				adv->short_name = (char *)block_data;
				break;
			}
		}

	}

//	printf("\n");

}

/**
 * @brief Update the advertised long name
 */
void advertising_setUser(char *user){

    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    // User name is always 8 chars
    sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *)user, 8);

}

/**
 * @brief Update the vector name broadcast
 */
void advertising_setVector(char *vector){

	// Vector is always 8 chars
	for(int i = 0; i < 8; i++){
		m_dc801_beacon_info[i] = vector[i];
	}
	advertising_init();

}

/**
 * @brief Update the die roll value
 */
void advertising_setDieRoll(uint8_t die){

	m_dc801_beacon_info[9] = die;
	advertising_init();

}

/**
 * @brief Set the command flags
 */
void advertising_setFlag(int flag){

	switch(flag){
		case COMMAND_NONE:
			m_dc801_beacon_info[8] = 0x00;
			m_dc801_beacon_info[9] = 0x00;
			advertising_init();
			break;
		case COMMAND_PARTY:
			m_dc801_beacon_info[8] = (COMMAND_PARTY >> 8) & 0xFF;
			m_dc801_beacon_info[9] = COMMAND_PARTY & 0xFF;
			advertising_init();
			break;
		case COMMAND_RESET:
			m_dc801_beacon_info[8] = (COMMAND_RESET >> 8) & 0xFF;
			m_dc801_beacon_info[9] = COMMAND_RESET & 0xFF;
			advertising_init();
			break;
	}

}

///**
// * @brief Get a friendly string for a badge type
// * @param manu The manufacturer name
// * @return A pointer to a string
// * @note Just for debugging
// */
//char * getManuNameString(uint16_t manu){
//
//	switch(manu){
//		case MANU_CRYPTO:
//			return "Crypto Village";
//			break;
//		case MANU_QUEERCON:
//			return "Queercon";
//			break;
//		case MANU_DC801:
//			return "DC801";
//			break;
//		case MANU_DC503:
//			return "DC503";
//			break;
//		case MANU_BENDER:
//			return "Bender";
//			break;
//		case MANU_B_D:
//			return "B&D";
//			break;
//		case MANU_DCZIA:
//			return "DCZIA";
//			break;
//		case MANU_DCZIA_SHOTBOT:
//			return "DCZIA Shotbot";
//			break;
//		default:
//			return "";
//			break;
//	}
//
//}

/**
 * @brief Handle the data sent from an 801 badge
 * @param data[] The extra data in the manufacturer block
 */
void handle801Badge(unsigned char *data){

	char vector[9];
	uint8_t flags[2];

	// Get the vector name, null terminate
	memcpy(vector, data, 8);
	vector[8] = '\0';

	// Get the flags/command
	flags[0] = data[8];
	flags[1] = data[9];

	//printf("Vector is %s, flags %02X %02X\n", vector, flags[0], flags[1]);

	// Figure out if this is a command or a game update
	if((strncmp(vector, COMMAND_KEYWORD, 8)) == 0){
		// This is a command
		//printf("Command received\n");
		updateCommandData(flags[1] | (flags[0] << 8));
	}
	else{
		// This is game data
		//printf("Game data: '%s', %d\n", vector, flags[1]);
		updateGameData(vector, flags[1]);
	}

}

