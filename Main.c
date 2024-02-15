/*
 * File:   Main.c
 * Author: maher
 *
 * Created on February 2, 2024, 10:30 AM
 */

#define FCY 2000000 
#include "xc.h"
#include <p24FJ64GU202.h>
#include <libpic30.h>

#define BAUDRATE 9600
#define BRGVAL ((FCY/BAUDRATE)/16)-1


#define RED LATBbits.LATB15
#define GREEN LATBbits.LATB14
#define BLUE LATBbits.LATB13
#define ON 0
#define OFF 1  
char RX;




void uart_init(){
    // Uart protocol einstelllen 8N1
    U1MODEbits.STSEL = 0;
    U1MODEbits.PDSEL = 0;
    U1MODEbits.ABAUD = 0;
    U1MODEbits.BRGH = 0;
    
    TRISBbits.TRISB7 = 0 ; // configure 
    __builtin_write_OSCCONL(OSCCON & 0xbf);
    RPOR3bits.RP7R =3;
    
    TRISBbits.TRISB5 = 1;
    __builtin_write_OSCCONL(OSCCON & 0xbf);
    RPINR18bits.U1RXR = 5;
    
    U1MODEbits.UARTEN = 1;  // UART enabled
    U1STAbits.UTXEN = 1;    // UART Tx enabled
    
    U1STAbits.URXEN = 1;    // UART Rx enabled
    
    __delay_ms(500);
    
} 

void interrupt_init(){
    U1STAbits.URXISEL0 = 0;
    U1STAbits.URXISEL1 = 0;  // Interrupt flag bit is set when character is received
    
    IEC0bits.U1RXIE = 1; // enable UART receive interrupt
    //IFS0bits.U1RXIF = 0; // Receive flag clear
    
    TRISAbits.TRISA2 = 0; // configure A2 as output to test the interrupt and measure interrupt time
    TRISAbits.TRISA0 = 0; // configure A0 as output to test the interrupt (LED)

}

void __attribute__((__interrupt__, auto_psv)) _U1RXInterrupt(void){
    LATAbits.LATA2 = 1; // Start time
    LATAbits.LATA0 = 0 ;
    IFS0bits.U1RXIF = 0; // Receive flag clear
    
    
    
    //__delay_ms(500);
    
    char rx = U1RXREG;
  
    
    if(rx == 0x0a || rx == 0x0d){   
    }else{
      RX =  rx ;
    }
        //RETFIE;
    
        LATAbits.LATA0 = 1 ;
        LATAbits.LATA2 = 0; // end time
}

void uart_transmit(char data){
    //while(U1STAbits.UTXBF); // warten solange buffer voll ist
    U1TXREG = data ;

}

void LED_init(){
    TRISBbits.TRISB13 = 0 ;
    TRISBbits.TRISB14 = 0 ;
    TRISBbits.TRISB15 = 0 ;
}

void turn_LED(char color){
    switch(color){
            case 'w': // white
               RED = ON ;
               GREEN = ON ;
               BLUE= ON ;
                break;
            case 'r': // Red
               RED = ON ;
               GREEN = OFF ;
               BLUE= OFF ;
                break;
            case 'g': // Green / lime
                RED = OFF ;
                GREEN = ON ;
                BLUE= OFF ;
                break;
            case 'b': // Blue
                RED = OFF ;
                GREEN = OFF ;
                BLUE= ON ;
                break;
            case 'm': // magenta
                RED = ON ;
                GREEN = OFF ;
                BLUE= ON ;
                break;
            case 'y': // Yellow
                RED = ON ;
                GREEN = ON ;
                BLUE= OFF ;
                break;
            case 'c': // Cyan
                RED = OFF ;
                GREEN = ON ;
                BLUE= ON ;
                break;
            default: // Black
                RED = OFF ;
                GREEN = OFF ;
                BLUE= OFF ;
                break;
    }
}

int main(void) {
    
    U1BRG = BRGVAL; // set Baudrate
    
    // turn on LED
    
    uart_init();
    interrupt_init();
    LED_init();
    char lastTX = 0x00;
    
    while(1){
        //LATAbits.LATA0 = 1 ; 
        
        if(lastTX == RX){
        } else{
        turn_LED(RX);       
        uart_transmit(RX);
        lastTX = RX ;
        
        }
        //LATAbits.LATA0 = 0 ; // turn off LED
    }
 
    return 0;
}

