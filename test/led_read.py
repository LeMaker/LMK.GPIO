#!/usr/bin/env python
import LMK.GPIO as GPIO
import time
PIN_0 = 3
PIN_1 = 5
#GPIO.setmode(GPIO.BCM)
GPIO.setmode(GPIO.BOARD)
GPIO.setup(PIN_0,GPIO.OUT)
GPIO.setup(PIN_1,GPIO.IN)

while True:
	GPIO.output(PIN_0,True)
	print "\n value = %d\n" %(GPIO.input(PIN_1))	
	time.sleep(1)
	GPIO.output(PIN_0,False)
	print "\n value = %d\n" %(GPIO.input(PIN_1))
	time.sleep(1)

GPIO.cleanup()
