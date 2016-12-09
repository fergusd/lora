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
#include <LowPower.h>
#include <TinyGPS++.h>

// Singleton instance of the radio driver
//RH_RF95 rf95;
RH_RF95 rf95(5, 2); // Rocket Scream Mini Ultra Pro with the RFM95W
//RH_RF95 rf95(8, 3); // Adafruit Feather M0 with RFM95 

// Need this on Arduino Zero with SerialUSB port (eg RocketScream Mini Ultra Pro)
#define Serial SerialUSB

// GPS Stream Parser
TinyGPSPlus gps;

void setup() 
{
  // Rocket Scream Mini Ultra Pro with the RFM95W only:
  // Ensure serial flash is not interfering with radio communication on SPI bus
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);

  // LED
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  Serial1.begin(57600);
  
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
uint32_t transmitCount=5000;
uint32_t transmitPeriod=6000;
uint32_t blippReload=1000;
uint32_t blipp=blippReload;

void flash(uint16_t flashLength)
{
  if((flashLength+millis())>flashCount)
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

  if(blipp<millis())
  {
    blipp=millis()+blippReload;
    flash(3);
  }

  // packet transmitter
  if(transmitCount<millis())
  {
    String string;
    string+=String(gps.location.lat(),6);
    string+=String(",");
    string+=String(gps.location.lng(),6);
    //uint8_t data[] = "xx0123456789";
    transmitCount=millis()+transmitPeriod;
    blippReload=100;
    flash(300);
    
    Serial.print("[");
    Serial.print(millis(),DEC);
    Serial.print("]TX: ");
    //Serial.println((char*)data);
    Serial.println(string);

    uint8_t data[40];
    int len=string.length();
    string.getBytes(data,len);
    rf95.send(data,len);
  }

  // packet receiver
  if(rf95.available())
  {
    uint8_t data[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(data);

    if (rf95.recv(data, &len))
    {
      flash(300);
      blippReload=1000;
      Serial.print("[");
      Serial.print(millis(),DEC);
      Serial.print("]RX: ");
      Serial.println((char*)data);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
      rf95.sleep();  
    }
  }

  // GPS Data Processing
  if(Serial1.available()>0)
  {
    //Serial.print((char)Serial1.read());
    gps.encode(Serial1.read());
  }
}

static void printInt(unsigned long val, bool valid, int len)
{
  char sz[32] = "*****************";
  if (valid)
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i=strlen(sz); i<len; ++i)
    sz[i] = ' ';
  if (len > 0) 
    sz[len-1] = ' ';
  Serial.print(sz);
}

static void printFloat(float val, bool valid, int len, int prec)
{
  if (!valid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      Serial.print(' ');
  }
}

static void printDateTime(TinyGPSDate &d, TinyGPSTime &t)
{
  if (!d.isValid())
  {
    Serial.print(F("********** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
    Serial.print(sz);
  }
  
  if (!t.isValid())
  {
    Serial.print(F("******** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
    Serial.print(sz);
  }

  printInt(d.age(), d.isValid(), 5);
}

