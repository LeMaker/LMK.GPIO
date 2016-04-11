/*
Copyright (c) 2015 Lemaker Team

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "cpuinfo.h"

#define MODE_UNKNOWN -1
#define BOARD        10
#define BCM          11
#define SERIAL       40
#define SPI          41
#define I2C          42
#define PWM          43

//#define DEBUG_ON
#ifdef DEBUG_ON
#define debug(fmt, args...)	printf(fmt, ##args)
#else
#define debug(fmt, args...)
#endif

int gpio_mode;
const int physToGpio_BP [64];
const int physToGpio_GT [64];
const int pinTobcm_BP [64];
const int pinTobcm_GT [64];
const int physToGpioR3 [64];
const int gpioEdge_GT[10];
const int gpioEdge_BP[15];

int gpio_direction[256];
int revision;
int setup_error;
int module_setup;

int support_event_detect(int gpio, const int *gpioEdge);
int check_gpio_priv(void);
int get_gpio_number(int channel, unsigned int *gpio, unsigned int *sys_gpio);
