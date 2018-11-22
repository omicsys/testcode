#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//#define __AVR_ATmega8__ 1
#define FOSC  8000000  /* in Hz */

#define sw    (PINA & 0x80)
#define inc   (PINC & 0x04)
#define menu  (PINC & 0x02)
#define dec   (PINC & 0x01)
#define led   (PORTB & 0x02)
#define CS0    PORTB=(PORTB & 0b11111011) // PB2 --> #CS  
#define CS1    PORTB=(PORTB | 0b00000100) 

#define SDA0   PORTB=(PORTB & 0b11110111) // PB3 --> SDATA
#define SDA1   PORTB=(PORTB | 0b00001000)

#define RESET0 PORTB=(PORTB & 0b11101111) // PB4 --> #RESET
#define RESET1 PORTB=(PORTB | 0b00010000)

#define CLK0   PORTB=(PORTB & 0b11011111) // PB5 --> SCLK
#define CLK1   PORTB=(PORTB | 0b00100000)

#define NOP 0x00  // nop
#define SWRESET  0x01  // software reset
#define BSTROFF  0x02  // booster voltage OFF
#define BSTRON   0x03  // booster voltage ON
#define RDDIDIF  0x04  // read display identification
#define RDDST    0x09  // read display status
#define SLEEPIN  0x10  // sleep in
#define SLEEPOUT 0x11  // sleep out
#define PTLON    0x12  // partial display mode
#define NORON    0x13  // display normal mode
#define INVOFF   0x20  // inversion OFF
#define INVON    0x21  // inversion ON
#define DALO     0x22  // all pixel OFF
#define DAL      0x23  // all pixel ON
#define SETCON   0x25  // write contrast
#define DISPOFF  0x28  // display OFF
#define DISPON   0x29  // display ON
#define CASET    0x2A  // column address set
#define PASET    0x2B  // page address set
#define RAMWR    0x2C  // memory write
#define RGBSET   0x2D  // colour set
#define PTLAR    0x30  // partial area
#define VSCRDEF  0x33  // vertical scrolling definition
#define TEOFF    0x34  // test mode
#define TEON     0x35  // test mode
#define MADCTL   0x36  // memory access control
#define SEP      0x37  // vertical scrolling start address
#define IDMOFF   0x38  // idle mode OFF
#define IDMON    0x39  // idle mode ON
#define COLMOD   0x3A  // interface pixel format
#define SETVOP   0xB0  // set Vop
#define BRS      0xB4  // bottom row swap
#define TRS      0xB6  // top row swap
#define DISCTR   0xB9  // display control
//#define DAOR   0xBA  // data order(DOR)
#define TCDFE    0xBD  // enable/disable DF temperature compensation
#define TCVOPE   0xBF  // enable/disable Vop temp comp
#define EC       0xC0  // internal or external oscillator
#define SETMUL   0xC2  // set multiplication factor
#define TCVOPAB  0xC3  // set TCVOP slopes A and B
#define TCVOPCD  0xC4  // set TCVOP slopes c and d
#define TCDF     0xC5  // set divider frequency
#define DF8COLOR 0xC6  // set divider frequency 8-color mode
#define SETBS    0xC7  // set bias system
#define RDTEMP   0xC8  // temperature read back
#define NLI      0xC9  // n-line inversion
#define RDID1    0xDA  // read ID1
#define RDID2    0xDB  // read ID2
#define RDID3    0xDC  // read ID3


// Booleans
#define NOFILL 0
#define FILL 1

// 12-bit color definitions

#define WHITE   0xFFF
#define BLACK   0x000
#define RED     0xF00
#define GREEN   0x0F0
#define BLUE    0x00F
#define CYAN    0x0FF
#define MAGENTA 0xF0F
#define YELLOW  0xFF0
#define BROWN   0xB22
#define ORANGE  0xFA0
#define PINK    0xF6A



const unsigned char FONT8x16[] PROGMEM = {  // Storing ASCII Table in to FLASH Memory

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // space 0x20
0x00,0x00,0x18,0x3C,0x3C,0x3C,0x18,0x18,0x18,0x00,0x18,0x18,0x00,0x00,0x00,0x00, // !
0x00,0x63,0x63,0x63,0x22,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // "
0x00,0x00,0x00,0x36,0x36,0x7F,0x36,0x36,0x36,0x7F,0x36,0x36,0x00,0x00,0x00,0x00, // #
0x0C,0x0C,0x3E,0x63,0x61,0x60,0x3E,0x03,0x03,0x43,0x63,0x3E,0x0C,0x0C,0x00,0x00, // $
0x00,0x00,0x00,0x00,0x00,0x61,0x63,0x06,0x0C,0x18,0x33,0x63,0x00,0x00,0x00,0x00, // %
0x00,0x00,0x00,0x1C,0x36,0x36,0x1C,0x3B,0x6E,0x66,0x66,0x3B,0x00,0x00,0x00,0x00, // &
0x00,0x30,0x30,0x30,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // '
0x00,0x00,0x0C,0x18,0x18,0x30,0x30,0x30,0x30,0x18,0x18,0x0C,0x00,0x00,0x00,0x00, // (
0x00,0x00,0x18,0x0C,0x0C,0x06,0x06,0x06,0x06,0x0C,0x0C,0x18,0x00,0x00,0x00,0x00, // )
0x00,0x00,0x00,0x00,0x42,0x66,0x3C,0xFF,0x3C,0x66,0x42,0x00,0x00,0x00,0x00,0x00, // *
0x00,0x00,0x00,0x00,0x18,0x18,0x18,0xFF,0x18,0x18,0x18,0x00,0x00,0x00,0x00,0x00, // +
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x18,0x30,0x00,0x00, // ,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // -
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x00, // .
0x00,0x00,0x01,0x03,0x07,0x0E,0x1C,0x38,0x70,0xE0,0xC0,0x80,0x00,0x00,0x00,0x00, // / (forward slash)
0x00,0x00,0x3E,0x63,0x63,0x63,0x6B,0x6B,0x63,0x63,0x63,0x3E,0x00,0x00,0x00,0x00, // 0 0x30
0x00,0x00,0x0C,0x1C,0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3F,0x00,0x00,0x00,0x00, // 1
0x00,0x00,0x3E,0x63,0x03,0x06,0x0C,0x18,0x30,0x61,0x63,0x7F,0x00,0x00,0x00,0x00, // 2
0x00,0x00,0x3E,0x63,0x03,0x03,0x1E,0x03,0x03,0x03,0x63,0x3E,0x00,0x00,0x00,0x00, // 3
0x00,0x00,0x06,0x0E,0x1E,0x36,0x66,0x66,0x7F,0x06,0x06,0x0F,0x00,0x00,0x00,0x00, // 4
0x00,0x00,0x7F,0x60,0x60,0x60,0x7E,0x03,0x03,0x63,0x73,0x3E,0x00,0x00,0x00,0x00, // 5
0x00,0x00,0x1C,0x30,0x60,0x60,0x7E,0x63,0x63,0x63,0x63,0x3E,0x00,0x00,0x00,0x00, // 6
0x00,0x00,0x7F,0x63,0x03,0x06,0x06,0x0C,0x0C,0x18,0x18,0x18,0x00,0x00,0x00,0x00, // 7
0x00,0x00,0x3E,0x63,0x63,0x63,0x3E,0x63,0x63,0x63,0x63,0x3E,0x00,0x00,0x00,0x00, // 8
0x00,0x00,0x3E,0x63,0x63,0x63,0x63,0x3F,0x03,0x03,0x06,0x3C,0x00,0x00,0x00,0x00, // 9
0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x00, // :
0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x18,0x18,0x18,0x30,0x00,0x00, // ;
0x00,0x00,0x00,0x06,0x0C,0x18,0x30,0x60,0x30,0x18,0x0C,0x06,0x00,0x00,0x00,0x00, // <
0x00,0x00,0x00,0x00,0x00,0x00,0x7E,0x00,0x00,0x7E,0x00,0x00,0x00,0x00,0x00,0x00, // =
0x00,0x00,0x00,0x60,0x30,0x18,0x0C,0x06,0x0C,0x18,0x30,0x60,0x00,0x00,0x00,0x00, // >
0x00,0x00,0x3E,0x63,0x63,0x06,0x0C,0x0C,0x0C,0x00,0x0C,0x0C,0x00,0x00,0x00,0x00, // ?
0x00,0x00,0x3E,0x63,0x63,0x6F,0x6B,0x6B,0x6E,0x60,0x60,0x3E,0x00,0x00,0x00,0x00, // @ 0x40
0x00,0x00,0x08,0x1C,0x36,0x63,0x63,0x63,0x7F,0x63,0x63,0x63,0x00,0x00,0x00,0x00, // A
0x00,0x00,0x7E,0x33,0x33,0x33,0x3E,0x33,0x33,0x33,0x33,0x7E,0x00,0x00,0x00,0x00, // B
0x00,0x00,0x1E,0x33,0x61,0x60,0x60,0x60,0x60,0x61,0x33,0x1E,0x00,0x00,0x00,0x00, // C
0x00,0x00,0x7C,0x36,0x33,0x33,0x33,0x33,0x33,0x33,0x36,0x7C,0x00,0x00,0x00,0x00, // D
0x00,0x00,0x7F,0x33,0x31,0x34,0x3C,0x34,0x30,0x31,0x33,0x7F,0x00,0x00,0x00,0x00, // E
0x00,0x00,0x7F,0x33,0x31,0x34,0x3C,0x34,0x30,0x30,0x30,0x78,0x00,0x00,0x00,0x00, // F
0x00,0x00,0x1E,0x33,0x61,0x60,0x60,0x6F,0x63,0x63,0x37,0x1D,0x00,0x00,0x00,0x00, // G
0x00,0x00,0x63,0x63,0x63,0x63,0x7F,0x63,0x63,0x63,0x63,0x63,0x00,0x00,0x00,0x00, // H
0x00,0x00,0x3C,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00, // I
0x00,0x00,0x0F,0x06,0x06,0x06,0x06,0x06,0x06,0x66,0x66,0x3C,0x00,0x00,0x00,0x00, // J
0x00,0x00,0x73,0x33,0x36,0x36,0x3C,0x36,0x36,0x33,0x33,0x73,0x00,0x00,0x00,0x00, // K
0x00,0x00,0x78,0x30,0x30,0x30,0x30,0x30,0x30,0x31,0x33,0x7F,0x00,0x00,0x00,0x00, // L
0x00,0x00,0x63,0x77,0x7F,0x6B,0x63,0x63,0x63,0x63,0x63,0x63,0x00,0x00,0x00,0x00, // M
0x00,0x00,0x63,0x63,0x73,0x7B,0x7F,0x6F,0x67,0x63,0x63,0x63,0x00,0x00,0x00,0x00, // N
0x00,0x00,0x1C,0x36,0x63,0x63,0x63,0x63,0x63,0x63,0x36,0x1C,0x00,0x00,0x00,0x00, // O
0x00,0x00,0x7E,0x33,0x33,0x33,0x3E,0x30,0x30,0x30,0x30,0x78,0x00,0x00,0x00,0x00, // P 0x50
0x00,0x00,0x3E,0x63,0x63,0x63,0x63,0x63,0x63,0x6B,0x6F,0x3E,0x06,0x07,0x00,0x00, // Q
0x00,0x00,0x7E,0x33,0x33,0x33,0x3E,0x36,0x36,0x33,0x33,0x73,0x00,0x00,0x00,0x00, // R
0x00,0x00,0x3E,0x63,0x63,0x30,0x1C,0x06,0x03,0x63,0x63,0x3E,0x00,0x00,0x00,0x00, // S
0x00,0x00,0xFF,0xDB,0x99,0x18,0x18,0x18,0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00, // T
0x00,0x00,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x3E,0x00,0x00,0x00,0x00, // U
0x00,0x00,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x36,0x1C,0x08,0x00,0x00,0x00,0x00, // V
0x00,0x00,0x63,0x63,0x63,0x63,0x63,0x6B,0x6B,0x7F,0x36,0x36,0x00,0x00,0x00,0x00, // W
0x00,0x00,0xC3,0xC3,0x66,0x3C,0x18,0x18,0x3C,0x66,0xC3,0xC3,0x00,0x00,0x00,0x00, // X
0x00,0x00,0xC3,0xC3,0xC3,0x66,0x3C,0x18,0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00, // Y
0x00,0x00,0x7F,0x63,0x43,0x06,0x0C,0x18,0x30,0x61,0x63,0x7F,0x00,0x00,0x00,0x00,// Z
0x00,0x00,0x3C,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x3C,0x00,0x00,0x00,0x00, // [
0x00,0x00,0x80,0xC0,0xE0,0x70,0x38,0x1C,0x0E,0x07,0x03,0x01,0x00,0x00,0x00,0x00, // \ (back slash)
0x00,0x00,0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3C,0x00,0x00,0x00,0x00, // ]
0x08,0x1C,0x36,0x63,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // ^
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00, // _
0x18,0x18,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // ` 0x60
0x00,0x00,0x00,0x00,0x00,0x3C,0x46,0x06,0x3E,0x66,0x66,0x3B,0x00,0x00,0x00,0x00, // a
0x00,0x00,0x70,0x30,0x30,0x3C,0x36,0x33,0x33,0x33,0x33,0x6E,0x00,0x00,0x00,0x00, // b
0x00,0x00,0x00,0x00,0x00,0x3E,0x63,0x60,0x60,0x60,0x63,0x3E,0x00,0x00,0x00,0x00, // c
0x00,0x00,0x0E,0x06,0x06,0x1E,0x36,0x66,0x66,0x66,0x66,0x3B,0x00,0x00,0x00,0x00, // d
0x00,0x00,0x00,0x00,0x00,0x3E,0x63,0x63,0x7E,0x60,0x63,0x3E,0x00,0x00,0x00,0x00, // e
0x00,0x00,0x1C,0x36,0x32,0x30,0x7C,0x30,0x30,0x30,0x30,0x78,0x00,0x00,0x00,0x00, // f
0x00,0x00,0x00,0x00,0x00,0x3B,0x66,0x66,0x66,0x66,0x3E,0x06,0x66,0x3C,0x00,0x00, // g
0x00,0x00,0x70,0x30,0x30,0x36,0x3B,0x33,0x33,0x33,0x33,0x73,0x00,0x00,0x00,0x00, // h
0x00,0x00,0x0C,0x0C,0x00,0x1C,0x0C,0x0C,0x0C,0x0C,0x0C,0x1E,0x00,0x00,0x00,0x00, // i
0x00,0x00,0x06,0x06,0x00,0x0E,0x06,0x06,0x06,0x06,0x06,0x66,0x66,0x3C,0x00,0x00, // j
0x00,0x00,0x70,0x30,0x30,0x33,0x33,0x36,0x3C,0x36,0x33,0x73,0x00,0x00,0x00,0x00, // k
0x00,0x00,0x1C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x1E,0x00,0x00,0x00,0x00, // l
0x00,0x00,0x00,0x00,0x00,0x6E,0x7F,0x6B,0x6B,0x6B,0x6B,0x6B,0x00,0x00,0x00,0x00, // m
0x00,0x00,0x00,0x00,0x00,0x6E,0x33,0x33,0x33,0x33,0x33,0x33,0x00,0x00,0x00,0x00, // n
0x00,0x00,0x00,0x00,0x00,0x3E,0x63,0x63,0x63,0x63,0x63,0x3E,0x00,0x00,0x00,0x00, // o
0x00,0x00,0x00,0x00,0x00,0x6E,0x33,0x33,0x33,0x33,0x3E,0x30,0x30,0x78,0x00,0x00, // p 0x70
0x00,0x00,0x00,0x00,0x00,0x3B,0x66,0x66,0x66,0x66,0x3E,0x06,0x06,0x0F,0x00,0x00, // q
0x00,0x00,0x00,0x00,0x00,0x6E,0x3B,0x33,0x30,0x30,0x30,0x78,0x00,0x00,0x00,0x00, // r
0x00,0x00,0x00,0x00,0x00,0x3E,0x63,0x38,0x0E,0x03,0x63,0x3E,0x00,0x00,0x00,0x00, // s
0x00,0x00,0x08,0x18,0x18,0x7E,0x18,0x18,0x18,0x18,0x1B,0x0E,0x00,0x00,0x00,0x00, // t
0x00,0x00,0x00,0x00,0x00,0x66,0x66,0x66,0x66,0x66,0x66,0x3B,0x00,0x00,0x00,0x00, // u
0x00,0x00,0x00,0x00,0x00,0x63,0x63,0x36,0x36,0x1C,0x1C,0x08,0x00,0x00,0x00,0x00, // v
0x00,0x00,0x00,0x00,0x00,0x63,0x63,0x63,0x6B,0x6B,0x7F,0x36,0x00,0x00,0x00,0x00, // w
0x00,0x00,0x00,0x00,0x00,0x63,0x36,0x1C,0x1C,0x1C,0x36,0x63,0x00,0x00,0x00,0x00, // x
0x00,0x00,0x00,0x00,0x00,0x63,0x63,0x63,0x63,0x63,0x3F,0x03,0x06,0x3C,0x00,0x00, // y
0x00,0x00,0x00,0x00,0x00,0x7F,0x66,0x0C,0x18,0x30,0x63,0x7F,0x00,0x00,0x00,0x00, // z
0x00,0x00,0x0E,0x18,0x18,0x18,0x70,0x18,0x18,0x18,0x18,0x0E,0x00,0x00,0x00,0x00, // {
0x00,0x00,0x18,0x18,0x18,0x18,0x18,0x00,0x18,0x18,0x18,0x18,0x18,0x00,0x00,0x00, // |
0x00,0x00,0x70,0x18,0x18,0x18,0x0E,0x18,0x18,0x18,0x18,0x70,0x00,0x00,0x00,0x00, // }
0x00,0x00,0x3B,0x6E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // ~
0x00,0x70,0xD8,0xD8,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; // DEL

/*************************************************************/            
/*            Function prototypes                            */            
/*************************************************************/           
void shiftBits(unsigned char data) // Serial data byte shift function      
{
unsigned char i;
for (i=0; i<8; i++)  // Write all 8 bits : D7,D6,D5,D4,D3,D2,D1,D0  
{
CLK0;          // Standby SCLK
if((data>>(7-i))&0x01) SDA1; 
else SDA0;
CLK1;         // Strobe signal bit 
}   
CLK0; 
}

void sendCMD(unsigned char cmd)  // Write LCD Commnnd function 
{
CS0; 
CLK0; SDA0; CLK1;  // Send 9th bit Low :   D8=0 ( for command  )
shiftBits(cmd); // Send the rest bits : D7,D6,D5,D4,D3,D2,D1,D0
CS1;
}

void sendData(unsigned char data) // Write LCD Data function 
{            
CS0; 
CLK0; SDA1;  CLK1;  // Send 9th bit High :   D8=1 ( for data  )
shiftBits(data); // Send the rest bits : D7,D6,D5,D4,D3,D2,D1,D0  
CS1;           
}



void lcd_init()   // Initialize LCD function       
{
RESET0; _delay_ms(100); // Hardware Reset LCD
RESET1; _delay_ms(100); 
sendCMD(SLEEPOUT);  // Sleep out (commmand 0x11)
sendCMD(INVOFF);    // Inversion on (command 0x20)
sendCMD(COLMOD); sendData(0x03); // Color Interface Pixel Format (command 0x3A), 0x03 = 12 bits-per-pixel
sendCMD(MADCTL); sendData(0xC8); // 0xC0 = mirror x and y, reverse rgb
sendCMD(SETCON); sendData(0x40); // contrast 0x30
sendCMD(DISPON);
}

	  
void lcd_clear(uint16_t color, unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1)

{
uint16_t xmin, xmax, ymin, ymax;
uint16_t i;

xmin = (x0 <= x1) ? x0 : x1;
xmax = (x0 > x1) ? x0 : x1;
ymin = (y0 <= y1) ? y0 : y1;
ymax = (y0 > y1) ? y0 : y1;

sendCMD(PASET); sendData(xmin); sendData(xmax);
sendCMD(CASET); sendData(ymin); sendData(ymax);
sendCMD(RAMWR);
// loop on total number of pixels / 2
for (i = 0; i < ((((xmax - xmin + 1) * (ymax - ymin + 1)) / 2) + 1); i++) 
{
// use the color value to output three data bytes covering two pixels
// For some reason, it has to send blue first then green and red
sendData((color << 4) | ((color & 0xF0) >> 4));
sendData(((color >> 4) & 0xF0) | (color & 0x0F));
sendData((color & 0xF0) | (color >> 8));
}

}

void draw_color_bar()   
{
lcd_clear(RED,0,0,131,33);
lcd_clear(GREEN,0,34,131,66);
lcd_clear(BLUE,0,67,131,99);
lcd_clear(WHITE,0,100,131,131);
}

void LCDSetXY(unsigned char x, unsigned char y) 

{  sendCMD(PASET); sendData(x); sendData(x);// Row address set (command 0x2B)
sendCMD(CASET); sendData(y); sendData(y);// Column address set (command 0x2A) 
}

void LCDSetPixel(unsigned char x, unsigned char y, int color) 
{
LCDSetXY(x, y);
sendCMD(RAMWR);
// For some reason, it has to send blue first then green and red
sendData((color << 4) | ((color & 0xF0) >> 4));
sendData(((color >> 4) & 0xF0));  sendCMD(NOP);
}

void LCD_Char(int c, char x, char y, int fcolor, int bcolor)
{
unsigned char i=0,j=0;
unsigned int  color1=0,color2=0;
unsigned char pxl1=0,pxl2=0;
unsigned char data=0;

sendCMD(PASET); sendData(x); sendData(x+15);
sendCMD(CASET); sendData(y); sendData(y+7); // Column address set (command 0x2A)
sendCMD(RAMWR); // WRITE MEMORY
for(i=0;i<16;i++)
{
data= pgm_read_byte(&(FONT8x16[(c-32)*16+i])); // Read the data from the Flash Merory
for(j=0;j<4;j++)
{ 
pxl1 = data & 0x80;  pxl2 = data & 0x40; 
data= data << 2;
if(pxl1) color1 = fcolor; else color1 = bcolor;
if(pxl2) color2 = fcolor; else color2 = bcolor;
sendData((color1 << 4) | ((color1 & 0xF0) >> 4));
sendData(((color1 >> 4) & 0xF0) | (color2 & 0x0F));
sendData((color2 & 0xF0) | (color2 >> 8));
}
}
}

void LCD_String(char *pString, int x, int y, int fColor, int bColor)
{
while (*pString != 0x00) 
{
LCD_Char(*pString++, x, y, fColor, bColor); // Draw the character
y = y + 8; 
if (y > 131) break;  
}
}

void LCD_Number(int data, char x, char y,int fcolor, int bcolor )
{ 
unsigned char a=0,b=0,c=0,d=0;
a=data/1000;
b=(data%1000)/100;
c=(data%100)/10;
d=data%10;

LCD_Char(a+48, x, y, fcolor, bcolor);
LCD_Char(b+48, x, y+8, fcolor, bcolor);
LCD_Char(c+48, x, y+16, fcolor, bcolor);
LCD_Char(d+48, x, y+24, fcolor, bcolor);
}


void ADC_Init()   // ADC Initialize Function   
{
unsigned int i;
ADMUX=0x60;
ADCSRA=0x87;
for(i=0;i<=1000;i++);
}

unsigned char ADC_Read(unsigned char channel) // Read ADC Channels Function  
{
int i;

ADMUX=(ADMUX&0xF0)|channel;
ADCSRA=(ADCSRA&0xbf);
for(i=0;i<=1000;i++);
ADCSRA|= 0x40; //starting the conversion   
while(ADCSRA & 0x40); // wait for the end of the conversion
return ADCH;
}

/*************************************************************/

/*            Main Code Start here                           */

/*************************************************************/   

int main()      
{
DDRB=0b11111111;      //  Data Direction Register Configuration
DDRD=0b11111110;      //    1 : Output
DDRC=0b11111000;      //    0 : Input

lcd_init(); _delay_ms(500);
ADC_Init();
draw_color_bar();   

LCD_String(" ADC0 to ADC5  ", 4,     5,    WHITE,   BLUE);
LCD_String(" ------------- ", 16+4,  5,    WHITE,   BLUE);
LCD_String(" ADC-0 : ", 32+4,  5,    RED,     WHITE); 
LCD_String(" ADC-1 : ", 48+4,  5,    RED,     WHITE); 
LCD_String(" ADC-2 : ", 64+4,  5,    RED,     WHITE); 
LCD_String(" ADC-3 : ", 80+4,  5,    RED,     WHITE); 
LCD_String(" ADC-4 : ", 96+4,  5,    RED,     WHITE); 
LCD_String(" ADC-5 : ", 112+4, 5,    RED,     WHITE); 

update:    LCD_Number(ADC_Read(0),32+4,75,RED,WHITE); // Update ADC Data
LCD_Number(ADC_Read(1),48+4,75,RED,WHITE);
LCD_Number(ADC_Read(2),64+4,75,RED,WHITE);
LCD_Number(ADC_Read(3),80+4,75,RED,WHITE);
LCD_Number(ADC_Read(4),96+4,75,RED,WHITE);
LCD_Number(ADC_Read(5),112+4,75,RED,WHITE);
goto update;

return 0;
} 