#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <pigpio.h>
#include "helper.h"

#define PIN_BTN 14
#define PIN_LEDR 16
#define PIN_LEDG 20
#define PIN_LEDB 21
#define PIN_SERVO 12

// NEVER change SERVO_POS_MIN and SERVO_POS_MAX.
// Changing the two values may break your servo motor.
#define SERVO_POS_MIN 1000
#define SERVO_POS_MAX 2000

#define LOOP_PERIOD_MS 50

volatile bool g_led_on = false;
volatile bool g_led_color = false;

void myISR_ledBlink()
{
    printf("Timer ISR_ledToggle at %lu ms\r\n", millis());
    g_led_on = !(g_led_on);
}

void myISR_ledColor()
{
    printf("Timer ISR_ledColor at %lu ms\r\n", millis());
    g_led_color = true;
}

int main()
{
    unsigned long t_start_ms, t_elapsed_ms;
    int led_color = 0;

    // GPIO settings
    if(gpioInitialise()<0) {
        printf("Cannot initialize GPIOs\r\n");
        return 1;
    }

    gpioSetMode(PIN_LEDR, PI_OUTPUT);
    gpioSetMode(PIN_LEDG, PI_OUTPUT);
    gpioSetMode(PIN_LEDB, PI_OUTPUT);
    gpioSetMode(PIN_BTN, PI_INPUT);

    gpioSetPullUpDown(PIN_BTN, PI_PUD_UP);
    gpioWrite(PIN_LEDR, PI_LOW);
    gpioWrite(PIN_LEDG, PI_LOW);
    gpioWrite(PIN_LEDB, PI_LOW);

    // Interrupt settings
    gpioSetTimerFunc(0, 200, myISR_ledBlink);
    gpioSetTimerFunc(1, 2000, myISR_ledColor);

    // Infinite loop
    while(1) {
        t_start_ms = millis();

        if(g_led_on) {
            // While the LED is turned on, switch the LED color if
            // `g_led_color` is asserted in the ISR.
            if(g_led_color)
            {
                g_led_color = false;
                led_color = led_color % 7 + 1;
            }
            // Setting LED color using bit-wise operators. Three LSBs of
            // `led_color` represents the state of BGR, respectively.
            gpioWrite(PIN_LEDR, (led_color & 0x01) && 1);
            gpioWrite(PIN_LEDG, (led_color & 0x02) && 1);
            gpioWrite(PIN_LEDB, (led_color & 0x04) && 1);
        }
        else {
            gpioWrite(PIN_LEDR, PI_LOW); 
            gpioWrite(PIN_LEDG, PI_LOW);
            gpioWrite(PIN_LEDB, PI_LOW);
        }

        t_elapsed_ms = millis() - t_start_ms;
        sleep_ms(LOOP_PERIOD_MS - t_elapsed_ms);
    }

    return 1;
}
