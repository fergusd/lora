#include <AES.h>
#define Serial SerialUSB

AES aes ;

byte key[] = 
{
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
} ;

byte plain[] =
{
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
} ;

byte my_iv[] = 
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
} ;

byte cipher [4*N_BLOCK] ;
byte check [4*N_BLOCK] ;

void loop () 
{
}


void setup ()
{
  Serial.begin (115200) ;
  while (!Serial) ;
  Serial.println("start");

  for(int loop=1;loop<100;loop++)
  {
    prekey (256, 2) ;
  }
}

void prekey (int bits, int blocks)
{
  byte iv [N_BLOCK] ;
  
  byte succ = aes.set_key (key, bits) ;

  if (blocks == 1)
  {
    succ = aes.encrypt (plain, cipher) ;
  }
  else
  {
    for (byte i = 0 ; i < 16 ; i++)
    {
      iv[i] = my_iv[i] ;
    }
    succ = aes.cbc_encrypt (plain, cipher, blocks, iv) ;
  }

  if (blocks == 1)
  {
    succ = aes.decrypt (cipher, plain) ;
  }
  else
  {
    for (byte i = 0 ; i < 16 ; i++)
    {
      iv[i] = my_iv[i] ;
    }
    succ = aes.cbc_decrypt (cipher, check, blocks, iv) ;
  }

  Serial.println("plain");
  for (byte i=0;i<32;i++)
  {
    Serial.print(plain[i],HEX);
    Serial.print(" ");
  }
  Serial.println("");

  Serial.println("cipher");
  for (byte i=0;i<32;i++)
  {
    Serial.print(cipher[i],HEX);
    Serial.print(" ");
  }
  Serial.println("");


  Serial.println("check");
  for (byte i=0;i<32;i++)
  {
    Serial.print(check[i],HEX);
    Serial.print(" ");
  }
  Serial.println("");
}


