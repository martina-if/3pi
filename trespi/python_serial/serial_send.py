import time
import serial

BINARY = False

# configure the serial connections (the parameters differs on the device you are connecting to)
ser = serial.Serial(
#	port='/dev/ttyACM1', # Para el programador orangutan
#	port='/dev/ttyACM0', # Para el Wixel
	port='/dev/wixel2', # Para el Wixel
	baudrate=9600,
#	parity=serial.PARITY_ODD,
#	stopbits=serial.STOPBITS_TWO,
	#bytesize=serial.SEVENBITS
)

ser.open()
ser.isOpen()

if BINARY:
	print 'BINARY MODE. Enter your commands below.\r\nInsert "exit" to leave the application.'
else:
	print 'ASCII MODE. Enter your message below.\r\nInsert"exit" to leave the application.'

while True :
	# Para enviar binario en hex:
	# ser.write('\x4b')
	# ser.write(chr(s)) # donde s es un numero 0-255
	# Para enviar binario teclear solo uno o dos digitos: ej. 5f, da, 65, 7(07), ...
	input = raw_input(">> ") #+ '\n'

	if input == 'exit':
		ser.close()
		exit()

	if BINARY:
		try:
			ser.write( chr(int(input, 16)) )
		except ValueError:
			print "Error de formato. Escribe dos cifras hexadecimales."
	else:
		# get keyboard input
		# send the character to the device
		ser.write(input)
#		ser.flushOutput()
