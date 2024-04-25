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

#define LOOP_PERIOD_MS 10

volatile uint8_t rIntensity = 255;
volatile uint8_t gIntensity = 0;
volatile uint8_t bIntensity = 125;
volatile int8_t rDelta = -5, gDelta = 5, bDelta = -5;

volatile uint16_t servo_angle = SERVO_POS_MIN;
volatile int8_t sDelta = 10;

volatile int mode = MODE_MOOD;
volatile int btn_state;
volatile bool g_led_fade = false;

void myISR_fade()
{
    if(mode == MODE_MOOD)
    {
        g_led_fade = true;

        // Slowly increase or decrease the intensity of each color
        rIntensity = rIntensity + rDelta;
        gIntensity = gIntensity + gDelta;
        bIntensity = bIntensity + bDelta;

        if (rIntensity >= 255 || rIntensity <= 0) rDelta *= -1;
        if (gIntensity >= 255 || gIntensity <= 0) gDelta *= -1;
        if (bIntensity >= 255 || bIntensity <= 0) bDelta *= -1;
    }
    else {
        g_led_fade = false;
    }
}

void myISR_setMode()
{
    btn_state = gpioReadDebounce(PIN_BTN);
    if(btn_state < 0) // If the input is noise, do nothing
        return;

    mode = (mode + 1) % 3; // Cycle through the modes
}

void myISR_servo()
{
    if (mode == MODE_MOTOR) {
        servo_angle += 225; // Increment by 45 degrees
        if (servo_angle > SERVO_POS_MAX || servo_angle < SERVO_POS_MIN) {
            sDelta = -sDelta;
            servo_angle += sDelta; // Reverse direction at bounds
        }
    }
}

void myISR_led()
{
    if (mode == MODE_LED) {
        g_led_fade = !g_led_fade; // Toggle LED fade state
    }
}

void myISR_color()
{
    if (mode == MODE_MOOD && g_led_fade) {
        rIntensity += rDelta;
        gIntensity += gDelta;
        bIntensity += bDelta;

        // Invert delta at the bounds to create fading effect
        if (rIntensity >= 255 || rIntensity <= 0) rDelta = -rDelta;
        if (gIntensity >= 255 || gIntensity <= 0) gDelta = -gDelta;
        if (bIntensity >= 255 || bIntensity <= 0) bDelta = -bDelta;
    }
}

void gpioRGBColor(int rIntensity, int gIntensity, int bIntensity)
{
    gpioAnalogWrite(PIN_LEDR, rIntensity);
    gpioAnalogWrite(PIN_LEDG, gIntensity);
    gpioAnalogWrite(PIN_LEDB, bIntensity);
}

int main()
{
    if (gpioInitialise() < 0) {
        printf("Cannot initialize GPIOs\r\n");
        return 1;
    }

    gpioSetMode(PIN_LEDR, PI_OUTPUT);
    gpioSetMode(PIN_LEDG, PI_OUTPUT);
    gpioSetMode(PIN_LEDB, PI_OUTPUT);
    gpioSetMode(PIN_SERVO, PI_OUTPUT);
    gpioSetMode(PIN_BTN, PI_INPUT);

    gpioSetPullUpDown(PIN_BTN, PI_PUD_UP);
    gpioWrite(PIN_LEDR, PI_LOW);
    gpioWrite(PIN_LEDG, PI_LOW);
    gpioWrite(PIN_LEDB, PI_LOW);

    gpioSetISRFunc(PIN_BTN, EITHER_EDGE, 0, myISR_setMode);

    while(1) {
        unsigned long t_start_ms = millis();

        // Depending on mode, update LED colors or servo position
        if (mode == MODE_MOOD && g_led_fade) {
            gpioRGBColor(rIntensity, gIntensity, bIntensity);
        } else if (mode == MODE_MOTOR) {
            gpioServo(PIN_SERVO, servo_angle);
        } else if (mode == MODE_LED && g_led_fade) {
            gpioRGBColor(rIntensity, gIntensity, bIntensity);
        }

        unsigned long t_elapsed_ms = millis() - t_start_ms;
        gpioDelay((LOOP_PERIOD_MS - t_elapsed_ms) * 1000); // Maintain loop timing
    }

    return 1;
}
