/*
 * @file fds.c
 *
 * @date Jul 18, 2017
 * @author hamster
 *
 * Handler interface for the FDS storage system
 * This uses Nordic SDK routines to map some chunk of flash space into something
 * much more high level
 *
 */


#include "system.h"
#include "storage.h"

volatile STORAGE_FLAGS storageFlags;

/**
 * @brief Handles the callbacks for the storage events
 * @param p_fds_evt The event we need to handle
 */
void storage_evt_handler(fds_evt_t const * const p_fds_evt){

    switch (p_fds_evt->id){
        case FDS_EVT_INIT:
        	storageFlags.initComplete = true;
            if (p_fds_evt->result == FDS_SUCCESS){
            	storageFlags.initSuccess = true;
            	printf("EVT: Storage init success\n");
            }
            else{
            	storageFlags.initSuccess = false;
            	printf("EVT: Storage init failed\n");
            }
            break;
        case FDS_EVT_UPDATE:
        	storageFlags.updateComplete = true;
            if (p_fds_evt->result == FDS_SUCCESS){
            	storageFlags.updateSuccess = true;
            	printf("EVT: Storage update success\n");
            }
            else{
            	storageFlags.updateSuccess = false;
            	printf("EVT: Storage update failed\n");
            }
        	break;
		case FDS_EVT_WRITE:
			storageFlags.writeComplete = true;
			if (p_fds_evt->result == FDS_SUCCESS){
				storageFlags.writeSuccess = true;
				printf("EVT: Write Success\n");
			}
			else{
				storageFlags.writeSuccess = false;
				printf("EVT: Write failed\n");
			}
			break;
		case FDS_EVT_DEL_FILE:
			storageFlags.deleteComplete = true;
			if (p_fds_evt->result == FDS_SUCCESS){
				storageFlags.deleteSuccess = true;
				printf("EVT: Delete Success\n");
			}
			else{
				storageFlags.deleteSuccess = false;
				printf("EVT: Delete failed\n");
			}
			break;
		case FDS_EVT_GC:
			storageFlags.gcComplete = true;
			if (p_fds_evt->result == FDS_SUCCESS){
				storageFlags.gcComplete = true;
				printf("EVT: GC Success\n");
			}
			else{
				storageFlags.gcComplete = false;
				printf("EVT: GC failed\n");
			}
			break;
        default:
            break;
    }
}

/**
 * @brief Init the storage system
 * @return Result of the init attempt
 */
ret_code_t storage_init(void){

	storageFlags.initComplete = false;
	storageFlags.readComplete = false;
	storageFlags.writeComplete = false;
	storageFlags.updateComplete = false;
	storageFlags.deleteComplete = false;
	storageFlags.gcComplete = false;

	storageFlags.initSuccess = false;
	storageFlags.readSuccess = false;
	storageFlags.writeSuccess = false;
	storageFlags.updateSuccess = false;
	storageFlags.deleteSuccess = false;
	storageFlags.gcSuccess = false;

	ret_code_t ret = fds_register(storage_evt_handler);

	storageFlags.initComplete = false;
	storageFlags.initSuccess = false;

	if (ret != FDS_SUCCESS){
		printf("Storage register failed\n");
		return ret;
	}

	ret = fds_init();
	if (ret != FDS_SUCCESS){
		printf("Storage init failed\n");
			return ret;
	}

	while(!storageFlags.initComplete);

	printf("Storage init success\n");
	return storageFlags.initSuccess;
}

/**
 * @brief Write a string out to storage
 * @param file_id The file ID name
 * @param record_id The record ID
 * @param data[] The data to store
 * @param len Number of chars to store
 * @return Result of the storage attempt
 *
 * @note Blocks until the storage has completed, pass or fail
 */
ret_code_t storage_write(uint16_t file_id, uint16_t record_id, char *data, uint8_t len){

	fds_record_t        record;
	fds_record_desc_t   record_desc;
	fds_record_chunk_t  record_chunk;
	// Set up data.

	record_chunk.p_data         = data;
	record_chunk.length_words   = len;
	// Set up record.
	record.file_id              = file_id;
	record.key              	= record_id;
	record.data.p_chunks        = &record_chunk;
	record.data.num_chunks      = 1;

	storageFlags.writeComplete = false;
	storageFlags.writeSuccess = false;

	ret_code_t ret = fds_record_write(&record_desc, &record);
	if (ret != FDS_SUCCESS){
		printf("Write failed: %d\n", ret);
			return ret;
	}

	printf("Writing Record ID %d data %08X\n", record_desc.record_id, data);

	while(!storageFlags.writeComplete);
	return storageFlags.writeSuccess;
}

/**
 * @brief Read an int from storage
 * @param file_id File ID wanted
 * @param record_id Record ID wanted
 * @param data[] Where to write the result
 * @return Result of attempt
 *
 * @note Returns only the first found instance - it's possible to have more than one with the same file_id and record_id!
 */
ret_code_t storage_read_int(uint16_t file_id, uint16_t record_id, uint32_t *ret_data){

	fds_flash_record_t  flash_record;
	fds_record_desc_t   record_desc;
	fds_find_token_t    ftok ={0}; //Important, make sure you zero init the ftok token
	bool foundit = false;
	uint32_t *data;
	uint32_t err_code;

	printf("Storage: Searching\n");

	// Loop until all records with the given key and file ID have been found.
	if (fds_record_find(file_id, record_id, &record_desc, &ftok) == FDS_SUCCESS){

			err_code = fds_record_open(&record_desc, &flash_record);
			if ( err_code != FDS_SUCCESS){
				printf("Can't open record\n");
				return err_code;
			}

			printf("Found Record ID %d\n",record_desc.record_id);
			printf("Data ");
			data = (uint32_t *) flash_record.p_data;
			for (uint8_t i=0;i<flash_record.p_header->tl.length_words;i++){
				printf("0x%8x ",data[i]);
			}
			printf("\n");

			// Save the record data to ret_data
			memcpy(ret_data, flash_record.p_data, sizeof(uint32_t));

			// Access the record through the flash_record structure.
			// Close the record when done.
			err_code = fds_record_close(&record_desc);
			if (err_code != FDS_SUCCESS){
				return err_code;
			}

			return NRF_SUCCESS;
	}

	printf("Storage: Not found\n");

	return NRF_ERROR_NOT_FOUND;

}

/**
 * @brief Read a string from storage
 * @param file_id File ID wanted
 * @param record_id Record ID wanted
 * @param data[] Where to write the result
 * @param len Max bytes to return
 * @return Result of attempt
 *
 * @note Returns only the first found instance - it's possible to have more than one with the same file_id and record_id!
 */
ret_code_t storage_read_string(uint16_t file_id, uint16_t record_id, char *ret_data, uint8_t len){

	fds_flash_record_t  flash_record;
	fds_record_desc_t   record_desc;
	fds_find_token_t    ftok ={0}; //Important, make sure you zero init the ftok token
	bool foundit = false;
	char *data;
	uint32_t err_code;

	printf("Searching\n");

	// Loop until all records with the given key and file ID have been found.
	if (fds_record_find(file_id, record_id, &record_desc, &ftok) == FDS_SUCCESS){

			err_code = fds_record_open(&record_desc, &flash_record);
			if ( err_code != FDS_SUCCESS){
				printf("Can't open record\n");
				return err_code;
			}

			printf("Found Record ID %d\n",record_desc.record_id);
			data = (char *) flash_record.p_data;
			printf("Data: %s\n", data);

			memcpy(ret_data, flash_record.p_data, len);

			// Access the record through the flash_record structure.
			// Close the record when done.
			err_code = fds_record_close(&record_desc);
			if (err_code != FDS_SUCCESS){
				return err_code;
			}

			return NRF_SUCCESS;
	}

	return NRF_ERROR_NOT_FOUND;

}

/**
 * @brief Delete a record
 * @param file_id File ID to delete
 * @param record_id Record ID to delete
 * @return The result of the deletion
 */
ret_code_t storage_delete(uint16_t file_id, uint16_t record_id){

	fds_record_desc_t   record_desc;
	fds_find_token_t    ftok;

	storageFlags.deleteComplete = false;
	storageFlags.deleteSuccess = false;
	storageFlags.gcComplete = false;
	storageFlags.gcSuccess = false;

	ftok.page=0;
	ftok.p_addr=NULL;
	// Loop and find records with same ID and rec key and mark them as deleted.
	while (fds_record_find(file_id, record_id, &record_desc, &ftok) == FDS_SUCCESS){
		fds_record_delete(&record_desc);
		printf("Deleted record ID: %d \n",record_desc.record_id);
	}

	// Waiting for delete to complete doesn't work if there was nothing to delete - oops.
	//while(!storageFlags.deleteComplete);

	// call the garbage collector to empty the data
	ret_code_t ret = fds_gc();
	if (ret != FDS_SUCCESS){
			return ret;
	}

	while(!storageFlags.gcComplete);

	return NRF_SUCCESS;
}

