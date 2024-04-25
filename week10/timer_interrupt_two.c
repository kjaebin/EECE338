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
#define LOOP_PERIOD_MS 1000  // Main loop interval

/* [P3] Write your global variables FROM here */
volatile bool led_on = false;  // Tracks the LED state
volatile int led_color_index = 0;  // Tracks the current color index
volatile int servo_angle_index = 0;  // Tracks the servo position index
/* [P3] Write your global variables UP TO here */

void myISR_ledToggle() {
    led_on = !led_on;  // Toggle the LED state on each call
    if (!led_on) {
        gpioWrite(PIN_LEDR, PI_LOW);
        gpioWrite(PIN_LEDG, PI_LOW);
        gpioWrite(PIN_LEDB, PI_LOW);
        servo_angle_index = 0; // Reset servo position when LED is turned off
    }
}

void myISR_ledColor() {
    if (led_on) {
        led_color_index = (led_color_index + 1) % 7;  // Cycle through 7 different colors
    }
}

void myISR_servo() {
    if (led_on) {
        servo_angle_index = (servo_angle_index + 1) % 5;  // Cycle through 5 servo positions
    }
}

int main() {
    unsigned long t_start_ms, t_elapsed_ms;
    int servo_positions[5] = {1000, 1250, 1500, 1750, 2000};  // Define 5 servo positions

    if (gpioInitialise() < 0) {
        printf("Cannot initialize GPIOs\r\n");
        return 1;
    }

    gpioSetMode(PIN_LEDR, PI_OUTPUT);
    gpioSetMode(PIN_LEDG, PI_OUTPUT);
    gpioSetMode(PIN_LEDB, PI_OUTPUT);
    gpioSetMode(PIN_SERVO, PI_OUTPUT);

    // Set timers for LED toggle, color change, and servo motion
    gpioSetTimerFunc(0, 4000, myISR_ledToggle);  // Every 4000 ms toggle LED
    gpioSetTimerFunc(1, 500, myISR_ledColor);  // Every 500 ms change LED color
    gpioSetTimerFunc(2, 600, myISR_servo);  // Every 600 ms change servo angle

    while (true) {
        t_start_ms = millis();

        if (led_on) {
            // Set LED color based on current index
            gpioWrite(PIN_LEDR, (led_color_index & 1) ? PI_HIGH : PI_LOW);
            gpioWrite(PIN_LEDG, (led_color_index & 2) ? PI_HIGH : PI_LOW);
            gpioWrite(PIN_LEDB, (led_color_index & 4) ? PI_HIGH : PI_LOW);
            // Set servo to current position
            gpioServo(PIN_SERVO, servo_positions[servo_angle_index]);
        }

        t_elapsed_ms = millis() - t_start_ms;
        if (LOOP_PERIOD_MS > t_elapsed_ms) {
            usleep((LOOP_PERIOD_MS - t_elapsed_ms) * 1000);
        }
    }

    return 1;
}
