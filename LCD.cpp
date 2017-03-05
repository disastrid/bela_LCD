/***** LCD.cpp *****/
// Written by Astrid Bin, February 2017.

#include <LCD.h>

// No constructor - put one here if needed

void LCD::send_char(BelaContext *context, int currentFrame, int characterNum) {
    digitalWrite(context, currentFrame, 0, HIGH);
    for (int i = 7; i>= 0; i--) {
        int pin_value = (characterNum >> i) & 1;
        digitalWrite(context, currentFrame, bus_pins[i], pin_value);
    }
    gBusyFlag = 0;
}


void LCD::setup(BelaContext *context) {
    // Do bit shifting to make a function command:
    
    // int function = 0x40;
    // int bit_rate = bits << 4;
    // int num_lines = lines << 3;
    // int type_font = font << 2;
    // int func = 0x20 + bit_rate + num_lines + type_font;
    // FUNCTION_SET = func;
    //rt_printf("Base value: %d\n Bits: %d\n Lines: %d\n Font: %d\n Custom function done! %d\n", 0x40, bit_rate, num_lines, type_font, FUNCTION_SET);
    init(context);
}

void LCD::init(BelaContext *context) {
    pinMode(context, 0, 6, OUTPUT); // command bit (lit when the message is a command)
    pinMode(context, 0, 7, OUTPUT); // data bit (lit when the message is data to write)
    pinMode(context, 0, 0, OUTPUT); // RS
    pinMode(context, 0, 1, OUTPUT); // E
    pinMode(context, 0, 2, OUTPUT); // d0
    pinMode(context, 0, 3, OUTPUT); // d1
    pinMode(context, 0, 4, OUTPUT); // d2
    pinMode(context, 0, 5, OUTPUT); // d3
    pinMode(context, 0, 8, OUTPUT); // d4
    pinMode(context, 0, 9, OUTPUT); // d5
    pinMode(context, 0, 11, OUTPUT);// d6
    pinMode(context, 0, 12, OUTPUT); //d7
    task_queue.insert(task_queue.end(), init_routine.begin(), init_routine.end());
    rt_printf("init tasks pushed to main queue! Main queue is now %d things long\n", task_queue.size());
    gSampleCounter = 0;
}

void LCD::set_pins(BelaContext *context, int currentFrame, int rs, int pins_to_set) {
    rt_printf("IN PIN SETTING! RS = %d, VALUE = %d\n", rs, pins_to_set);
    
    
    digitalWrite(context, currentFrame, 0, LOW); // Set the RS to either data or command (rs is pin 0)

    for (int i = 7; i >= 0; i--) {
        
        int pin_value = (pins_to_set >> i) & 1; // Evaluate next bit in the byte
        // if (rs == 1) // print out if we're writing data to make sure we have it right
        // rt_printf("Pin %d set to %d\n", i, pin_value);
        
        digitalWrite(context, currentFrame, bus_pins[i], pin_value);
    }
    
    gBusyFlag = 0;
    // data bus is now set
}

void LCD::set_e(BelaContext *context, int currentFrame, int e_value) {
    digitalWrite(context, currentFrame, 1, e_value); // Set the E pin (pin 1) to the e_value

    gBusyFlag = 0;
    // e pin is now set
}

void LCD::task_manager(BelaContext *context, int currentFrame, int task) {
    // if (!gBusyFlag) {
    //  gBusyFlag = 1; // If there's a task in the queue, we have to raise the busy flag and get something to complete the task.
    //  // gCurrentTask = task; // Store the current task so the whole system knows what we're doing
        switch (task) {
            case SET_PINS: { // set pins
                // If we're setting pins, we need the next value - that's the pin value.
                int rs = task_queue.at(0); // Next is the RS value, telling us if it's data or a command.
                task_queue.pop_front(); // We need to now get rid of that RS value in the queue, we don't need it anymore.
                int pinsToSet = task_queue.at(0); // The next value is the pin values; we need that.
                task_queue.pop_front(); // We need to now get rid of that pin value now we've set it.
                set_pins(context, currentFrame, rs, pinsToSet);
                break;
            }
            case SET_E: { // Set E pin
                // The next value is what E needs to be set to; we should store that.
                int e_value = task_queue.at(0);
                task_queue.pop_front(); // Then get rid of that from the queue, we've got it.
                set_e(context, currentFrame, e_value);
                break;
            }
            case PAUSE: {
                // The next value is the number of samples we need to wait for.
                gNumSamplesToPause = task_queue.at(0);
                task_queue.pop_front(); // Then get rid of that from the queue, we've got it.
                gSamplePauseStarted = gSampleCounter;
                gPaused = 1;
                break;
            }
            case FLASH_E: {
                
                // Insert the "flash_e" routine at the head of the queue
                task_queue.insert(task_queue.begin(), flash_e.begin(), flash_e.end());
                gBusyFlag = 0;
                break;
            }
            case SEND_STRING: {
                rt_printf("Got message to assemble a string for a message %d long\n", message.size());
                rt_printf("task_queue is %d long\n", task_queue.size() );
                for (int i = 0; i < message.size(); i++) {
                    send_string.push_back(SEND_CHAR);
                    send_string.push_back(message.at(i));
                    send_string.push_back(FLASH_E);
                }
                task_queue.insert(task_queue.begin(), send_string.begin(), send_string.end());
                rt_printf("String tasks pushed! task_queue is %d long\n", task_queue.size());
                gBusyFlag = 0;
                break;
            }
            case SEND_CHAR: { // set pins
                // If we're setting pins, we need the next value - that's the pin value.
                
                int characterNum = task_queue.at(0); // The next value is the pin values; we need that.
                task_queue.pop_front(); // We need to now get rid of that pin value now we've set it.
                send_char(context, currentFrame, characterNum);
                
                break;
            }
            default: {
                rt_printf("Got a command I didn't understand: %d.\n", task);
                gBusyFlag = 0;
                break;
            }
            
        }// end switch statement
        
    // end if statement
}

void LCD::watcher(BelaContext *context, int currentFrame) {
    gSampleCounter++;
    if (gCurrentState == -1 && gSampleCounter == 44100) {
        rt_printf("TIME'S UP, we're going now\n");
        gBusyFlag = 0;
    } else if (!gBusyFlag) {
        // What happens when we're not completing a task? We need to check the queue.
        if (task_queue.size() > 0) {
            // if our task queue is bigger than zero then there's something waiting to be done.
            int thingToDo = task_queue.at(0); // Get the thing at the head of the queue
            gQueueSize = task_queue.size();
            gCurrentTask = thingToDo;
            
            task_queue.pop_front(); // delete the thing at index 0 - we've passed it, we don't need it again
            gBusyFlag = 1;
            task_manager(context, currentFrame, thingToDo); // Pass it for evaluation (this will raise the busy flag)
            
            // testCounter++;
            // int size = task_queue.size();
            // rt_printf("we've passed the task and removed it from the queue %d and it's now %d items long\n", testCounter, size);
        }
    
    }
    
    // ------------------------------
    // LOOK AFTER BUTTONS AND LIGHTS
    // ------------------------------
    
    gCurrentButtonState = digitalRead(context, currentFrame, 7);
    
    if (gCurrentButtonState == 1 && gPreviousButtonState == 0) { //&& gBusyFlag = 0
        gPreviousButtonState = 1;
        gButtonTriggerPoint = gSampleCounter;
        digitalWrite(context, currentFrame, 6, HIGH);
        task_queue.push_back(TURN_LIGHT_ON); // push the light on to the back of the queue. This can only be done on the first impulse. 
        rt_printf("button pressed! BF status: %d\n", gBusyFlag);
        rt_printf("pause status: %d\n", gPaused);
        rt_printf("current task: %d current queue size: %d\n", gCurrentTask, gQueueSize);
    }
    
    if (gCurrentButtonState == 0 && gPreviousButtonState == 1) {
        if (gSampleCounter - gButtonTriggerPoint <= 5000) {
            // we're still debouncing, do nothing
        } else {
            gPreviousButtonState = 0;
            // We're debounced now, set button state to 0. 
        }
    }
    
    // ------------------------------
    // MANAGE STATES
    // ------------------------------
    
    if (gPaused) {
        if (gSampleCounter - gSamplePauseStarted >= 2000) {
            
            gPaused = 0;
            gBusyFlag = 0; // we're not paused anymore. Return the flag.
        }
    }
    
}