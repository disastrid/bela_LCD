/***** LCD.h *****/
// Written by Astrid Bin, February 2017.


#include <deque>
#include <string>
#include <Bela.h>

class LCD {
    public:
    // variables:
    BelaContext * classContext;
    int gSampleCounter;
    int gCurrentButtonState;
    int gPreviousButtonState;
    int gButtonTriggerPoint = 0;
    int gLedOn = 1;
    int gLedOff = 0;
    int gSamplePauseStarted;
    int gNumSamplesToPause;
    int gTaskReadPointer = 0;
    int gCurrentState = -1;
    int gQueueSize;
    int gStringIndex = 0;
    int gTaskCounter = 0;
    // These are the pins, numbered from d0 (LSB) to d7 (MSB):
    int bus_pins[8] = {2, 3, 4, 5, 8, 9, 11, 12};
    int rs_pin;
    int e_pin;
    int gPaused = 0;
    int gSendingString = 0;
    int gCurrentTask;
    int gBusyFlag = 1;
    // COMMANDS
    int CLEAR_DISPLAY        = 0x01;
    int RETURN_HOME          = 0x02;
    int ENTRY_MODE_SET        = 0x07;
    int DISPLAY_CONTROL      = 0x08;
    int CURSOR_SHIFT         = 0x10;
    // int FUNCTION_SET         = 0x1c;
    int FUNC                = 56;
    int DISP                = 15;
    int ENTRY               = 6;
    
    int CURSE               = 12;
    int POS                 = 195;
    int H                   = 72;
    
    
    int SETCGRAMADDR        = 0x40;
    int SETDDRAMADDR        = 0x80;
    
    int DISPLAY_ON           = 0x04;
    int DISPLAY_OFF          = 0x00;
    int CURSOR_ON            = 0x02;
    int CURSOR_OFF           = 0x00;
    int BLINK_ON             = 0x01;
    int BLINK_OFF            = 0x00;
    
    // FUNCTION SETTINGS
    int LCD_8BITMODE            = 0x10;
    int LCD_4BITMODE            = 0x00;
    int LCD_2LINE               = 0x08;
    int LCD_1LINE               = 0x00;
    int LCD_5x10DOTS            = 0x04;
    int LCD_5x8DOTS             = 0x00;
    
    // PIN SETTINGS
    // RS PINS (to set command or data)
    int COMMAND = 0;
    int DATA = 1;
    // Tasks:
    enum {
        TURN_LIGHT_ON,
        SET_PINS,
        SET_E, 
        PAUSE,
        FLASH_E,
        SEND_STRING,
        SEND_CHAR
    };
    std::deque<int> flash_e = {
        PAUSE,
        22000,
        SET_E,
        0,
        PAUSE,
        22000,
        SET_E,
        1,
        PAUSE,
        22000,
        SET_E,
        0,
        PAUSE,
        22000
    };
    std::deque<int> init_routine = {
        // Clear display (0x01)
        SET_PINS,
        COMMAND,
        CLEAR_DISPLAY,
        FLASH_E,
        // Send function set three times (b0011100, 8 bit 2 line 5x8 font)
        SET_PINS,
        COMMAND,
        FUNC,
        FLASH_E,
        SET_PINS,
        COMMAND,
        FUNC,
        FLASH_E,
        SET_PINS,
        COMMAND,
        FUNC,
        FLASH_E,
        SET_PINS,
        COMMAND,
        0x0f,
        FLASH_E,
        SEND_STRING
    };
    std::deque<int> send_string;
    std::deque<int> task_queue;

    std::string message = "Ciao Bela!";

    // Member functions:
    void set_pins(BelaContext *context, int currentFrame, int rs_value, int bus_value);
    void set_e(BelaContext *context, int currentFrame, int e_value);
    void setup(BelaContext *context);
    void init(BelaContext *context);
    void lcdsetup(int bits, int lines, int font);
    void send_char(BelaContext *context, int currentFrame, int characterNum);
    void task_manager(BelaContext *context, int currentFrame, int task);
    void watcher(BelaContext *context, int currentFrame);
    
    
};