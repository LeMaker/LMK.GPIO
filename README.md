LMK.GPIO README

LMK.GPIO is a GPIO library that can support LeMaker SBC products, such as Banana Pro/Pi, LeMaker Guitar. In order to use convenient, we set most functions name to the same as the RPi.GPIO for Raspberry Pi.
The LMK.GPIO API usage are the same to the [RPi.GPIO](https://pypi.python.org/pypi/RPi.GPIO).
You can donwload the LMK.GPIO from:
https://github.com/LeMaker/LMK.GPIO.
## Support Hardware
    Banana Pro/Pi
    LeMaker Guitar 
    HiKey (TBD)

## Download
    git clone https://github.com/LeMaker/LMK.GPIO

## Installation
    sudo apt-get update
    sudo apt-get install python-dev
    cd /LMK.GPIO
    python setup.py install                 
    sudo python setup.py install
    
Please be attention that you need use both python and sudo python to make the LMK.GPIO work well.

## Remove
    cd /usr/local/lib/python2.7/dist-packages/
    sudo rm -r LMK
    
Note that the LMK library might be under /usr/lib/python2.7/dist-packages/, depending your system path setup. Remove the folder LMK and the python egg info file.

## Examples
You can go to LeMaker wiki to see the basic examples: http://wiki.lemaker.org/LMK.GPIO

And the source directory test also has many demo.

Thanks!

LeMaker Team
