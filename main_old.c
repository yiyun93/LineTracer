#include "msp.h"
#include "Clock.h"
#include <stdio.h>

#define LED_RED 1
#define LED_GREEN (LED_RED << 1)
#define LED_BLUE (LED_RED << 2)
#define LED_WHITE 7


void sensor_init(){
// 0,2,4,6 IR Emitter
P5->SEL0 &= ~0x08;
P5->SEL1 &= ~0x08; //GPIO
P5->DIR |= 0x08;  //OUTPUT
P5->OUT &= ~0x08;   //turn off 4 even IR LEDs

// 1,3,5,7 IR Emitter
P9->SEL0 &= ~0x04;
P9->SEL1 &= ~0x04; //GPIO
P9->DIR |= 0x04;  //OUTPUT
P9->OUT &= ~0x04;   //turn off 4 odd IR LEDs

// 0~7 IR Emitter
P7->SEL0 &= ~0x08;
P7->SEL1 &= ~0x08; //GPIO
P7->DIR &= ~0x08;  //OUTPUT
}
void switch_init(){
    //setup switch as GPIO
    P1->SEL0 &= ~0x12;
    P1->SEL1 &= ~0x12;

    //setup switch as Input
    P1->DIR &= ~0x12;

    //Enalbe pull-up resistors
    P1->REN |= 0x12;

    //Now pull-up
    P1->OUT |= 0x12;
}
void led_init(){
    //set P2 as GPTO
    P2->SEL0 &= ~0x07;
    P2->SEL1 &= ~0x07;

    //input or output
    //current type is output
    P2->DIR |= 0x07;

    //Turn off LED
    P2->OUT &= ~0x07;
}

void turn_on_led(int color){
    P2->OUT &= 0x07;
    P2->OUT |= color;
}
void turn_off_led(){
    P2->OUT &= ~0x07;
}

void systick_init(void){
    SysTick->LOAD = 0x00FFFFFF;
    SysTick->CTRL = 0x00000005;
}

void systick_wait1ms(){
    SysTick->LOAD = 0x0000BB80;
    SysTick->VAL = 0;
    while((SysTick->CTRL & 0x00010000) == 0) {};
}
void systick_wait1s(){
    int i;
    int count = 1000;

    for (i=0; i<count; i++){
        systick_wait1ms();
    }
}

void IRSensorBasic(){

    int sensor = 0;
    while(1){
        //Turn on IR LEDs
        P5->OUT |= 0x08;
        P9->OUT |= 0x04;

        // Make P7.0-P7.7 as output
        P7->DIR = 0xFF;
        // Charges a capacitor
        P7->OUT = 0xFF;
        // Wait for fully charged
        Clock_Delay1us(10);

        // Make P7.0-P7.7 as input
        P7->DIR = 0x00;

        //Wait for a while
        //Wait for a certain amount of charge to be released
        Clock_Delay1us(1000);

        // Read P7.7-P7.0 Input
        // Read P7.5 Input, read 5th sensor, not entire
        // Reads the value of the 5th of the IR sensors, returning the value
        // corresponding to that bit (16, 0x10 in this case) if there is any
        // charge left, or 0 if there is no charge left.
        // white: 0, black: 1
        sensor = P7->IN & 0x10;

        if(sensor){
            P2->OUT |= 0x01;
        } else {
            P2->OUT &= ~0x07;
        }

        //Turn off IR LEDs
        P5->OUT &= ~0x08;
        P9->OUT &= ~0x04;

        Clock_Delay1ms(10);
    }
}

void IRSensorCenter(){
    int sensor = 0;
        while(1){
            //Turn on IR LEDs
            P5->OUT |= 0x08;
            P9->OUT |= 0x04;

            // Make P7.0-P7.7 as output
            P7->DIR = 0xFF;
            // Charges a capacitor
            P7->OUT = 0xFF;
            // Wait for fully charged
            Clock_Delay1us(10);

            // Make P7.0-P7.7 as input
            P7->DIR = 0x00;

            //Wait for a while
            //Wait for a certain amount of charge to be released
            Clock_Delay1us(1000);

            sensor = P7->IN & 0xFF;

            if(sensor == 0x18){
                P2->OUT |= 0x01;
            } else {
                P2->OUT &= ~0x07;
            }

            //Turn off IR LEDs
            P5->OUT &= ~0x08;
            P9->OUT &= ~0x04;

            Clock_Delay1ms(10);
        }
}

void pwm_init34(uint16_t period, uint16_t duty3, uint16_t duty4){
    // CCR0 period
    TIMER_A0->CCR[0] = period;

    //divide by 1
    TIMER_A0->EX0 = 0x0000;

    //toggle/reset
    TIMER_A0->CCTL[3] = 0x0040;
    TIMER_A0->CCR[3] = duty3;
    TIMER_A0->CCTL[4] = 0x0040;
    TIMER_A0->CCR[3] = duty3;

    // 0x200 -> SMCLK
    // 0b1100 0000 -> input divider /8
    // 0b0011 0000 -> up/down mode
    TIMER_A0->CTL = 0x02F0;

    // set alternative
    P2->DIR |= 0xC0;
    P2->SEL0 |= 0xC0;
    P2->SEL1 &= ~0xC0;
}

void motor_init(void){
    P3->SEL0 &= ~0xC0;
    P3->SEL1 &= ~0xC0; // 1) configure nSLPR & nSLPL as GPIO
    P3->DIR |= 0xC0;  // 2) make nSLPR & nSLPL as output
    P3->OUT &= ~0xC0; // 3) output LOW

    P5->SEL0 &= ~0x30;
    P5->SEL1 &= ~0x30; // 1) configure DIRR & DIRL as GPIO
    P5->DIR |= 0x30;  // 2) make DIRR & DIRL as output
    P5->OUT &= ~0x30; // 3) output LOW

    P2->SEL0 &= ~0xC0;
    P2->SEL1 &= ~0xC0; // 1) configure PWMR & PWML as GPIO
    P2->DIR |= 0xC0;  // 2) make PWMR & PWML as output
    P2->OUT &= ~0xC0; // 3) output LOW

    pwm_init34(7500, 0, 0);
}

void motorFoward(int speed){
    // 0<speed<10000
    printf("moveFoward\n");
    while(1){
        // PWM High : move forward
        P5->OUT &= ~0x30;   // PH = 0
        P2->OUT |= 0xC0;    // EN = 1
        P3->OUT |= 0xC0;    // nSleep = 1
        Clock_Delay1us(speed);

        // PWM Low : Stop
        P2->OUT &= ~0xC0;
        Clock_Delay1us(10000-speed);   // EN = 0
    }
}

int sensorOut(){
    int sensor = 0;

                //Turn on IR LEDs
                P5->OUT |= 0x08;
                P9->OUT |= 0x04;

                // Make P7.0-P7.7 as output
                P7->DIR = 0xFF;
                // Charges a capacitor
                P7->OUT = 0xFF;
                // Wait for fully charged
                Clock_Delay1us(10);

                // Make P7.0-P7.7 as input
                P7->DIR = 0x00;

                //Wait for a while
                //Wait for a certain amount of charge to be released
                Clock_Delay1us(1000);

                sensor = P7->IN & 0xFF;

                return sensor;

}

void motorFowardLine(int speed){
    int sensor = 0;
            while(1){
                //Turn on IR LEDs
                P5->OUT |= 0x08;
                P9->OUT |= 0x04;

                // Make P7.0-P7.7 as output
                P7->DIR = 0xFF;
                // Charges a capacitor
                P7->OUT = 0xFF;
                // Wait for fully charged
                Clock_Delay1us(10);

                // Make P7.0-P7.7 as input
                P7->DIR = 0x00;

                //Wait for a while
                //Wait for a certain amount of charge to be released
                Clock_Delay1us(1000);

                sensor = P7->IN & 0xFF; // white: 0, black: 1

                if(sensor == 0x18){
                    P2->OUT |= 0x01;
                            // PWM High : move forward
                            P5->OUT &= ~0x30;   // PH = 0
                            P2->OUT |= 0xC0;    // EN = 1
                            P3->OUT |= 0xC0;    // nSleep = 1
                            Clock_Delay1us(speed);

                            // PWM Low : Stop
                            P2->OUT &= ~0xC0;
                            Clock_Delay1us(10000-speed);   // EN = 0
                } else {
                    P2->OUT &= ~0x07;
                }

                //Turn off IR LEDs
                P5->OUT &= ~0x08;
                P9->OUT &= ~0x04;

                Clock_Delay1us(10);
            }
}


void move(uint16_t leftDuty, uint16_t rightDuty){
    P3->OUT |= 0xC0;
    TIMER_A0->CCR[3] = leftDuty;
    TIMER_A0->CCR[4] = rightDuty;
}
void left_forward(){
    P5->OUT &= ~0x10;
}
void left_backward(){
    P5->OUT |= 0x10;
}
void right_forward(){
    P5->OUT &= ~0x20;
}
void right_backward(){
    P5->OUT |= 0x20;
}


void main(void){
    Clock_Init48MHz();
    systick_init();
    sensor_init();
    motor_init();

    int sensor=0;
    while(1){
        sensor = 0;
        //Turn on IR LEDs
                        P5->OUT |= 0x08;
                        P9->OUT |= 0x04;

                        // Make P7.0-P7.7 as output
                        P7->DIR = 0xFF;
                        // Charges a capacitor
                        P7->OUT = 0xFF;
                        // Wait for fully charged
                        Clock_Delay1us(10);

                        // Make P7.0-P7.7 as input
                        P7->DIR = 0x00;

                        //Wait for a while
                        //Wait for a certain amount of charge to be released
                        Clock_Delay1us(1000);

                        sensor = P7->IN & 0xFF;
        //sensor = sensorOut();
        printf("%x \n", sensor);
        if(sensor&0x18){
            left_forward();
            right_forward();
            move(600, 600);
        }
        else if(sensor<=8){
            left_forward();
            right_forward();
            move(3000, 500);
        }
        else if(sensor>8){
            left_forward();
            right_forward();
            move(500, 3000);
        }
        Clock_Delay1ms(10);
    }
}





