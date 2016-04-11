'''
We use the program to test the pull-up and pull-down of the pins. and observe 
the on and off of the leds after running the program.
'''
import LMK.GPIO as GPIO
import time

#pull-up/down pins for the LeMaker Guitar, and the P3, P5, P19 and P23 of the pull-up has been enabled by 
#connecting the external pull-up resistor, don't need to operate by using the software.

#-----------P8 P10 P12 P13 P15 P16 P22-----------#
pullUpGt = (8, 10, 12, 13, 15, 16, 22)  #Pull-UP pins which be configured by the software
pullDwGt = (16,)                        #Pull-Down pins which be configured by the software

#pull-up/down pins for the BananaPro, and the P3, P5, P27 and P28 of the pull-up have been enabled by
#connecting the external pull-up resistor, don't need to operate by using the software.

#-----------P7  P8  P10  P11  P12  P13  P15  P16  P18  P19 P21  P22  P23  P24 P26 P29  P31  P32  P33  P35  P36  P37  P38  P40-----------#
pullUpBP = (7,  8,  10,  11,  12,  13,  15,  16,  18,  19, 21,  22,  23,  24, 26, 29,  31,  32,  33,  35,  36,  37,  38,  40)
PullDwBP = (7,  8,  10,  11,  12,  13,  15,  16,  18,  19, 21,  22,  23,  24, 26, 29,  31,  32,  33,  35,  36,  37,  38,  40)

#GPIO.PUD_UP    #enable pull-up
#GPIO.PUD_DOWN  #enable pull-down
#GPIO.PUD_OFF   #disable pull-up/down

GPIO.setmode(GPIO.BOARD)
for i in pullUpGt:
	GPIO.setup(i, GPIO.IN, pull_up_down=GPIO.PUD_UP)  #enable pull-up
	time.sleep(1)  
	GPIO.setup(i, GPIO.IN, pull_up_down=GPIO.PUD_OFF) #disable pull-up
	time.sleep(0)

GPIO.cleanup()
