/*
 * =====================================================================================
 *
 *       Filename:  linefollower-pid.c
 *
 *    Description:  Linefollower code for trospo. Based on 3pi-linefollower.
 *    				It uses the following pin configuration
 *    				PC0:	QTR sensor 1
 *    				PC1:	QTR sensor 2
 *    				PC2:	QTR sensor 3
 *    				PC3:	QTR sensor 4
 *    				PC4:	QTR sensor 5
 *    				PC5:	QTR sensor 6
 *    				PD7:	QTR sensor 7
 *    				PD4:	QTR sensor 8
 *    				PB0:	Buzzer
 *    				PB1:	PushButton (same as BUTTON_A in 3pi)
 *
 *        Version:  1.0
 *        Created:  05/07/2011 10:09:06 PM
 *       Compiler:  gcc
 *
 *         Author:  Martina Iglesias, martina.iglesiasf @ gmail.com
 *
 *                  Copyright (c) 2011, Martina Iglesias
 * =====================================================================================
 */


#include <pololu/orangutan.h>

#define BUZZER_PIN IO_B0
//#define DEBUG


const char welcome[] PROGMEM = ">g32>>c32";
const char bye[] PROGMEM = ">>c32>g32";



// A global ring buffer for data coming in.  This is used by the
// read_next_byte() and previous_byte() functions, below.
char buffer[100];


void initialize()
{
	play_from_program_space(welcome);

#ifdef DEBUG
	// start receiving data at 9600 baud.
	serial_set_baud_rate(9600);
	serial_receive_ring(buffer, 100);
#endif

	// initialize your QTR sensors
	//  unsigned char qtr_rc_pins[] = {IO_C0, IO_C1, IO_C2};
//	unsigned char qtr_rc_pins[] = {IO_C0, IO_C1, IO_C2, IO_C3, IO_C4, IO_C5, IO_D7, IO_D4};
	unsigned char qtr_rc_pins[] = {IO_D4, IO_D7, IO_C5, IO_C4, IO_C3, IO_C2, IO_C1, IO_C0};
	qtr_rc_init(qtr_rc_pins, 8, 2000, IO_D2);  // 800 us timeout, emitter pin PD2

#ifdef DEBUG
	serial_send_blocking("Press Button A to start calibrating...\n", 39);
#endif
	wait_for_button_press(BUTTON_A);
	// Always wait for the button to be released so that the robot doesn't
	// start moving until your hand is away from it.
	wait_for_button_release(BUTTON_A);
	delay_ms(800);

	// then start calibration phase and move the sensors over both
	// reflectance extremes they will encounter in your application:
	// We use a value of 2000 for the timeout, which
	// corresponds to 2000*0.4 us = 0.8 ms on our 20 MHz processor.
	unsigned int counter; // used as a simple timer
	for(counter = 0; counter < 82; counter++)
	{
		if(counter < 20 || counter >= 60)
			set_motors(60,-60);
		else
			set_motors(-60,60);

		qtr_calibrate(QTR_EMITTERS_ON);

		// Since our counter runs to 80, the total delay will be
		// 80*20 = 1600 ms.
		delay_ms(20);
	}
	set_motors(0,0);

#ifdef DEBUG
	serial_send_blocking("Press Button A to start line following...\n", 42);
#endif
	wait_for_button_press(BUTTON_A);
	wait_for_button_release(BUTTON_A);
}

// This is the main function, where the code starts.  All C programs
// must have a main() function defined somewhere.
int main()
{
	unsigned int sensors[8]; // an array to hold sensor values

#ifdef DEBUG
	unsigned int counter = 0;
#endif

	// set up the 3pi
	initialize();

	// This is the "main loop" - it will run forever.
	while(1)
	{
		// Get the position of the line.  Note that we *must* provide
		// the "sensors" argument to read_line() here, even though we
		// are not interested in the individual sensor readings.
		unsigned int position = qtr_read_line(sensors,QTR_EMITTERS_ON);
		
		// If I lift the robot, it should stop the motors. When the 
		// sensors are no longer near a surface, they will all read 1000,
		// and the position given by read_line functions will be 3500.
		//if (sensors[0] + sensors[1] + sensors[2] + sensors[3] + sensors[4]
		//		+ sensors[5] + sensors[6] + sensors[7] == 7000)
		if (sensors[0] + sensors[3] + sensors[4] + sensors[7] == 4000) // Shorter and quicker
		{
			set_motors(0,0);
			break;
		}

		if(position < 1000)
		{
			// We are far to the right of the line: turn left.

			// Set the right motor to 100 and the left motor to zero,
			// to do a sharp turn to the left.  Note that the maximum
			// value of either motor speed is 255, so we are driving
			// it at just about 40% of the max.
			set_motors(0,170);
#ifdef DEBUG
			if (counter > 100)
				serial_send_blocking("LEFT \n", 6);
#endif

		}
		else if(position < 2000)
		{
			set_motors(60,150);
#ifdef DEBUG
			if (counter > 100)
				serial_send_blocking("left \n", 6);
#endif
		}
		else if (position < 3000)
		{
			set_motors(90,150);
		}
		else if (position < 3300) // CENTER = 3500
		{
			set_motors(130,150);
		}
		else if (position < 3700)
		{
			// We are somewhat close to being centered on the line:
			// drive straight.
			set_motors(160,160);
#ifdef DEBUG
			if (counter > 100)
				serial_send_blocking("OK   \n", 6);
#endif
		}
		else if (position < 4000)
		{
			set_motors(150,130);
		}
		else if (position < 5000)
		{
			set_motors(150,90);
#ifdef DEBUG
			if (counter > 100)
				serial_send_blocking("right\n", 6);
#endif
		}
		else if (position < 6000)
		{
			set_motors(150,60);
		}
		else // (position < 7000)
		{
			// We are far to the left of the line: turn right.
			set_motors(170,0);
#ifdef DEBUG
			if (counter > 100)
				serial_send_blocking("RIGHT\n", 6);
#endif
		}

#ifdef DEBUG
		if (counter > 100)
			counter = 0;
		else
			counter++;
#endif

	}

	// This part of the code is never reached.  A robot should
	// never reach the end of its program, or unpredictable behavior
	// will result as random code starts getting executed.  If you
	// really want to stop all actions at some point, set your motors
	// to 0,0 and run the following command to loop forever:
	play_from_program_space(bye);
	set_motors(0,0);
	while(1);
}

