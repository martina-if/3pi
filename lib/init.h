/*
 * =====================================================================================
 *
 *       Filename:  init.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/14/2010 04:50:14 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Martina Iglesias Fernández
 *
 * =====================================================================================
 */

#include <pololu/3pi.h>
#include <bargraph.h>

void display_battery();
void calibrate();

void display_battery()
{
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
}

void calibrate()
{
	unsigned int counter; // used as a simple timer
	unsigned int sensors[5]; // an array to hold sensor values

	// This must be called at the beginning of 3pi code, to set up the
	// sensors.  We use a value of 2000 for the timeout, which
	// corresponds to 2000*0.4 us = 0.8 ms on our 20 MHz processor.
	pololu_3pi_init(2000);

	delay_ms(1000);

	// Auto-calibration: turn right and left while calibrating the
	// sensors.
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
	set_motors(0,0);
	
	// Display calibrated values as a bar graph.
	while(!button_is_pressed(BUTTON_B))
	{
		// Read the sensor values and get the position measurement.
		unsigned int position = read_line(sensors,IR_EMITTERS_ON);

		// Display the position measurement, which will go from 0
		// (when the leftmost sensor is over the line) to 4000 (when
		// the rightmost sensor is over the line) on the 3pi, along
		// with a bar graph of the sensor readings.  This allows you
		// to make sure the robot is ready to go.
		clear();
		print_long(position);
		lcd_goto_xy(0,1);
		display_readings(sensors);

		delay_ms(100);
	}
	wait_for_button_release(BUTTON_B);

	clear();

}

