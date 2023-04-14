/*
 * File:   mainPostLab7.c
 * Author: Amilcar Rodriguez
 *
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


// LIBRERIAS
#include <xc.h>
#include <stdint.h>

/*
 * Variables globales
 */
uint8_t contador;
uint8_t pot_val3;

/*
 * Constantes
 */
#define _tmr0_value 200
#define _XTAL_FREQ 8000000

/*
 * Prototipos de funcion
 */
void setup(void);
int mapear(int valor, int rango_min, int rango_max, int nuevo_min, int nuevo_max);

/*
 * Interrupciones
 */

void __interrupt() isr (void)
{
    if (INTCONbits.T0IF) // Interrupcion del TMR0
    {
        contador++;
        if (contador < pot_val3) // cuando el valor del contador sea menor al de la conv del pot, enciende LED
            PORTCbits.RC3 = 1;
        else
            PORTCbits.RC3 = 0;
        TMR0 = 6;
        INTCONbits.T0IF = 0;
    }
}

void main(void) {
    setup();
    __delay_us(50); // Delay ANTES de activar una medicion
    
    while(1){
        ADCON0bits.CHS = 0; 
        ADCON0bits.GO = 1; // Iniciar la conversión
        while(ADCON0bits.GO); // Esperar a que termine la conversión
        int adc_value_1 = ADRESH; // Leer el valor convertido
        int servo1 = mapear(adc_value_1, 0, 255, 125, 250);
        CCPR1L = servo1;
        __delay_ms(10);
        
        ADCON0bits.CHS = 1; // Seleccionar el canal AN1 para la segunda entrada analógica
        ADCON0bits.GO = 1; // Iniciar la conversión
        while(ADCON0bits.GO); // Esperar a que termine la conversión
        int adc_value_2 = ADRESH; // Leer el valor convertido
        int servo2 = mapear(adc_value_2, 0, 255, 125, 250);
        CCPR2L = servo2;
        __delay_ms(10);
        
        ADCON0bits.CHS = 2; 
        ADCON0bits.GO = 1; // Iniciar la conversión
        while(ADCON0bits.GO); // Esperar a que termine la conversión
        pot_val3 = ADRESH;
        __delay_ms(10);
    }
    return;
}

void setup(void){
    // CONFIGURACION DE ENTRADAS Y SALIDAS
    // Pines digitales
    ANSEL = 0;
    ANSELH = 0;
    // Puerto A como salida
    // TRISA = 0;
    TRISB = 0;
    TRISC = 0;
    TRISD = 0;
    
    // Configuracion oscilador interno
    OSCCONbits.IRCF = 0b111; // 8MHz
    OSCCONbits.SCS = 1;
    
    //Configuracion TMR0
    OPTION_REGbits.T0CS = 0; //Se selecciona el timer como temporizador
    OPTION_REGbits.PSA = 0; // Prescaler activado para TMR0
    OPTION_REGbits.PS = 0b11; // Prescaler 1:256
    TMR0 = _tmr0_value;
    
    //Configuracion de las interrupciones
    INTCONbits.T0IF = 0; // Bandera tmr0
    INTCONbits.T0IE = 1; // Enable tmr0
    INTCONbits.PEIE = 1; // Interrupciones perifericas
    INTCONbits.GIE = 1; // Interrupciones globales
    
    // Configuracion ADC
    ANSELbits.ANS0 = 1;
    ANSELbits.ANS1 = 1;
    TRISAbits.TRISA0 = 1;
    TRISAbits.TRISA1 = 1;
    ADCON0bits.ADCS = 0b10; // FOSC/32
    __delay_ms(1);
    ADCON1bits.ADFM = 0; //Justificado a la izquierda
    ADCON1bits.VCFG0 = 0; //Voltaje de ref. a GND
    ADCON1bits.VCFG1 = 0; //Voltaje de ref. a 5v
    ADCON0bits.ADON = 1; // ADC is enabled
    ADIF = 0;
    
    // Configuracion PWM
    // PASO 1
    TRISCbits.TRISC2 = 1;   //RC2/CCP1 como entrada
    TRISCbits.TRISC1 = 1;   //RC1/CCP2 como entrada
    // PASO 2
    PR2 = 250;              // Periodo del tmr2 (4ms)
    // PASO 3
    CCP1CONbits.P1M = 0;    // mode single output (P1A modulated; P1B, P1C, P1D assigned as port pins) CCP1
    CCP1CONbits.CCP1M = 0b00001100;    // Modo PWM CCP1
    CCP2CONbits.CCP2M = 0b00001100;    // Modo PWM CCP2
    // PASO 4
    CCPR1L = 0x0f;          // Duty cycle inicial CCP1
    CCPR2L = 0x0f;          // Duty cycle inicial CCP2
    CCP1CONbits.DC1B1 = 0;  // bits menos significativos CCP1
    CCP1CONbits.DC1B0 = 0;  // bits menos singificativos CCP1
    CCP2CONbits.DC2B1 = 0;  // bits menos singificativos CCP2
    CCP2CONbits.DC2B0 = 0;  // bits menos singificativos CCP2
    // PASO 5  
    PIR1bits.TMR2IF = 0;    
    T2CONbits.T2CKPS = 0b11;    // Prescaler 1:16
    T2CONbits.TMR2ON = 1;       // Prender TMR2
    // PASO 6
    while (!PIR1bits.TMR2IF);   // esperar un ciclo
    PIR1bits.TMR2IF = 0;
    
    TRISCbits.TRISC2 = 0;       // Habilitar salida CCP1
    TRISCbits.TRISC1 = 0;       // Habilitar salida CCP2
    return;
}

/*
 * Interrupciones
 */

int mapear(int valor, int rango_min, int rango_max, int nuevo_min, int nuevo_max)
{
    int new_val = nuevo_min + (valor - rango_min)*(nuevo_max - nuevo_min)/(rango_max - rango_min);
    return new_val;
}