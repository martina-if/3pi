#include <pololu/3pi.h>
#include <init.h>

/*
 *	Martina Iglesias Fernández 
 *  Basado en pololu serial slave.
 */

// PID constants
unsigned int pid_enabled = 0;
unsigned char max_speed = 255;
unsigned char p_num = 0;
unsigned char p_den = 0;
unsigned char d_num = 0;
unsigned char d_den = 0;
unsigned int last_proportional = 0;
unsigned int sensors[5];


// A global ring buffer for data coming in.  This is used by the
// read_next_byte() and previous_byte() functions, below.
char buffer[100];

// A pointer to where we are reading from.
unsigned char read_index = 0;

// Waits for the next byte and returns it.  Runs play_check to keep
// the music playing and serial_check to keep receiving bytes.
char read_next_byte()
{
	while(serial_get_received_bytes() == read_index)
	{
		serial_check();
		play_check();
	}
	char ret = buffer[read_index];
	read_index ++;
	if(read_index >= 100)
		read_index = 0;
	return ret;
}

/////////////////////////////////////////////////////////////////////

const char ok[] PROGMEM = ">g50>>c50";
const char wrong[] PROGMEM = ">c50>>g50>";

int main()
{
	pololu_3pi_init(2000);  
	play_mode(PLAY_CHECK);

	display_battery();

	clear();
	print("Slave");

	// start receiving data at 9600 baud.
	serial_set_baud_rate(9600);
	serial_set_mode(SERIAL_CHECK);
	serial_receive_ring(buffer, 100);

	char left_m, right_m;

	while(1)
	{
		// wait for a byte
		char command = read_next_byte();
		clear();

		switch (command)
		{
			case (char)0x00:
				set_motors(0,0);
				lcd_goto_xy(0,0);
				print("Stop");
//				play_from_program_space(ok);
				break;
			case (char)0x01:
				// Move forward
				left_m = read_next_byte();
				right_m = read_next_byte();
				set_motors((long)left_m, (long)right_m);
				lcd_goto_xy(0,0);
				print("Forward");
//				play_from_program_space(ok);
				break;
			case (char)0x02:
				// Move backwards. FIXME Después habrá que hacer las 4
				// combinaciones.
				left_m = read_next_byte();
				right_m = read_next_byte();
				set_motors(-(long)left_m, -(long)right_m);
				lcd_goto_xy(0,0);
				print("Backwards");
//				play_from_program_space(ok);
				break;
			default:
				play_from_program_space(wrong);
				set_motors(0,0);
				lcd_goto_xy(1,0);
				print("Error");
		}
	
	}
}

