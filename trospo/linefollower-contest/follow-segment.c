/*
 * =====================================================================================
 *
 *       Filename:  follow-segment.c
 *
 *    Description:  Follow a segment untill an intersection is found.
 *
 *        Version:  1.0
 *        Created:  05/08/2011 11:08:51 PM
 *       Compiler:  gcc
 *
 *         Author:  Martina Iglesias, martina.iglesiasf @ gmail.com
 *
 *                  Copyright (c) 2011, Martina Iglesias
 * =====================================================================================
 */


#include <pololu/orangutan.h>


void follow_segment()
{
	int last_proportional = 0;
	unsigned int counter = 0;
	unsigned int sensors[8]; // an array to hold sensor values

	while(1)
	{

		// Get the position of the line.  Note that we *must* provide
		// the "sensors" argument to read_line() here, even though we
		// are not interested in the individual sensor readings.
		unsigned int position = qtr_read_line(sensors,QTR_EMITTERS_ON);

		// The "proportional" term should be 0 when we are on the line.
		int proportional = ((int)position) - 3590;

		// Compute the derivative (change) and integral (sum) of the
		// position.
		int derivative = proportional - last_proportional;

		// Remember the last position.
		last_proportional = proportional;

		// Compute the difference between the two motor power settings,
		// m1 - m2.  If this is a positive number the robot will turn
		// to the right.  If it is a negative number, the robot will
		// turn to the left, and the magnitude of the number determines
		// the sharpness of the turn.
		int power_difference = proportional/50 + derivative*2;
		if (counter >= 100)
		{
			char command = 't';
			serial_send(&command, 1);					// Char
			serial_send((char*)&position, 2);			// unsigned int
			serial_send((char*)&proportional, 2);		// int
			serial_send((char*)&derivative, 2);		// int
			serial_send((char*)&power_difference, 2);	// int

			counter = 0;
		}

		// Compute the actual motor settings.  We never set either motor
		// to a negative value.
		const int max = 60;
		if(power_difference > max)
			power_difference = max;
		if(power_difference < -max)
			power_difference = -max;

		if(power_difference < 0)
			set_motors(max+power_difference, max);
		else
			set_motors(max, max-power_difference);

		counter++;

		// We use the inner three sensors (1, 2, and 3) for
		// determining whether there is a line straight ahead, and the
		// sensors 0 and 4 for detecting lines going to the left and
		// right.

		if(sensors[0] > 700 || sensors[7] > 700)
		{
			// Found an intersection.
			return;
		}

	}
}

