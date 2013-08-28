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

// After selecting the device address, select the pin(s) you want to access
// using their addresses.

#define IC_P0 (1)
#define IC_P1 (2)
#define IC_P2 (4)
#define IC_P3 (8)
#define IC_P4 (16)
#define IC_P5 (32)
#define IC_P6 (64)
#define IC_P7 (128)

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
  
  // Since this chip drives pins "high", we need to mask out the pin(s) we 
  // want to leave "high".
  int allBits = IC_P0 + IC_P1 + IC_P2 + IC_P3 + IC_P4 + IC_P5 + IC_P6 + IC_P7;
  
  
  // Only leave P4 and P6 "on". Don't forget the parenthesis! (Order of operations and whatnot.)
  bits = allBits - (IC_P6 + IC_P4); 
  
#ifdef DEBUG_GPIO
    
    //Serial.println(bits, HEX);
#endif
    
    
    //  mirror direction of bits for output display
  ledBits = bits;
    
    /*(
      ((bits & 1) << 3) | ((bits & 2) << 1) |
      ((bits & 4) >> 1) | ((bits & 8) >> 3)
    );*/
    
  gpio_write(LED_ADDRESS, bits);
  delay(10);
  
  
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
