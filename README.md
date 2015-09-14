LMK.GPIO README

LMK.GPIO is a GPIO library that can support LeMaker SBC products, such as Banana Pro/Pi, LeMaker Guitar. In order to use convinient, we set most functions name to the same as the RPi.GPIO for Raspberry Pi.
The LMK.GPIO API usage are the same to the [RPi.GPIO](https://pypi.python.org/pypi/RPi.GPIO).
You can donwload the LMK.GPIO from:
https://github.com/LeMaker/LMK.GPIO.
## Support Hardware
    Banana Pro/Pi
    LeMaker Guitar 
    HiKey (TBD)

## Download
    git clone https://github.com/LeMaker/LMK.GPIO_BP

## Installation
    sudo apt-get update
    sudo apt-get install python-dev
    cd /LMK.GPIO
    python setup.py install                 
    sudo python setup.py install
    
Please be attention that you need use both python and sudo pytohn to make the LMK.GPIO work well.

## Examples
You can go to LeMaker wiki to see the basic examples: http://wiki.lemaker.org/LMK.GPIO

And the source directory test also has many demo.

## Extra
This version supports a new addressing mode "RAW" which enables you to use any GPIO pin. Below is an example which sets PD10 (which is pin 29 on the LCD connector) to a high level.

    import LMK.GPIO as GPIO
    GPIO.setmode(GPIO.RAW)
    GPIO.setup(GPIO.PD+10, GPIO.OUT)
    GPIO.output(GPIO.PD+10, 1)


Thanks!

LeMaker Team
