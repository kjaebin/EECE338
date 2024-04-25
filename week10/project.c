#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
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

#define MODE_LED 0
#define MODE_MOTOR 1
#define MODE_MOOD 2

volatile int mode = MODE_LED; // Start in LED mode
volatile int btn_state = 0;
volatile bool update_flag = false;

int led_color = 0;
int servo_position = SERVO_POS_MIN;
int servo_step = 500; // Change this to adjust the movement increments

void gpioRGBColor(int rIntensity, int gIntensity, int bIntensity)
{
    gpioAnalogWrite(PIN_LEDR, rIntensity);
    gpioAnalogWrite(PIN_LEDG, gIntensity);
    gpioAnalogWrite(PIN_LEDB, bIntensity);
}

// ISR for button press to change mode
void myISR_setMode(int gpio, int level, uint32_t tick)
{
    if (level == 1) { // Button release
        mode = (mode + 1) % 3; // Cycle through the modes
        update_flag = true; // Set flag to handle mode change in main loop
    }
}

// Initialize GPIOs and ISR
void setupGPIO() {
    if (gpioInitialise() < 0) {
        fprintf(stderr, "Cannot initialize GPIOs\n");
        exit(1);
    }

    gpioSetMode(PIN_BTN, PI_INPUT);
    gpioSetPullUpDown(PIN_BTN, PI_PUD_UP);
    gpioSetISRFunc(PIN_BTN, RISING_EDGE, 0, myISR_setMode);

    gpioSetMode(PIN_LEDR, PI_OUTPUT);
    gpioSetMode(PIN_LEDG, PI_OUTPUT);
    gpioSetMode(PIN_LEDB, PI_OUTPUT);
    gpioSetMode(PIN_SERVO, PI_OUTPUT);
}

int main() {
    setupGPIO();

    unsigned long lastUpdate = 0;
    const unsigned int ledPeriod = 500; // 500 ms for LED mode
    const unsigned int motorPeriod = 800; // 800 ms for Motor mode

    while (1) {
        if (millis() - lastUpdate >= (mode == MODE_MOTOR ? motorPeriod : ledPeriod)) {
            lastUpdate = millis();
            if (mode == MODE_LED || mode == MODE_MOOD) {
                if (mode == MODE_LED || update_flag) {
                    led_color = (led_color + 1) % 8; // Cycle through colors
                    gpioWrite(PIN_LEDR, led_color & 1);
                    gpioWrite(PIN_LEDG, led_color & 2);
                    gpioWrite(PIN_LEDB, led_color & 4);
                }
                if (mode == MODE_MOOD) {
                    // Implement mood mode color changing
                    // Example: Fade in/out colors using gpioRGBColor()
                }
            } else if (mode == MODE_MOTOR) {
                servo_position = (servo_position + servo_step > SERVO_POS_MAX) ? SERVO_POS_MIN : servo_position + servo_step;
                gpioServo(PIN_SERVO, servo_position);
            }
        }

        if (update_flag) {
            if (mode == MODE_LED) {
                // Ensure LEDs are on and cycling in LED mode
            } else if (mode == MODE_MOTOR) {
                // Turn off LEDs in Motor mode
                gpioWrite(PIN_LEDR, 0);
                gpioWrite(PIN_LEDG, 0);
                gpioWrite(PIN_LEDB, 0);
            } else if (mode == MODE_MOOD) {
                // Setup for mood mode if needed
            }
            update_flag = false;
        }
    }

    gpioTerminate();
    return 0;
}
