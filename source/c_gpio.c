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

#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "c_gpio.h"
#include <errno.h>

//add for A20 Banana Pro
#define GPIO_BASE_BP		(0x01C20000)//Keep pace with Wiringpi 	
#define SUNXI_GPIO_BASE		(0x01C20800)	

//add for S500 LeMaker Guitar
#define S500_CLOCK_BASE_BP	(0xB0160000)
#define S500_GPIO_BASE_BP	(0xB01B0000)

#define PAGE_SIZE  (4*1024)
#define BLOCK_SIZE (4*1024)

extern int f_a20;
extern int f_s500;
#define	MAX_BUF	1024

#define MAP_SIZE	(4096*2)
#define MAP_MASK	(MAP_SIZE - 1)


static volatile uint32_t *gpio_map;
static volatile uint32_t *clk_map;

void short_wait(void)
{
	int i;
	for (i=0; i<150; i++)     // wait 150 cycles
	{
		asm volatile("nop");
	}
}

extern int errno;
int setup(void)
{
	int mem_fd;
	uint8_t *gpio_mem,clk_mem;
	if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0)
	{
		return SETUP_DEVMEM_FAIL;
	}

	if ((gpio_mem = malloc(BLOCK_SIZE + (PAGE_SIZE-1))) == NULL){
		return SETUP_MALLOC_FAIL;
	}
	
	if ((clk_mem = malloc(BLOCK_SIZE + (PAGE_SIZE-1))) == NULL){
                return SETUP_MALLOC_FAIL;
        }

	if ((uint32_t)gpio_mem % PAGE_SIZE)
		gpio_mem += PAGE_SIZE - ((uint32_t)gpio_mem % PAGE_SIZE);
	
	if ((uint32_t)clk_mem % PAGE_SIZE)
                clk_mem += PAGE_SIZE - ((uint32_t)clk_mem % PAGE_SIZE);


	//all the page must be 4kb for start
	if(f_a20){
		gpio_map = (uint32_t *)mmap( (caddr_t)gpio_mem, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_FIXED, mem_fd, GPIO_BASE_BP);
	}else{ //add for guitar
		gpio_map = (uint32_t *)mmap( (caddr_t)gpio_mem , BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_FIXED, mem_fd, S500_GPIO_BASE_BP);
		clk_map = (uint32_t *)mmap( (caddr_t)clk_mem , BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_FIXED, mem_fd, S500_CLOCK_BASE_BP);
	}

	if(lemakerDebug)
		printf("gpio_mem = 0x%x\t gpio_map = 0x%x\n",gpio_mem,gpio_map);
	if ((uint32_t)gpio_map < 0)
		return SETUP_MMAP_FAIL;
	return SETUP_OK;
}

//add for guitar
static uint32_t s500_readl(uint32_t *addr)
{
          uint32_t val = 0;
          val = *addr;
          return val;

}
static void s500_writel(uint32_t val, uint32_t *addr)
{
        *addr = val;
}
//end

 uint32_t sunxi_readl(uint32_t addr)
{
	uint32_t val = 0;
	uint32_t mmap_base = (addr & ~MAP_MASK);
	uint32_t mmap_seek = ((addr - mmap_base) >> 2);
	val = *(gpio_map + mmap_seek);
	if(lemakerDebug)
		printf("mmap_base = 0x%x\t mmap_seek = 0x%x\t gpio_map = 0x%x\t total = 0x%x\n",mmap_base,mmap_seek,gpio_map,(gpio_map + mmap_seek));

	return val;
}


void sunxi_writel(uint32_t val, uint32_t addr)
{
  uint32_t mmap_base = (addr & ~MAP_MASK);
  uint32_t mmap_seek = ((addr - mmap_base) >> 2);
  *(gpio_map + mmap_seek) = val;
}


void clear_event_detect(int gpio)
{
	if(f_a20 || f_s500) {
D		return;
	}
}


int eventdetected(int gpio)
{  
	if(f_a20 || f_s500) {
D		return 0;
	}
}


void set_rising_event(int gpio, int enable)
{
	if(f_a20 || f_s500) {
D		return;
	}
}


void set_falling_event(int gpio, int enable)
{
	if(f_a20 || f_s500) {
D		return;
	}
}


void set_high_event(int gpio, int enable)
{
	if(f_a20 || f_s500) {
D		return;
	}
}


void set_low_event(int gpio, int enable)
{
	if(f_a20 || f_s500) {
D		return;
	}
}


void sunxi_set_pullupdn(int gpio, int pud)//void sunxi_pullUpDnControl (int pin, int pud)
{
	uint32_t regval = 0;
	int bank = gpio >> 5;
	int index = gpio - (bank << 5);
	int sub = index >> 4;
	int sub_index = index - 16*sub;
	uint32_t phyaddr = SUNXI_GPIO_BASE + (bank * 36) + 0x1c + 4*sub; // +0x10 -> pullUpDn reg

	if (lemakerDebug)
		printf("func:%s gpio:%d,bank:%d index:%d sub:%d phyaddr:0x%x\n",__func__, gpio,bank,index,sub,phyaddr); 
	
	regval = sunxi_readl(phyaddr);
	if (lemakerDebug)
		printf("pullUpDn reg:0x%x, pud:0x%x sub_index:%d\n", regval, pud, sub_index);
	regval &= ~(3 << (sub_index << 1));
	regval |= (pud << (sub_index << 1));
	if (lemakerDebug)
		printf("pullUpDn val ready to set:0x%x\n", regval);
	sunxi_writel(regval, phyaddr);
	regval = sunxi_readl(phyaddr);
	if (lemakerDebug)
		printf("pullUpDn reg after set:0x%x  addr:0x%x\n", regval, phyaddr);
}

//add for guitar
void s500_set_pullupdn (int gpio, int pud)
{
	uint32_t regval = 0;
	uint32_t *phyaddr = NULL;	
	pud &= 0x01;

	switch(gpio)
	{
	case 40: /* KS_OUT1/GPIOB8 */
		{
			phyaddr = gpio_map + (0x0060>>2); //0x0060/4 is bit to 32bit format,
			regval = s500_readl(phyaddr);
			if(pud)//Enable
			{
				regval |= (1 << 1); // bit 1
			}
			else//Disable
			{
				regval &= ~(1 << 1);
			}
			s500_writel(regval, phyaddr);
		}
		break;

	case 41: /* KS_OUT2/GPIOB9 */
		{
			phyaddr = gpio_map + (0x0060>>2); //0x0060/4 is bit to 32bit format
			regval = s500_readl(phyaddr);			
			if(pud)//Enable
			{
				regval |= (1 << 28); // bit 28
			}
			else//Disable
			{
				regval &= ~(1 << 28);
			}
			s500_writel(regval, phyaddr);
		}
		break;
	
	case 65: /* DSI_DN3/GPIOC1 */
                {
                        phyaddr = gpio_map + (0x0060>>2); //0x0060/4 is bit to 32bit format
                        regval = s500_readl(phyaddr);
                        if(pud)//Enable
                        {
                                regval |= (1 << 26); // bit 26
                        }
                        else//Disable
                        {
                                regval &= ~(1 << 26);
                        }
                        s500_writel(regval, phyaddr);
                }
                break;

	case 68: /* DSI_CP/GPIOC4 */
                {
                        phyaddr = gpio_map + (0x0064>>2); //0x0064/4 is bit to 32bit format
                        regval = s500_readl(phyaddr);
                        if(pud)//Enable
                        {
                                regval |= (1 << 31); // bit 31
                        }
                        else//Disable
                        {
                                regval &= ~(1 << 31);
                        }
                        s500_writel(regval, phyaddr);
                }
                break;
	
	case 69: /* DSI_CN/GPIOC5 */
                {
                        phyaddr = gpio_map + (0x0064>>2); //0x0064/4 is bit to 32bit format
                        regval = s500_readl(phyaddr);
                        if(pud)//Enable
                        {
                                regval |= (1 << 30); // bit 30
                        }
                        else//Disable
                        {
                                regval &= ~(1 << 30);
                        }
                        s500_writel(regval, phyaddr);
                }
                break;
	
	case 90: /* UART0_RX/GPIOC26 */
		{
			phyaddr = gpio_map + (0x0064>>2); //0x0064
			regval = s500_readl(phyaddr);			
			if(pud)//Enable
			{
				regval |= (1 << 2); // bit 2
			}
			else//Disable
			{
				regval &= ~(1 << 2);
			}
			s500_writel(regval, phyaddr);
		}
		break;

	case 91: /* UART0_TX/GPIOC27 */
		{
			phyaddr = gpio_map + (0x0064>>2);
			regval = s500_readl(phyaddr);			
			if(pud)//Enable
			{
				regval |= (1 << 1); // bit 1
			}
			else//Disable
			{
				regval &= ~(1 << 1);
			}
			s500_writel(regval, phyaddr);
		}
		break;
		
	case 130: /* TWI2_SCK/GPIOE2 */
		{
			phyaddr = gpio_map + (0x0068>>2);
			regval = s500_readl(phyaddr);			
			if(pud)//Enable
			{
				regval |= (1 << 7); // bit 7
			}
			else//Disable
			{
				regval &= ~(1 << 7);
			}
			s500_writel(regval, phyaddr);
		}
		break;

	case 131: /* TWI2_SDA/GPIOE3  */
		{
			phyaddr = gpio_map + (0x0068>>2);
			regval = s500_readl(phyaddr);			
			if(pud)//Enable
			{
				regval |= (1 << 8); // bit 8
			}
			else//Disable
			{
				regval &= ~(1 << 8);
			}
			s500_writel(regval, phyaddr);
		}
		break;

	default:
		return;
	}
	
	if(lemakerDebug)
	{
		printf ("%s,%d,gpio:%d, set over reg val: 0x%x\n", __func__, __LINE__,gpio,regval) ;
	}

	short_wait();
	 
	return ;
}

void s500_pwm_set_enable(int en)
{
	uint32_t val = 0;
	uint32_t *phyaddr = NULL;

	//Disable output
	phyaddr = gpio_map + (0x000C>>2); 
	val = s500_readl(phyaddr);
	val &= ~(0x100);
	s500_writel(val, phyaddr);

	//Disable input
	phyaddr = gpio_map + (0x0010>>2); 
	val = s500_readl(phyaddr);
	val &= ~(0x100);
	s500_writel(val, phyaddr);

	//MFP_CTL1
	phyaddr = gpio_map + (0x0044>>2);
	val = s500_readl(phyaddr);
	val &= 0xFC7FFFFF;
	val |= 0x01800000;
	s500_writel(val, phyaddr);
	
}

void s500_pwm_set_mode(int mode)
{
	//Nothing
}

void s500_pwm_set_clk_source(int select)
{
	uint32_t regval = 0;
	uint32_t *phyaddr = clk_map + (0x007C>>2); 

	regval = s500_readl(phyaddr);
	if(select == 0)//IC_32K
	{
		regval &= ~(1<<12 );
	}
	else//HOSC 24M
	{
		regval |= (1<<12);
	}

	s500_writel(regval, phyaddr);

	if(lemakerDebug)
	{
		printf(">>function%s,no:%d,clk sel :0x%x\n",__func__, __LINE__, regval);
	}
	
	short_wait();
}

/*
 clk_div 范围: 0~1023
*/
void s500_pwm_set_clk(int clk_div)
{
	uint32_t regval = 0;
	int temp;
	uint32_t *phyaddr = clk_map + (0x007C>>2); 

	regval = s500_readl(phyaddr);
	regval &= (1 <<12);
	
	temp = clk_div;
	temp |=  regval;
	s500_writel(temp, phyaddr);

	if(lemakerDebug)
	{
		printf(">>function%s,no:%d,clk sel :0x%x\n",__func__, __LINE__, temp);
	}
	
	short_wait();
}

void s500_pwm_set_period(int period)
{
	uint32_t val = 0;
	uint32_t *phyaddr = gpio_map + ((0x0050+4*3)>>2); 

	period &= 0x3FF; //set max period to 2^10
	val = s500_readl(phyaddr);
	val &= 0x1FFC00;
	period |= val;
	s500_writel(period, phyaddr);
	
	if (lemakerDebug)
	{
		printf(">>func:%s,no:%d,period/range:%d\n",__func__,__LINE__,period);
	}

	short_wait();
}

void s500_pwm_set_act(int act)
{
	uint32_t val = 0;
	uint32_t *phyaddr = gpio_map + ((0x0050+4*3)>>2); 

	act &= 0x3FF; //set max period to 2^10
	val = s500_readl(phyaddr);
	val &= 0x1003FF;
	act <<= 10;
	act |= val;
	s500_writel(act, phyaddr);
	
	if (lemakerDebug)
	{
		printf(">>func:%s,no:%d,act:%d\n",__func__,__LINE__,act);
	}

	short_wait();
}
//end

void setup_gpio(int gpio, int direction, int pud)//void sunxi_set_gpio_mode(int pin,int mode)
{
	uint32_t regval = 0;
	int bank = gpio >> 5;
	int index = gpio - (bank << 5);	
	int offset = 0,t = 200;
	uint32_t phyaddr = NULL;
	if(f_a20){
		offset = ((index - ((index >> 3) << 3)) << 2);
		phyaddr = SUNXI_GPIO_BASE + (bank * 36) + ((index >> 3) << 2);
		if (lemakerDebug)
			printf("func:%s gpio:%d, direction:%d bank:%d index:%d phyaddr:0x%x\n",__func__, gpio , direction,bank,index,phyaddr); 
			
		regval = sunxi_readl(phyaddr);
		if (lemakerDebug)
			printf("read reg val: 0x%x offset:%d\n",regval,offset);
			
		sunxi_set_pullupdn(gpio, pud);
			
		if(INPUT == direction)
		{
			regval &= ~(7 << offset);
			sunxi_writel(regval, phyaddr);
			regval = sunxi_readl(phyaddr);
			if (lemakerDebug)
				printf("Input mode set over reg val: 0x%x\n",regval);
		}
		else if(OUTPUT == direction)
		{
		   regval &= ~(7 << offset);
		   regval |=  (1 << offset);
		   if (lemakerDebug)
				printf("Out mode ready set val: 0x%x\n",regval);
		   sunxi_writel(regval, phyaddr);
		   regval = sunxi_readl(phyaddr);
		   if (lemakerDebug)
				printf("Out mode set over reg val: 0x%x\n",regval);
		}
		else
		{
			printf("line:%dgpio number error\n",__LINE__);
		}
	} else{ //add for guitar
		if(gpio == 42 || gpio == 45 || gpio == 46|| gpio == 47 || gpio == 48 || gpio == 50 || gpio == 51)
		{//lvds port must be set digital function.The default function is LVDS ODD PAD.
			phyaddr = gpio_map + (0x0044>>2);
			regval = s500_readl(phyaddr);
			regval |= (1 << 22);
			regval &= ~(1 << 21);
			s500_writel(regval, phyaddr);
		} else if(gpio == 64 || gpio == 65){
			phyaddr = gpio_map + (0x0044>>2);
                        regval = s500_readl(phyaddr);
			if(gpio == 64){
                        regval |= (1 << 13);
                        regval |= (1 << 12);
			} else{
			regval |= (1 << 11);
                        regval |= (1 << 10);
			}
                        s500_writel(regval, phyaddr);
		} else if(gpio == 68 || gpio == 69){
			phyaddr = gpio_map + (0x0048>>2);
                        regval = s500_readl(phyaddr);
                        regval |= (1 << 30); 
                        regval &= ~(1 << 29);
                        s500_writel(regval, phyaddr);
		}
		s500_set_pullupdn(gpio, pud);
		if(INPUT == direction || OUTPUT == direction)
		{
			if(INPUT == direction)
			{
				phyaddr = gpio_map + (bank * 3) + 0x01; // +0x04(Byte) -> Input Enable Register
			}
			else
			{
				phyaddr = gpio_map + (bank * 3) + 0x00; // +0x00 -> Output Enable Register
			}
			regval = s500_readl(phyaddr);
			if (lemakerDebug)
			{
				printf("func:%s gpio:%d, MODE:%d bank:%d index:%d phyaddr:0x%x read reg val: 0x%x \n",__func__, gpio , direction,bank,index,phyaddr,regval);
			}
			regval |= (1 << index);
			s500_writel(regval, phyaddr);
			if (lemakerDebug)
			{
				regval = s500_readl(phyaddr);
				printf("set over reg val: 0x%x\n",regval);
			}
		} else if(PWM_OUTPUT == direction){
			if(gpio != 40)
			{
			printf("the gpio you choose is not surport hardware PWM\n");
			printf("you can select KS_OUT1/GPIOB8 for PWM gpio\n");
			printf("or you can use it in softPwm mode\n");
			return ;
			}


			s500_pwm_set_enable(1);
			//set default M:S to 1/2
			s500_pwm_set_clk_source(1); //default clk:24M
			s500_pwm_set_clk(120); // 24M/120
			s500_pwm_set_period(1023);
			s500_pwm_set_act(512);
			do{short_wait();}while(t--);
			
			printf("PWM_OUTPUT...............\n");
			}
		else
		{
				//TODO
		}
	} 
}


// Contribution by Eric Ptak <trouch@trouch.com>
int gpio_function(int gpio)
{
	uint32_t regval = 0;
	int bank = gpio >> 5; //gpio/32
	int index = gpio - (bank << 5); //gpio - bank*32
	//int offset = ((index - ((index >> 3) << 3)) << 2);
	//uint32_t phyaddr = SUNXI_GPIO_BASE + (bank * 36) + ((index >> 3) << 2);
	int offset = 0;
	uint32_t phyaddr = NULL;

	if(f_a20){
		offset = ((index - ((index >> 3) << 3)) << 2);
		phyaddr = SUNXI_GPIO_BASE + (bank * 36) + ((index >> 3) << 2);
		regval = sunxi_readl(phyaddr);
		if (lemakerDebug)
			printf("read reg val: 0x%x offset:%d\n",regval,offset);
		regval >>= offset;
		regval &= 7;
		if (lemakerDebug)
			printf("read reg val: 0x%x\n",regval);
	} else{ //add for guitar
		offset = 0;
		//input
		//GPIOA input enable:0x0004
		//GPIOB input enable:0x0010
		//GPIOC input enable:0x001C
		//GPIOD input enable:0x0028
		//GPIOE input enable:0x0034
		phyaddr = gpio_map + (bank * 3) + 0x1;
		regval = s500_readl(phyaddr);
		regval = (regval >> index)&0x1;
		if(regval == 1)
		{
			if(lemakerDebug)
				printf("func:%s gpio:%d, input reval:0x%x\n",__func__, gpio , regval);
			return 0;
		}
		//Output
		//GPIOA output enable:0x0000
		//GPIOB output enable:0x000C
		//GPIOC output enable:0x0018
		//GPIOD output enable:0x0024
		//GPIOE output enable:0x0030
		phyaddr = gpio_map + (bank * 3) + 0x00; // +0x00 -> Output Enable Register
		regval = s500_readl(phyaddr);
		regval = (regval >> index)&0x1;
		if(regval == 1)
		{
			if(lemakerDebug)
				printf("func:%s gpio:%d, output reval:0x%x\n",__func__, gpio , regval);
			return 1;
		}

		//other function
		return 4;
	}


	return regval;// 1=input, 0=output, 4=alt0
}


void output_gpio(int gpio, int value)//void sunxi_digitalWrite(int pin, int value)
{   
	uint32_t regval = 0;
	int bank = gpio >> 5;
	int index = gpio - (bank << 5);
	uint32_t phyaddr = NULL;
	if(f_a20){
		phyaddr = SUNXI_GPIO_BASE + (bank * 36) + 0x10; // +0x10 -> data reg
		if (lemakerDebug)
			printf("func:%s gpio:%d, value:%d bank:%d index:%d phyaddr:0x%x\n",__func__, gpio , value,bank,index,phyaddr);

		regval = sunxi_readl(phyaddr);
		if (lemakerDebug)
			printf("before write reg val: 0x%x,index:%d\n",regval,index);
		if(0 == value)
		{
			regval &= ~(1 << index);
			sunxi_writel(regval, phyaddr);
			regval = sunxi_readl(phyaddr);
			if (lemakerDebug)
				printf("LOW val set over reg val: 0x%x\n",regval);
		}
		else
		{
			regval |= (1 << index);
			sunxi_writel(regval, phyaddr);
			regval = sunxi_readl(phyaddr);
			if (lemakerDebug)
				printf("HIGH val set over reg val: 0x%x\n",regval);
		}
	} else{//add for guitar
		phyaddr = gpio_map + (bank * 3) + 0x02; // +0x08 -> Output/Input Data Register
		if (lemakerDebug)
			printf("func:%s gpio:%d, value:%d bank:%d index:%d phyaddr:0x%x\n",__func__, gpio , value,bank,index,phyaddr);
		regval = s500_readl(phyaddr);
		if(0 == value){
			regval &= ~(1 << index);
			s500_writel(regval, phyaddr);
			regval = sunxi_readl(phyaddr);
				if (lemakerDebug)
					printf("LOW val set over reg val: 0x%x\n",regval);
		} else{
			regval |= (1 << index);
			s500_writel(regval, phyaddr);
			regval = sunxi_readl(phyaddr);
				if (lemakerDebug)
					printf("HIGH val set over reg val: 0x%x\n",regval);
		}
	}
}


int input_gpio(int gpio)//int sunxi_digitalRead(int pin)
{
	uint32_t regval = 0;
	int bank = gpio >> 5;
	int index = gpio - (bank << 5);
	uint32_t phyaddr = NULL;
	if(f_a20){
		phyaddr = SUNXI_GPIO_BASE + (bank * 36) + 0x10; // +0x10 -> data reg
		if (lemakerDebug)
			printf("func:%s gpio:%d,bank:%d index:%d phyaddr:0x%x\n",__func__, gpio,bank,index,phyaddr); 

		regval = sunxi_readl(phyaddr);
		regval = regval >> index;
		regval &= 1;
		if (lemakerDebug)
			printf("***** read reg val: 0x%x,bank:%d,index:%d,line:%d\n",regval,bank,index,__LINE__);
	} else{ //add for guitar

		phyaddr = gpio_map + (bank * 3) + 0x02; // +0x08 -> Output/Input Data Register
		if (lemakerDebug)
			printf("func:%s gpio:%d,bank:%d index:%d phyaddr:0x%x\n",__func__, gpio,bank,index,phyaddr); 
		regval = s500_readl(phyaddr);
		regval = regval >> index;
		regval &= 0x1;
		if (lemakerDebug)
			printf("*****read %d read reg val: 0x%x,bank:%d,index:%d,line:%d\n",gpio,regval,bank,index,__LINE__);
	}	
	return regval;
}


void cleanup(void)
{
    // fixme - set all gpios back to input
	if(f_a20){
		munmap((caddr_t)gpio_map, BLOCK_SIZE);
	} else{ //add for guitar
		munmap(0, BLOCK_SIZE);
	}
    
}
