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

#define SERVO_POS_MIN 500
#define SERVO_POS_MAX 2500

#define LOOP_PERIOD_MS 10  // Set the loop period properly


/* [P3] Write your global variables FROM here */
volatile bool g_led_on = false;
volatile bool g_led_color = false;
volatile bool g_servo = false;
/* [P3] Write your global variables UP TO here */


/* [P3] Write your function FROM here, if needed */
int change_servo_angle(int servo_state) {
    int servo_angle;
    servo_angle = SERVO_POS_MIN + servo_state * 500; // Each state changes the angle by 45 degrees
    if (servo_angle > SERVO_POS_MAX) {
        servo_angle = SERVO_POS_MIN;
    }
    return servo_angle;
}
/* [P3] Write your function UP TO here, if needed */

void myISR_ledToggle()
{
    /* [P3] Write your code FROM here */
    g_led_on = !(g_led_on);
    /* [P3] Write your code UP TO here */
}

void myISR_ledColor()
{
    /* [P3] Write your code FROM here */
    g_led_color = true;
    /* [P3] Write your code UP TO here */
}

void myISR_servo()
{
    /* [P3] Write your code FROM here */
    if (g_led_on) {
        g_servo = 1;

    }
    /* [P3] Write your code UP TO here */
}

int main()
{
    unsigned long t_start_ms, t_elapsed_ms;
    /* [P3] Write your variables FROM here*/
    int led_state = 0;
    int angle;
    int n = 1;
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
    gpioSetTimerFunc(0, LOOP_PERIOD_MS, myISR_ledToggle);
    gpioSetTimerFunc(1, 500, myISR_ledColor);
    gpioSetTimerFunc(2, 600, myISR_servo);
    /* [P3] Write your code UP TO here */

    // Infinite loop
    while(1) {
        t_start_ms = millis();
        
        /* [P3] Write your code FROM here */
        if (g_led_on) {
            // While the LED is turned on, switch the LED color if
            // `g_led_color` is asserted in the ISR.

            // Setting LED color using bit-wise operators. Three LSBs of
            // `led_color` represents the state of BGR, respectively.
            if (g_led_color) {
                g_led_color = false;
                led_state = (led_state + 1) % 7 + 1;
            }
            gpioWrite(PIN_LEDR, (led_state & 0x01) && 1);
            gpioWrite(PIN_LEDG, (led_state & 0x02) && 1);
            gpioWrite(PIN_LEDB, (led_state & 0x04) && 1);

            if (g_servo) {
                g_servo = false;
                angle = change_servo_angle(n++);
                gpioServo(PIN_SERVO, angle);
                n %= 5;
            }
            gpioServo(PIN_SERVO, angle);
        }
        else {
            gpioWrite(PIN_LEDR, PI_LOW);
            gpioWrite(PIN_LEDG, PI_LOW);
            gpioWrite(PIN_LEDB, PI_LOW);
        }
        /* [P3] Write your code UP TO here */

        t_elapsed_ms = millis() - t_start_ms;
        sleep_ms(LOOP_PERIOD_MS - t_elapsed_ms);
    }

    return 1;
}
