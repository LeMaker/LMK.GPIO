'''
We use the program to test the serial on the 40Pin Header;
if you encounter error like "ImportError: No module named serial", please install the 'python-serial' package first.
'''
import LMK.GPIO as GPIO
import serial,time

serialDevGt = "/dev/ttyS0"  #For LeMaker Guitar
serialDevBP = "/dev/ttyS2"  #For BananaPro: UART3(ttyS2) and UART7(ttyS3)

baudrate=115200           # baud rate  
bytesize=8                # number of databits  
parity='N'        	  # enable parity checking  
stopbits=1                # number of stopbits  
timeout=None              # set a timeout value, None for waiting forever  
xonxoff=0                 # enable software flow control  
rtscts=0                  # enable RTS/CTS flow control  

readNum = 10

ser = serial.Serial(serialDevGt, baudrate, bytesize, parity, stopbits, timeout, xonxoff, rtscts)

strInput = raw_input("Enter some words:")
ser.write(strInput)

print "Start to read %d character" %(readNum)
str = ser.read(readNum)
print str

ser.close()
