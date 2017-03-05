#include <Bela.h>
#include <LCD.h>


LCD display;

bool setup(BelaContext *context, void *userData)
{

    rt_printf("We are starting now\n");
   
    display.setup(context); // Pass the function set values here eventually
    return true;
}

void render(BelaContext *context, void *userData)
{
    
    for (int n = 0; n < context->audioFrames; n++) {
        display.watcher(context, n);
    }
    

}

void cleanup(BelaContext *context, void *userData)
{

}