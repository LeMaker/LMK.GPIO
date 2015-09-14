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
#GPIO.setmode(GPIO.BOARD)

#Switch BCM [4,14,23,24,10,9,11,8,7]
#Switch BOARD [7,8,16,18,19,21,23,24,26]   18,19,26
PIN_NUM = 24
GPIO.setmode(GPIO.BCM)
#GPIO.setmode(GPIO.BOARD)
GPIO.setup(PIN_NUM,GPIO.IN,GPIO.PUD_UP)


print "The value of Pin %d is %d" %(PIN_NUM,GPIO.input(PIN_NUM))


def my_callback(channel):
	print "The value of Pin %d is %d" %(PIN_NUM,GPIO.input(PIN_NUM))

	print "Callback trigger %d" %channel
	print "Now value of the Pin is %d" %(GPIO.input(PIN_NUM))
	print "Click Ctr + C to exit"

GPIO.add_event_detect(PIN_NUM,GPIO.RISING,callback = my_callback,bouncetime = 300)


print "The value of Pin %d is %d" %(PIN_NUM,GPIO.input(PIN_NUM))


try:
    while True:
	time.sleep(0.1)
except KeyboardInterrupt:
    pass

GPIO.cleanup()

