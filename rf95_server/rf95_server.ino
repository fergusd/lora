// rf95_server.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing server
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95  if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf95_client
// Tested with Anarduino MiniWirelessLoRa, Rocket Scream Mini Ultra Pro with
// the RFM95W, Adafruit Feather M0 with RFM95

#include <SPI.h>
#include <RH_RF95.h>

// Singleton instance of the radio driver
//RH_RF95 rf95;
RH_RF95 rf95(5, 2); // Rocket Scream Mini Ultra Pro with the RFM95W
//RH_RF95 rf95(8, 3); // Adafruit Feather M0 with RFM95 

// Need this on Arduino Zero with SerialUSB port (eg RocketScream Mini Ultra Pro)
#define Serial SerialUSB

int led = 9;

void setup() 
{
  // Rocket Scream Mini Ultra Pro with the RFM95W only:
  // Ensure serial flash is not interfering with radio communication on SPI bus
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);

  // LED
  pinMode(LED_BUILTIN, OUTPUT);
      
  Serial.begin(9600);
  //while (!Serial) ; // Wait for serial port to be available
  if (!rf95.init())
    Serial.println("init failed");
  rf95.setFrequency(865.0);  
  RH_RF95::ModemConfig modem_config = {
  0x78, // Reg 0x1D: BW=125kHz, Coding=4/8, Header=explicit
  0xc4, // Reg 0x1E: Spread=4096chips/symbol, CRC=enable
  0x0c  // Reg 0x26: LowDataRate=On, Agc=On
  };
  rf95.setModemRegisters(&modem_config);
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
  // If you are using Modtronix inAir4 or inAir9,or any other module which uses the
  // transmitter RFO pins and not the PA_BOOST pins
  // then you can configure the power transmitter power for -1 to 14 dBm and with useRFO true. 
  // Failure to do that will result in extremely low transmit powers.
//  driver.setTxPower(14, true);
  Serial.println("start");
}

uint32_t flashCount=0;

void flash(uint16_t flashLength)
{
  flashCount=millis()+flashLength;
}

void loop()
{
  // LED Flasher
  if(flashCount>millis())
  {
    digitalWrite(LED_BUILTIN,HIGH);
  }
  else
  {
    digitalWrite(LED_BUILTIN,LOW);
  }
  
  if (rf95.available())
  {
    flash(200);
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len))
    {
      // debug received
      Serial.print("[");
      Serial.print(millis(),DEC);
      Serial.print("][");
      Serial.print(rf95.lastRssi(), DEC);
      Serial.print("]RX:");
      Serial.println((char*)buf);
      
      // Send a reply
      uint8_t data[] = "yy0123456789";
      Serial.print("[");
      Serial.print(millis(),DEC);
      Serial.print("]TX:");
      Serial.println((char*)data);
      rf95.send(data, sizeof(data));
      //rf95.waitPacketSent();  
    }
    else
    {
      Serial.println("recv failed");
    }
  }
}


