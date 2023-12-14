/*
 * uart_fsm.c
 *
 *  Created on: Nov 18, 2022
 *      Author: DELL
 */


#include "uart_fsm.h"

//use type !RST# to ask for ADC value
// => stm32 read ADC_value  => return value !ADC#
// after that if user doesnt type !OK# (timeout 3000ms)
// stm32 continue to send !ADC#
enum COMMAND_PARSER_STATE { INIT			, RECEIVED_EXCLAMATION_MARK	, RECEIVED_R,
							RECEIVED_S		, RECEIVED_T				, RECEIVED_O,
							RECEIVED_K		, RECEIVED_HASHTAG_RST		, RECEIVED_HASHTAG_OK };

enum COMMUNICATION_STATE  {	INIT_			, SEND_ADC };

enum COMMAND_PARSER_STATE command_parser_state;
enum COMMUNICATION_STATE  communication_state;

//uint8_t temp;
uint8_t buffer[MAX_BUFFER_SIZE];
uint8_t previous_index_buffer;
uint8_t index_buffer;
uint8_t buffer_flag, command_flag, command_data;
uint8_t isSendADC_flag, isReadADC_flag;

uint32_t ADC_VALUE;

void uart_fsm_init(void){
	index_buffer = previous_index_buffer = 0;
	buffer_flag = command_flag = command_data = 0;
	isSendADC_flag = isReadADC_flag = 0;
	ADC_VALUE = 0;
	command_parser_state = INIT;
	communication_state = INIT_;
}



void command_parser_fsm(void){
	switch(command_parser_state){
	case INIT:
		command_flag = command_data = 0;
		if(buffer[previous_index_buffer] == '!')
			command_parser_state = RECEIVED_EXCLAMATION_MARK;

		break;
	case RECEIVED_EXCLAMATION_MARK:
		if(buffer[previous_index_buffer] == 'R')
			command_parser_state = RECEIVED_R;
		else if(buffer[previous_index_buffer] == 'O')
			command_parser_state = RECEIVED_O;
		else command_parser_state = INIT;

		break;
	case RECEIVED_R:
		if(buffer[previous_index_buffer] == 'S')
			command_parser_state = RECEIVED_S;
		else command_parser_state = INIT;

		break;
	case RECEIVED_S:
		if(buffer[previous_index_buffer] == 'T')
			command_parser_state = RECEIVED_T;
		else command_parser_state = INIT;

		break;
	case RECEIVED_T:
		if(buffer[previous_index_buffer] == '#'){
			command_flag = 1;
			HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		}
		command_parser_state = INIT;

		break;
	case RECEIVED_O:
		if(buffer[previous_index_buffer] == 'K')
			command_parser_state = RECEIVED_K;
		else command_parser_state = INIT;

		break;
	case RECEIVED_K:
		if(buffer[previous_index_buffer] == '#'){
			command_data = 1;
			HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		}

		command_parser_state = INIT;

		break;
	default:
		break;
	}
}


void uart_communication_fsm(void){
	switch(communication_state){
	case INIT_:
		isSendADC_flag = isReadADC_flag = 0;
		if(command_flag == 1) {
			isReadADC_flag = 1;
			communication_state = SEND_ADC;
		}
		break;
	case SEND_ADC:
		isSendADC_flag = 1;
		if(command_data == 1) communication_state = INIT_;
		break;
	default:
		break;
	}
}


