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

#define SERVO_ANGLE_STEP 225  // Define the step size for servo movement

// Global variables
volatile int btn_state = -1;  // Make sure btn_state is declared globally
volatile int led_state = 0;   // Global variable to keep track of LED state

void myISR()
{
    int read_state = gpioRead(PIN_BTN);  // Read the current button state
    if(read_state != PI_LOW)  // Check if the button is pressed
        return;  // Return if not pressed to ignore noise

    printf("Interrupt! Button state: %d\r\n", read_state);

    led_state = (led_state + 1) % 7 + 1;  // Change LED state
    // You can place your LED control logic here or call a function
}

int main()
{
    unsigned long t_start_ms, t_elapsed_ms;
    int servo_angle;

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

    gpioSetISRFunc(PIN_BTN, EITHER_EDGE, 0, myISR);  // Setup ISR for the button

    while(1) {
        t_start_ms = millis();

        // Update servo angle based on some logic, if necessary
        servo_angle = SERVO_POS_MIN + (rand() % 5) * SERVO_ANGLE_STEP;
        gpioServo(PIN_SERVO, servo_angle);
        sleep_ms(1000);  // Delay to simulate time-based changes

        t_elapsed_ms = millis() - t_start_ms;
        sleep_ms(LOOP_PERIOD_MS - t_elapsed_ms);
    }

    return 1;
}
