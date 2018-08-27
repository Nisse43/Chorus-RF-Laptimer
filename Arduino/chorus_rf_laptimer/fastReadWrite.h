/**
 * DIY RF Laptimer by Andrey Voroshkov (bshep)
 * SPI driver based on fs_skyrf_58g-main.c by Simon Chambers
 * fast ADC reading code is by "jmknapp" from Arduino forum
 * fast port I/O code from http://masteringarduino.blogspot.com.by/2013/10/fastest-and-smallest-digitalread-and.html

The MIT License (MIT)

Copyright (c) 2016 by Andrey Voroshkov (bshep)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

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

/*I have been trying to set the Feather M0 up as an SPI slave, 
 using SCK (PB11), MOSI (PB10) and MISO (PA12). Attached is some code 
 by Nick Gammon, appearing to work on boards based on ATmega328 chips. However lines*/

#if (SAML21) || (SAML22) || (SAMC20) || (SAMC21) || (SAM) || defined(__DOXYGEN__) || defined(__SAMD51__)
#define digitalLow(P) digitalWrite(P, LOW)
#define digitalHigh(P) digitalWrite(P, HIGH)
#define sei() interrupts()
#define cli() noInterrupts()
//pinMode (spiDataPin, OUTPUT); (MISO)
#define SPI_DATA PORT_PA19
#define spiDataDirOut REG_PORT_DIRSET0 = SPI_DATA;
#define spiDataHigh REG_PORT_OUTSET0 = SPI_DATA;
#define spiDataLow REG_PORT_OUTCLR0 = SPI_DATA;
//pinMode (slaveSelectPin, OUTPUT); (MOSI)
#define SLAVE PORT_PA16
#define slaveSelectDirOut REG_PORT_DIRSET0 = SLAVE;
#define slaveSelectHigh REG_PORT_OUTSET0 = SLAVE;
#define slaveSelectLow REG_PORT_OUTCLR0 = SLAVE;
//pinMode (spiClockPin, OUTPUT); (SCK) 
#define SPI_CLOCK PORT_PA17
#define spiClockDirOut REG_PORT_DIRSET0 = SPI_CLOCK;
#define spiClockHigh REG_PORT_OUTSET0 = SPI_CLOCK;
#define spiClockLow REG_PORT_OUTCLR0 = SPI_CLOCK;
    
#else
#define portOfPin(P)\
  (((P)>=0&&(P)<8)?&PORTD:(((P)>7&&(P)<14)?&PORTB:&PORTC))
#define ddrOfPin(P)\
  (((P)>=0&&(P)<8)?&DDRD:(((P)>7&&(P)<14)?&DDRB:&DDRC))
#define pinOfPin(P)\
  (((P)>=0&&(P)<8)?&PIND:(((P)>7&&(P)<14)?&PINB:&PINC))
#define pinIndex(P)((uint8_t)(P>13?P-14:P&7))
#define pinMask(P)((uint8_t)(1<<pinIndex(P)))

#define pinAsInput(P) *(ddrOfPin(P))&=~pinMask(P)
#define pinAsInputPullUp(P) *(ddrOfPin(P))&=~pinMask(P);digitalHigh(P)
#define pinAsOutput(P) *(ddrOfPin(P))|=pinMask(P)
#define digitalLow(P) *(portOfPin(P))&=~pinMask(P)
#define digitalHigh(P) *(portOfPin(P))|=pinMask(P)
#define digitalToggle(P) *(portOfPin(P))^=pinMask(P)
#define isHigh(P)((*(pinOfPin(P))& pinMask(P))>0)
#define isLow(P)((*(pinOfPin(P))& pinMask(P))==0)
#define digitalState(P)((uint8_t)isHigh(P))
#endif
