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
print  CABECERA_RECV, ser.read(35)
sys.stdout.flush()

while True:
	sync_byte = []
	sync_byte = ser.read(1)
	print CABECERA_RECV , sync_byte,
	if sync_byte == 't':
		print "=>", # Es un byte de sincronizacion

		data = []

		for i in range(NUM_SENSORS + 1):
			data = ser.read(2) # Se envian 2 bytes por cada medida de cada sensor [sizeof(int) = 16]
			theint = struct.unpack("H", data) # Desempaquetar 2 bytes en un int unsigned ("H") [signed seria "h"]
	####	Otra opcion de desempaquetar dos bytes en un int ########################
	#		msb = ser.read(1)
	#		lsb = ser.read(1)
	#		theint = ord(lsb) + ord(msb) * 256
	###############################################################################
			print str(theint[0]),
		print
	else:
		print "Tiro byte! (%b)" % sync_byte

ser.close()

