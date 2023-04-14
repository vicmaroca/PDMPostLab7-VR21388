/*
 * File:   mainLab7.c
 * Author: Amilcar Rodriguez
 */

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

//Libraries
#include <xc.h>
#include <stdint.h>
#define _XTAL_FREQ 8000000

//Variables
char motor;

void setup(void);

//Interruption
void __interrupt() isr (void) {
    
    //When the conversion is complete
    if (PIR1bits.ADIF == 1) {
        
        //Controlling the frequency of the module CCP1
        if(motor == 1) {
            CCPR1L = (ADRESH >> 1) + 124;
        }
        
        //Controlling the frequency of the module CCP2
        else if(motor == 0) {
            CCPR2L = (ADRESH >> 1) + 124;
        }
        
        //Turning off the conversion 
        PIR1bits.ADIF = 0;
    }
}

void main(void) {
    
    setup();
    while (1) {
        
        if(ADCON0bits.GO == 0) { //When the conversion has finished
            
            if(ADCON0bits.CHS == 0) {
                motor = 0;
                ADCON0bits.CHS = 1;
        }
            else if(ADCON0bits.CHS == 1) {
                motor = 1;
                ADCON0bits.CHS = 0;
            }
            
            __delay_ms(50); //Waiting, because of the change of channel
            
            ADCON0bits.GO = 1;
        }
    }     
    return;
}

void setup(void) {
    
    ANSEL = 0b00000011; //ANS0 as analogic input
    ANSELH = 0x00;         //PORTB USED AS DIGITAL        
    
    TRISA = 0b00000011;       //RA0 and RA1 AS INPUTS
    TRISC = 0x00;       //
       
    //Configuring the oscillator 8MHz and internal
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 1;
    OSCCONbits.SCS = 1;
    
    //Configuring ADC
    ADCON1bits.ADFM = 0;    //Justified to the left
    ADCON1bits.VCFG0 = 0;   //Reference voltages of 0 and 5V
    ADCON1bits.VCFG1 = 0;
    
    ADCON0bits.ADCS = 0b10; //ADC Clock Fosc/32 
    ADCON0bits.CHS = 0;     //Channel 0
    __delay_ms(50);
    ADCON0bits.ADON = 1;    //Turning on the module
    
    //Configuring PWM
    TRISCbits.TRISC2 = 1; //RC2/CCP2 as input
    TRISCbits.TRISC1 = 1; //CCP2 as output
    
    PR2 = 250;            //Period of tmr2 (4ms)
    
    CCP1CONbits.P1M = 0;  //Single output mode
    CCP1CONbits.CCP1M = 0b00001100; //PWM mode
    
    CCP2CONbits.CCP2M = 0;
    CCP2CONbits.CCP2M = 0b00001100; //PWM mode
    
    CCPR1L = 0x0F;      //duty cycle start
    CCPR2L = 0x0F;
    
    CCP1CONbits.DC1B0 = 0; //less significative bits
    CCP1CONbits.DC1B1 = 0;
    CCP2CONbits.DC2B0 = 0;
    CCP2CONbits.DC2B1 = 0;
    
    PIR1bits.TMR2IF = 0;        //Flag off
    T2CONbits.T2CKPS = 0b11;    //Preescaler 1:16
    T2CONbits.TMR2ON = 1;       //Turning on tmr2
    
    while (!PIR1bits.TMR2IF == 0);   //Wait one cycle
    
    PIR1bits.TMR2IF = 0;
    TRISCbits.TRISC2 = 0;       //CCP1 as output  
    TRISCbits.TRISC1 = 0; //CCP2 as output

    
    //Configuring interruptions
    PIR1bits.ADIF = 0;  //Flag of ADC conversion
    PIE1bits.ADIE = 1;  //Enabling ADC conversion
    INTCONbits.PEIE = 1;    //Peripheral Interruption
    INTCONbits.GIE = 1;     //Global interruptions
    ADCON0bits.GO = 1;      //Starts the ADC conversion
    
    motor = 1; //Starting at motor 1
}
