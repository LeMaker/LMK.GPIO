'''
We use the program to test variation of duty cycle, and observe the brightness of the leds after running the program.
'''
import LMK.GPIO as GPIO
import time

#----------P3  P5  P7  P8  P10  P11  P12  P13  P15  P16  P18  P19 P21  P22  P23  P24 P26 P27 P28 P29  P31  P32  P33  P35  P36  P37  P38  P40-----------#
phyPins = (3,  5,  7,  8,  10,  11,  12,  13,  15,  16,  18,  19, 21,  22,  23,  24, 26, 27, 28, 29,  31,  32,  33,  35,  36,  37,  38,  40)
bcmPins = (2,  3,  4,  14, 15,  17,  18,  27,  22,  23,  24,  10,  9,  25,  11,  8,   7,  0,  1,  5,  6,   12,  13,  19,  16,  26,  20,  21)

pinNum = 12
frequency = 100  #100HZ

GPIO.setmode(GPIO.BOARD)
GPIO.setup(pinNum,GPIO.OUT)

p = GPIO.PWM(pinNum,frequency)
p.start(0)
try:
    while True:
        for dutyCycle in range(0,100,5):
            p.ChangeDutyCycle(dutyCycle)
            time.sleep(0.1)
        for dutyCycle in range(100,0,-5):
            p.ChangeDutyCycle(dutyCycle)
            time.sleep(0.1)
except KeyboardInterrupt:
    pass

p.stop()

GPIO.cleanup()
