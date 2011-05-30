/*
 * =====================================================================================
 *
 *       Filename:  qtr_test.c
 *
 *    Description:  Test for Pololu QTR Sensor Array (RC). 
 *    				  http://www.pololu.com/docs/0J20/6.k
 *    				  Uses the following pin configuration
 *    				  PC0:	QTR sensor 1
 *    				  PC1:	QTR sensor 2
 *    				  PC2:	QTR sensor 3
 *    				  PC3:	QTR sensor 4
 *    				  PC4:	QTR sensor 5
 *    				  PC5:	QTR sensor 6
 *    				  PD7:	QTR sensor 7
 *    				  PD4:	QTR sensor 8
 *    				  PB0:	Buzzer
 *    				  PB1:	PushButton (same as BUTTON_A in 3pi)
 *
 *
 *        Version:  1.0
 *        Created:  05/01/2011 09:40:40 PM
 *       Compiler:  gcc-avr
 *
 *         Author:  Martina Iglesias Fernández <martina.iglesiasf @ gmail.com>
 *
 * =====================================================================================
 */

#include <pololu/orangutan.h>


#define BUZZER_PIN IO_B0

void beep(unsigned int ms)
{
	set_digital_output(BUZZER_PIN, HIGH);
	delay_ms(ms);
	set_digital_output(BUZZER_PIN, LOW);
}


// A global ring buffer for data coming in.  This is used by the
// read_next_byte() and previous_byte() functions, below.
char buffer[100];

int main()
{

	unsigned int counter; // used as a simple timer

	// start receiving data at 9600 baud.
	serial_set_baud_rate(9600);
	serial_receive_ring(buffer, 100);

	// initialize your QTR sensors
	// With this configuration, sensors[i] = value from sensor (i+1) (sensor array numbering)
	// unsigned char qtr_rc_pins[] = {IO_C0, IO_C1, IO_C2, IO_C3, IO_C4, IO_C5, IO_D7, IO_D4};
	// With this configuration, sensors are 0,1,...,7, ordered from left to right.
	unsigned char qtr_rc_pins[] = {IO_D4, IO_D7, IO_C5, IO_C4, IO_C3, IO_C2, IO_C1, IO_C0};
	qtr_rc_init(qtr_rc_pins, 8, 2000, IO_D2);  // 800 us timeout, emitter pin PD2
//	qtr_rc_init(qtr_rc_pins, 6, 2000, 255);  // 800 us timeout, no emitter pin 
	// int qtr_analog_pins[] = {0, 1, 2};
	// qtr_analog_init(qtr_analog_pins, 3, 10, IO_C0); // 10 samples, emitter pin is PC0

	beep(200);
	serial_send_blocking("Press Button A to start calibrating...\n", 39);
	wait_for_button_press(BUTTON_A);

	// then start calibration phase and move the sensors over both
	// reflectance extremes they will encounter in your application:
	// We use a value of 2000 for the timeout, which
	// corresponds to 2000*0.4 us = 0.8 ms on our 20 MHz processor.
	for(counter = 0; counter < 80; counter++)
	{
		if(counter < 20 || counter >= 60)
			set_motors(50,-50);
		else
			set_motors(-50,50);

		qtr_calibrate(QTR_EMITTERS_ON);

		// Since our counter runs to 80, the total delay will be
		// 80*20 = 1600 ms.
		delay_ms(20);
	}
	set_motors(0,0);
	play(">>a32");

//	// then start calibration phase and move the sensors over both
//	// reflectance extremes they will encounter in your application:
//	int i;
//	for (i = 0; i < 250; i++)  // make the calibration take about 5 seconds
//	{
//		qtr_calibrate(QTR_EMITTERS_ON);
//		delay(20);
//	}

	serial_send_blocking("Press Button A to start reading...\n", 35);
	wait_for_button_press(BUTTON_A);

	// optional: signal that the calibration phase is now over and wait for further
	// input from the user, such as a button press

	char command = 't';
	while (1)
	{
		// main body of program goes here
		unsigned int sensors[8];
		int position = qtr_read_line(sensors, QTR_EMITTERS_ON);
//		qtr_read_calibrated(sensors, QTR_EMITTERS_ON);
		beep(200);

		serial_send_blocking(&command, 1);
		serial_send_blocking((char*)sensors, 8*2);
		serial_send_blocking((char*)&position, 1*2);
		delay_ms(300);

	}

	while(1);
	return 0;
}

