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
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "c_gpio.h"
#include <errno.h>

//add for A20 Banana Pro
#define GPIO_BASE_BP		(0x01C20000)//Keep pace with Wiringpi 	
#define SUNXI_GPIO_BASE		(0x01C20800)	

//add for S500 LeMaker Guitar
#define S500_CLOCK_BASE	(0xB0160000)
#define S500_GPIO_BASE	(0xB01B0000)

#define PAGE_SIZE  (4*1024)
#define BLOCK_SIZE (4*1024)

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

int setup(void)
{
	int mem_fd;
	uint8_t *gpio_mem, *clk_mem;
	if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0)
	{
		return SETUP_DEVMEM_FAIL;
	}

	if ((gpio_mem = (uint8_t *)malloc(BLOCK_SIZE + (PAGE_SIZE-1))) == NULL){
		return SETUP_MALLOC_FAIL;
	}
	
	if ((clk_mem = (uint8_t *)malloc(BLOCK_SIZE + (PAGE_SIZE-1))) == NULL){
                return SETUP_MALLOC_FAIL;
        }

	if ((uint32_t)gpio_mem % PAGE_SIZE)
		gpio_mem += PAGE_SIZE - ((uint32_t)gpio_mem % PAGE_SIZE);
	
	if ((uint32_t)clk_mem % PAGE_SIZE)
                clk_mem += PAGE_SIZE - ((uint32_t)clk_mem % PAGE_SIZE);


	//all the page must be 4kb for start
	if(is_a20_platform()){
		gpio_map = (uint32_t *)mmap( (caddr_t)gpio_mem, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_FIXED, mem_fd, GPIO_BASE_BP);
	}else if(is_s500_platform()){ //add for guitar
		gpio_map = (uint32_t *)mmap( (caddr_t)gpio_mem , BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_FIXED, mem_fd, S500_GPIO_BASE);
		clk_map = (uint32_t *)mmap( (caddr_t)clk_mem , BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_FIXED, mem_fd, S500_CLOCK_BASE);
	}else{
		printf("Please use Banana Pro or LeMaker Guitar\n");
                return -1;
	}

	debug ("gpio_mem = 0x%x\t gpio_map = 0x%x\n", (uint32_t)gpio_mem, (uint32_t)gpio_map); 
	
	if ((uint32_t)gpio_map < 0)
		return SETUP_MMAP_FAIL;
	return SETUP_OK;
}

//add for guitar
static uint32_t s500_readl(volatile uint32_t *addr) 
{
          uint32_t val = 0;
          val = *addr;
          return val;

}
static void s500_writel(uint32_t val, volatile uint32_t *addr)
{
        *addr = val;
}
//end

 uint32_t sunxi_readl(volatile uint32_t *addr)
{
	uint32_t val = 0;
	uint32_t mmap_base = (uint32_t)addr & (~MAP_MASK);
	uint32_t mmap_seek = ((uint32_t)addr - mmap_base) >> 2;
	val = *(gpio_map + mmap_seek);

	debug("mmap_base = 0x%x\t mmap_seek = 0x%x\t gpio_map = 0x%x\t total = 0x%x\n",mmap_base,mmap_seek,(uint32_t)gpio_map,(uint32_t)(gpio_map + mmap_seek));

	return val;
}

void sunxi_writel(uint32_t val, volatile uint32_t *addr)
{
  uint32_t mmap_base = (uint32_t)addr & (~MAP_MASK);
  uint32_t mmap_seek =( (uint32_t)addr - mmap_base) >> 2;
  *(gpio_map + mmap_seek) = val;
}

void sunxi_set_pullupdn(int gpio, int pud)//void sunxi_pullUpDnControl (int pin, int pud)
{
	uint32_t regval = 0;
	int bank = gpio >> 5;
	int index = gpio - (bank << 5);
	int sub = index >> 4;
	int sub_index = index - 16*sub;
	volatile uint32_t *phyaddr = (volatile uint32_t *)(SUNXI_GPIO_BASE + (bank * 36) + 0x1c + 4*sub); // +0x10 -> pullUpDn reg

	debug("func:%s gpio:%d,bank:%d index:%d sub:%d phyaddr:0x%x\n",__func__, gpio,bank,index,sub,(uint32_t)phyaddr); 
	
	regval = sunxi_readl(phyaddr);
	
	debug("pullUpDn reg:0x%x, pud:0x%x sub_index:%d\n", regval, pud, sub_index);
		
	regval &= ~(3 << (sub_index << 1));
	regval |= (pud << (sub_index << 1));
	
	debug("pullUpDn val ready to set:0x%x\n", regval);
		
	sunxi_writel(regval, phyaddr);
	regval = sunxi_readl(phyaddr);

	debug("pullUpDn reg after set:0x%x  addr:0x%x\n", regval, (uint32_t)phyaddr);
}

//add for guitar
void s500_set_pullupdn (int gpio, int pud)
{
	uint32_t regval = 0;
	volatile uint32_t *phyaddr = NULL;	

	switch(gpio)
	{
	case 25:/*SIRQ1/GPIOA25*/
            {
			phyaddr = gpio_map + (0x0060>>2); //0x0060/4 is bit to 32bit format,
			regval = s500_readl(phyaddr);
                     if(PUD_UP == pud)
                     {
                        regval |= (1 << 12); //bit12 bit13
                     }
                     else if(PUD_DOWN == pud)
                     {
                        regval |= (2 << 12); //bit12 bit13
                     }
                     else
                     {
                        regval &= ~(3 << 12);
                     }
                     s500_writel(regval, phyaddr);
            }
            break;
            
	case 40: /* KS_OUT1/GPIOB8 */
		{
			phyaddr = gpio_map + (0x0060>>2); //0x0060/4 is bit to 32bit format,
			regval = s500_readl(phyaddr);
			if(PUD_UP == pud)//Enable
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
	
	case 65: /* DSI_DN3/GPIOC1 */
                {
                        phyaddr = gpio_map + (0x0060>>2); //0x0060/4 is bit to 32bit format
                        regval = s500_readl(phyaddr);
                        if(PUD_UP == pud)//Enable
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
                        if(PUD_UP == pud)//Enable
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
                        if(PUD_UP == pud)//Enable
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
       
       case 86:/*SPI0_SCLK/GPIOC22*/
                {
                        phyaddr = gpio_map + (0x0068>>2); //0x0068/4 is bit to 32bit format
                        regval = s500_readl(phyaddr);
                        if(PUD_UP == pud)
                        {
                            regval |= (1 << 12);  //bit 12
                        }
                        else
                        {
                            regval &= ~(1 << 12); //bit 12
                        }
                        s500_writel(regval, phyaddr);
                }
                break;
                
       case 89:/*SPI0_MOSI/GPIOC25*/
                {
                        phyaddr = gpio_map + (0x0068>>2); //0x0068/4 is bit to 32bit format
                        regval = s500_readl(phyaddr);
                        if(PUD_UP == pud)
                        {
                            regval |= (1 << 11);  //bit 11
                        }
                        else
                        {
                            regval &= ~(1 << 11);  //bit 11
                        }
                        s500_writel(regval, phyaddr);
                }
                break;
	
	case 90: /* UART0_RX/GPIOC26 */
		{
			phyaddr = gpio_map + (0x0064>>2); //0x0064
			regval = s500_readl(phyaddr);			
			if(PUD_UP == pud)//Enable
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
			if(PUD_UP == pud)//Enable
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
			if(PUD_UP == pud)//Enable
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
			if(PUD_UP == pud)//Enable
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
	
	debug ("%s,%d,gpio:%d, set over reg val: 0x%x\n", __func__, __LINE__,gpio,regval) ;

	short_wait();
	 
	return ;
}

void setup_gpio(int gpio, int direction, int pud)//void sunxi_set_gpio_mode(int pin,int mode)
{
	uint32_t regval = 0;
	int bank = gpio >> 5;
	int index = gpio - (bank << 5);	
	int offset = 0;
	volatile uint32_t *phyaddr = NULL;
       volatile uint32_t *input_phyaddr = NULL;
       volatile uint32_t *output_phyaddr =NULL;
       
	if(is_a20_platform()){
		offset = ((index - ((index >> 3) << 3)) << 2);
		phyaddr = (uint32_t *)(SUNXI_GPIO_BASE + (bank * 36) + ((index >> 3) << 2));

		debug("func:%s gpio:%d, direction:%d bank:%d index:%d phyaddr:0x%x\n",__func__, gpio , direction,bank,index,(uint32_t)phyaddr); 
			
		regval = sunxi_readl(phyaddr);
		
		debug("read reg val: 0x%x offset:%d\n",regval,offset);
			
		sunxi_set_pullupdn(gpio, pud);
			
		if(INPUT == direction)
		{
			regval &= ~(7 << offset);
			sunxi_writel(regval, phyaddr);
			regval = sunxi_readl(phyaddr);
			
			debug("Input mode set over reg val: 0x%x\n",regval);
		}
		else if(OUTPUT == direction)
		{
		   regval &= ~(7 << offset);
		   regval |=  (1 << offset);
		   
		   debug("Out mode ready set val: 0x%x\n",regval);
		   
		   sunxi_writel(regval, phyaddr);
		   regval = sunxi_readl(phyaddr);
		   
		  debug("Out mode set over reg val: 0x%x\n",regval);
		}
		else
		{
			printf("line:%dgpio number error\n",__LINE__);
		}
	} else if(is_s500_platform()){ //add for guitar
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
		       input_phyaddr = gpio_map + (bank * 3) + 0x01; // +0x04(Byte) -> Input Enable Register
		       output_phyaddr = gpio_map + (bank * 3) + 0x00; // +0x00 -> Output Enable Register
		       
			if(INPUT == direction)
			{
			        regval = s500_readl(input_phyaddr);
			        regval |= (1 << index);
			        s500_writel(regval, input_phyaddr);
                    
			        debug("func:%s gpio:%d, MODE:%d bank:%d index:%d input_phyaddr:0x%x read reg val: 0x%x \n",__func__, gpio , direction,bank,index,(uint32_t)input_phyaddr,regval);
                             
			        regval = s500_readl(output_phyaddr);
			        regval &= ~(1 << index);
			        s500_writel(regval, output_phyaddr);
			
			        debug("func:%s gpio:%d, MODE:%d bank:%d index:%d output_phyaddr:0x%x read reg val: 0x%x \n",__func__, gpio , direction,bank,index,(uint32_t)output_phyaddr,regval);
			}
			else  /*OUTPUT*/
			{
			        regval = s500_readl(input_phyaddr);
			        regval &= ~(1 << index);
			        s500_writel(regval, input_phyaddr);
                    
			        debug("func:%s gpio:%d, MODE:%d bank:%d index:%d input_phyaddr:0x%x read reg val: 0x%x \n",__func__, gpio , direction,bank,index,(uint32_t)input_phyaddr,regval);
                             
			        regval = s500_readl(output_phyaddr);
			        regval |= (1 << index);
			        s500_writel(regval, output_phyaddr);
			
			        debug("func:%s gpio:%d, MODE:%d bank:%d index:%d output_phyaddr:0x%x read reg val: 0x%x \n",__func__, gpio , direction,bank,index,(uint32_t)output_phyaddr,regval);				
			}
		}
		else
		{
				//TODO
		}
	} else{
		printf("Please use Banana Pro or LeMaker Guitar\n");
                return;
	} 
}


// Contribution by Eric Ptak <trouch@trouch.com>
int gpio_function(int gpio)
{
	uint32_t regval = 0;
	int bank = gpio >> 5; //gpio/32
	int index = gpio - (bank << 5); //gpio - bank*32
	int offset = 0;
	volatile uint32_t *phyaddr = NULL;

	if(is_a20_platform()){
		offset = ((index - ((index >> 3) << 3)) << 2);
		phyaddr = (volatile uint32_t *)(SUNXI_GPIO_BASE + (bank * 36) + ((index >> 3) << 2));
		regval = sunxi_readl(phyaddr);
		
		debug("read reg val: 0x%x offset:%d\n",regval,offset);
			
		regval >>= offset;
		regval &= 7;

		debug("read reg val: 0x%x\n",regval);
			
	} else if(is_s500_platform()){ //add for guitar
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
			debug("func:%s gpio:%d, input reval:0x%x\n",__func__, gpio , regval);
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
			debug("func:%s gpio:%d, output reval:0x%x\n",__func__, gpio , regval);
			return 1;
		}

		//other function
		return 4;
	} else{
		printf("Please use Banana Pro or LeMaker Guitar\n");
                return -1;
	}


	return regval;// 1=input, 0=output, 4=alt0
}


void output_gpio(int gpio, int value)
{   
	uint32_t regval = 0;
	int bank = gpio >> 5;
	int index = gpio - (bank << 5);
	volatile uint32_t *phyaddr = NULL;
	if(is_a20_platform()){
		phyaddr = (volatile uint32_t *)(SUNXI_GPIO_BASE + (bank * 36) + 0x10); // +0x10 -> data reg

		debug("func:%s gpio:%d, value:%d bank:%d index:%d phyaddr:0x%x\n",__func__, gpio , value,bank,index,(uint32_t)phyaddr);

		regval = sunxi_readl(phyaddr);
		debug("before write reg val: 0x%x,index:%d\n",regval,index);
		
		if(0 == value)
		{
			regval &= ~(1 << index);
			sunxi_writel(regval, phyaddr);
			regval = sunxi_readl(phyaddr);
			
			debug("LOW val set over reg val: 0x%x\n",regval);
		}
		else
		{
			regval |= (1 << index);
			sunxi_writel(regval, phyaddr);
			regval = sunxi_readl(phyaddr);
			
			debug("HIGH val set over reg val: 0x%x\n",regval);
		}
	} else if(is_s500_platform()){//add for guitar
		phyaddr = (volatile uint32_t *)(gpio_map + (bank * 3) + 0x02); // +0x08 -> Output/Input Data Register
		debug("func:%s gpio:%d, value:%d bank:%d index:%d phyaddr:0x%x\n",__func__, gpio , value,bank,index,(uint32_t)phyaddr);
		regval = s500_readl(phyaddr);
		if(0 == value){
			regval &= ~(1 << index);
			s500_writel(regval, phyaddr);
			regval = sunxi_readl(phyaddr);
			
			debug("LOW val set over reg val: 0x%x\n",regval);
		} else{
			regval |= (1 << index);
			s500_writel(regval, phyaddr);
			regval = sunxi_readl(phyaddr);
			
			debug("HIGH val set over reg val: 0x%x\n",regval);
		}
	} else{
		printf("Please use Banana Pro or LeMaker Guitar\n");
                return;
	}
}


int input_gpio(int gpio)//int sunxi_digitalRead(int pin)
{
	uint32_t regval = 0;
	int bank = gpio >> 5;
	int index = gpio - (bank << 5);
	volatile uint32_t *phyaddr = NULL;
	if(is_a20_platform()){
		phyaddr = (volatile uint32_t *)(SUNXI_GPIO_BASE + (bank * 36) + 0x10); // +0x10 -> data reg
		
		debug("func:%s gpio:%d,bank:%d index:%d phyaddr:0x%x\n",__func__, gpio,bank,index, (uint32_t)phyaddr); 

		regval = sunxi_readl(phyaddr);
		regval = regval >> index;
		regval &= 1;
		
		debug("***** read reg val: 0x%x,bank:%d,index:%d,line:%d\n",regval,bank,index,__LINE__);
	} else if(is_s500_platform()){ //add for guitar

		phyaddr = gpio_map + (bank * 3) + 0x02; // +0x08 -> Output/Input Data Register
		
		debug("func:%s gpio:%d,bank:%d index:%d phyaddr:0x%x\n",__func__, gpio,bank,index,(uint32_t)phyaddr); 
		regval = s500_readl(phyaddr);
		regval = regval >> index;
		regval &= 0x1;
	
		debug("*****read %d read reg val: 0x%x,bank:%d,index:%d,line:%d\n",gpio,regval,bank,index,__LINE__);
	} else{
		printf("Please use Banana Pro or LeMaker Guitar\n");
                return 1;
	}
	
	return regval;
}


void cleanup(void)
{
    // fixme - set all gpios back to input
	if(is_a20_platform()){
		munmap((caddr_t)gpio_map, BLOCK_SIZE);
	} else if(is_s500_platform()){ //add for guitar
		munmap(0, BLOCK_SIZE);
	} else{
		printf("Please use Banana Pro or LeMaker Guitar\n");
                return;
	}
    
}
