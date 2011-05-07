/*
 * =====================================================================================
 *
 *       Filename:  obstacle-avoid.c
 *
 *    Description:  3pi uses two prosimity sensors (Sharp GP2Y0D810Z0F Digital Distance 
 *    				Sensor 10cm) to avoid walls and other obstacles. They are connected 
 *    				to PD0 and PD1 pins.
 *    				Based ib 3pi-wall-follower.
 *
 *        Version:  1.0
 *        Created:  05/07/2011 07:40:29 PM
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
const char name_line1[] PROGMEM = "Wall";
const char name_line2[] PROGMEM = "Follower";

// A couple of simple tunes, stored in program space.
const char welcome[] PROGMEM = ">g32>>c32";
const char go[]      PROGMEM = "L16 cdegreg4";

// Refresh the LCD display every tenth of a second.
const int display_interval_ms = 100;


void initialize()
{
	//        // Set PC5 as an input with internal pull-up disabled
	//        DDRC  &= ~(1<< PORTC5);
	//        PORTC &= ~(1<< PORTC5);
	
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
	print("Go!");

	// Play music and wait for it to finish before we start driving.
	play_from_program_space(go);
	while(is_playing());
}

int main()
{
	// set up the 3pi
	initialize();

	// This is the "main loop" - it will run forever.
	set_motors(50,50);

	while(1)
	{
		if (!is_digital_input_high(IO_D0))
		{
			turn_right(50);
		}
		else if (!is_digital_input_high(IO_D1))
		{
			turn_left(50);
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

