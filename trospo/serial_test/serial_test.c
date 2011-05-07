/*
 * =====================================================================================
 *
 *       Filename:  serial_send.c
 *
 *    Description:  Test for baby orangutan UART (serial port). It sends "hello world"
 *    				message indefinitely.
 *
 *        Version:  1.0
 *        Created:  05/07/2011 03:15:17 PM
 *       Compiler:  gcc
 *
 *         Author:  Martina Iglesias, martina.iglesiasf @ gmail.com
 *
 *                  Copyright (c) 2011, Martina Iglesias
 * =====================================================================================
 */


#include <pololu/orangutan.h>
#include <string.h>


int main()
{
//	char buffer[20];

	// configure serial clock for 9.6 kbaud
	serial_set_baud_rate(9600);

	while(1)
	{
		serial_send_blocking("h",1);
		serial_send_blocking("e",1);
		serial_send_blocking("l",1);
		serial_send_blocking("l",1);
		serial_send_blocking("o",1);
		serial_send_blocking(" ",1);

		delay_ms(2000);

		serial_send_blocking("w",1);
		serial_send_blocking("o",1);
		serial_send_blocking("r",1);
		serial_send_blocking("l",1);
		serial_send_blocking("d",1);
		serial_send_blocking("!",1);

		delay_ms(2500);

		serial_send_blocking("HELLO WORLD!", 12);
		delay_ms(2000);
	}

	while(1);
}

