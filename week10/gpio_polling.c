#include <stdio.h>
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
#define SERVO_POS_MIN 500
#define SERVO_POS_MAX 2500

#define LOOP_PERIOD_MS 1000

/* [P1] Write your function FROM here, if needed */

/* [P1] Write your function UP TO here, if needed */


int change_servo_angle(int servo_state){
    int servo_angle;
	/* [P1] Write your function for servo*/

	/* [P1] Write your function FROM here*/
    return servo_angle;
}



int main()
{
    unsigned long t_start_ms, t_elapsed_ms;

    /* [P1] Write your variables FROM here */

    /* [P1] Write your variables UP TO here */

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

    // Infinite loop
    while(1) {
        t_start_ms = millis();

        /* [P1] Write your codes FROM here */
        // Set the servo angle to 0->45->90->145->180...

        // Read the button pin state

        // If the button is pushed, change the color of the LED. Be
        // sure the LED color switches between five or more colors.

        /* [P1] Write your codes UP TO here */

        t_elapsed_ms = millis() - t_start_ms;
        sleep_ms(LOOP_PERIOD_MS - t_elapsed_ms);
    }

    return 1;
}
