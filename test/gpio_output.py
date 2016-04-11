'''
We use the program to test the output of the pins
'''
import LMK.GPIO as GPIO
import time

#----------P3  P5  P7  P8  P10  P11  P12  P13  P15  P16  P18  P19 P21  P22  P23  P24 P26 P27 P28 P29  P31  P32  P33  P35  P36  P37  P38  P40-----------#
phyPins = (3,  5,  7,  8,  10,  11,  12,  13,  15,  16,  18,  19, 21,  22,  23,  24, 26, 27, 28, 29,  31,  32,  33,  35,  36,  37,  38,  40)
bcmPins = (2,  3,  4,  14, 15,  17,  18,  27,  22,  23,  24,  10,  9,  25,  11,  8,   7,  0,  1,  5,  6,   12,  13,  19,  16,  26,  20,  21)

GPIO.setmode(GPIO.BOARD)

def testOutput(mode, pins):
	if mode == "BOARD":
		print "Start to test the mode: %s" %(mode)
		GPIO.setmode(GPIO.BOARD)
	elif mode == "BCM":
		print "Start to test the mode: %s" %(mode)
		GPIO.setmode(GPIO.BCM)
	else:
		print "Invalid test mode: %s" %(mode)

	for i in pins:
		try:
			GPIO.setup(i, GPIO.OUT)
		except:
			print("Failed to setup GPIO %d", i)
			continue

		GPIO.output(i, True)
		time.sleep(0.5)
		GPIO.output(i, False)
		time.sleep(0.5)

	GPIO.cleanup()

testOutput("BOARD", phyPins)
testOutput("BCM", bcmPins)
