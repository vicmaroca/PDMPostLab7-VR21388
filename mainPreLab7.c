/*
 * File:   mainPreLab7.c
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
    
#include <xc.h>
#define _tmr0_value 200
#define _XTAL_FREQ 8000000 
#include <pic16f887.h>  // Incluimos la librería del PIC16F887


void main() {
    // Configuramos el pin RA0 como salida y lo ponemos en estado alto (5 V)
    TRISAbits.TRISA0 = 0; 
    PORTAbits.RA0 = 1; 
    
    // Configuramos el pin RA0 como salida y lo ponemos en estado alto (5 V)
    TRISAbits.TRISA1 = 0; 
    PORTAbits.RA1 = 0; 

    // Configuramos el módulo PWM1 para controlar un servo motor
    TRISCbits.TRISC2 = 0; // Configuramos el pin RC2 como salida
    CCP1CONbits.CCP1M = 0b1100; // Configuramos el modo PWM del CCP1 para single output, active-high
    T2CONbits.T2CKPS = 0b11; // Configuramos el preescaler del Timer 2 a 1:16
    T2CONbits.TMR2ON = 1; // Encendemos el Timer 2

    while (1) {
        // Generamos la señal PWM para posicionar el servo a 90 grados
        CCPR1L = 0b01011100; // Configuramos el valor de duty cycle alto para un ciclo de trabajo del 9%
        
        //180 grados
       // CCPR1L = 0b10111000; // Configuramos el valor de duty cycle alto para un ciclo de trabajo del 22.5%
        __delay_ms(2000); // Esperamos 1 segundo para que el servo se posicione
    }
    
    return;
}