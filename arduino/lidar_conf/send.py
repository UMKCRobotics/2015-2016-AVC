import serial

ser = serial.Serial('/dev/arduino_pathfinding',9600)

def ard_init(ser):
	connected = False
	while not connected:
		serin = ser.read()
		connected = True

ard_init(ser)

while True:
	try:
		userinp = raw_input("Servo position: ")
		if userinp == 'exit':
			quit()
		else:
			int(userinp)
			print userinp
			print type(userinp)
			ser.write(userinp)
			#print(ser.inWaiting())
			#print(ser.outWaiting())
	except Exception,e:
		print str(e)
