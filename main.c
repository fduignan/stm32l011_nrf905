
/* 
NRF905 Radio base-station.  For use with remote NRF905 sensor nodes
Wiring:
   NRF905: Nucleo pin : STM32L011 Pin
    MOSI :      A_6   :    PA7
    MISO :      A_5   :    PA6
    SCK  :      A_4   :    PA5
    CSN  :      A_3   :    PA4
    CE   :      D13   :    PB3
    TXEN :      D12   :    PB4
    PWR  :      D11   :    PB5
    DR   :      D10   :    PA11
Not yet wired:
    CD    
    AM
*/


#include "stm32l011.h"
#include "serial.h"
#include "spi.h"
#include "nrf905.h"
const uint8_t BaseStationAddr[]={0xde,0xad,0xb0,0x55};
uint8_t RxPkt[32];
void delay(unsigned);

void delay(unsigned dly)
{
  while( dly--);
}


void initClockHSI16()
{
    // Use the HSI16 clock as the system clock - allows operation down to 1.5V
        RCC_CR &= ~BIT24;
        RCC_CR |= BIT0; // turn on HSI16 (16MHz clock)
        while ((RCC_CR & BIT2)==0); // wait for HSI to be ready
        // set HSI16 as system clock source 
        RCC_CFGR |= BIT0;
}
void configPins()
{
	// Enable PORTB where LED is connected
	RCC_IOPENR |= BIT1;
	GPIOB_MODER |= BIT6; // make bit3  an output
	GPIOB_MODER &= ~BIT7; // make bit3  an output
}	
void TxPacket(uint8_t *Pkt,int len)
{
    
    PwrHigh();
    TXEnHigh();
    delay(1000000);
    writeTXPayload(Pkt,len);
    setTXAddress(BaseStationAddr);
    CEHigh();
    while( (getStatus()&0x20) == 0); // wait for tx to complete
   // CELow();
   // PwrLow();
   // TXEnLow();
}
uint8_t pkg[]="Hello World";
int main()
{
    uint32_t Counter=0;
    int i=0;
    NRF905 nrf;
	initClockHSI16();
	configPins(); 
    initUART(9600);
    initSPI();
    initNRF905();
    setTXAddress(BaseStationAddr);
    setRXAddress(BaseStationAddr);

    PwrHigh();


// Set Frequency to 868MHz  
    setChannel(0x76);
    setRange(0);
    setRXPower(0); // normal power level
    setTXPower(3);
    setAutoRetran(0);    
    writeRegister(9,0x58); // 8 bit CRC, enable CRC, 16MHz crystal, Not using clock output
    // Go to listen mode
    CEHigh();
    TXEnLow(); 
    while(1)
	{                       
        readRegisters(&nrf);             
        //if (nrf.Status & 0x60) 
        if (DataReady()) {           
            /*eputs("\r\n");
            eputs("St: ");
            printByte(nrf.Status);    
            eputs(" 0:");
            printByte(nrf.Register[0]);                
            eputs(" 1:");
            printByte(nrf.Register[1]);
            eputs(" 2:");
            printByte(nrf.Register[2]);
            eputs(" 3:");
            printByte(nrf.Register[3]);
            eputs(" 4:");
            printByte(nrf.Register[4]);
            eputs(" 5:");
            printByte(nrf.Register[5]);
            eputs(" 6:");
            printByte(nrf.Register[6]);
            eputs(" 7:");
            printByte(nrf.Register[7]);
            eputs(" 8:");
            printByte(nrf.Register[8]);
            eputs(" 9:");
            printByte(nrf.Register[9]);    
            delay(2000000);*/
            readRXPayload(RxPkt,32);
            eputs(RxPkt);
            eputs("\r\n");
           // delay(20000);

        }
        
    }
	return 0;
}








