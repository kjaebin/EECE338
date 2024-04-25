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

#define SERVO_POS_MIN 1000
#define SERVO_POS_MAX 2000
#define LOOP_PERIOD_MS 1000


/* [P2] Write your global variables FROM here*/

volatile int ledColorIndex = 0;  // Used to track current LED color index
int ledStates[5][3] = {  // RGB states for different colors
    {1, 0, 0},  // Red
    {0, 1, 0},  // Green
    {0, 0, 1},  // Blue
    {1, 1, 0},  // Yellow
    {1, 0, 1}   // Magenta
};
volatile int servoState = 0;  // Used to track current servo position state

/* [P2] Write your global variables UP TO here*/



/* [P2] Write your function FROM here, if needed */

void updateLEDColor() {
    gpioWrite(PIN_LEDR, ledStates[ledColorIndex][0]);
    gpioWrite(PIN_LEDG, ledStates[ledColorIndex][1]);
    gpioWrite(PIN_LEDB, ledStates[ledColorIndex][2]);
    ledColorIndex = (ledColorIndex + 1) % 5;  // Cycle through colors
}

/* [P2] Write your function UP TO here, if needed */

void myISR()
{
    btn_state = gpioReadDebounce(PIN_BTN);
    if (btn_state < 0) // If the input is noise, do nothing
        return;

    printf("Interrupt! Button state: %d\r\n", btn_state);

    /*** [P2] Write your code FROM here ***/
    // If the button is pushed, change the color of the LED. Be
    // sure the LED color switches between five or more colors.

    if (btn_state == 1) {  // Check if the button is pressed
        updateLEDColor();  // Change LED color
    }

    /*** [P2] Write your code UP TO here ***/
}

int main()
{
    unsigned long t_start_ms, t_elapsed_ms;

    /* [P2] Write your variables FROM here, if needed */

    /* [P2] Write your variables UP TO here, if needed */

    // GPIO settings
    if (gpioInitialise() < 0) {
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
    while (1) {
        t_start_ms = millis();

        /*** [P2] Delete all codes in between and write your code FROM here ***/

        int servo_angle = SERVO_POS_MIN + servoState * SERVO_ANGLE_STEP;
        if (servo_angle > SERVO_POS_MAX || servo_angle < SERVO_POS_MIN) {
            servoState = 0; // Reset state if out of bounds
            servo_angle = SERVO_POS_MIN;
        }
        gpioServo(PIN_SERVO, servo_angle);
        servoState = (servoState + 1) % 5;  // Increment state for next iteration

        /*** [P2] Write your code UP TO here ***/

        t_elapsed_ms = millis() - t_start_ms;
        sleep_ms(LOOP_PERIOD_MS - t_elapsed_ms);
    }

    return 1;
}
