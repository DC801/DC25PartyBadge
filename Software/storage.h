/*
 * @file fds.h
 *
 * @date Jul 18, 2017
 * @author hamster
 *
 */

#ifndef STORAGE_H_
#define STORAGE_H_

#include "fds.h"
#include "fstorage.h"

ret_code_t storage_init(void);
void storage_evt_handler(fds_evt_t const * const p_fds_evt);
ret_code_t storage_read_int(uint16_t file_id, uint16_t record_id, uint32_t *ret_data);
ret_code_t storage_read_string(uint16_t file_id, uint16_t record_id, char *ret_data, uint8_t len);
ret_code_t storage_write(uint16_t file_id, uint16_t record_id, char *data, uint8_t len);
ret_code_t storage_delete (uint16_t file_id, uint16_t record_id);

//  Doesn't matter what these numbers are, just so they are consistent
#define FILE_CONFIGURATION	0x1111
#define RECORD_CONFIGURED	0x1111
#define RECORD_USERNAME		0x1112
#define RECORD_VECTOR		0x1113

typedef struct{
	bool initComplete;
	bool readComplete;
	bool writeComplete;
	bool updateComplete;
	bool deleteComplete;
	bool gcComplete;

	bool initSuccess;
	bool readSuccess;
	bool writeSuccess;
	bool updateSuccess;
	bool deleteSuccess;
	bool gcSuccess;
} STORAGE_FLAGS;

extern volatile STORAGE_FLAGS storageFlags;


#endif /* STORAGE_H_ */
