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
	sync_byte = []
	sync_byte = ser.read(1)
	print CABECERA_RECV , sync_byte,
	if sync_byte == 't':
		print "=>", # Es un byte de sincronizacion

		position = ser.read(2)							# unsigned int
		position = struct.unpack("H", position)
		position = position[0]
		
		proportional = ser.read(2)						# int
		proportional = struct.unpack("h", proportional)
		proportional = proportional[0]
		
		integral = ser.read(2)							# long
		integral = struct.unpack("h", integral)
		integral = integral[0]

		derivative = ser.read(2)						# int
		derivative = struct.unpack("h", derivative)
		derivative = derivative[0]

		power_diff = ser.read(2)						# int
		power_diff = struct.unpack("h", power_diff)
		power_diff = power_diff[0]

		print "Position = %d" % position
		print "Power diff %d= %d/20 + %d/10000 + %d*3/2" % (power_diff, proportional, integral, derivative)
#		print "My power diff = %d" % int(proportional)/20 + int(integral)/10000 + int(derivative)*3/2

		print

ser.close()

