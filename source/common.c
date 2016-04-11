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

#include "Python.h"
#include "c_gpio.h"
#include "common.h"

int gpio_mode = MODE_UNKNOWN;
int setup_error = 0;
int module_setup = 0;
int revision = -1;

const int physToGpio_BP [64] =		//BOARD MODE
{
	-1, // 0
	-1, -1, //1, 2
	53, -1, //3, 4
	52, -1, //5, 6
	226, 228, //7, 8
	-1, 229, //9, 10
	275, 259, //11, 12
	274, -1, //13, 14
	273, 244, //15, 16
	-1, 245, //17, 18
	268, -1, //19, 20
	269, 272, //21, 22
	267, 266, //23, 24
	-1, 270, //25, 26
	257, 256, //27, 28
	35, -1, //29, 30
	277, 276, //31, 32
	45, -1, //33, 34
	39, 38, //35, 36
	37, 44, //37, 38
	-1, 40, //39, 40
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, //41-> 55
	-1, -1, -1, -1, -1, -1, -1, -1 // 56-> 63
} ;

const int pinTobcm_BP [64] =	//BCM MODE
{
	257,256, //map to BCM GPIO0,1
	53,52, //map to BCM GPIO2,3
	226,35, //map to BCM GPIO4,5
	277,270, //map to BCM GPIO6,7
	266,269, //map to BCM GPIO8,9
	268,267, //map to BCM GPIO10,11
	276,45, //map to BCM GPIO12,13
	228,229, //map to BCM GPIO14,15
	38,275, //map to BCM GPIO16,17
	259,39, //map to BCM GPIO18,19
	44, 40, //map to BCM GPIO20,21
	273,244, //map to BCM GPIO22,23
	245,272, //map to BCM GPIO24,25
	37, 274, //map to BCM GPIO26,27
	-1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 28... 43
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, //44... 59
	-1, -1, -1, -1 // ...63
} ;

const int physToGpioR3 [64] =//head num map to BCMpin
{
	-1, // 0
	-1, -1, // 1, 2
	2, -1,
	3, -1,
	4, 14,
	-1, 15,
	17, 18,
	27, -1,
	22, 23,
	-1, 24,
	10, -1,
	9, 25,
	11, 8,
	-1, 7, // 25, 26
	0, 1, //27, 28
	5, -1, //29, 30
	6, 12, //31, 32
	13, -1, //33, 34
	19, 16, //35, 36
	26, 20, //37, 38
	-1, 21, //39, 40
	// Padding:
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // ... 56
	-1, -1, -1, -1, -1, -1, -1, // ... 63
} ;

const int gpioEdge_BP[15] = {226,228,229,275,274,273,244, 245,268,269,272,267,266,270,-1};  //'P7 P8 P10 P11 P13 P15 P16 P18 P19 P21 P22 P23 P24 P26' on 40-Pin Header of the BananaPro

//add for guitar
const int physToGpio_GT [64] =
{
   -1,          // 0
   -1,     -1,   //1,           2
   131,    -1,   //3(SDA2),      4
   130,    -1,   //5(SCK2),      6
   50,     91,   //7(B18),       8(UART0_TX)
   -1,     90,   //9,           10(UART0_RX)
   64,     40,   //11(C0),      12(B8-PWM)
   65,    -1,   //13(C1),       14
   68,     25,   //15(C4),      16(A25)
   -1,     70,   //17,          18(B9)
   89,     -1,   //19(CE0),     20
   88,     69,   //21(MOSI),    22(C5)
   86,     87,   //23(SCLK),    24(B19)
   -1,     51,   //25,          26(MISO)
   48,     46,   //27(B16),     28(B14)
   47,     -1,   //29(B15),     30
   42,     45,   //31(B10),     32(B13)      
   32,     -1,   //33(B0),      34
   33,     28,   //35(B1),      36(A28)
   34,     31,   //37(B2),      38(A31)
   -1,     27,   //39,          40(A27)
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, //41-> 55
   -1, -1, -1, -1, -1, -1, -1, -1 // 56-> 63
} ;

const int pinTobcm_GT [64] =	//BCM MODE
{
48,46,   //map to BCM GPIO0,1
131,130,                //map to BCM GPIO2,3
50,47,         //map to BCM GPIO4,5
42,51,   //map to BCM GPIO6,7
87,88,   //map to BCM GPIO8,9
89,86,   //map to BCM GPIO10,11
45,32,          //map to BCM GPIO12,13
91,90,   //map to BCM GPIO14,15
28,64,    //map to BCM GPIO16,17
40,33,    //map to BCM GPIO18,19
31,27,         //map to BCM GPIO20,21
68,25, //map to BCM GPIO22,23
70,69, //map to BCM GPIO24,25
34,65,  //map to BCM GPIO26,27
-1,-1,	//map to BCM GPIO28,29
-1,-1,	//map to BCM GPIO30,31
-1,-1,	//map to BCM GPIO32,33
-1,-1,	//map to BCM GPIO34,35
-1,-1,	//map to BCM GPIO36,37
-1,-1,	//map to BCM GPIO38,39
-1,-1,  //map to BCM GPIO40
 -1, -1, // 28... 43
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, //44... 59
-1, -1, -1, -1 // ...63
} ;

const int gpioEdge_GT[10] = {91, 90, 40, 65, 68, 25, 69, 89, 86,-1};  //'P8 P10 P12 P13 P15 P16 P22 P19 P23' on 40-Pin Header of the LeMaker Guitar
//end

int check_gpio_priv(void)
{
	// check module has been imported cleanly
	if (setup_error)
	{
		PyErr_SetString(PyExc_RuntimeError, "Module not imported correctly!");
		return 1;
	}

	// check mmap setup has worked
	if (!module_setup)
	{
		PyErr_SetString(PyExc_RuntimeError, "No access to /dev/mem.  Try running as root!");
		return 2;
	}
	return 0;
}


int is_valid_raw_port(int channel)
{
	if(is_a20_platform()){
		if (channel >= 0 && channel < 18) return 1; // PA
		if (channel >= 32 && channel < 56) return 2; // PB
		if (channel >= 64 && channel < 89) return 3; // PC
		if (channel >= 96 && channel < 124) return 4; // PD
		if (channel >= 128 && channel < 140) return 5; // PE
		if (channel >= 160 && channel < 166) return 6; // PF
		if (channel >= 192 && channel < 204) return 7; // PG
		if (channel >= 224 && channel < 252) return 8; // PH
		if (channel >= 256 && channel < 278) return 9; // PI
	} else if(is_s500_platform()){ //add for guitar
		if (channel >= 0 && channel < 32) return 1; // PA
		if (channel >= 32 && channel < 64) return 2; // PB
		if (channel >= 64 && channel < 96) return 3; // PC
		if (channel >= 96 && channel < 128) return 4; // PD
		if (channel >= 128 && channel < 160) return 5; // PE
	} else{
		printf("Please use Banana Pro or LeMaker Guitar\n");
                return 0;
	}
	return 0;
}


int get_gpio_number(int channel, unsigned int *gpio,unsigned int *sys_gpio)
{
	// check channel number is in range
	if ( (gpio_mode == BCM && (channel < 0 || channel > 64))
	|| (gpio_mode == BOARD && (channel < 1 || channel > 64)) )
	{
		PyErr_SetString(PyExc_ValueError, "The channel sent is invalid on a Banana Pi");
		return 4;
	}

	// convert channel to gpio
	if (gpio_mode == BOARD)
	{
		if(is_a20_platform())
		{
			if ((*(physToGpio_BP+channel) == -1) || (*(physToGpioR3+channel) == -1))
			{
				PyErr_SetString(PyExc_ValueError, "The channel sent is invalid on a Banana Pro");
				return 5;
			} 
			*gpio = *(physToGpio_BP+channel);	//pin_to_gpio is initialized in py_gpio.c, the last several lines
			*sys_gpio = *(physToGpioR3 + channel);			
		}
		else if(is_s500_platform())  //add for guitar
		{
			if (*(physToGpio_GT+channel) == -1)
			{
				PyErr_SetString(PyExc_ValueError, "The channel sent is invalid on a Lemaker Guitar");
				return 5;
			} 
			
			*gpio = *(physToGpio_GT+channel);        //pin_to_gpio is initialized in py_gpio.c, the last several lines
			*sys_gpio = *(physToGpio_GT + channel);
		}
		else
		{
			printf("Please use Banana Pro or LeMaker Guitar\n");
			return 3;
		}
	}
	else if (gpio_mode == BCM)
	{
		if(is_a20_platform())
		{
			if (*(pinTobcm_BP+channel) == -1 )
			{
				PyErr_SetString(PyExc_ValueError, "The channel sent is invalid on a Banana Pro");
				return 5;
			} 
			*gpio = *(pinTobcm_BP + channel);
			*sys_gpio = channel;
		} 
		else if(is_s500_platform())  //add for guitar
		{
			if (*(pinTobcm_GT+channel) == -1 )
			{
				PyErr_SetString(PyExc_ValueError, "The channel sent is invalid on a Lemaker Guitar");
				return 5;
			} 
			*gpio = *(pinTobcm_GT + channel);
			*sys_gpio = *(pinTobcm_GT + channel); 
		} 
		else 
		{
			printf("Please use Banana Pro or LeMaker Guitar\n");
                	return 3;
		}
	}
	else 
	{
		// setmode() has not been run
		PyErr_SetString(PyExc_RuntimeError, "Please set pin numbering mode using GPIO.setmode(GPIO.BOARD)");
		return 3;
	}

	debug("GPIO = %d,sys_gpio = %d\n", *gpio,*sys_gpio);
	
	return 0;
}

int support_event_detect(int gpio, const int *gpioEdge)
{
    int i = 0;
    
    if(NULL == gpioEdge)
        return -1;

    while(*(gpioEdge + i) != -1) 
    {
        if(gpio == *(gpioEdge + i))
            return 0;
        i++;
    }
    
    return -1;
}
