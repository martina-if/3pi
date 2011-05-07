import serial
import sys

BINARY = False

ser = serial.Serial(
#		port = '/dev/ttyACM2', # Wixel
		port = '/dev/wixel1', # Wixel
		baudrate=9600,
#		parity=serial.PARITY_ODD,
#		stopbits=serial.STOPBITS_TWO,
		)

ser.open()
ser.isOpen()

if BINARY:
	print 'BINARY MODE.\n'
else:
	print 'ASCII MODE.\n'

while True:


	if BINARY:
		print "0x%.2x" % ord(ser.read(1))
	else:
		out = ser.read(1)
		sys.stdout.write(out)
		sys.stdout.flush()
#		print "Quedan ", ser.inWaiting()
		while ser.inWaiting():
			sys.stdout.write(ser.read(1))
			sys.stdout.flush()
		sys.stdout.write('\n')
		sys.stdout.flush()
#			out += ser.read(1)
#			sys.stdout.write(out) # Importante, con print, mete espacios.
#			# El bucle es para imprimir una frase hasta el \r.
		
#	sys.stdout.write(ord(ser.read()))

