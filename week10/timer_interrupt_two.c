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

#define LOOP_PERIOD_MS 0000  // Set the loop period properly


/* [P3] Write your global variables FROM here */

volatile bool g_led_on = false;
volatile bool g_led_color = false;
volatile int servo_angle;

/* [P3] Write your global variables UP TO here */

void myISR_ledToggle()
{
    /* [P3] Write your code FROM here */
    
    g_led_on = !(g_led_on);
    
    /* [P3] Write your code UP TO here */
}

void myISR_ledColor()
{
    /* [P3] Write your code FROM here */

    g_led_color = true;

    /* [P3] Write your code UP TO here */
}

void myISR_servo()
{
    /* [P3] Write your code FROM here */
    
    if (g_led_on) {
        servo_angle = rand()%(SERVO_POS_MAX-SERVO_POS_MIN) + SERVO_POS_MIN;
    }

    /* [P3] Write your code UP TO here */
}

int main()
{
    unsigned long t_start_ms, t_elapsed_ms;
    /* [P3] Write your variables FROM here*/
    
    int led_color = 0;
    
    /* [P3] Write your variables UP TO here*/

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

    // Interrupt settings
    /* [P3] Write your code FROM here */
    gpioSetTimerFunc(0, 2000, myISR_ledToggle);
    gpioSetTimerFunc(1, 500, myISR_ledColor);
    gpioSetTimerFunc(2, 1000, myISR_servo);
    /* [P3] Write your code UP TO here */

    // Infinite loop
    while(1) {
        t_start_ms = millis();
        
        /* [P3] Write your code FROM here */
        
        if(g_led_on) {
            // While the LED is turned on, switch the LED color if
            // `g_led_color` is asserted in the ISR.
            if(g_led_color) {
                g_led_color = false;
                led_color = led_color % 7 + 1;
            }
            // Setting LED color
            switch(led_color) {
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
            
            // Setting Servo motor
            gpioServo(PIN_SERVO, servo_angle);
        }
        else {
            gpioWrite(PIN_LEDR, PI_LOW); 
            gpioWrite(PIN_LEDG, PI_LOW);
            gpioWrite(PIN_LEDB, PI_LOW);
        }
        
        /* [P3] Write your code UP TO here */

        t_elapsed_ms = millis() - t_start_ms;
        sleep_ms(LOOP_PERIOD_MS - t_elapsed_ms);
    }

    return 1;
}
