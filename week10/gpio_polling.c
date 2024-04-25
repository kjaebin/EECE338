#include <stdio.h>
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

int led_state = 1; // Global variable to cycle through LED states

/* [P1] Write your function FROM here, if needed */
void switch_led_color(int led_state) {
    switch(led_state) {
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
/* [P1] Write your function UP TO here, if needed */

int change_servo_angle(int servo_state){
    // Calculate new servo angle based on current state
    int servo_angle = SERVO_POS_MIN + servo_state * 180; // Each state changes the angle by 45 degrees
    if (servo_angle > SERVO_POS_MAX) servo_angle = SERVO_POS_MIN; // Reset to minimum if it exceeds max
    return servo_angle;
}

int main() {
    unsigned long t_start_ms, t_elapsed_ms;
    int servo_state = 0;  // To cycle servo from 0-180 degrees in steps
    int btn_state;

    srand((unsigned int)time(NULL));

    if(gpioInitialise() < 0) {
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

    while(1) {
        t_start_ms = millis();

        /* [P1] Write your codes FROM here */
        // Change servo angle based on state
        int servo_angle = change_servo_angle(servo_state++);
        gpioServo(PIN_SERVO, servo_angle);
        servo_state %= 5; // There are 5 states (0-4) corresponding to 0-180 degrees

        // Check button state and change LED color if pressed
        btn_state = gpioRead(PIN_BTN);
        if(btn_state == PI_LOW) { // Assuming low means pressed
            led_state = (led_state % 7) + 1; // Cycle through 7 color states
            switch_led_color(led_state);
        }
        /* [P1] Write your codes UP TO here */

        t_elapsed_ms = millis() - t_start_ms;
        sleep_ms(LOOP_PERIOD_MS - t_elapsed_ms);
    }

    return 1;
}
