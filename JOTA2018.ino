//////////////////////////////////////////////
//     JOTA 2018 Sensor Gadget              //
//     Menu code from http://www.educ8s.tv  //
//     malcolm@faed.name                    //
//////////////////////////////////////////////

// 20181006 Remove magnetometer code for IMU6050

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <ClickEncoder.h>
#include <TimerOne.h>
#include <MPU9250.h> //sparkfun library
#include <quaternionFilters.h>
#include <Adafruit_BME280.h>
//#include <Adafruit_SI1145.h>
#include <EEPROM.h>

// Prefered font
#include <Fonts/FreeMono9pt7b.h>

int menuitem = 1;
int page  = 1;
char* menuItem[] = {"" , "Weather", "Level", "Altitude", "Light: On", "Brightness", "Contrast", ""};
int menuItems = 9; // Text items plus 2 for first and last

// EEPROM data - store values every 8 bytes (8 byte EEPROM page) to try and reduce wear.
// Can store up to 0x3FF
// 0x00 Contrast
// 0x08 Backlight Brightness
// 0x10 Backlight on / off state
int eeContrast = 0x00;
int eeBrightness = 0x08;
int eeBacklight = 0x10;
int eeValid = 0x18;
int eehPaMSL = 0x20;
int eeMagBias = 0x28; // and 0x29, 0x2a

int backlightPin = 10;

boolean backlight = true;
uint8_t contrast = 12; // 0-31 * 4 = 0-127
uint8_t brightness = 16; // 0-31 * 32 = 0-1023
uint8_t hPaMSL = 113; // 900+113 = 1013hPa at MSL

boolean up = false;
boolean down = false;
boolean middle = false;

//boolean si1145missing = false;

ClickEncoder *encoder;
int16_t last, value;

Adafruit_PCD8544 display = Adafruit_PCD8544( 5, 4, 3); //Download the latest Adafruit Library in order to use this constructor

// BME280
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme; // I2C

// MPU9250
MPU9250 mpu;

// SI1145
//Adafruit_SI1145 uv = Adafruit_SI1145();

void setup() {
  //Serial.begin(115200);
  //Serial.println("Hello");

  if (EEPROM.read(eeValid) == 0xfa) // read eeprom values if valid.
  {
    brightness = EEPROM.read(eeBrightness);
    backlight = EEPROM.read(eeBacklight);
    contrast = EEPROM.read(eeContrast);
    hPaMSL = EEPROM.read(eehPaMSL);
    mpu.magbias[0] = (int8_t)EEPROM.read(eeMagBias) * 4;
    mpu.magbias[1] = (int8_t)EEPROM.read(eeMagBias + 1) * 4;
    mpu.magbias[2] = (int8_t)EEPROM.read(eeMagBias + 2) * 4;
    if (backlight)
    {
      menuItem[4] = "Light: On";
      turnBacklightOn();
    }
    else
    {
      menuItem[5] = "Light: Off";
      turnBacklightOff();
    }
  }
  else // write defaults if not valid.
  {
    EEPROM.write(eeBacklight, backlight);
    EEPROM.write(eeBrightness, brightness);
    EEPROM.write(eeContrast, contrast);
    EEPROM.write(eeValid, 0xfa);
    EEPROM.write(eehPaMSL, hPaMSL);
    EEPROM.write(eeMagBias, int8_t(0));
    EEPROM.write(eeMagBias + 1, int8_t(0));
    EEPROM.write(eeMagBias + 2, int8_t(0));
    menuitem = 8; // start with contrast adjustment
  }

  // initalise backlight PWM pin
  pinMode(backlightPin, OUTPUT);
  Timer1.initialize(1000); // initalise timer for backlight PWM and encoder

  backlight = !backlight; //inverts backlight
  setbacklight(); //inverts backlight again

  // Init Display
  display.begin(40, 4);
  //display.setContrast(48);
  setContrast();
  display.clearDisplay();
  display.display();

  // Set Font
  //display.setFont(&FreeMono9pt7b);

  // Welcome Display
  display.setTextSize(2); // 1 in small, 2 is large. Small characters are 6 x 8 pixels
  //display.setTextColor(BLACK, WHITE);
  display.setCursor(18, 4);
  display.println("JOTA");
  display.setCursor(18, 20);
  display.println("2018");
  display.display();
  delay(2000);

  display.clearDisplay();
  display.setCursor(6, 4);
  //display.setTextColor(WHITE, BLACK); // 'inverted' text
  display.println("Sensor");
  display.setCursor(6, 20);
  display.println("Gadget");
  display.display();
  delay(2000);

  display.clearDisplay();
  display.display();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("Init Sensors");

  // Initalise BME280

  if (! bme.begin()) {
    display.println("BME280 sensor");
    display.println("I2C Error");
    display.display();
    while (1);
  }
  display.println("BME280 OK");
  display.display();
  /*
    // Initalise SI1145
    if (! uv.begin()) {
      display.println("SI1145 sensor");
      display.println("I2C Error");
      display.display();
      si1145missing = true;
      // while (1);
    } else {
      display.println("SI1145 OK");
      display.display();
    }
  */
  // Initalise MPU9250
  byte MPUWhoAmI = 0;
  MPUWhoAmI = mpu.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
  if (MPUWhoAmI != 0x70) {
    display.println("MPU9250 Sensor");
    display.println("I2C Error");
    display.display();
    while (1);
  }
  display.print("MPU9250 OK ");
  display.println(MPUWhoAmI, HEX);
  display.display();

  // Initialize device for active mode read of acclerometer, gyroscope, and temperature
  mpu.initMPU9250();

  // Get magnetometer calibration from AK8963 ROM
  mpu.initAK8963(mpu.magCalibration);

  encoder = new ClickEncoder(8, 9, 6);
  encoder->setAccelerationEnabled(false);

  //Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr);

  last = encoder->getValue();

}

void loop() {

  drawMenu();
  readRotaryEncoder();

  ClickEncoder::Button b = encoder->getButton();
  if (b != ClickEncoder::Open) {
    switch (b) {
      case ClickEncoder::Clicked:
        middle = true;
        break;
    }
  }

  if (up && page == 1)  //We have turned the Rotary Encoder Counter Clockwise
  {
    up = false;
    menuitem--;
    if (menuitem < 1)
    {
      menuitem = 8;
    }
  }
  else if (up && page == 2 && menuitem == 3) { //Altitude
    up = false;
    if (hPaMSL > 0) hPaMSL--;
    EEPROM.write(eehPaMSL, hPaMSL);
  }
  else if (up && page == 2 && menuitem == 5 ) { //Brightness
    up = false;
    if (brightness > 0) brightness--;
    turnBacklightOn();
    EEPROM.write(eeBrightness, brightness);
  }
  else if (up && page == 2 && menuitem == 6 ) { //Contrast
    up = false;
    if (contrast > 5) contrast--;
    setContrast();
  }

  if (down && page == 1) // We have turned the Rotary Encoder Clockwise
  {
    down = false;
    menuitem++;
    if (menuitem > 8)
    {
      menuitem = 1;
    }
  }
  else if (down && page == 2 && menuitem == 3) {//Altitude
    down = false;
    if (hPaMSL < 255) hPaMSL++;
    EEPROM.write(eehPaMSL, hPaMSL);
  }
  else if (down && page == 2 && menuitem == 5) {
    down = false;
    if (brightness < 31) brightness++;
    turnBacklightOn();
    EEPROM.write(eeBrightness, brightness);
  }
  else if (down && page == 2 && menuitem == 6) {
    down = false;
    if (contrast < 16) contrast++;
    setContrast();
  }

  if (middle) //Middle Button is Pressed
  {
    middle = false;

    if ( page == 1 && menuitem == 4) // Backlight On / Off Control
    {
      setbacklight();
    }
    else if (page == 1) {
      page = 2;
    }
    else if (page == 2)
    {
      page = 1;
    }
  }

}

void setbacklight() {
  if (backlight)
  {
    backlight = false;
    menuItem[4] = "Light: Off";
    turnBacklightOff();
    EEPROM.write(eeBacklight, backlight);
  }
  else
  {
    backlight = true;
    menuItem[4] = "Light: On";
    turnBacklightOn();
    EEPROM.write(eeBacklight, backlight);
  }
}

void setContrast()
{
  display.setContrast(contrast * 4);
  display.display();
  //delay(1000);
  //Serial.println(contrast);
  EEPROM.write(eeContrast, contrast);
}

void turnBacklightOn()
{
  //analogWrite(backlightPin, brightness);
  Timer1.pwm(backlightPin, brightness * 32);
  EEPROM.write(eeBrightness, brightness);
  EEPROM.write(eeBacklight, 1);
}

void turnBacklightOff()
{
  //analogWrite(backlightPin, 0);
  Timer1.pwm(backlightPin, 0);
  EEPROM.write(eeBacklight, 0);
}

void timerIsr() {
  encoder->service();
}

void displayIntMenuPage(String menuItem, int value)
{
  display.setTextSize(1);
  display.clearDisplay();
  display.setTextColor(BLACK, WHITE);
  display.setCursor(5, 0);
  display.print(menuItem);
  display.drawFastHLine(0, 10, 83, BLACK);
  display.setCursor(5, 15);
  display.print("Value");
  display.setTextSize(2);
  display.setCursor(5, 25);
  display.print(value);
  display.setTextSize(2);
  display.display();
}

void readRotaryEncoder()
{
  value += encoder->getValue();

  if (value / 2 > last) {
    last = value / 2;
    down = true;
    delay(150);
  } else   if (value / 2 < last) {
    last = value / 2;
    up = true;
    delay(150);
  }
}


int autoScale( int originalMin, int originalMax, int newBegin, int newEnd, int inputValue) {

  long zeroRefOriginalMax = 0;
  long zeroRefnewEnd = 0;
  long zeroRefCurVal = 0;
  long rangedValue = 0;
  boolean invFlag = 0;

  // Check for out of range inputValues
  if (inputValue < originalMin) {
    inputValue = originalMin;
  }
  if (inputValue > originalMax) {
    inputValue = originalMax;
  }

  // Zero Refference the values
  zeroRefOriginalMax = originalMax - originalMin;

  if (newEnd > newBegin) {
    zeroRefnewEnd = newEnd - newBegin;
  }
  else
  {
    zeroRefnewEnd = newBegin - newEnd;
    invFlag = 1;
  }

  zeroRefCurVal = inputValue - originalMin;

  // Check for originalMin > originalMax  - the math for all other cases i.e. negative numbers seems to work out fine
  if (originalMin > originalMax ) {
    return 0;
  }
  if (invFlag == 0) {
    rangedValue =  ((zeroRefCurVal * zeroRefnewEnd) / zeroRefOriginalMax) + newBegin ;
  }
  else     // invert the ranges
  {
    rangedValue =  newBegin - ((zeroRefCurVal * zeroRefnewEnd) / zeroRefOriginalMax)  ;
  }
  return rangedValue;
}
