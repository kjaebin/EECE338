#include <stdio.h>
#include <stdlib.h>
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
#define LOOP_PERIOD_MS 1000

/* [P2] Write your global variables FROM here*/
volatile int btn_state;
volatile bool g_led_color = false;
/* [P2] Write your global variables UP TO here*/



/* [P2] Write your function FROM here, if needed */
void switch_led_color(int led_state)
{
    switch (led_state) {
    case 1:
        gpioWrite(PIN_LEDR, PI_HIGH);
        gpioWrite(PIN_LEDG, PI_LOW);
        gpioWrite(PIN_LEDB, PI_LOW);
        break;
    case 2:
        gpioWrite(PIN_LEDR, PI_LOW);
        gpioWrite(PIN_LEDG, PI_HIGH);
        gpioWrite(PIN_LEDB, PI_LOW);
        break;
    case 3:
        gpioWrite(PIN_LEDR, PI_HIGH);
        gpioWrite(PIN_LEDG, PI_HIGH);
        gpioWrite(PIN_LEDB, PI_LOW);
        break;
    case 4:
        gpioWrite(PIN_LEDR, PI_LOW);
        gpioWrite(PIN_LEDG, PI_LOW);
        gpioWrite(PIN_LEDB, PI_HIGH);
        break;
    case 5:
        gpioWrite(PIN_LEDR, PI_HIGH);
        gpioWrite(PIN_LEDG, PI_LOW);
        gpioWrite(PIN_LEDB, PI_HIGH);
        break;
    case 6:
        gpioWrite(PIN_LEDR, PI_LOW);
        gpioWrite(PIN_LEDG, PI_HIGH);
        gpioWrite(PIN_LEDB, PI_HIGH);
        break;
    case 7:
        gpioWrite(PIN_LEDR, PI_HIGH);
        gpioWrite(PIN_LEDG, PI_HIGH);
        gpioWrite(PIN_LEDB, PI_HIGH);
        break;
    default:
        gpioWrite(PIN_LEDR, PI_LOW);
        gpioWrite(PIN_LEDG, PI_LOW);
        gpioWrite(PIN_LEDB, PI_LOW);
        break;
    }
}
int change_servo_angle(int servo_state) {
    int servo_angle;
    servo_angle = SERVO_POS_MIN + servo_state * 500; // Each state changes the angle by 45 degrees
    if (servo_angle > SERVO_POS_MAX) {
        servo_angle = SERVO_POS_MIN;
    }
    return servo_angle;
}
/* [P2] Write your function UP TO here, if needed */

void myISR()
{
    btn_state = gpioReadDebounce(PIN_BTN);
    if(btn_state < 0) // If the input is noise, do nothing
        return;

    printf("Interrupt! Button state: %d\r\n", btn_state);

    /*** [P2] Write your code FROM here ***/
    // If the button is pushed, change the color of the LED. Be
    // sure the LED color switches between five or more colors.
     g_led_color = true;
  
    /*** [P2] Write your code UP TO here ***/
}

int main()
{
    unsigned long t_start_ms, t_elapsed_ms;

    /* [P2] Write your variables FROM here, if needed */
    int angle;
    int n = 0;
    int led_state = 0;
    /* [P2] Write your variables UP TO here, if needed */

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
    gpioSetISRFunc(PIN_BTN, EITHER_EDGE, 0, myISR);

    // Infinite loop
    while(1) {
        t_start_ms = millis();

        /*** [P2] Delete all codes in between and write your code FROM here ***/
        angle = change_servo_angle(n++);
        gpioServo(PIN_SERVO, angle);
        n %= 5;
        sleep_ms(LOOP_PERIOD_MS);
        if (g_led_color) {
            g_led_color = false;
            led_state = led_state % 7 + 1;
            switch_led_color(led_state);
        }
        /*** [P2] Write your code UP TO here ***/

        t_elapsed_ms = millis() - t_start_ms;
        sleep_ms(LOOP_PERIOD_MS - t_elapsed_ms);
    }

    return 1;
}
