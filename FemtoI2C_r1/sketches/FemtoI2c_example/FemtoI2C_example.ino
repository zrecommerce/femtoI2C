/******************************************************************************
 *  i2c_gpio
 *  Keith Neufeld
 *  May 26, 2008
 *
 *  Prototype I2C interface to TI 9535 and 9555 GPIO expanders.
 *
 *  Arduino analog input 5 - I2C SCL
 *  Arduino analog input 4 - I2C SDA
 *
 * Wed, Aug 7 2013 - Modified by Alex Albino (femtoduino.com) - Added gpio_scan() 
 * routine picked up from another blog at 3am...it's all too fuzzy to 
 * remember now. ...Can someone tell me why my wallet is in the fridge?
 *
 ******************************************************************************/

#include <Wire.h>


//#define DEBUG_GPIO


//  I2C device address is 0 1 0 0   A2 A1 A0

#define REGISTER_INPUT (0)
#define REGISTER_OUTPUT (2)
#define REGISTER_CONFIG (6)

#define DEBUG_GPIO true

byte LED_ADDRESS;
int bits;

void setup() {
  Wire.begin();
  
#ifdef DEBUG_GPIO
  Serial.begin(9600);
#endif  
  
  LED_ADDRESS = gpio_scan();
  
#ifdef DEBUG_GPIO
  Serial.println(LED_ADDRESS, HEX); delay(1000);
  
#endif
  
  gpio_dir(LED_ADDRESS, REGISTER_OUTPUT);
  
}


void loop() {

  LED_ADDRESS = gpio_scan();
  
  int bits;
  int ledBits;

  for (byte i = 0; i < 16; i++) {
#ifdef DEBUG_GPIO
    Serial.print("AT index: ");
    
    Serial.print(i);
    Serial.println("...");
#endif
    switch(i) {
      case 15:
        bits = 0x00f0;
        break;
      case 14:
        bits = 0x01e0;
        break;
      case 13:
        bits = 0x02d0;
        break;
      case 12:
        bits = 0x03c0;
        break;
      case 11:
        bits = 0x04b0;
        break;
      case 10:
        bits = 0x05a0;
        break;
      case 9:
        bits = 0x0690;
        break;
      case 8:
        bits = 0x0780;
        break;
      case 7:
        bits = 0x0870;
        break;
      case 6:
        bits = 0x0960;
        break;
      case 5:
        bits = 0x0a50;
        break;
      case 4:
        bits = 0x0b40;
        break;
      case 3:
        bits = 0x0c30;
        break;
      case 2:
        bits = 0x0d20;
        break;
      case 1:
        bits = 0x0e10;
        break;
      default:
        bits = 0x0f00;
        break;
    }
    //  mirror direction of bits for output display
    ledBits = (
      ((bits & 1) << 3) | ((bits & 2) << 1) |
      ((bits & 4) >> 1) | ((bits & 8) >> 3)
    );
    
#ifdef DEBUG_GPIO
    Serial.println("BIN:");
    Serial.println(bits, BIN);
#endif
    

    gpio_write(LED_ADDRESS, bits);
    delay(100);
  }
  
  
#ifdef DEBUG_GPIO
  Serial.println("========================");
  delay(200);
#endif
}

void gpio_config() {

}

int gpio_scan() {
  
  byte error, address;
  byte finalAddress;
  int nDevices;
  
#ifdef DEBUG_GPIO
  Serial.println("Scanning...");
#endif

  nDevices = 0;
  for(address = 1; address < 127; address++ ) 
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
#ifdef DEBUG_GPIO
      Serial.print("I2C device found at address 0x");

      if (address<16) 
        Serial.print("0");
        
      Serial.print(address,HEX);
      Serial.println("  !");
#endif
      finalAddress = address;
      nDevices++;
    }
    else if (error==4) 
    {
#ifdef DEBUG_GPIO
      Serial.print("Unknown error at address 0x");
      if (address<16) 
        Serial.print("0");
      Serial.println(address,HEX);
#endif
    }    
  }
  
#ifdef DEBUG_GPIO
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
#endif;

  return finalAddress;
}

int gpio_read(int address) {
  int data = 0;

  //  Send input register address
  Wire.beginTransmission(address);
  Wire.write(REGISTER_INPUT);
  Wire.endTransmission();

  //  Connect to device and request two bytes
  Wire.beginTransmission(address);
  Wire.requestFrom(address, 2);

  if (Wire.available()) {
    data = Wire.read();
  } else {
#ifdef DEBUG_GPIO
    Serial.println("DEBUG: cant read, not available yet.");
#endif
  }
  

  if (Wire.available()) {
    data |= Wire.read() << 8;
  }

  Wire.endTransmission();

  return data;
}

void gpio_dir(int address, int dir) {
  //  Send config register address
  Wire.beginTransmission(address);
  Wire.write(REGISTER_CONFIG);
  
  //  Connect to device and send two bytes
  Wire.write(0xff & dir);  //  low byte
  Wire.write(dir >> 8);    //  high byte

  Wire.endTransmission();
}

void gpio_write(int address, int data) {
  //  Send output register address
  Wire.beginTransmission(address);
  Wire.write(REGISTER_OUTPUT);
  
  //  Connect to device and send two bytes
#ifdef DEBUG_GPIO
  Serial.print("Low: ");
  Serial.print(0xff & data, HEX);
  Serial.print(", ");
#endif

  Wire.write(0xff & data);  //  low byte
  
#ifdef DEBUG_GPIO
  Serial.print("High: ");
  Serial.print((data >> 8), HEX);
  Serial.println("\n----------");
#endif;
  Wire.write(data >> 8);    //  high byte

  Wire.endTransmission();
}
