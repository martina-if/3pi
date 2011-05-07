/*
 * Code to perform various types of turns.  The delays here had to be
 * calibrated for the 3pi's motors.
 */

#include <pololu/3pi.h>

#define MSPER90 50
// Turns according to the parameter dir, which should be 'L', 'R', 'S'
// (straight), or 'B' (back).
void turn(char dir, int angle)
{
   int time = (long)((float)(angle*MSPER90/9.0));

	switch(dir)
	{
	case 'L':
		// Turn left.
		set_motors(-40,40);
		delay_ms(time);
		break;
	case 'R':
		// Turn right.
		set_motors(40,-40);
		delay_ms(time);
		break;
	case 'B':
		// Turn around.
		set_motors(40,-40);
		delay_ms(time*2);
		break;
	case 'S':
		// Don't do anything!
		break;
	}
}

void turn_right(int v)
{
   turn('R', 90);
   set_motors(v,v);
}

void turn_left(int v)
{
   turn('L', 90);
   set_motors(v,v);
}


// Local Variables: **
// mode: C **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
