
#include "mbed.h"
#include "platform/mbed_thread.h"



DigitalOut led1(LED1); //green LED
DigitalOut led2(LED2); //blue LED
DigitalOut led3(LED3); //red LED

InterruptIn button(USER_BUTTON);

Timeout button_debounce_timeout;
float debounce_time_interval = 0.3;

int val = 0;
int* t = &val;


const int n = 10;
int* col = new int[n];


int no = 0;

void onButtonStopDebouncing(void);

void onButtonPress(void)
{
    col[no] = *t;
    no++;
    wait_us(300000);
    button_debounce_timeout.attach(onButtonStopDebouncing, debounce_time_interval);

}

void onButtonStopDebouncing(void)
{
    button.rise(onButtonPress);
}
void ledOn()
{
    if(*t==1)
    {
        led1 = true;
        led2 = false;
        led3 = false;
    }
    else if (*t==2)
    {
        led1 = false;
        led2 = true;
        led3 = false;

    }
    else if (*t==3)
    {
        led1 = false;
        led2 = false;
        led3 = true;
    }
}

void displayLight(int l)
{
    if(l==1)
    {
        led1 = true;
        led2 = false;
        led3 = false;
    }
    else if (l==2)
    {
        led1 = false;
        led2 = true;
        led3 = false;

    }
    else if (l==3)
    {
        led1 = false;
        led2 = false;
        led3 = true;
    }
    wait(1);
}
void repeat()
{
    for(int j = 0; j<n; j++)
    {
        displayLight(col[j]);
    }
}
int main()
{
    // Initialise the digital pin LED1 as an output

    led1 = false;
    led2 = false;
    led3 = false;
    button.rise(onButtonPress);
    while (no < 10) {
        ledOn();
        wait(1);
        val = (*t%3)+1;
        t = &val;
    }
    led1 = false;
    led2 = false;
    led3 = false;
    wait(0.5);
    repeat();
    led1 = false;
    led2 = false;
    led3 = false;
}
