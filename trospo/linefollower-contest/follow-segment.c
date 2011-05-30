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
#include <stdio.h>
#include "follow-segment.h"
#include "turn.h"
//#define PID
//#define DEBUG

void send_int(unsigned int* i)
{
	char buffer[100];

	sprintf(buffer, "%04d %04d %04d %04d %04d %04d %04d %04d\n", i[0], i[1], i[2], 
			i[3], i[4], i[5], i[6], i[7]);

	serial_send_blocking(buffer, 40);
}


// This function decides which way to turn during the learning phase of
// maze solving.  It uses the variables found_left, found_straight, and
// found_right, which indicate whether there is an exit in each of the
// three directions, applying the "left hand on the wall" strategy.
char select_turn(int* tips, unsigned int tam, unsigned char found_left, unsigned char found_straight, unsigned char found_right)
{
	int i, dir = 0;
	unsigned int count_l = 0;
	unsigned int count_r = 0;

	if (tam < 4)
		return 'S';
		
	for (i = 0; i < tam; i++)
	{
		if (tips[i] == FORWARD)
		{
			dir = FORWARD;
			break;
		}
		else if (tips[i] == LEFT)
			count_l ++;
		else if (tips[i] == RIGHT)
			count_r ++;
	}

	if (dir != FORWARD)
	{
		if (count_r >= count_l)
			dir = RIGHT;
		else if (count_l > count_r)
			dir = LEFT;
	}

//	if (tam < 2)
//	{
//		dir = tips[0]; // The only one that I found
//		serial_send_blocking("1 -> ", 5);
//	}
//	else if (tam < 3)
//	{
//		dir = tips[1]; // The center one
//		serial_send_blocking("2 -> ", 5);
//	}
//	else
//	{
//		dir = tips[2]; // Don't know how much tips I found but follow the third, which should be correct.
//		serial_send_blocking("3+ -> ", 6);
//	}



	// Make a decision about how to turn.  The following code
	// implements a left-hand-on-the-wall strategy, where we always
	// turn as far to the left as possible.
	if (dir == RIGHT && found_right)
		return 'R';
	else if (dir == LEFT && found_left)
		return 'L';
	else
		return 'S';
	/*
	if (dir == LEFT)
	{
		if(found_left)
			return 'L';
		else if (found_straight) // If tip seems to be left but I didn't find left, it should be straight (but i read wrong the tip)
			return 'S';
	}

	if (dir == FORWARD)
	{
		if (found_straight)
			return 'S';
		else if (found_left)
			return 'L';
		else
			return 'R';
	}

	if (dir == RIGHT)
	{
		if (found_right)
			return 'R';
		else if (found_straight) // Wrong reading, must be FORWARD
			return 'S';
	}
	else
	{
		if (found_straight)
			return 'S';
		if (found_left)
			return 'L';
		if (found_right)
			return 'R';
	}
	return 'S';*/
}

int follow_segment()
{
	unsigned int sensors[8]; // an array to hold sensor values
	unsigned int slow = 50;

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


//		// If I lift the robot, it should stop the motors. When the 
//		// sensors are no longer near a surface, they will all read 1000,
//		// and the position given by read_line functions will be 3500.
//		//if (sensors[0] + sensors[1] + sensors[2] + sensors[3] + sensors[4]
//		//		+ sensors[5] + sensors[6] + sensors[7] == 7000)
//		if (sensors[0] + sensors[1] + sensors[2] + sensors[3] + sensors[4] + 
//				sensors[5] + sensors[6] + sensors[7] == 8000)
//		{
//			set_motors(0,0);
//			return STOP;
//		}

		// Look for tips
//		if (sensors[3] > 700 && sensors[0]> 700 && (sensors[2] + sensors[1]) < 700)
		if (sensors[0] > 800 || sensors[7] > 800)
			set_motors(40,40);

		if (sensors[3] > 700 && sensors[0]> 300 && ((sensors[1] + sensors[2]) < 600))
		{
			if (sensors[7] > 300 && ((sensors[5] + sensors[6]) < 600))
			{
				//    |
				// ---|---
				//  | | |
				//    |
				// Tip found! Drive forward in the next intersection
				//...
#ifdef DEBUG
				serial_send_blocking("tip forward\n", 12);
#endif
				play("<<c32");
				while(is_playing());
				return FORWARD;
			}
			else if (sensors[7] < 300)
			{
				//    |
				// ---|---
				//  | | 
				//    |
				// Tip found! Drive left in the next intersection
				// ...
#ifdef DEBUG
				serial_send_blocking("tip left\n", 9);
#endif
				play("<<c32");
				while(is_playing());
				return LEFT;
			}
		}
//		else if (sensors[4] > 700 && sensors[7] > 700 && (sensors[5] + sensors[6]) < 900)
		else if (sensors[4] > 700 && sensors[7] > 300 && ((sensors[5] + sensors[6]) < 600))
		{
			//    |
			// ---|---
			//    | |
			//    |
			// Tip found! Drive right in the next intersection
			//...
			// Found an intersection.
#ifdef DEBUG
			serial_send_blocking("tip right\n", 10);
#endif
			play("<<c32");
			while(is_playing());
			return RIGHT;
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
		if(position < 1000)
		{
			// We are far to the right of the line: turn left.

			// Set the right motor to 100 and the left motor to zero,
			// to do a sharp turn to the left.  Note that the maximum
			// value of either motor speed is 255, so we are driving
			// it at just about 40% of the max.
			set_motors(0, 170 - slow);

		}
		else if(position < 2000)
		{
			set_motors(60 - slow, 150 - slow);
		}
		else if (position < 3000)
		{
			set_motors(90 - slow, 150 - slow);
		}
		else if (position < 3300) // CENTER = 3500
		{
			set_motors(130 - slow, 150 - slow);
		}
		else if (position < 3700)
		{
			// We are somewhat close to being centered on the line:
			// drive straight.
			set_motors(160 - slow, 160 - slow);
		}
		else if (position < 4000)
		{
			set_motors(150 - slow, 130 - slow);
		}
		else if (position < 5000)
		{
			set_motors(150 - slow, 90 - slow);
		}
		else if (position < 6000)
		{
			set_motors(150 - slow, 60 - slow);
		}
		else // (position < 7000)
		{
			// We are far to the left of the line: turn right.
			set_motors(170 - slow, 0);
		}
#endif
//		return CONTINUE;

	}

}

void follow_til_interesection(int op)
{
#ifdef DEBUG
	serial_send_blocking("Til intersection ------\n", 24);
#endif

//	unsigned char found_left, found_straight, found_right;
	unsigned char found_left;
	unsigned char found_right;
	unsigned char found_straight;
	unsigned int sensors[8];
	int tips[100];
	tips[0] = op;
	unsigned int pos = 1;

	while (1)
	{
		qtr_read_line(sensors,QTR_EMITTERS_ON);
//		send_int(sensors);

		// Look for tips
//		if (sensors[3] > 700 && sensors[0]> 600 && (sensors[2] + sensors[1]) < 700)
		if (sensors[0] > 200 && ((sensors[1] + sensors[2]) < 600))
		{
			if (sensors[7] > 200 && ((sensors[5] + sensors[6]) < 600))
			{
				//    |
				// ---|---
				//  | | |
				//    |
				// Tip found! Drive forward in the next intersection
				//...
#ifdef DEBUG
				serial_send_blocking("-> tip forward\n", 15);
#endif
				play("<<c32");
				while(is_playing());
				tips[pos] = FORWARD;
				pos ++;
			}
			else if (sensors[7] < 150)
			{
				//    |
				// ---|---
				//  | | 
				//    |
				// Tip found! Drive left in the next intersection
				// ...
#ifdef DEBUG
				serial_send_blocking("-> tip left\n", 12);
#endif
				play("<<c32");
				while(is_playing());
				tips[pos] = LEFT;
				pos ++;
			}
		}
//		else if (sensors[4] > 700 && sensors[7] > 600 && (sensors[5] + sensors[6]) < 700)
		else if (sensors[7] > 200 && ((sensors[5] + sensors[6]) < 600))
		{
			//    |
			// ---|---
			//    | |
			//    |
			// Tip found! Drive right in the next intersection
			//...
			// Found an intersection.
//			set_motors(60,60);
#ifdef DEBUG
			serial_send_blocking("-> tip right\n", 13);
#endif
			play("<<c32");
			while(is_playing());
			tips[pos] = RIGHT;
			pos ++;
		}

		if ((sensors[1] > 900 && sensors[2] > 900 && sensors[0] > 900)  || (sensors[5] > 900 && sensors[6] > 900 && sensors[7] > 900))
		{
#ifdef DEBUG
			serial_send("\t---\n", 5);
#endif
			play("<a16");
			while(is_playing());
			break; // Found an intersection
		}

#ifdef DEBUG
		send_int(sensors);
#endif
		if (sensors[3] > (sensors[4] + 100))
			set_motors(42,50);
		else if(sensors[4] > (sensors[3] + 100))
			set_motors(50,42);
		else
			set_motors(50,50);
		
			
		/*
//		if (sensors[3] > 500 && sensors[4] > 500)
		if (sensors[3] + sensors[4] > 1600)
		{
			serial_send_blocking("^\n", 2);
			set_motors(40,40);
		}
		else if (sensors[2] > 180)
		{
			serial_send_blocking("<\n", 2);
			set_motors(33, 40);
		}
		else if (sensors[2] > 500)// || sensors[1] > 300)
		{
			serial_send_blocking("<\n", 2);
			set_motors(20, 40);
		}
		else if (sensors[5] > 180)
		{
			serial_send_blocking(">\n", 2);
			set_motors(40, 33);
		}
		else if (sensors[5] > 500)// || sensors[6] > 300)
		{
			serial_send_blocking(">\n", 2);
			set_motors(40, 20);
		}
		else
		{
			serial_send_blocking("~\n", 2);
//			serial_send_blocking("DEFAULT!\n", 9);
			set_motors(40,40);
		}
		*/
	}

	// Check for left and right exits.
	if(sensors[0] > 400)
		found_left = 1;
	if(sensors[7] > 400)
		found_right = 1;

	// Drive straight a bit more - this is enough to line up our
	// wheels with the intersection.
	set_motors(40,40);
	delay_ms(200);

	// Check for a straight exit.
	qtr_read_line(sensors, QTR_EMITTERS_ON);
	if(sensors[2] > 400 || sensors[3] > 400 || sensors[4] > 400 || sensors[5] > 400)
		found_straight = 1;


	// Intersection identification is complete.
	// If the maze has been solved, we can follow the existing
	// path.  Otherwise, we need to learn the solution.
	char dir = select_turn(tips, pos, found_left, found_straight, found_right);
#ifdef DEBUG
	serial_send_blocking(&dir, 1);
	serial_send_blocking("\n", 1);
#endif

	// Make the turn indicated by the path.
	turn(dir, 90);
	set_motors(50,50); // DEBUG
//	delay_ms(00);

#ifdef DEBUG
	serial_send_blocking("End intersection ------\n", 24);
#endif

}

