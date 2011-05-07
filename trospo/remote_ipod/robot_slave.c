#include <pololu/orangutan.h>

/*
 *	Martina Iglesias Fernández 
 * Basado en pololu serial slave.
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

/* // Backs up by one byte in the ring buffer. */
/* void previous_byte() */
/* { */
/* 	read_index --; */
/* 	if(read_index == 255) */
/* 		read_index = 99; */
/* } */


/////////////////////////////////////////////////////////////////////


int main()
{

	// start receiving data at 9600 baud.
	serial_set_baud_rate(9600);
	serial_set_mode(SERIAL_CHECK);
	serial_receive_ring(buffer, 100);

	char left_m, right_m;

	while(1)
	{
		// wait for a byte
		char command = read_next_byte();
		char debug[4];

		red_led(0);
		switch (command)
		{
			case (char)0x00:
				set_motors(0,0);
				break;
			case (char)0x01:
				// Move forward
				left_m = read_next_byte();
				right_m = read_next_byte();
				set_motors((long)left_m, (long)right_m);
				// DEBUG ===============================
				serial_send_blocking(&command, 1);
				serial_send_blocking(&left_m, 1);
				serial_send_blocking(&right_m, 1);
//				debug[0] = command;
//				debug[1] = left_m;
//				debug[2] = right_m;
//				serial_send_blocking(debug, 3);
				// =====================================

				red_led(1);
				delay_ms(200);
				red_led(0);
				break;
			case (char)0x02:
				// Move backwards. FIXME Después habrá que hacer las 4
				// combinaciones.
				left_m = read_next_byte();
				right_m = read_next_byte();
				set_motors(-(long)left_m, -(long)right_m);
				break;
			default:
				red_led(1);
				delay_ms(100);
				red_led(0);
				delay_ms(100);
		}
	
//		lcd_goto_xy(0,1);
//		print_hex_byte(byte);
//		play_from_program_space(ok);
//		serial_send(&byte, 1);

	}
}

