// rf95_client.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing client
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf95_server
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

void setup() 
{
  // Rocket Scream Mini Ultra Pro with the RFM95W only:
  // Ensure serial flash is not interfering with radio communication on SPI bus
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);

  // LED
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  Serial1.begin(115200);
  
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
  //rf95.setModemConfig(RH_RF95::Bw125Cr48Sf4096);
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
}

uint32_t flashCount=0;
uint32_t transmitCount=0;
uint32_t transmitPeriod=20000;

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

  // packet transmitter
  if(transmitCount<millis())
  {
    uint8_t data[] = "xx0123456789";
    transmitCount=millis()+transmitPeriod;
    flash(200);
    Serial.print("[");
    Serial.print(millis(),DEC);
    Serial.print("]TX: ");
    Serial.println((char*)data);

    rf95.send(data, sizeof(data));
  }

  // packet receiver
  if(rf95.available())
  {
    uint8_t data[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(data);

    if (rf95.recv(data, &len))
    {
      flash(200);
      Serial.print("[");
      Serial.print(millis(),DEC);
      Serial.print("]RX: ");
      Serial.println((char*)data);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);  
    }
  }

  // GPS Data Processing
  while(Serial1.available()>0)
  {
    Serial.print((char)Serial1.read());
  }

/*
  rf95.send(data, sizeof(data));
  
  rf95.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (rf95.waitAvailableTimeout(1000))
  { 
    // Should be a reply message for us now   
    if (rf95.recv(buf, &len))
   {
      flash();
        
    }
    else
    {
      Serial.println("recv failed");
    }
  }
  else
  {
    Serial.println("No reply, is rf95_server running?");
  }
  delay(500);
  */
}


