# Bela LCD library

This is a library for running an LCD screen (using chipset HD44780). 

##Some notes:

- Communication rate is slow atm (10000 samples) because I have an LED output board hooked up and I'm reading the voltages off the pins to make the lights go blinky blink and it looks cool at 10000 samples.
- This structure successfully uses the BelaContext struct (which I was worried about) by passing the current instance back and forth between member functions. Seems to work.
- This entire endeavour was a bumpain because sleeping the main thread isn't allowed. Other similar systems (BBB and Arduino) use this approach because they're not concerned about underruns, but that is not an option for this platform so something else had to be found. NOTE: No, AuxiliaryTask threads won't work because they don't have access to the context struct. Yes I'm sure. 
- Therefore, this communication is done sample-by-sample, by calling the watcher() function every sample (within the audio loop inside render()) and passing some flags around. 
- To execute tasks there is a deque that the watcher looks at to see what it should be doing next. If a task has smaller tasks within it (like FLASH_E means setting the pin and waiting a few times), those tasks are stored in another deque and inserted into the task deque at the front of the queue. Deques are great - they're totally elastic, I tried with vectors but since the task_queue can theoretically contain tons and tons of items and vectors hold on to the memory that's used to hold what's in them, that's asking for megatrouble. Deques are totally the way to go.

##To Do:

- Implement 4 bit communication. 8 bit works and 4 bit just needs to send the message in two chunks, but I haven't done it yet. This will have the major benefit of using 6 GPIO, and 0-5 are all near each other on the board. (This also means that maybe we could make a little module that fit onto a Bela board).
- Figure out the FUNCTION_SET number and its edge cases. I've got the maths going, I just haven't double checked the documentation so I'm not using it. 
- Determine the best way of sending pin numbers - right now I have a specific set up but you'll have to be able to declare this. Overloaded init() functions are 
- Neaten up the send_char and set_pins functions.
- Write a create_task() function, where you could set_pins/command/flash_e (so, for example, you could return the cursor home or whatever).
- Any way to get chars in from the keyboard? That would be a cool demo.

##Fun project:
- I would really like to do some ultra-quantised crappy waveform visualisation with this screen. 

##This is a good blog post on HD44780 communication:

http://www.site2241.net/november2014.htm
