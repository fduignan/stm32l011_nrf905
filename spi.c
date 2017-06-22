// spi.h for stm32l011
// Author: Frank Duignan.  

#include "stm32l011.h"
#include <stdint.h>
#include "spi.h"
void initSPI()
{
    int dummy;
    RCC_APB2ENR |= BIT12;		// turn on SPI1 			
	// GPIOA bits 5,6,7 are used for SPI1 (Alternative functions 0)
	RCC_AHBENR |= BIT18+BIT17;           // enable ports A and B
    GPIOA_MODER &= ~(BIT14+BIT12+BIT10); // select Alternative function
    GPIOA_MODER |= (BIT15+BIT13+BIT11);  // for bits 5,6,7    
    
    GPIOA_OSPEEDR |= BIT10+BIT11;        // Need to set SCK pin to high speed to fix
                                         // bug in silicon (see errata doc DocID028471 Rev 4)
    GPIOA_AFRL &= 0x000fffff;            // select Alt. Function 0
	
	// Now configure the SPI interface
	dummy = SPI1_SR;				// dummy read of SR to clear MODF
	// enable SSM, set SSI, enable SPI, PCLK/16 MSB First Master
	//SPI1_CR1 = BIT9+BIT8+BIT6+BIT5+BIT4+BIT3+BIT2; // Run really slow for debugging
	SPI1_CR1 = BIT9+BIT8+BIT6+BIT5+BIT2; // Set clock speed to 500kHz
	SPI1_CR2 = 0;//BIT10+BIT9+BIT8; 	// configure for 8 bit operation    
    
   
}

uint8_t transferSPI(uint8_t data)
{
    unsigned Timeout = 1000000;
    uint8_t ReturnValue;	
    while (((SPI1_SR & BIT7)!=0)&&(Timeout--)); // poll TXBusy
    SPI1_DR8 = data;
    Timeout = 1000000;    
    while (((SPI1_SR & BIT7)!=0)&&(Timeout--)); // poll TXBusy     
    Timeout = 1000000;    
    while (((SPI1_SR & BIT0)==0)&&(Timeout--)); // poll RXNE
    
    ReturnValue = SPI1_DR8;   
    return ReturnValue;
}

