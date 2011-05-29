#include <pololu/orangutan.h>
#include "follow-segment.h"
//#include "turn.h"

#define BUZZER_PIN IO_B0

void beep(unsigned int ms)
{
	set_digital_output(BUZZER_PIN, HIGH);
	delay_ms(ms);
	set_digital_output(BUZZER_PIN, LOW);
}

// A couple of simple tunes, stored in program space.
const char welcome[] PROGMEM = ">g32>>c32";
const char go[] PROGMEM = "L16 cdegreg4";

// A global ring buffer for data coming in.  This is used by the
// read_next_byte() and previous_byte() functions, below.
char buffer[100];

void initialize()
{

	unsigned int counter; // used as a simple timer

	play_from_program_space(welcome);

	// start receiving data at 9600 baud.
	serial_set_baud_rate(9600);
	serial_receive_ring(buffer, 100);

	// initialize your QTR sensors
//	unsigned char qtr_rc_pins[] = {IO_C0, IO_C1, IO_C2, IO_C3, IO_C4, IO_C5, IO_D7, IO_D4};
	unsigned char qtr_rc_pins[] = {IO_D4, IO_D7, IO_C5, IO_C4, IO_C3, IO_C2, IO_C1, IO_C0};
	qtr_rc_init(qtr_rc_pins, 8, 2000, IO_D2);  // 800 us timeout, emitter pin PD2


	while(is_playing());
	serial_send_blocking("Press Button A to start calibrating...\n", 39);
	wait_for_button_press(BUTTON_A);
	// Always wait for the button to be released so that the robot doesn't
	// start moving until your hand is away from it.
	wait_for_button_release(BUTTON_A);
	delay_ms(800);

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

	serial_send_blocking("Press Button A to start line following...\n", 42);
	wait_for_button_press(BUTTON_A);
	wait_for_button_release(BUTTON_A);

	// Play music and wait for it to finish before we start driving.
	play_from_program_space(go);
	while(is_playing());
}



// This function is called once, from main.c.
int main()
{
	int op;

	// set up the 3pi
	initialize();

	// Loop until we find an intersection.
	while(1)
	{
		// FIRST MAIN LOOP BODY  
		op = follow_segment();

		if (op == STOP)
		{
			set_motors(0,0);
			goto stop;
		}
		else if (op == LEFT || op == RIGHT || op == FORWARD)
		{
			follow_til_interesection(op);
		}

		// Drive straight a bit.  This helps us in case we entered the
		// intersection at an angle.
		// Note that we are slowing down - this prevents the robot
		// from tipping forward too much.
//		set_motors(50,50);
//		delay_ms(50);

	}

stop:

	set_motors(0,0);
	serial_send_blocking("STOP\n", 5);
	while(1);

	// This part of the code is never reached.  A robot should
	// never reach the end of its program, or unpredictable behavior
	// will result as random code starts getting executed.  If you
	// really want to stop all actions at some point, set your motors
	// to 0,0 and run the following command to loop forever:
	set_motors(0,0);
	while(1);
}

