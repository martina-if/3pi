import serial
import socket
import sys

BINARY = False

ser = serial.Serial(
#		port = '/dev/ttyACM0',
		port = '/dev/wixel',
		baudrate=9600,
#		parity=serial.PARITY_ODD,
#		stopbits=serial.STOPBITS_TWO,
		)

ser.open()
ser.isOpen()
#
##if BINARY:
##	print 'BINARY MODE.\n'
##else:
##	print 'ASCII MODE.\n'

#while True:
#Creo el objeto socket
s = socket.socket()

#Invoco al metodo bind, pasando como parametro una tupla con IP y puerto
s.bind(("192.168.1.123", 1958))

#Invoco el metodo listen para escuchar conexiones con el numero maximo de conexiones como parametro
s.listen(1)

#El metodo accept bloquea la ejecucion a la espera de conexiones
#accept devuelve un objeto socket y una tupla Ip y puerto
sc, addr = s.accept()
print "Recibo conexion de " + str(addr[0]) + ":" + str(addr[1])

while True:

	#invoco recv sobre el socket cliente, para recibir un maximo (segun parametro) de 1024 bytes
	recibido = sc.recv(32) # Creo que son 25 los caracteres que llegan
	datos = recibido.split()
	print "Ipod: ", recibido
#	print "Datos: ", datos
	if len(datos) == 4:
		(_,x,y,_) = recibido.split() # ('accel', 'x', 'y', 'z')

		x = float(x)
		y = float(y)
		if y > 0: # Giro izquierda
			m1 = int(70 + 50*x)
			m2 = int(70 + 50*x + 50*y)
		else:
			m1 = int(70 + 50*x + 50*abs(y))
			m2 = int(70 + 50*x)

		ser.write(chr(1))
		ser.write(chr(m1))
		ser.write(chr(m2))	
		# DEBUG =====================================
#		out = ser.read(3)
		a = ord(ser.read(1))
		b = ord(ser.read(1))
		c = ord(ser.read(1))
#		a = int(a)
#		b = int(b)
#		c = int(c)
		sys.stdout.write("Baby: " + str(a) + ", " + str(b) + ", " + str(c) + "\n")
		sys.stdout.flush()
##	print "Quedan ", ser.inWaiting()
#	while ser.inWaiting():
#		sys.stdout.write(ser.read(1))
#		sys.stdout.flush()
#	sys.stdout.write('\n')
#	sys.stdout.flush()
	# DEBUG =====================================
		

#	#Envio la respuesta al socket cliente
#	sc.send(recibido)

print "adios"

#cierro sockets cliente y servidor
sc.close()
s.close()

		
##	sys.stdout.write(ord(ser.read()))
#
