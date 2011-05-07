/*
 * =====================================================================================
 *
 *       Filename:  hole-avoid.c
 *
 *    Description:  3pi uses its line sensors to avoid holes in the floor, like when
 *    				approaching the stairs. It also works with black tape to simulate the
 *    				hole.
 *    				Based on 3pi-wall-follower.
 *
 *        Version:  1.0
 *        Created:  05/07/2011 07:36:55 PM
 *       Compiler:  gcc
 *
 *         Author:  Martina Iglesias, martina.iglesiasf @ gmail.com
 *
 *                  Copyright (c) 2011, Martina Iglesias
 * =====================================================================================
 */


// The 3pi include file must be at the beginning of any program that
// uses the Pololu AVR library and 3pi.
#include <pololu/3pi.h>
#include <pololu/orangutan.h>

// This include file allows data to be stored in program space.  The
// ATmegaxx8 has 16x more flash than RAM, so large
// pieces of static data should be stored in program space.
#include <avr/pgmspace.h>

#include "turn.h"

// Introductory messages.  The "PROGMEM" identifier causes the data to
// go into program space.
const char welcome_line1[] PROGMEM = " Pololu";
const char welcome_line2[] PROGMEM = "3\xf7 Robot";
const char name_line1[] PROGMEM = "Obstacle";
const char name_line2[] PROGMEM = "Avoider";

// A couple of simple tunes, stored in program space.
const char welcome[] PROGMEM = ">g32>>c32";
const char go[]      PROGMEM = "L16 cdegreg4";

// Refresh the LCD display every tenth of a second.
const int display_interval_ms = 100;

#define MS_ELAPSED_IS(n) (get_ms() % n == 0)
#define TIME_TO_DISPLAY (MS_ELAPSED_IS(display_interval_ms))

#define NUM_SENSORS	5
#define LEFT			0
#define LEFT_FRONT	1
#define FRONT			2
#define RIGHT_FRONT	3
#define RIGHT			4

#define SPEED			250
void initialize()
{
	// This must be called at the beginning of 3pi code, to set up the
	// sensors.  We use a value of 2000 for the timeout, which
	// corresponds to 2000*0.4 us = 0.8 ms on our 20 MHz processor.
	pololu_3pi_init(2000);

	// Play welcome music and display a message
	print_from_program_space(welcome_line1);
	lcd_goto_xy(0,1);
	print_from_program_space(welcome_line2);
	play_from_program_space(welcome);
	delay_ms(1000);

	clear();
	print_from_program_space(name_line1);
	lcd_goto_xy(0,1);
	print_from_program_space(name_line2);
	delay_ms(1000);

	// Display battery voltage and wait for button press
	while(!button_is_pressed(BUTTON_B))
	{
		clear();
		print_long(read_battery_millivolts());
		print("mV");
		lcd_goto_xy(0,1);
		print("Press B");
		delay_ms(100);
	}

	// Always wait for the button to be released so that 3pi doesn't
	// start moving until your hand is away from it.
	wait_for_button_release(BUTTON_B);
	clear();
	/*
	// Auto-calibration: turn right and left while calibrating the
	// sensors.
	// It is important to calibrate the sensors with both floor and
	// hole, otherwise 3pi would think all time it is near a hole.
	int counter;
	for(counter=0;counter<80;counter++)
	{
		if(counter < 20 || counter >= 60)
			set_motors(40,-40);
		else
			set_motors(-40,40);

		// This function records a set of sensor readings and keeps
		// track of the minimum and maximum values encountered.  The
		// IR_EMITTERS_ON argument means that the IR LEDs will be
		// turned on during the reading, which is usually what you
		// want.
		calibrate_line_sensors(IR_EMITTERS_ON);

		// Since our counter runs to 80, the total delay will be
		// 80*20 = 1600 ms.
		delay_ms(20);
	}
	*/
	set_motors(0,0);
	delay_ms(1000);

	print("Go!");

	// Play music and wait for it to finish before we start driving.
	play_from_program_space(go);
	while(is_playing());
}

void back_up()
{
	if (TIME_TO_DISPLAY)
	{
		clear();
		lcd_goto_xy(0,0);
		print("Uuuuuh..");
	}

	// Back up slightly to the left
	set_motors(-30,-30);
	delay_ms(260);
//	unsigned int sensors[5];
//	read_line(sensors, IR_EMITTERS_ON);
//	while(sensors[FRONT] > 900)
//		read_line(sensors, IR_EMITTERS_ON);
	turn('R', 135);
	clear();
	set_motors(SPEED, SPEED);

}

int any_greater(unsigned int* sensors, unsigned int val)
{
	int i;
	for (i=0; i<NUM_SENSORS; i++)
		if (sensors[i] > val)
			return 1;
	return 0;
}
	
int main()
{
	// set up the 3pi
	initialize();
	unsigned int sensors[5];

	// This is the "main loop" - it will run forever.
	set_motors(SPEED, SPEED);

	while(1)
	{
		// Next lines are for obstacle detection-avoidance.
		if (!is_digital_input_high(IO_D0))
		{
			turn_right(SPEED);
		}
		else if (!is_digital_input_high(IO_D1))
		{
			turn_left(SPEED);
		}

		read_line_sensors(sensors, IR_EMITTERS_ON);

//		if (sensors[FRONT] > 990)
//		{
//			set_motors(0,0);
//			back_up();
//		}
		if (any_greater(sensors, 1900))
		{
			set_motors(0,0);
			back_up();
		}

	}

	// This part of the code is never reached.  A robot should
	// never reach the end of its program, or unpredictable behavior
	// will result as random code starts getting executed.  If you
	// really want to stop all actions at some point, set your motors
	// to 0,0 and run the following command to loop forever:
	//
	set_motors(0,0);
	while(1);
}

