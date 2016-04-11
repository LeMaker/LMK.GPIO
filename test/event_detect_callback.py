import LMK.GPIO as GPIO
import time

#The following pins support the external event detected on the 40Pin Header of the LeMake Guitar
'''P8 P10 P12 P13 P15 P16 P22 P19 P23'''

#The following pins support the external event detected on the 40Pin Header of the BananaPro
'''P7 P8 P10 P11 P13 P15 P16 P18 P19 P21 P22 P23 P24 P26'''

testPinOnGt = 8   #For LeMaker Guitar
testPinOnBP = 18  #For BananaPro

GPIO.setmode(GPIO.BOARD)
GPIO.setup(testPinOnGt, GPIO.IN, GPIO.PUD_UP)

def testCallback(channel):
	print "running callback function", channel

GPIO.add_event_detect(testPinOnGt, GPIO.RISING, callback=testCallback, bouncetime=300)

try:
	while True:
		time.sleep(1)
except KeyboardInterrupt:
	pass

GPIO.remove_event_detect(testPinOnGt)

GPIO.cleanup()

