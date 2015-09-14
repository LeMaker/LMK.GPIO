#!/usr/bin/env python
import LMK.GPIO as GPIO
import time

#LED Mode BCM
#PINs = [2,3,4,14,15,17,18,27,22,23,24,10,9,25,11,8,7,0,1,5,6,12,13,19,16,26,20,21]
#[2,3,4,14,18,23,24,10,9,11,8,7,0,1,5,6,12,13,19,16,26,20,21]
#[15,17,27,22,25]
#GPIO.setmode(GPIO.BCM)

#LED Mode BOARD
#PINs = [3,5,7,8,10,11,12,13,15,16,18,19,21,22,23,24,26,27,28,29,31,32,33,35,36,37,38,40]
#[3,5,7,8,12,16,18,19,21,23,24,26,27,28,29,31,32,33,35,36,37,38,40]
#[10,11,13,15,22]
PINs = [8]
GPIO.setmode(GPIO.BOARD)

while True:
	for PIN_NUM in range(len(PINs)):
		try:
			GPIO.setup(PINs[PIN_NUM], GPIO.OUT)
		except:
			print("Failed to setup GPIO %d", PIN_NUM)

		GPIO.output(PINs[PIN_NUM], True)
		time.sleep(0.5)
		GPIO.output(PINs[PIN_NUM], False)
                time.sleep(0.5)
