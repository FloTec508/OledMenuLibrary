//---------------------------------------
// header file
// OLED-Menu library for making menus with SSD1306 I2C Oled screens using the adafruit SSD1306 library
// By Florian Winkler
//------------------------------------------

#ifndef OledMenu_h
#define OledMenu_h
#include "Arduino.h"
#include <time.h>

#define SSD1306_SWITCHCAPVCC 0x02
#define enter 0
#define up 1
#define down 2
#define left 3
#define right 4
#define standard 0
#define toggle 10
#define slider 11

//the items for the statusBar
#define bluetooth 0
#define battery_Voltage 1
#define battery_Icon 2
#define wifi 3

//the items for the dashboard
#define timefield 0
#define datefield 1
#define line 2
    
    
    

class OledMenu
{
public:
    // constructor
    OledMenu(int SCREEN_ADDRESS, int SCREEN_HEIGHT, int SCREEN_WIDTH, int OLED_RESET);
    // methods

    bool initialize(uint8_t switchvcc = SSD1306_SWITCHCAPVCC, uint8_t i2caddr = 0); // initializes the screen as "screen". returns FALSE if display allocation failed

    void attachInput(int pin, int action); // enter=0; up=1; down=2; left=3; right=4

    void clear();

    void addMenuItem(char label[], const unsigned char picture[] = NULL, int style = 0); // pictogram can be set to NULL for a pictureless Menu item

    void addMenuTitle(char title[], bool underline = true); // add a title to the menu

    void startMenu(bool loop=false); // if loop is true, than an automatic loop is started. If false, you have to call this funtion inside a loop to continously check for key inputs

    void setSelected(int Selected); // set the currently selected item

    bool getEnter(bool getAll = false); // returns true, if the enter key has been pressed and therefore, a MenuItem has been selected

    int getSelected(); // return the currently selected item, if 'afterEnter' is true, the funtion does not reurn the selected item, but instead NULL until the ENTER key has been pressed.
    // get the value of a toggle box, 'field' specifies the numboer of the element that contains the toggle box....
    bool getToggleValue(int field);
    // set the value of a toggle box
    void setToggleValue(int field, bool value);
    //get the given Silder Value
    int getSliderValue(int field);

    void setSliderValue(int field, int value);
    // max sets the maximum number that the user can select, label sets a small label to let the user know what input is wanted.
    void numberPicker(int max, char label[]);

    int startslider(int val = 0, int maxval = 10);


    //functions for working with the dashboard
    void addStatusBarItem(int type);//adds a field to the status Bar

    void addDashboardField(int type);//adds a field to the dashboard
    
    void updateDashboard(struct tm time, int wifi_status, bool bluetoothConnected, int batteryStatus, float BatteryVoltage);//updates the dashboard
    
    

    // variables
    int numberSelected = 0; // returned Value of the NumberPicker
    int selected = 0;       // currently selected Item in the menu
    
    int UP;
    int DOWN;
    int ENTER;
    int LEFT;
    int RIGHT;
    // variables to get, if buttons have been pressed before
    int pressed = 0;
    int presseddown = 0;
    int pressedenter = 0;
    int pressedleft = 0;
    int pressedright = 0;
    int entered = 0;
    int checkboxed = 0; // 0, if the enter-Action toggled a switch or a checkbox, so the pressedEnter doesn't get trigered.

    // has the screen been initialized?
    bool initialized = false;

    // the current index of the elements
    int indx = 0;

    //the current index of the dashBoard array
    int indxDash = 0;

    //the current index of the statusBar array
    int indxBar = 0;

    
    


private:

    

    
};



#endif