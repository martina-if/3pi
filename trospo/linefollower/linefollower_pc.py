# coding: utf-8
import serial
import socket
import sys
import struct

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
print  CABECERA_RECV, ser.read(39)
sys.stdout.flush()
print  CABECERA_RECV, ser.read(42)
sys.stdout.flush()

while True:

	data = ser.read(6)
	print data,
#		print "My power diff = %d" % int(proportional)/20 + int(integral)/10000 + int(derivative)*3/2

	print

ser.close()

