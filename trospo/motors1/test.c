#include <pololu/orangutan.h>
#include "turn.h"

/*
 * MODIFIED VERSION
 *
 * motors1: for for the Orangutan LV, SV, SVP, X2, Baby-O and 3pi robot.
 *
 * This example uses the OrangutanMotors functions to drive
 * motors in response to the position of user trimmer potentiometer
 * and blinks the red user LED at a rate determined by the trimmer
 * potentiometer position.  It uses the OrangutanAnalog library to measure
 * the trimpot position, and it uses the OrangutanLEDs library to provide
 * limited feedback with the red user LED.
 *
 * http://www.pololu.com/docs/0J20
 * http://www.pololu.com
 * http://forum.pololu.com
 *
 * MODIFIED VERSION
 */

unsigned long prevMillis = 0;

int main()
{
	// Display battery voltage and wait for button press
	while(!button_is_pressed(BUTTON_A))
	{
		delay_ms(100);
//		delay_ms(50);
	}

	// Always wait for the button to be released so that 3pi doesn't
	// start moving until your hand is away from it.
	wait_for_button_release(BUTTON_A);

	delay_ms(500);

	while (1)
	{
		turn('R', 90);
		set_motors(0,0);
		delay_ms(1000);

		turn('L', 90);
		set_motors(0,0);
		delay_ms(1000);
	}

	while (1)
	{
		set_motors(50,50);
		delay_ms(2000);
		set_motors(0,0);
		delay_ms(500);

		set_motors(-50,-50);
		delay_ms(2000);
		set_motors(0,0);
		delay_ms(500);

		set_motors(100,100);
		delay_ms(2000);
		set_motors(0,0);
		delay_ms(500);

		set_motors(-100,-100);
		delay_ms(2000);
		set_motors(0,0);
		delay_ms(500);

//		set_motors(150,150);
//		delay_ms(2000);
//		set_motors(0,0);
//		delay_ms(500);
//
//		set_motors(200,200);
//		delay_ms(2000);
//		set_motors(0,0);
//		delay_ms(500);
//
//		set_motors(255,255);
//		delay_ms(2000);
//		set_motors(0,0);
//		delay_ms(500);

	}

	set_motors(0,0);
	while(1);
}
