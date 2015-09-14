import os

os.chdir("/usr/local/lib/python2.7/dist-packages/")
os.system("sudo rm LMK.GPIO-0.0.1.egg-info")
os.system("sudo rm -r LMK")

os.chdir("/home/bananapi/LMK.GPIO/")
os.system("python setup.py install")
os.system("sudo python setup.py install")

os.chdir("/home/bananapi/LMK.GPIO/test/")
os.system("sudo python led.py")