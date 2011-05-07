import Tkinter
import time
import threading
import Queue
import serial
import joystick

DEBUG = False

class GuiPart:
	def __init__(self, root, queue, endCommand):
		self.queue = queue
		# Set up the GUI
		self.j = joystick.Joystick(parent = root)
#		console = Tkinter.Button(root, text='Done', command=endCommand)
#		console.pack()
#		Add more GUI stuff here

	def processIncoming(self):
		self.queue.put((self.j.translate, self.j.rotate))

class ThreadedClient:
	"""
	Launch the main part of the GUI and the worker thread. periodicCall and
	endApplication could reside in the GUI part, but putting them here
	means that you have all the thread controls in a single place.
	"""
	def __init__(self, root):
		"""
		Start the GUI and the asynchronous threads. We are in the main
		(original) thread of the application, which will later be used by
		the GUI. We spawn a new thread for the worker.
		"""
		self.root = root
		if not DEBUG:
			# configure the serial connections (the parameters differs on the device you are connecting to)
			self.ser = serial.Serial(
#				port='/dev/ttyACM1', # Serial port of the orangutan programmer
#				port='/dev/wixel',
				port='/dev/orangutan_serial',
				baudrate=9600,
				#parity=serial.PARITY_ODD,
				#stopbits=serial.STOPBITS_TWO,
				#bytesize=serial.SEVENBITS
			)
			self.ser.open()
			self.ser.isOpen()

		# Create the queue
		self.queue = Queue.Queue()

		# Set up the GUI part
		self.gui = GuiPart(root, self.queue, self.endApplication)

		# Set up the thread to do asynchronous I/O
		# More can be made if necessary
		self.running = 1
		self.thread1 = threading.Thread(target=self.workerThread1)
		self.thread1.start()

		# Start the periodic call in the GUI to check if the queue contains
		# anything
		self.periodicCall()

	def periodicCall(self):
		"""
		Check every 500 ms if there is something new in the queue.
		"""
		self.gui.processIncoming()
		if not self.running:
			# This is the brutal stop of the system. You may want to do
			# some cleanup before actually shutting it down.
			self.ser.close()
			import sys
			sys.exit(1)
		self.root.after(500, self.periodicCall)

	def workerThread1(self):
		"""
		This is where we handle the asynchronous I/O. For example, it may be
		a 'select()'.
		One important thing to remember is that the thread has to yield
		control.
		"""
		while self.running:
			(trans, rot) = self.queue.get()
			trans = float(trans)
			rot = float(rot)
			(dir, left, right) = self.calcular_ruedas(trans, rot)
			print float(trans), rot, "---->", dir, left, right

			if not DEBUG:
				self.ser.write( chr(dir) )
				self.ser.write( chr(left) )
				self.ser.write( chr(right) )

	def endApplication(self):
		self.running = 0


	def calcular_ruedas(self, trans, rot):
		if rot > 0:
			left = trans*128
			right = left + rot*64
		else:
			right = trans*128
			left = right - rot*64
		dir = calcular_direccion(trans, rot)
		return ( dir, abs(int(left)), abs(int(right)))


def calcular_direccion(trans, rot):
	if trans > 0:
		return 1
	elif trans == 0:
		if rot == 0:
			return 0
		else:
			return 1
	else:
		return 2

root = Tkinter.Tk()
root.withdraw()

client = ThreadedClient(root)
root.mainloop()
