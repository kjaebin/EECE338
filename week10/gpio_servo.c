#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>
#include <time.h>
#include "helper.h"

#define PIN_SERVO 12
// NEVER change SERVO_POS_MIN and SERVO_POS_MAX.
// Changing two values may damage your servo motor.
#define SERVO_POS_MIN 1000
#define SERVO_POS_MAX 2000

int main()
{
    int servo_angle;

    srand((unsigned int)time(NULL));

    // GPIO settings
    if(gpioInitialise()<0) {
        printf("Cannot initialize GPIOs\r\n");
        return 1;
    }

    gpioSetMode(PIN_SERVO, PI_OUTPUT); // Can be omitted

    // Infinite loop
    while(1) {
        // Locate the servo horn to random position
        servo_angle = rand()%(SERVO_POS_MAX-SERVO_POS_MIN) + SERVO_POS_MIN;
        gpioServo(PIN_SERVO, servo_angle);
        printf("Servo position: %d\r\n", servo_angle);
        sleep_ms(1000);
    }

    return 1;
}
