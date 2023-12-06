/*
 * shared_mem_handler.h
 *
 *  Created on: Oct 18, 2023
 *      Author: Bhargav-4836
 */

#ifndef INC_SHARED_MEM_HANDLER_H_
#define INC_SHARED_MEM_HANDLER_H_


#endif /* INC_SHARED_MEM_HANDLER_H_ */


typedef enum
{
	child_lock_enabled,
	child_lock_disabled,
	emergency_button_pressed,
	emergency_button_released,
	contact_charger_available,
	contact_charger_not_available,
	android_tab_connected,
	no_floor_press,
	ground_floor_pressed,
	first_floor_pressed,
	second_floor_pressed,
	third_floor_pressed,
	battery_state_good,
	battery_state_mid,
	battery_state_critical,
	siren_released,
	siren_pressed

}shared_mem_data;

void send_data_ringbuffer(shared_mem_data sh_dt);
void printshared();
