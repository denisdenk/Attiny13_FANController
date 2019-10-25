/*
* Attiny13_MotorController.c
*
* Created: 24.10.2019 16:28:06
* Author : Denis Denk
*/

/*
PB3 -> LED
PB4 -> Button+ Work time
PB0 -> Button+ Relax time
PB1 -> Button Change work mode
PB2 -> Output for motor
*/

// Private defines
#define F_CPU 960000UL

#define LED           PB3
#define BUTTON_WORK   PB4
#define BUTTON_RELX   PB0
#define BUTTON_CNGM   PB1
#define OUTPUT_MOTOR  PB2

// Includes
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

// Private variables
uint8_t work_time[4] = {0};
uint8_t relx_time[4] = {0};
uint8_t work_mode = 0;
uint8_t time_counter = 0;
uint8_t flag = 1;

// Private function prototypes
void work(uint8_t mode);
void blink_led(void);
void EEPROM_write(uint8_t uiAddress, uint8_t ucData);
uint8_t EEPROM_read(uint8_t uiAddress);

int main(void) {

	// Pin setup
	DDRB |= (1 << LED) | (1 << OUTPUT_MOTOR);
	DDRB &= ~((1<<BUTTON_WORK)|(1<<BUTTON_RELX)|(1<<BUTTON_CNGM));

	PORTB |= ((1 << BUTTON_WORK) | (1 << BUTTON_RELX) | (1 << BUTTON_CNGM));
	PORTB &= ~((1 << LED) | (1 << OUTPUT_MOTOR));
	
	// Read last work mode
	work_mode = EEPROM_read(0x00);
	
	// Read time parameters from EEPROM
	for(uint8_t i = 1; i <= 3; i++)  work_time[i] = EEPROM_read(i);
	for(uint8_t i = 1; i <= 3; i++)  relx_time[i] = EEPROM_read(i+3);
	
	// Indicate current work mode
	if(work_mode>=1 && work_mode<=3) {
		for(uint8_t i=0;i<(work_mode);i++) blink_led();
	}
	_delay_ms(2000);

	// turn LED on
	PORTB |= (1 << LED);

	for (;;) {
		
		// Change working time
		if (!(bit_is_set(PINB, BUTTON_WORK))) {
			if(work_mode>=1 && work_mode<=3) {
				work_time[work_mode]++;
				if (work_time[work_mode] > 30) work_time[work_mode] = 1;
				// Save new parameters to EEPROM
				EEPROM_write(work_mode, work_time[work_mode]);
				// Indicate current working time in seconds (Number of seconds = number of blink`s)
				for (uint8_t i = 0; i < work_time[work_mode]; i++) blink_led();
				_delay_ms(200);
			}
		}

		// Change relaxing time
		if (!(bit_is_set(PINB, BUTTON_RELX))) {
			if(work_mode>=1 && work_mode<=3) {
				relx_time[work_mode]++;
				if (relx_time[work_mode] > 30) relx_time[work_mode] = 1;
				// Save new parameters to EEPROM
				EEPROM_write(work_mode+3, relx_time[work_mode]);
				// Indicate current relaxing time in seconds (Number of seconds = number of blink`s)
				for (uint8_t i = 0; i < relx_time[work_mode]; i++) blink_led();
				_delay_ms(200);
			}
		}
		
		// Change work mode
		if (!(bit_is_set(PINB, BUTTON_CNGM))) {
			work_mode++;
			if (work_mode > 3) work_mode = 0;
			// Write current mode to EEPROM
			EEPROM_write(0x00, work_mode);
			// Indicate current work mode (work mode = number of blinks)
			if(work_mode>=1 && work_mode<=3) {
				for(uint8_t i=0;i<(work_mode);i++) blink_led();
			}
			_delay_ms(200);
		}
		work(work_mode);
	}
}


void work(uint8_t mode) {

	time_counter++;
	if(mode == 0) {
		PORTB &= ~(1<<OUTPUT_MOTOR);
		time_counter=0;
		} else {
		_delay_ms(250);
		if(flag == 1) {
			if((time_counter/4)<work_time[mode]) PORTB |= (1<<OUTPUT_MOTOR);
			else {
				flag = 0;
				time_counter = 0;
			}
		}
		else {
			if((time_counter/4)<relx_time[mode]) PORTB &= ~(1<<OUTPUT_MOTOR);
			else {
				flag = 1;
				time_counter = 0;
			}
		}
	}
	
}

void blink_led(void) {
	PORTB ^= (1 << LED);
	_delay_ms(250);
	PORTB ^= (1 << LED);
	_delay_ms(250);
}

void EEPROM_write(uint8_t uiAddress, uint8_t ucData) {
	while(EECR & (1<<EEPE));
	EEAR = uiAddress;
	EEDR = ucData;
	EECR |= (1<<EEMPE);
	EECR |= (1<<EEPE);
}

uint8_t EEPROM_read(uint8_t uiAddress) {
	while(EECR & (1<<EEWE));
	EEAR = uiAddress;
	EECR |= (1<<EERE);
	return EEDR;
}

