#include <stdio.h>
#include <pigpio.h>
#include "helper.h"

#define PIN_LEDR 16
#define PIN_LEDG 20
#define PIN_LEDB 21


int main()
{
    int led_state = 0;
    // GPIO settings
    if(gpioInitialise()<0) {
        printf("Retrying GPIO initialization");
        gpioTerminate();

        if(gpioInitialise()<0) {
            printf("Cannot initialize GPIOs\r\n");
            return 1;
        }
        printf("Successfully initialized GPIOs");
    }

    gpioSetMode(PIN_LEDR, PI_OUTPUT);
    gpioSetMode(PIN_LEDG, PI_OUTPUT);
    gpioSetMode(PIN_LEDB, PI_OUTPUT);

    gpioWrite(PIN_LEDR, PI_LOW); // PI_LOW == 0, PI_HIGH == 1
    gpioWrite(PIN_LEDG, PI_LOW);
    gpioWrite(PIN_LEDB, PI_LOW);

    gpioTerminate();

    return 1;
}
