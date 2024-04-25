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

#define LOOP_PERIOD_MS 10 // Short period to check the flags and update GPIO states

/* [P3] Global variables */
volatile bool led_on = false;
volatile int led_color_index = 0;
volatile int servo_state = 0;

/* Servo positions corresponding to 45-degree increments */
int servo_positions[] = {SERVO_POS_MIN, 950, 1400, 1850, SERVO_POS_MAX};

/* [P3] ISRs - Toggle flags only */
void myISR_ledToggle() {
    led_on = !led_on;
    if (!led_on) {
        servo_state = 0; // Reset servo when LED is OFF
    }
}

void myISR_ledColor() {
    if (led_on) {
        led_color_index = (led_color_index + 1) % 7; // Cycle through 7 colors
    }
}

void myISR_servo() {
    if (led_on) {
        servo_state = (servo_state + 1) % 5; // Cycle through 5 servo positions
    }
}

int main() {
    unsigned long t_start_ms, t_elapsed_ms;

    if (gpioInitialise() < 0) {
        printf("Cannot initialize GPIOs\r\n");
        return 1;
    }

    gpioSetMode(PIN_LEDR, PI_OUTPUT);
    gpioSetMode(PIN_LEDG, PI_OUTPUT);
    gpioSetMode(PIN_LEDB, PI_OUTPUT);
    gpioSetMode(PIN_SERVO, PI_OUTPUT);
    gpioSetPullUpDown(PIN_BTN, PI_PUD_UP);

    gpioSetTimerFunc(0, 4000, myISR_ledToggle); // LED toggle every 4000ms
    gpioSetTimerFunc(1, 500, myISR_ledColor);   // LED color change every 500ms
    gpioSetTimerFunc(2, 600, myISR_servo);      // Servo adjustment every 600ms

    while (1) {
        t_start_ms = millis();
        
        if (led_on) {
            gpioWrite(PIN_LEDR, led_color_index & 0x01);
            gpioWrite(PIN_LEDG, led_color_index & 0x02);
            gpioWrite(PIN_LEDB, led_color_index & 0x04);
            gpioServo(PIN_SERVO, servo_positions[servo_state]);
        } else {
            gpioWrite(PIN_LEDR, PI_LOW);
            gpioWrite(PIN_LEDG, PI_LOW);
            gpioWrite(PIN_LEDB, PI_LOW);
            gpioServo(PIN_SERVO, SERVO_POS_MIN); // Reset servo position when LED is off
        }

        t_elapsed_ms = millis() - t_start_ms;
        if (LOOP_PERIOD_MS > t_elapsed_ms) {
            gpioDelay((LOOP_PERIOD_MS - t_elapsed_ms) * 1000); // Delay to maintain the loop frequency
        }
    }

    return 1;
}
