/*
 * ow_cmd.h
 *
 * Created: 02-06-2019 17:35:59
 *  Author: dorspi
 */ 


#ifndef OW_CMD_H_
#define OW_CMD_H_


typedef struct {
	uint8_t cmd;
	uint16_t m_data_len;
	uint16_t s_data_len;
} ow_cmd_t;


#endif /* OW_CMD_H_ */