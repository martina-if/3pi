# coding: utf-8
import serial
import socket
import sys
import struct

def read_line(ser):
	line = ''
	byte = ser.read(1);
	
	while byte != '\n':
		line += byte
		byte = ser.read(1)

	return line



BINARY = False

ser = serial.Serial(
#		port = '/dev/ttyACM2',
		port = '/dev/wixel',
		baudrate=9600,
#		parity=serial.PARITY_ODD,
#		stopbits=serial.STOPBITS_TWO,
		)

ser.open()
ser.isOpen()

NUM_SENSORS = 8
CABECERA_RECV =  u'\u03C0' + " => "

#sensor_data_char = []

#	sensor_data = []
print "Waiting..."
ser.read(1) # No sé por qué, pero siempre, envía un byte con basura al principio...
print  CABECERA_RECV, read_line(ser)
sys.stdout.flush()
print  CABECERA_RECV, read_line(ser)
sys.stdout.flush()

while True:

	print read_line(ser)
	sys.stdout.flush()

ser.close()

