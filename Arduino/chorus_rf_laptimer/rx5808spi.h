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

#include <SPI.h>
#include "wiring_private.h" 

// rx5808 module needs >30ms to tune.
#define MIN_TUNE_TIME 30

SPIClass rxSPI (&sercom1, 12, 13, 11, SPI_PAD_0_SCK_1, SERCOM_RX_PAD_3);

void setupSPIpins() {

    // do this first, for Reasons
    rxSPI.begin();
   
    // Assign pins 11, 12, 13 to SERCOM functionality
    pinPeripheral(11, PIO_SERCOM);
    pinPeripheral(12, PIO_SERCOM);
    pinPeripheral(13, PIO_SERCOM);
    
}

uint16_t setModuleFrequency(uint16_t frequency) {
    Serial.print(F("Setup module frequency: "));
    Serial.print(frequency);
    uint8_t i;
    uint16_t channelData;
    uint8_t message_type_command = 0x10;
    uint8_t message_type_alert = 0x40;
    uint16_t command_id_0 = 0x0000;

    channelData = frequency - 479;
    channelData /= 2;
    i = channelData % 32;
    channelData /= 32;
    channelData = (channelData << 7) + i;

    // bit bang out 25 bits of data
    // Order: A0-3, !R/W, D0-D19
    // A0=0, A1=0, A2=0, A3=1, RW=0, D0-19=0
    //cli();
    rxSPI.beginTransaction(SPISettings(4000000, LSBFIRST, SPI_MODE0));
    rxSPI.transfer(0);
    rxSPI.transfer(0);
    rxSPI.transfer(0);
    rxSPI.transfer(1);
    
    rxSPI.transfer(0);

    // remaining zeros
    for (i = 20; i > 0; i--) {
        rxSPI.transfer(0);
    }
    rxSPI.endTransaction();

    // Second is the channel data from the lookup table
    // 20 bytes of register data are sent, but the MSB 4 bits are zeros
    // register address = 0x1, write, data0-15=channelData data15-19=0x0

    // Register 0x1
    
    rxSPI.beginTransaction(SPISettings(4000000, LSBFIRST, SPI_MODE0));
    rxSPI.transfer(1);
    rxSPI.transfer(0);
    rxSPI.transfer(0);
    rxSPI.transfer(0);
    
    // Write to register
    rxSPI.transfer(1);

    // D0-D15
    //   note: loop runs backwards as more efficent on AVR
    /*for (i = 16; i > 0; i--) {
        // Is bit high or low?
        if (channelData & 0x1) {
            SERIAL_SENDBIT1();
        }
        else {
            SERIAL_SENDBIT0();
        }
        // Shift bits along to check the next one
        channelData >>= 1;
    }*/
    rxSPI.transfer16(channelData);

    // Remaining D16-D19
    for (i = 4; i > 0; i--) {
        rxSPI.transfer(0);
    }

    rxSPI.endTransaction();
    
    delay(MIN_TUNE_TIME);

    Serial.println(F("Done!"));
    
    return frequency;
}

uint16_t setModuleChannel(uint8_t channel, uint8_t band) {
    uint16_t frequency = pgm_read_word_near(channelFreqTable + channel + (8 * band));
    return setModuleFrequency(frequency);
}
