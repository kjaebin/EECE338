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

#define SERVO_POS_MIN 1000
#define SERVO_POS_MAX 2000

#define MODE_NORMAL 0
#define MODE_BLINK 1
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
/* [P4] Write your global variables FROM here */
volatile bool g_led_on = false;
volatile bool g_led_color = false;
volatile bool g_led_blink = false;
volatile bool g_servo = true;
/* [P4] Write your global variables UP TO here */

void myISR_fade()
{
    if(mode == MODE_MOOD)
    {
        g_led_fade = true;

        // Slowly increase or derease the intensity of each color
        rIntensity = rIntensity + rDelta;
        gIntensity = gIntensity + gDelta;
        bIntensity = bIntensity + bDelta;

        if (rIntensity >= 255 || rIntensity <= 0)
            rDelta *= -1;
        if (gIntensity >= 255 || gIntensity <= 0)
            gDelta *= -1;
        if (bIntensity >= 255 || bIntensity <= 0)
            bDelta *= -1;

        servo_angle = servo_angle + sDelta;
        if (servo_angle >= SERVO_POS_MAX || servo_angle <= SERVO_POS_MIN)
            sDelta *= -1;
    }
    else
        g_led_fade = false;
}


void myISR_setMode()
{
    btn_state = gpioReadDebounce(PIN_BTN);
    if(btn_state < 0) // If the input is noise, do nothing
        return;

    printf("Interrupt! Button state: %d\r\n", btn_state);

    /*** [P4] Write your code FROM here ***/
    mode = (++mode) % 3;
    /*** [P4] Write your code UP TO here ***/
}

void myISR_blink()
{
    /*** [P4] Write your code FROM here ***/
    if (mode == MODE_BLINK) {
        g_led_on = !(g_led_on);
    }
    /*** [P4] Write your code UP TO here ***/
}

void myISR_color()
{
    /*** [P4] Write your code FROM here ***/
    if (mode == MODE_NORMAL) {
        g_led_color = true;
    }
    else if (mode == MODE_BLINK) {
        if (g_led_blink) {
            g_led_color = true;
            g_led_blink = false;
        }
        else {
            g_led_blink = true;
        }
    }
    /*** [P4] Write your code UP TO here ***/
}

void myISR_servo()
{
    /* [P4] Write your code FROM here */
    if (mode == MODE_BLINK) {
        g_servo = true;
    }
    /* [P4] Write your code UP TO here */
}

void gpioRGBColor(int rIntensity, int gIntensity, int bIntensity)
{
    gpioAnalogWrite(PIN_LEDR, rIntensity);
    gpioAnalogWrite(PIN_LEDG, gIntensity);
    gpioAnalogWrite(PIN_LEDB, bIntensity);
}

int main()
{
    unsigned long t_start_ms, t_elapsed_ms;
    int led_color = 0;
    /* [P4] Write your variables FROM here*/

    /* [P4] Write your variables UP TO here*/

    srand((unsigned int)time(NULL));

    // GPIO settings
    if(gpioInitialise()<0) {
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

    // Interrupt settings
    /* [P4] Write your code FROM here */
    gpioSetISRFunc(PIN_BTN, EITHER_EDGE, 0, myISR_setMode);
    gpioSetTimerFunc(1, 30, myISR_fade);
    gpioSetTimerFunc(2, 250, myISR_blink);
    gpioSetTimerFunc(3, 500, myISR_color);
    gpioSetTimerFunc(4, 500, myISR_servo);
    /* [P4] Write your code UP TO here */

    // Infinite loop
    while(1) {
        t_start_ms = millis();
        
        /* [P4] Write your code FROM here */
        if(mode == MODE_NORMAL) {
            g_led_on = true;
            if (g_led_color) {
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
        }
        
        else if (mode == MODE_BLINK) {
            if (g_servo) {
                g_servo = false;
                servo_angle = rand()%(SERVO_POS_MAX-SERVO_POS_MIN) + SERVO_POS_MIN;
                gpioServo(PIN_SERVO, servo_angle);
            }
            if (g_led_on) {
                if (g_led_color) {
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
            }            
            else {
            gpioWrite(PIN_LEDR, PI_LOW); 
            gpioWrite(PIN_LEDG, PI_LOW);
            gpioWrite(PIN_LEDB, PI_LOW);
            }
        }
        
        else {
            if (g_led_fade) {
                g_led_fade = false;
                gpioRGBColor(rIntensity, gIntensity, bIntensity);
                gpioServo(PIN_SERVO, servo_angle);
            }
        }
            
        /* [P4] Write your code UP TO here */

        t_elapsed_ms = millis() - t_start_ms;
        sleep_ms(LOOP_PERIOD_MS - t_elapsed_ms);
    }

    return 1;
}
