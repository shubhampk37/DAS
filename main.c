/*
 * File:   main.c
 * Author: shubh
 *
 * Created on November 26, 2020, 3:56 PM
 */
// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = ON         // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3/PGM pin has PGM function; low-voltage programming enabled)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

#include <xc.h>
#include <math.h>
#define _XTAL_FREQ 20000000
#define RS PORTDbits.RD0
#define RW PORTDbits.RD1
#define EN PORTDbits.RD2

unsigned int  temp, adc;
double temperature, adc1;
int THERMISTORPIN = 0,  BETA = 3900 ;

float THERMISTORNOMINAL = 10000 , TEMPERATURENOMINAL = 25 , SERIESRESISTOR = 10000    ;
float ROOM_TEMP = 298.15;

void lcd_data(unsigned char data)
{
    PORTC = data;
    RS = 1;
    RW = 0;
    EN = 1;
    __delay_ms(5);
    EN = 0;
}
void lcd_command(unsigned char command)
{
    PORTC = command;
    RS = 0;
    RW = 0;
    EN = 1;
    __delay_ms(5);
    EN = 0;
}
void lcd_string(const unsigned char *str, int num)
{
    for(int i=0; i < num; i++){
       lcd_data(str[i]); 
    }
}
void lcd_init()
{
    lcd_command(0x38); //Set to 16x2 mode
    lcd_command(0x06); //Sets the cursor to the next position
    lcd_command(0x0C); //Display ON and cursor OFF
    lcd_command(0x01); // Clear Screen
}

void interrupt adc_conv(void)
{
    if (PIR1bits.ADIF == 1)
    {
        adc = (ADRESH<<8);
        adc  = adc + ADRESL;
        PIR1bits.ADIF = 0;
    }
}
float ReadTemp(unsigned int value)
{
  double rThermistor = 0;            // Holds thermistor resistance value
  double tKelvin     = 0;            // Holds calculated temperature
  double tCelsius    = 0;            // Hold temperature in celsius
  
  rThermistor = SERIESRESISTOR / ( (1023.0 / value) - 1);   //Rs/(Vs/Vo -1)
  
  tKelvin = (BETA * ROOM_TEMP) / 
            (BETA + (ROOM_TEMP * log(rThermistor / THERMISTORNOMINAL)));
  tCelsius = tKelvin - 273.15;  // convert kelvin to celsius */
  

  return tCelsius;    // Return the temperature in Celsius
}
void displayTemp(double temperature)
{       unsigned int a, b, c, d, e, f,g,h;
        a = (int)temperature / 10; //0.x
        b = (int)temperature % 10; //0.0x
        
        c = a % 10;
        d = a / 10;
        
        e = d % 10;
        f = d / 10;
        
        g = f % 10;
        h = f / 10;
        
      
        
        lcd_data(h + 0x30);
        lcd_data(g + 0x30);
        lcd_data(e + 0x30);
        lcd_data('.');
        lcd_data(c + 0x30);
        lcd_data(b + 0x30);
}
void main(void) {
    
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    PIE1bits.ADIE = 1;
    
    TRISC = 0x00;
    TRISD = 0x00;
    
    PORTC = 0x00;
    PORTD = 0x00;
    lcd_init();
    lcd_command(0x80);
    lcd_string("Sensor", 6);
    
    ADCON0 = 0x81;   //Set ADON and frequency to fosc/4 and select ANO
    ADCON1 = 0xCE;  //Select Right shifted output format;Set ANO as Analog and all others as digital
    while(1)
    {
        float average;
        ADCON0 = ADCON0 | (0x04);  //setting the GO bit to start the conversion
        
        temperature = ReadTemp(adc) * 100;
        
        lcd_command(0x89); 
        displayTemp(temperature);
        
        
    }
    //return;
}
