/*
 * Code to perform various types of turns.  The delays here had to be
 * calibrated for the 3pi's motors.
 */

#include <pololu/3pi.h>
#include "turn.h"

// Turns according to the parameter dir, which should be 'L', 'R', 'S'
// (straight), or 'B' (back).
void turn(char dir)
{
	switch(dir)
	{
	case 'L':
		// Turn left.
		set_motors(-80,80);
		delay_ms(200);
		break;
	case 'R':
		// Turn right.
		set_motors(80,-80);
		delay_ms(200);
		break;
	case 'B':
		// Turn around.
		set_motors(80,-80);
		delay_ms(450);
		break;
	case 'S':
		// Don't do anything!
		break;
	}
}

void turn_north (position_t* pos)
{
	if (pos->orientation == 'E')
	{
		turn('L');
		pos->orientation = 'N';
	}
	pos->dify--;
}

void turn_east(position_t* pos)
{
	if (pos->orientation == 'N')
	{
		turn('R');
		pos->orientation = 'E';
	}
	pos->difx--;
}
	

// Local Variables: **
// mode: C **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
