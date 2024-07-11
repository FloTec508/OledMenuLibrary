# OledMenuLibrary
An Arduino library for creating menus with Arduino

# Introduction

This library allows you to easily create simple, modern menus and other UI-related graphics like a configurable dashboard and a number picker. This library uses the 'Adafruit_SSD1306' and 'Adafruit_GFX' library for handling the display. These two libraries are required and have to be installed in order to use the 'OledMenu' library.

# How to use

## installation
1. Download the repository and unzip/move it into your Arduino Library folder.
2. Make shure that both the `Adafruit_SSD1306` and `Adafruit_GFX` Libraries are installed.
3. include the `<OledMenu.h>` file in your sketch and start coding

## Commands and usecases

You can use many different commands to build your menu. Use the example sketch or this small introduction to get an overiview of how to use this library.

#### initialisation

+ First, create an `OledMenu` object. In this example, let's call it `menu`:

`OledMenu menu(SCREEN_ADDRESS,SCREEN_HEIGHT,SCREEN_WIDTH,OLED_RESET);`

- `SCREEN_ADDRESS`: The i2C adress of the screen
- `SCREEN_HEIGHT`: The height(in px) of the screen. Supported are 128x32 and 128x64 screens.
- `SCREEN_WIDTH`: The width(in px) of the screen.
- `OLED_RESET`: The reset pin of the Oled Screen. Set to -1 if your screen doesn't have one.

+ Next, initialize the library and the screen:
  `if(!menu.initialize(switchvcc, i2caddr)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }`

This code ensures, that both the library and the screen have been initialized correctly. For the `switchvcc` parameter, use the value `0x02`,see the Adafruit SSD1306 documentation for more info. In `i2caddr` use the I2C address of the screen.

Now the initialisation process is complete and you can start designing your menu.

### commands for creating menus

+ `attachInput(int pin, int action)`: Attatches the given Input pin to an action in the menu (up, down, left, right, enter). Use the provided macros to specify the type. For example: `menu.attachInput(14, up)`
+ `clear()`: Clears the current menu buffer and allows for a new menu to be created
+ `addMenuItem(char label[], const unsigned char picture[] = NULL, int style = 0)`: Adds a Menu item to the buffer. `label` specifies the text of the item, `picture` is a small pictogram(width: 15px, height 13px; set to NULL for a pictureless item) and `style` is the type of the item: standard-a normal item, toggle-an item with a toggle switch, slider-item which shows a slider to select numbers when the item is selected.
+ `addMenuTitle(char title[], bool underline = true)`: adds a title to the menu. `title` specifies the string and `underline` toggles, wether the text should be undelined or not.
+ `startMenu()`: Draws the menu and recognizes attatched key inputs. Has to be executed in a loop. For example:
  `while(!menu.getEnter())
  menu.startMenu();
  }` draws the menu while no item has been selected
+ `setSelected(int Selected)`: Set the currently selected item
+ `getEnter(bool getAll = false)`: Returns `true`, when a menu item has been selected. `getAll` specifies, wether interactions with toggle switches and sliders shall be returned too(normaly set to false).
+ `getSelected()`: Returns the currently selected menu Item.
+ `getToggleValue(int field)`: Returns the toggle switch value of the given menu Item. `NULL` when the item is not a toggle switch
+ `setToggleValue(int field, bool value)`: Sets the toggle switch state of the given Menu Item.
+ `getSliderValue(int field)`: Get the slider value of the given menuItem. Returns `NULL` if the item is not a slider.
+ `setSliderValue(int field, int value)`: Set the slider value of the given item
+ `numberPicker(int max, char label[])`: Starts a number picker screen. `max` sets the maximum Value that can be selected. `label` is a optional label to let the user know, what information has to be entered.
  
### commands for the dashboard

This library also supports drawing a dashboard. Use the following commands to build it:

+  `addStatusBarItem(int type)`: Add a field to the statusBar of the Dashboard. Available Macros are: `bluetooth` for a bluetooth indicator, `battery_Voltage` for a battery Voltage meter, `battery_Icon` for a icon showing the battery charge graphically and `wifi` for a wifi indicator.
+  `addDashboardField(int type)`: Add a field to the dashboard. EACH FIELD CAN ONLY BE USED ONCE AND HAS TO BE SPECIFIED! Available macros are: `timefield` for a big digital Clock showing the current time, `datefield` for the current date and `line` for a horizontal line for decoration.
+  `updateDashboard(struct tm time, int wifi_status, bool bluetoothConnected, int batteryStatus, float BatteryVoltage)` draw and update the dashboard with the given Values


**WIP: A NOTIFICATION CENTER. CURRENTLY ONLY SHOWS "NO NOTIFICATIONS"**

# Images

