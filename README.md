# Bela LCD library

This is a library for running an LCD screen (using chipset HD44780). 

##Some notes:

- Communication rate is slow atm (10000 samples) because I have an LED output board hooked up and I'm reading the voltages off the pins to make the lights go blinky blink and make sure the messages and sequences of actions are correct. It looks cool at 10000 samples, but will eventually be 8 samples.
- This structure successfully uses the BelaContext struct by passing the current sample's instance back and forth between member functions. Seems to work.
- This entire endeavour was a bumpain because sleeping the main thread isn't an option. Other similar systems (BBB and Arduino) use this approach because they're not concerned about underruns, but of course that's out of the question with Bela. (NOTE: No, AuxiliaryTask threads won't work because they don't have access to the context struct for every sample and can't set the pins. Yes I'm sure.)
- What I did is set up a system that does this sample-by-sample, using a watcher() function every sample within the audio loop inside render(), setting some states and passing some flags around. That function keeps track of what task is executing, and looks for a new one in the task_queue if it's finished what it's oding.
- The task_queue is a deque that contains the sequences of tasks that have to be performed (when a task is triggered, the micro tasks are pushed to the queue, and if a task has smaller tasks within it - like FLASH_E - those tasks are stored in another deque and inserted into the task deque at the front of the queue when needed. Deques are great - they're totally elastic, I tried doing all this with vectors but since the task_queue can theoretically contain hundreds of items and vectors hold on to the memory that's used to hold whatever is in them, that's asking for megatrouble. Deques are totally the way to go.

##To Do:

- Implement 4 bit communication. 8 bit works and 4 bit just needs to send the message in two chunks, but I haven't done it yet. This will have the major benefit of using 6 GPIO, and 0-5 are all near each other on the board. (This also means that maybe we could make a little module that fit onto a Bela board).
- Figure out the FUNCTION_SET number and its edge cases. I've got the maths going, I just haven't double checked the documentation so I'm not using it yet.
- Determine the best way of setting up the pin numbers - right now I have a specific set up but you'll have to be able to declare this. Overloaded init() functions are the plan I think.
- Neaten up the send_char and set_pins functions, which are kind of a mess.
- Write a create_task() function, where you could set_pins/command/flash_e (so, for example, you could return the cursor home or whatever).
- Any way to get chars in from the keyboard? That would be a cool demo.

##Fun project:
- I would really like to do some ultra-quantised crappy waveform visualisation with this screen. 

##This is a good blog post on HD44780 communication:

http://www.site2241.net/november2014.htm
