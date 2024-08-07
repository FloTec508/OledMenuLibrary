//---------------------------------------
// main File
// OLED-Menu library for making menus with I2C Oled screens
// By Florian Winkler
//-----------------------------------------
#include "Arduino.h"
#include "OledMenu.h"
#include <OledMenu.h>
#include <string.h>
#include <stdio.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_SH110X.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <time.h>

// TODO: Change logo size to 10x20 and update images
#define LOGO_HEIGHT 13
#define LOGO_WIDTH 15

// The input pins for buton recognition

#define IMAGE_HEIGHT 9
#define IMAGE_WIDTH 8

#define WIFI_HEIGHT 9
#define WIFI_WIDTH 13


//the pre-installed images and icons
const unsigned char Bluetooth[] PROGMEM = {
    0x18, 0x14, 0x52, 0x34, 0x18, 0x34, 0x52, 0x14, 0x18};

const unsigned char BatteryFull[] PROGMEM = {
    0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0x7f, 0x00, 0x00};

const unsigned char BatteryEmpty[] PROGMEM = {
    0x00, 0x00, 0x7f, 0xc1, 0x81, 0xc1, 0x7f, 0x00, 0x00};

const unsigned char BatteryLow[] PROGMEM = {
    0x00, 0x00, 0x7f, 0xc7, 0x87, 0xc7, 0x7f, 0x00, 0x00};

const unsigned char BatteryHalf[] PROGMEM = {
    0x00, 0x00, 0x7f, 0xdf, 0x9f, 0xdf, 0x7f, 0x00, 0x00};

const unsigned char WifiFull[] PROGMEM = {
    0x0f, 0x00, 0x30, 0xc0, 0x46, 0x20, 0x99, 0x90, 0x20, 0x40, 0x4f, 0x20, 0x10, 0x80, 0x06, 0x00,
    0x06, 0x00};

const unsigned char WifiHalf[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x22, 0x00, 0x41, 0x00, 0x1c, 0x00, 0x22, 0x00,
    0x08, 0x00};

const unsigned char WifiOkay[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x22, 0x00,
    0x08, 0x00};

const unsigned char WifiBad[] PROGMEM = {
    0x0f, 0x00, 0x30, 0xc0, 0x46, 0x20, 0x99, 0x90, 0x20, 0x48, 0x4f, 0x28, 0x10, 0x88, 0x06, 0x00,
    0x06, 0x08

};

const unsigned char* batterystatus[] = {
  BatteryEmpty,
  BatteryLow, 
  BatteryHalf,
  BatteryFull
};

int addr = 0;    // I2C addres
int reset = -1;  // reset pin of the screen
int height = 64; // Screen height
int width = 128; // Screen width


Adafruit_SH1106G screen(width, height, &Wire, reset); 


int piclength = 0;  // the length of the picture array
int menulength = 0; // the length of the menuItems array

bool toggles[50] = {}; // stores the toggle value of the menu

int styles[50] = {}; // the styles of the menu

char MenuItems[50][50] = {}; // All the text items of the Menu(max.50)

bool titleUnderline = false;

bool hasTitle = false; // does the menu have a title?

char MenuTitle[50] = ""; // the menu title

int sliderValues[50] = {}; // the values of all sliders

int statusBar[4] = {}; // the items in the StatusBar

int dashBoard[3] = {}; // the items in the dashboard

bool notify_center = false; // are we in the notifiocation overwiew?

int sliderMaxValues[50] = {};

unsigned char pictures[50][100] = {0}; // all the Pictures of the menu(max.50)

int screenType = 0;//what type of driver? 0=SSD1306, 1=SH110X
OledMenu::OledMenu(int SCREEN_ADDRESS, int SCREEN_HEIGHT, int SCREEN_WIDTH, int OLED_RESET, int type)
{

  // Set all the parameters
  addr = SCREEN_ADDRESS;
  reset = OLED_RESET;
  height = SCREEN_HEIGHT;
  width = SCREEN_WIDTH;

  // and initialize the Screen
 
  screen = Adafruit_SH1106G(width, height, &Wire, reset);
  screenType = type;
  
}

int OledMenu::getSliderValue(int field){
  return sliderValues[field];
}
void OledMenu::setSliderValue(int field, int value){
  sliderValues[field] = value;
}
void OledMenu::addStatusBarItem(int type)//add an item to the StatusBar
{
  if (indxBar <= 3)
  {
    statusBar[indxBar] = type;
    indxBar++;
  }
}
void OledMenu::addDashboardField(int type)// add an item to the DashBoard
{
  if (indxDash <= 2)
  {
    dashBoard[indxDash] = type;
    indxDash++;
  }
}
void OledMenu::updateDashboard(struct tm time, int wifi_status, bool bluetoothConnected, int batteryStatus, float BatteryVoltage)
{
  int timeHeight = 0;
  int lineheight = 0;
  int dateheight = 0;

  int prevHeight = 0;

  // recognize key inputs
  if (digitalRead(DOWN) == HIGH && (presseddown == 0))
  {
    presseddown = 1;

    if (notify_center == false)
    {
      for (int i = 0; i <= height; i = i + 8)
      {

        screen.fillRect(0, 0, width, i, 0);
        screen.setCursor((width / 2) - 10, i - (height - 20));
        screen.setFont(&FreeMono9pt7b);
        screen.setTextColor(1);
        screen.print("no");
        screen.setCursor(0, i - (height - 35));
        screen.print("notifications");
        screen.display();
      }
      notify_center = true;
    }
  }
  if (digitalRead(UP) == HIGH && (pressed == 0))
  {
    pressed = 1;
    //when the user is not in the notification center, than enter it with an animation
    if (notify_center == true)
    {
      for (int i = height; i > 0; i = i - 8)
      {

        screen.setFont(&FreeMonoBold18pt7b);
        screen.setTextColor(1);
        screen.setCursor(10, timeHeight);
        screen.drawLine(8, lineheight, 120, lineheight, 1);
        screen.printf("%d:%d", time.tm_hour, time.tm_min);
        screen.setFont(&FreeMonoBold9pt7b);
        screen.setCursor(8, dateheight);
        // screen.print("03/12/24");
        screen.printf("%d/%d/%d", time.tm_mday, time.tm_mon + 1, time.tm_year);

        screen.fillRect(0, 0, width, i, 0);
        screen.setCursor((width / 2) - 10, i - (height - 20));
        screen.setFont(&FreeMono9pt7b);
        screen.setTextColor(1);
        screen.print("no");
        screen.setCursor(0, i - (height - 35));
        screen.print("notifications");
        screen.display();
      }
      notify_center = false;
    }
  }
  if (digitalRead(RIGHT) == HIGH && (pressedright == 0))
  {

    pressedright = 1;
  }
  if (digitalRead(LEFT) == HIGH && (pressedleft == 0))
  {

    pressedleft = 1;
  }
  if (digitalRead(ENTER) == HIGH && (pressedenter == 0))
  {
    pressedenter = 1;
    entered = 1;
  }

  // draw the statusBar

  switch (statusBar[0])
  {
  case 0:
    if(bluetoothConnected){
    screen.drawBitmap(2, 2, Bluetooth, IMAGE_WIDTH, IMAGE_HEIGHT, 1);
    }
    break;

  case 1:
    screen.setFont(&FreeMono9pt7b);
    screen.setCursor(2, 10);
    screen.printf("%fV", battery_Voltage);
    break;

  case 2:
    screen.drawBitmap(2, 2, batterystatus[batteryStatus], IMAGE_WIDTH, IMAGE_HEIGHT, 1);
    break;

  case 3:
    if(wifi_status == 1){
      screen.drawBitmap(2, 2, WifiFull, WIFI_WIDTH, WIFI_HEIGHT, 1);
    }else{
      screen.drawBitmap(2, 2, WifiBad, WIFI_WIDTH, WIFI_HEIGHT, 1);
    }    
    break;
  default:
    break;
  }
  switch (statusBar[1])
  {
  case 0:
    if(bluetoothConnected){
    screen.drawBitmap(16, 2, Bluetooth, IMAGE_WIDTH, IMAGE_HEIGHT, 1);
    }
    break;

  case 1:
    screen.setFont(&FreeMono9pt7b);
    screen.setCursor(16, 10);
    screen.printf("%fV", battery_Voltage);
    break;

  case 2:
    screen.drawBitmap(16, 2, batterystatus[batteryStatus], IMAGE_WIDTH, IMAGE_HEIGHT, 1);
    break;

  case 3:
  if(wifi_status == 1){
    screen.drawBitmap(16, 2, WifiFull, WIFI_WIDTH, WIFI_HEIGHT, 1);
  }else{
    screen.drawBitmap(16, 2, WifiBad, WIFI_WIDTH, WIFI_HEIGHT, 1);
  }
    break;
  default:
    break;
  }
  switch (statusBar[2])
  {
  case 0:
  if(bluetoothConnected){
    screen.drawBitmap(100, 2, Bluetooth, IMAGE_WIDTH, IMAGE_HEIGHT, 1);
    }
    break;

  case 1:
    screen.setFont(&FreeMono9pt7b);
    screen.setCursor(100, 10);
    screen.printf("%fV", battery_Voltage);
    break;

  case 2:
    screen.drawBitmap(100, 2, batterystatus[batteryStatus], IMAGE_WIDTH, IMAGE_HEIGHT, 1);
    break;

  case 3:
  if(wifi_status == 1){
    screen.drawBitmap(100, 2, WifiFull, WIFI_WIDTH, WIFI_HEIGHT, 1);
    }else{
    screen.drawBitmap(100, 2, WifiBad, WIFI_WIDTH, WIFI_HEIGHT, 1);
    }
    break;
  default:
    break;
  }
  switch (statusBar[3])
  {
  case 0:
  if(bluetoothConnected){
    screen.drawBitmap(112, 2, Bluetooth, IMAGE_WIDTH, IMAGE_HEIGHT, 1);
  }
  break;

  case 1:
    screen.setFont(&FreeMono9pt7b);
    screen.setCursor(112, 10);
    screen.printf("%fV", battery_Voltage);
    break;

  case 2:
    screen.drawBitmap(112, 2, batterystatus[batteryStatus], IMAGE_WIDTH, IMAGE_HEIGHT, 1);
    break;

  case 3:
  if(wifi_status == 1){
    screen.drawBitmap(112, 2, WifiFull, WIFI_WIDTH, WIFI_HEIGHT, 1);
  }else{
    screen.drawBitmap(112, 2, WifiBad, WIFI_WIDTH, WIFI_HEIGHT, 1);
  }
    break;
  default:
    break;
  }
screen.display();
// draw the dashboard
  //calculate the position of the 3rd element
  

  //when the notification center isn't active, show the dashboard
  if (notify_center == false)
  {
    switch (dashBoard[0])
    {
    case 0:
    timeHeight = height*0.59375;
    prevHeight = timeHeight + 5;
    screen.setFont(&FreeMonoBold18pt7b);
    screen.setTextColor(1);
    screen.setCursor(10, timeHeight);
    screen.printf("%d:%d", time.tm_hour, time.tm_min);
      break;
    case 1:
    dateheight = height*0.40625;
    prevHeight = dateheight + 5;
    screen.setFont(&FreeMonoBold9pt7b);
    screen.setCursor(8, dateheight);
    screen.printf("%d/%d/%d", time.tm_mday, time.tm_mon + 1, time.tm_year);
      break;
    case 2:
    lineheight = ((height / 4));
    prevHeight = lineheight + 3;
    screen.drawLine(8, lineheight, 120, lineheight, 1);
      break;
    default:
      break;
    }

    switch (dashBoard[1])
    {
    case 0:
    timeHeight = prevHeight + 22;
    prevHeight = timeHeight + 5;
    screen.setFont(&FreeMonoBold18pt7b);
    screen.setTextColor(1);
    screen.setCursor(10, timeHeight);
    screen.printf("%d:%d", time.tm_hour, time.tm_min);
      break;
    case 1:
    dateheight = prevHeight + 10;
    prevHeight = dateheight + 3;
    screen.setFont(&FreeMonoBold9pt7b);
    screen.setCursor(8, dateheight);
    screen.printf("%d/%d/%d", time.tm_mday, time.tm_mon + 1, time.tm_year);
      break;
    case 2:
    lineheight = prevHeight;
    prevHeight = lineheight + 3;
    screen.drawLine(8, lineheight, 120, lineheight, 1);
      break;
    default:
      break;
    }
    switch (dashBoard[2])
    {
    case 0:
    timeHeight = prevHeight + 22;
    prevHeight = timeHeight + 5;
    screen.setFont(&FreeMonoBold18pt7b);
    screen.setTextColor(1);
    screen.setCursor(10, timeHeight);
    screen.printf("%d:%d", time.tm_hour, time.tm_min);
      break;
    case 1:
    dateheight = prevHeight + 10;
    prevHeight = dateheight + 3;
    screen.setFont(&FreeMonoBold9pt7b);
    screen.setCursor(8, dateheight);
    screen.printf("%d/%d/%d", time.tm_mday, time.tm_mon + 1, time.tm_year);
      break;
    case 2:
    lineheight = prevHeight;
    prevHeight = lineheight + 3;
    screen.drawLine(8, lineheight, 120, lineheight, 1);
      break;
    default:
      break;
    }

    //screen.setFont(&FreeMonoBold18pt7b);
    //screen.setTextColor(1);
    //screen.setCursor(10, timeHeight);
    //screen.drawLine(8, lineheight, 120, lineheight, 1);
    //screen.printf("%d:%d", time.tm_hour, time.tm_min);
    //screen.setFont(&FreeMonoBold9pt7b);
    //screen.setCursor(8, dateheight);
    //screen.printf("%d/%d/%d", time.tm_mday, time.tm_mon + 1, time.tm_year);

    screen.display();
  }

  // reset key inputs
  if (digitalRead(UP) == LOW && (pressed == 1))
  {
    pressed = 0;
  }
  if (digitalRead(DOWN) == LOW && (presseddown == 1))
  {
    presseddown = 0;
  }
  if (digitalRead(LEFT) == LOW && (pressedleft == 1))
  {
    pressedleft = 0;
  }
  if (digitalRead(RIGHT) == LOW && (pressedright == 1))
  {
    pressedright = 0;
  }
  if (digitalRead(ENTER) == LOW && (pressedenter == 1))
  {
    pressedenter = 0;
  }
}

void OledMenu::numberPicker(int max, char label[])
{
  int maxdigits;

  if (max < 1000 && max > 99)
  {
    maxdigits = 3;
  }
  else if (max < 100 && max > 9)
  {
    maxdigits = 2;
  }
  else if (max < 10 && max > 0)
  {
    maxdigits = 1;
  }
  else
  {
    maxdigits = 3;
  }

  int currentdigit = 1; /// the edited digit
  int current = 0;      // buffer of the current selection
  int digit1 = 0;       // current selection of the 1st number
  int digit2 = 0;       // current selection of the second number
  int digit3 = 0;       // current selection of the third number

  int digits[] = {
      120,//just a random number as a spacer
      digit1,
      digit2,
      digit3};

  bool selected = false;

  int currentvalue = digits[currentdigit];
  while (!selected)
  {

    // recognize key inputs
    if (digitalRead(DOWN) == HIGH && (presseddown == 0) && (current != 9))
    {
      presseddown = 1;
      current++;
    }
    if (digitalRead(UP) == HIGH && (pressed == 0) && (current != 0))
    {
      pressed = 1;
      current--;
    }
    if (digitalRead(RIGHT) == HIGH && (pressedright == 0))
    {
      if (currentdigit != maxdigits)
      {

        digits[currentdigit] = current; // save the buffer
        currentdigit++;
        current = digits[currentdigit]; // update the buffer
      }
      pressedright = 1;
    }
    if (digitalRead(LEFT) == HIGH && (pressedleft == 0))
    {
      if (currentdigit != 1)
      {

        digits[currentdigit] = current; // save the buffer
        currentdigit--;
        current = digits[currentdigit]; // update the buffer
      }
      pressedleft = 1;
    }
    if (digitalRead(ENTER) == HIGH && (pressedenter == 0))
    {
      pressedenter = 1;
      if (currentdigit != maxdigits)
      {
        digits[currentdigit] = current; // save the buffer
        currentdigit++;
        current = digits[currentdigit]; // update the buffer
      }
      else
      {
        selected = true;
      }
    }

    digits[currentdigit] = current; // save the buffer into the currently selected digit

    // draw numberPicker --currently edited digit
    screen.clearDisplay();
    screen.setTextColor(SSD1306_WHITE);
    screen.setTextSize(1);
    // draw the title-text
    screen.setCursor(5, height*0.15625);
    screen.setFont(&FreeSans9pt7b);
    screen.print("select");
    screen.setCursor(5, height*0.46875);
    screen.print(label);
    screen.setFont(&FreeSans12pt7b);

    // draw the selected number
    screen.setCursor(100, height*0.625);
    screen.setFont(&FreeSansBold12pt7b);
    screen.print(current);

    screen.drawLine(100, height*0.3125, 117, height*0.3125, 1);
    screen.drawLine(100, height*0.703125, 117, height*.703125, 1);
    // draw the number before the selected one
    if (digits[currentdigit] != 0)
    {
      screen.setCursor(100, height*0.21875);
      screen.setTextColor(SSD1306_WHITE);
      screen.setTextSize(1);
      screen.setFont(&FreeSans12pt7b);
      screen.print(current - 1);
    }
    // draw the number after the selected one
    if (digits[currentdigit] != 9)
    {
      screen.setCursor(100, height);
      screen.setTextColor(SSD1306_WHITE);
      screen.setTextSize(1);
      screen.setFont(&FreeSans12pt7b);
      screen.print(current + 1);
    }

    // if the user selects the next digit, show the previously entered one next to it
    if (currentdigit == 2 || currentdigit == 3)
    {

      // draw the selected number
      screen.setCursor(80, height*0.625);
      screen.setFont(&FreeSansBold12pt7b);
      screen.print(digits[currentdigit - 1]);

      // draw the number before the selected one
      if (digits[currentdigit - 1] != 0)
      {
        screen.setCursor(80, height*0.21875);
        screen.setTextColor(SSD1306_WHITE);
        screen.setTextSize(1);
        screen.setFont(&FreeSans12pt7b);
        screen.print(digits[currentdigit - 1] - 1);
      }
      // draw the number after the selected one
      if (digits[currentdigit - 1] != 9)
      {
        screen.setCursor(80, height);
        screen.setTextColor(SSD1306_WHITE);
        screen.setTextSize(1);
        screen.setFont(&FreeSans12pt7b);
        screen.print(digits[currentdigit - 1] + 1);
      }
    }

    if (currentdigit == 3)
    {
      // draw the selected number
      screen.setCursor(60, height*0.625);
      screen.setFont(&FreeSansBold12pt7b);
      screen.print(digits[currentdigit - 2]);

      // draw the number before the selected one
      if (digits[currentdigit - 2] != 0)
      {
        screen.setCursor(60, height*0.21875);
        screen.setTextColor(SSD1306_WHITE);
        screen.setTextSize(1);
        screen.setFont(&FreeSans12pt7b);
        screen.print(digits[currentdigit - 2] - 1);
      }
      // draw the number after the selected one
      if (digits[currentdigit - 2] != 9)
      {
        screen.setCursor(60, height);
        screen.setTextColor(SSD1306_WHITE);
        screen.setTextSize(1);
        screen.setFont(&FreeSans12pt7b);
        screen.print(digits[currentdigit - 2] + 1);
      }
    }
    screen.display();
    // reset key inputs
    if (digitalRead(UP) == LOW && (pressed == 1))
    {
      pressed = 0;
    }
    if (digitalRead(DOWN) == LOW && (presseddown == 1))
    {
      presseddown = 0;
    }
    if (digitalRead(LEFT) == LOW && (pressedleft == 1))
    {
      pressedleft = 0;
    }
    if (digitalRead(RIGHT) == LOW && (pressedright == 1))
    {
      pressedright = 0;
    }
    if (digitalRead(ENTER) == LOW && (pressedenter == 1))
    {
      pressedenter = 0;
    }
  }

  numberSelected = (100 * digits[1]) + (10 * digits[2]) + digits[3];
}

bool OledMenu::initialize(uint8_t switchvcc, uint8_t i2caddr)
{

  // if the screen.begin() hasn't been successful, than return false
  if(screenType == 0){
  if (!screen.begin(switchvcc, i2caddr))
  {
    initialized = false;
    return false;
  }
  else
  { // otherwise, continue the initialisation and return "true"
    screen.display();
    delay(500);
    screen.clearDisplay();
    initialized = true;
    return true;
  }
  }else if(screenType==1){
    if (!screen.begin(i2caddr, true))
  {
    initialized = false;
    return false;
  }
  else
  { // otherwise, continue the initialisation and return "true"
    screen.display();
    delay(500);
    screen.clearDisplay();
    initialized = true;
    return true;
  }
  }
}

void OledMenu::addMenuItem(char label[], const unsigned char picture[], int style, int sliderMaxVal)
{

  // copy the text string into our text-array
  strcpy(MenuItems[indx], label);

  sliderMaxValues[indx] = sliderMaxVal;

  // if a picture is provided, copy it
  if (picture != NULL)
  {
    memcpy(pictures[indx], picture, 100);
  }

  switch (style)
  {
  case toggle:
    styles[indx] = style;
    toggles[indx] = false;
    break;
  case slider:
    styles[indx] = style;
    sliderValues[indx] = 0;
  default:
    break;
  }
  // advance the index by 1
  indx++;
}

void OledMenu::addMenuTitle(char title[], bool underline)
{
  strcpy(MenuTitle, title);
  titleUnderline = underline;
  hasTitle = true;
}

void OledMenu::attachInput(int pin, int action)
{ // enter=0; up=1; down=2; left=3; right=4
  // set the pin as an input
  pinMode(pin, INPUT);

  // set the corresponding pin-varialble to the given Pin-Number
  switch (action)
  {
  case 0:
    ENTER = pin;
    break;
  case 1:
    UP = pin;
    break;
  case 2:
    DOWN = pin;
    break;
  case 3:
    LEFT = pin;
    break;
  case 4:
    RIGHT = pin;
    break;
  default:
    break;
  }
}

void OledMenu::setSelected(int Selected)
{

  selected = Selected;
}

bool OledMenu::getToggleValue(int field)
{
  return toggles[field];
}

void OledMenu::setToggleValue(int field, bool value)
{
  toggles[field] = value;
}

int OledMenu::startslider(int val, int maxVal)
{
  int sliderwidth = 92;
  
  bool entered = false;
  // get key inputs
  while (entered == false)
  {
    if ((digitalRead(ENTER) == HIGH) && (pressedenter == 0) && (ENTER != NULL))
    {
      pressedenter = 1;
      entered = true;
    }
    if ((digitalRead(LEFT) == HIGH) && (pressedleft == 0) && (LEFT != NULL))
    {
      if (val < maxVal)
      {
        val++;
      }
      pressedleft = 1;
    }
    if ((digitalRead(RIGHT) == HIGH) && (pressedright == 0) && (RIGHT != NULL))
    {
      if (val > 0)
      {
        val--;
      }
      pressedright = 1;
    }

    float sliderpos = ((float)val / (float)maxVal) * (float)sliderwidth;

    //--------draw the slider--------------------------------------------

    screen.fillRect(10, height/6.4, 108, height*0.6875, 0); // a black backgroud to overdraw the background graphics
    screen.drawRoundRect(10, height/6.4, 108, height*0.6875, 3, 1);
    screen.drawLine(18, height*0.515625, 110, height*0.515625, 1);
    screen.fillCircle(18 + sliderpos, 33, 4, 1);
    screen.setCursor(63, 25);
    screen.setFont(&FreeSans9pt7b);
    screen.print(val);

    screen.display();

    // reset keypress variables
    if ((digitalRead(ENTER) == LOW) && (pressedenter == 1))
    {
      pressedenter = 0;
    }
    if ((digitalRead(LEFT) == LOW) && (pressedleft == 1))
    {
      pressedleft = 0;
    }
    if ((digitalRead(RIGHT) == LOW) && (pressedright == 1))
    {
      pressedright = 0;
    }
  }
  return val;
}

void OledMenu::startMenu(bool loop)
{
  // reset the entered variable
  entered = 0;

  // recognize key inputs, if the key hasn"t been held down and the pin is declared
  if ((digitalRead(DOWN) == HIGH) && (presseddown == 0) && (selected < indx - 1) && (DOWN != NULL))
  {
    presseddown = 1;
    selected++;
    delay(100);
  }

  if ((digitalRead(UP) == HIGH) && (pressed == 0) && (selected != 0) && (UP != NULL))
  {
    pressed = 1;
    selected--;
    delay(100);
  }

  if ((digitalRead(ENTER) == HIGH) && (pressedenter == 0) && (ENTER != NULL))
  {
    pressedenter = 1;

    if (styles[selected] == toggle)
    {
      toggles[selected] = !toggles[selected];
      entered = 2;
    }
    else if (styles[selected] == slider)
    {
      entered = 2;
      sliderValues[selected] = startslider(sliderValues[selected], sliderMaxValues[selected]);
    }
    else
    {
      entered = 1;
    }
  }
  if ((digitalRead(LEFT) == HIGH) && (pressedleft == 0) && (LEFT != NULL))
  {
    pressedleft = 1;
  }
  if ((digitalRead(RIGHT) == HIGH) && (pressedright == 0) && (RIGHT != NULL))
  {
    pressedleft = 1;
  }

  // first, clear the screen
  screen.clearDisplay();
  screen.setFont(&FreeSans9pt7b); // set the font

  // then, draw the menu-points

  // title-------------------------------------------------
  if (selected == 0 && hasTitle)
  {
    int startpoint = (width - (strlen(MenuTitle) * 10)) / 2;
    int length = (strlen(MenuTitle) * 7) + ((strlen(MenuTitle) - 1) * 2);
    screen.setFont(&FreeSansBold9pt7b);
    screen.setCursor(startpoint, height/4);
    screen.setTextSize(1);
    screen.print(MenuTitle);
    screen.setFont(&FreeSans9pt7b);
    if (titleUnderline == true)
    {
      screen.drawLine(startpoint, (height/4)+2, (startpoint + length), (height/4)+2, 1);
    }
  }

  // Menu point 1------------------------------------------

  if (selected != 0)
  {
    if (styles[selected - 1] == standard)
    {
      screen.setCursor(30, (height/8)+4);
      screen.setTextSize(1);
      screen.setTextColor(SSD1306_WHITE);
      screen.print(MenuItems[selected - 1]);
      if (pictures[selected - 1] != NULL)
      {
        screen.drawBitmap(2, height/32, pictures[selected - 1], LOGO_WIDTH, LOGO_HEIGHT, 1);
      }
    }
    else if (styles[selected - 1] == toggle)
    {
      screen.setCursor(10, (height/8)+4);
      screen.setTextColor(SSD1306_WHITE);
      screen.setTextSize(1);
      screen.print(MenuItems[selected - 1]);
      if (toggles[selected - 1] == false)
      {
        screen.fillCircle(100, (height/8)+1, 5, 1);
      }
      else
      {
        screen.fillCircle(115, (height/8)+1, 5, 1);
      }
      screen.drawRoundRect(93, height/32, 30, (height/4)-1, 4, 1);
    }
    else if (styles[selected - 1] == slider)
    {
      screen.setCursor(10, (height/8)+4);
      screen.setTextColor(SSD1306_WHITE);
      screen.setTextSize(1);
      screen.print(MenuItems[selected - 1]);

      screen.setCursor(98, (height/8)+4);
      screen.print(sliderValues[selected - 1]);

      screen.drawTriangle(120, (height/32)+1, 126, (height/16)+2, 120, (height/8)+1, 1);
    }
  }
  // menu point 2 (selected)---------------------------------
  if (styles[selected] == standard)
  {
    screen.drawRoundRect(0, (height/3)+1, 128, height/3, 2, 2); // Draws a rounded rectangle around the text and pictogram to mark it as selected
    screen.setCursor(30, (height/2)+4);
    screen.setTextColor(SSD1306_WHITE);

    screen.setTextSize(1);
    screen.print(MenuItems[selected]);
    if (pictures[selected] != NULL) 
    {

      screen.drawBitmap(2, (height/3)+6, pictures[selected], LOGO_WIDTH, LOGO_HEIGHT, 1);
    }
  }
  else if (styles[selected] == toggle)
  {
    screen.drawRoundRect(0, (height/3)+1, 128, height/3, 2, 2); // Draws a rounded rectangle around the text and pictogram to mark it as selected
    screen.setCursor(10, (height/2)+4);
    screen.setTextColor(SSD1306_WHITE);
    screen.setTextSize(1);
    screen.print(MenuItems[selected]);
    if (toggles[selected] == false)
    {
      screen.fillCircle(100, height/2, (height/16)+1, 1);
    }
    else
    {
      screen.fillCircle(115, height/2, (height/16)+1, 1);
    }
    screen.drawRoundRect(93, (height/3)+4, 30, (height/4), (height/16), 1);
  }
  else if (styles[selected] == slider)
  {
    screen.drawRoundRect(0, (height/3)+1, 128, height/3, 2, 2); // Draws a rounded rectangle around the text and pictogram to mark it as selected
    screen.setCursor(10, (height/2)+4);
    screen.setTextColor(SSD1306_WHITE);
    screen.setTextSize(1);
    screen.print(MenuItems[selected]);

    screen.setCursor(96, (height/2)+4);
    screen.print(sliderValues[selected]);

    screen.drawTriangle(120, (height/3)+6, 126, (height/2)-2, 120, (height/2)+3, 1);
  }

  // menu point 3----------------------------------------
  if (selected != indx)
  {
    if (styles[selected + 1] == standard)
    {
      screen.setCursor(30, (height/4)*4);
      screen.setTextColor(SSD1306_WHITE);
      screen.print(MenuItems[selected + 1]);
      if (pictures[selected + 1] != NULL)
      {
        screen.drawBitmap(2, height/1.361702127659574, pictures[selected + 1], LOGO_WIDTH, LOGO_HEIGHT, 1);
      }
    }
    else if (styles[selected + 1] == toggle)
    {
      screen.setCursor(10, 56);
      screen.setTextColor(SSD1306_WHITE);
      screen.setTextSize(1);
      screen.print(MenuItems[selected + 1]);
      if (toggles[selected + 1] == false)
      {
        screen.fillCircle(100, 55, 5, 1);
      }
      else
      {
        screen.fillCircle(115, 55, 5, 1);
      }
      screen.drawRoundRect(93, 48, 30, 15, 4, 1);
    }
    else if (styles[selected + 1] == slider)
    {
      screen.setCursor(10, 56);
      screen.setTextColor(SSD1306_WHITE);
      screen.setTextSize(1);
      screen.print(MenuItems[selected + 1]);

      screen.setCursor(96, 56);
      screen.print(sliderValues[selected + 1]);

      screen.drawTriangle(120, 47, 126, 50, 120, 55, 1);
    }
  }

  screen.display();

  // resetting the isPressed variables
  if ((digitalRead(UP) == LOW) && (pressed == 1))
  {
    pressed = 0;
  }

  if ((digitalRead(DOWN) == LOW) && (presseddown == 1))
  {
    presseddown = 0;
  }
  if ((digitalRead(ENTER) == LOW) && (pressedenter == 1))
  {
    pressedenter = 0;
  }
  if ((digitalRead(LEFT) == LOW) && (pressedleft == 1))
  {
    pressedleft = 0;
  }
  if ((digitalRead(RIGHT) == LOW) && (pressedright == 1))
  {
    pressedright = 0;
  }
}

bool OledMenu::getEnter(bool getAll)
{
  if (getAll == true)
  {
    if (entered == 1 || entered == 2)
    {
      entered = 0;
      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    if (entered == 1)
    {
      entered = 0;
      return true;
    }
    else
    {
      return false;
    }
  }
}

int OledMenu::getSelected()
{
  return selected;
}

void OledMenu::clear()
{
  memset(MenuItems, 0, sizeof(MenuItems));
  memset(pictures, 0, sizeof(pictures));
  memset(styles, 0, sizeof(styles));
  memset(toggles, 0, sizeof(toggles));
  strcpy(MenuTitle, "");
  hasTitle = false;
  selected = 0;
  indx = 0;
}

int getArraySize(char array[][50])
{
  int capacity = sizeof(array) / sizeof(array[0]);
  int size = 0;

  for (int i = 0; i <= capacity; i++)
  {
    if (array[i] != NULL)
    {
      size++;
    }
    else
    {
      break;
    }
  }
  return size;
}