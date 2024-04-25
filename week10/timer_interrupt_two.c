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

#define SERVO_POS_MIN 1000
#define SERVO_POS_MAX 2000

#define LOOP_PERIOD_MS 1000  // Set the loop period properly


/* [P3] Write your global variables FROM here */

volatile bool led_on = false;
volatile int led_color_index = 0;
volatile int servo_angle_index = 0;

/* [P3] Write your global variables UP TO here */


/* [P3] Write your function FROM here, if needed */

/* [P3] Write your function UP TO here, if needed */

void myISR_ledToggle()
{
    /* [P3] Write your code FROM here */
    
    led_on = !led_on;
    if (!led_on) {
        servo_angle_index = 0; // Reset servo state when LED is OFF
    }

    /* [P3] Write your code UP TO here */
}

void myISR_ledColor()
{
    /* [P3] Write your code FROM here */
    
    if (led_on) {
        led_color_index = (led_color_index + 1) % 7; // Cycle through 7 colors
    }

    /* [P3] Write your code UP TO here */
}

void myISR_servo()
{
    /* [P3] Write your code FROM here */
    
    if (led_on) {
        servo_angle_index = (servo_angle_index + 1) % 5; // Cycle through 5 servo states
    }

    /* [P3] Write your code UP TO here */
}

int main()
{
    unsigned long t_start_ms, t_elapsed_ms;
    /* [P3] Write your variables FROM here*/
    
    int servo_positions[5] = { 1000, 1125, 1250, 1375, 1500 }; // Define servo positions

    /* [P3] Write your variables UP TO here*/

    srand((unsigned int)time(NULL));

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
    /* [P3] Write your code FROM here */
    // Use gpioSetTimerFunc() properly.
    //gpioSetTimerFunc(0, *, *);
    //gpioSetTimerFunc(1, *, *);
    //gpioSetTimerFunc(2, *, *);

    gpioSetTimerFunc(0, 4000, myISR_ledToggle);
    gpioSetTimerFunc(1, 500, myISR_ledColor);
    gpioSetTimerFunc(2, 600, myISR_servo);

    /* [P3] Write your code UP TO here */

    // Infinite loop
    while(1) {
        t_start_ms = millis();
        
        /* [P3] Write your code FROM here */
        
            // While the LED is turned on, switch the LED color if
            // `g_led_color` is asserted in the ISR.
            
            // Setting LED color using bit-wise operators. Three LSBs of
            // `led_color` represents the state of BGR, respectively.
        
        if (led_on) {
            gpioWrite(PIN_LEDR, led_color_index & 0x01);
            gpioWrite(PIN_LEDG, led_color_index & 0x02);
            gpioWrite(PIN_LEDB, led_color_index & 0x04);
        }
        else {
            gpioWrite(PIN_LEDR, PI_LOW);
            gpioWrite(PIN_LEDG, PI_LOW);
            gpioWrite(PIN_LEDB, PI_LOW);
        }
        // Control Servo based on the current servo_angle_index
        if (servo_angle_index != 0) {
            gpioServo(PIN_SERVO, servo_positions[servo_angle_index]);
        }

        /* [P3] Write your code UP TO here */

        t_elapsed_ms = millis() - t_start_ms;
        sleep_ms(LOOP_PERIOD_MS - t_elapsed_ms);
    }

    return 1;
}
