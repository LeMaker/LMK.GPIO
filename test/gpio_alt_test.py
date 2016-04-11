'''
We use the program to detect the pin function, eg: input, output
'''
import LMK.GPIO as GPIO

#----------P3  P5  P7  P8  P10  P11  P12  P13  P15  P16  P18  P19 P21  P22  P23  P24 P26 P27 P28 P29  P31  P32  P33  P35  P36  P37  P38  P40-----------#
phy_pins = (3,  5,  7,  8,  10,  11,  12,  13,  15,  16,  18,  19, 21,  22,  23,  24, 26, 27, 28, 29,  31,  32,  33,  35,  36,  37,  38,  40)
bcm_pins = (2,  3,  4,  14, 15,  17,  18,  27,  22,  23,  24,  10,  9,  25,  11,  8,   7,  0,  1,  5,  6,   12,  13,  19,  16,  26,  20,  21)

validPins = (11,12,13,15)

GPIO.setmode(GPIO.BOARD)

GPIO.setup(11,GPIO.IN)
GPIO.setup(12,GPIO.OUT)
GPIO.setup(13,GPIO.IN)
GPIO.setup(15,GPIO.OUT)

for pin in validPins:
	ret = GPIO.gpio_function(pin)
	print ret

GPIO.cleanup()
