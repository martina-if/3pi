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
#define PID

void follow_segment()
{
	unsigned int sensors[8]; // an array to hold sensor values
//	unsigned int counter = 0;

#ifdef PID
	int last_proportional = 0;
	long integral=0;
#endif

	while(1)
	{

		// Get the position of the line.  Note that we *must* provide
		// the "sensors" argument to read_line() here, even though we
		// are not interested in the individual sensor readings.
		unsigned int position = qtr_read_line(sensors,QTR_EMITTERS_ON);

		// Look for tips
		if (sensors[3] > 700 && sensors[0]> 700 && (sensors[2] + sensors[1]) < 800)
		{
			if (sensors[4] > 700 && sensors[7] > 700 && (sensors[5] + sensors[6]) < 800)
			{
				//    |
				// ---|---
				//  | | |
				//    |
				// Tip found! Drive forward in the next intersection
				//...
				set_motors(60,60);
				serial_send_blocking("tip forward\n", 12);
				play("<<c32");
				while(is_playing());
			}
			else
			{
				//    |
				// ---|---
				//  | | 
				//    |
				// Tip found! Drive left in the next intersection
				// ...
				set_motors(60,60);
				serial_send_blocking("tip left\n", 9);
				play("c32");
				while(is_playing());
			}
			return;
		}
		else if (sensors[4] > 700 && sensors[7] > 700 && (sensors[5] + sensors[6]) < 800)
		{
			//    |
			// ---|---
			//    | |
			//    |
			// Tip found! Drive right in the next intersection
			//...
			// Found an intersection.
			set_motors(60,60);
			serial_send_blocking("tip right\n", 10);
			play("<c32");
			while(is_playing());
			return;
		}

#ifdef PID	
		// The "proportional" term should be 0 when we are on the line.
		int proportional = ((int)position) - 3590;

		// Compute the derivative (change) and integral (sum) of the
		// position.
		int derivative = proportional - last_proportional;
		integral += proportional;

		// Remember the last position.
		last_proportional = proportional;

		// Compute the difference between the two motor power settings,
		// m1 - m2.  If this is a positive number the robot will turn
		// to the right.  If it is a negative number, the robot will
		// turn to the left, and the magnitude of the number determines
		// the sharpness of the turn.
		int power_difference = proportional/40 + derivative*2;
//		if (counter >= 100)
//		{
//			char command = 't';
//			serial_send_blocking(&command, 1);					// Char
//			serial_send_blocking((char*)&position, 2);			// unsigned int
//			serial_send_blocking((char*)&proportional, 2);		// int
//			serial_send_blocking((char*)&integral, 2);			// long (2 bytes?)
//			serial_send_blocking((char*)&derivative, 2);		// int
//			serial_send_blocking((char*)&power_difference, 2);	// int
//
//			counter = 0;
//		}

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

//		counter++;



#else
		// Get the position of the line.  Note that we *must* provide
		// the "sensors" argument to read_line() here, even though we
		// are not interested in the individual sensor readings.
		unsigned int position = qtr_read_line(sensors,QTR_EMITTERS_ON);

		if(position < 2500)
		{
			// We are far to the right of the line: turn left.

			// Set the right motor to 100 and the left motor to zero,
			// to do a sharp turn to the left.  Note that the maximum
			// value of either motor speed is 255, so we are driving
			// it at just about 40% of the max.
			set_motors(0,60);

		}
		else if(position < 3000)
		{
			set_motors(40,60);
		}
		else if (position < 4000)
		{
			// We are somewhat close to being centered on the line:
			// drive straight.
			set_motors(60,60);
		}
		else if (position < 4500)
		{
			set_motors(60,40);
		}
		else
		{
			// We are far to the left of the line: turn right.
			set_motors(60,0);
		}
#endif

	}
}

