
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <usart.h>

#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF
#pragma config CCP2MX = PORTBE


#define _XTAL_FREQ      8000000         // Set operation for 8 Mhz
#define TMR_CLOCK       _XTAL_FREQ/4    // Timer Clock 2 Mhz

#define TFT_DC          PORTCbits.RC1   		// Location of TFT D/C
#define TFT_CS          PORTCbits.RC2   		// Location of TFT Chip Select
#define TFT_RST         PORTCbits.RC0   		// Location of TFT Reset
#define SEC_LED         PORTDbits.RD7

#define NS_RED           PORTAbits.RA1   
#define NS_GREEN        PORTAbits.RA2

#define NSLT_RED        PORTAbits.RA3
#define NSLT_GREEN      PORTAbits.RA4

#define EW_RED          PORTBbits.RB4
#define EW_GREEN        PORTBbits.RB5

#define EWLT_RED        PORTEbits.RE0
#define EWLT_GREEN      PORTEbits.RE2

#define NS_LT_SW        PORTBbits.RB1
#define NS_PED_SW       PORTAbits.RA5

#define EW_PED_SW       PORTBbits.RB0
#define EW_LT_SW        PORTBbits.RB2

#define MODE_LED        PORTBbits.RB6
//colors
#define OFF 		0               // Defines OFF as decimal value 0
#define RED 		1               // Defines RED as decimal value 1
#define GREEN 		2               // Defines GREEN as decimal value 2
#define YELLOW 		3               // Defines YELLOW as decimal value 3

#define Circle_Size     7               // Size of Circle for Light
#define Circle_Offset   15              // Location of Circle
#define TS_1            1               // Size of Normal Text
#define TS_2            2               // Size of PED Text
#define Count_Offset    10              // Location of Count

#define XTXT            30              // X location of Title Text 
#define XRED            40              // X location of Red Circle
#define XYEL            60              // X location of Yellow Circle
#define XGRN            80              // X location of Green Circle
#define XCNT            100             // X location of Sec Count

#define EW              0               // Number definition of East/West
#define EWLT            1               // Number definition of East/West Left Turn
#define NS              2               // Number definition of North/South
#define NSLT            3               // Number definition of North/South Left Turn
 
#define Color_Off       0               // Number definition of Off Color
#define Color_Red       1               // Number definition of Red Color
#define Color_Green     2               // Number definition of Green Color
#define Color_Yellow    3               // Number definition of Yellow Color

#define EW_Txt_Y        20
#define EW_Cir_Y        EW_Txt_Y + Circle_Offset
#define EW_Count_Y      EW_Txt_Y + Count_Offset
#define EW_Color        ST7735_CYAN

#define EWLT_Txt_Y      50
#define EWLT_Cir_Y      EWLT_Txt_Y + Circle_Offset
#define EWLT_Count_Y    EWLT_Txt_Y + Count_Offset
#define EWLT_Color      ST7735_WHITE

#define NS_Txt_Y        80
#define NS_Cir_Y        NS_Txt_Y + Circle_Offset
#define NS_Count_Y      NS_Txt_Y + Count_Offset
#define NS_Color        ST7735_BLUE 

#define NSLT_Txt_Y      110
#define NSLT_Cir_Y      NSLT_Txt_Y + Circle_Offset
#define NSLT_Count_Y    NSLT_Txt_Y + Count_Offset
#define NSLT_Color      ST7735_MAGENTA

#define PED_EW_Count_Y  30
#define PED_NS_Count_Y  90
#define PED_Count_X     2
#define Switch_Txt_Y    140



void drawRoundRect(unsigned char x, unsigned char y, unsigned char w, unsigned char h, unsigned char r, unsigned int color);
void fillRoundRect(unsigned char x, unsigned char y, unsigned char w, unsigned char h, unsigned char r, unsigned int color);
void drawTriangle(signed int x0, signed int y0, signed int x1, signed int y1, signed int x2, signed int y2, unsigned int color);
void fillTriangle(signed int x0, signed int y0, signed int x1, signed int y1, signed int x2, signed int y2, unsigned int color);
void setTextWrap(BOOL w);
void invertDisplay(BOOL i);
void NormalDisplay();
int Color565(int r, int g, int b);
void spiwrite(unsigned char spidata);
void write_command(unsigned char cmd_);
void write_data(unsigned char data_);
void Rcmd1();
void Rcmd2red();
void Rcmd2green();
void Rcmd3();
void setScrollDefinition(unsigned char top_fix_height, unsigned char bottom_fix_height, BOOL _scroll_direction);
void setAddrWindow(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1);
void drawPixel(unsigned char x, unsigned char y, unsigned int color);
void drawFastVLine(unsigned char x, unsigned char y, unsigned char h, unsigned int color);
void fillRect(unsigned char x, unsigned char y, unsigned char w, unsigned char h, unsigned int color);
void drawtext(unsigned char x, unsigned char y, char *_text, unsigned int color, unsigned int bg, unsigned char size);
void drawChar(unsigned char x, unsigned char y, unsigned char c, unsigned int color, unsigned int bg,  unsigned char size);
void fillRectangle(unsigned char x, unsigned char y, unsigned char w, unsigned char h, unsigned int color);
void drawCircle(signed int x0, signed int y0, signed int r, unsigned int color);
void drawCircleHelper(signed int x0, signed int y0, signed int r, unsigned int cornername, unsigned int color);
void fillCircle(signed int x0, signed int y0, signed int r, unsigned int color);
void fillCircleHelper(signed int x0, signed int y0, signed int r, unsigned int cornername, signed int delta, unsigned int color);
void fillScreen(unsigned int color) ;
void drawFastHLine(unsigned char x, unsigned char y, unsigned char w, unsigned int color);
void VerticalScroll(unsigned char _vsp);
void TFT_BlackTab_Initialize();
void TFT_GreenTab_Initialize();
void TFT_RedTab_Initialize(void);
void TFT_ST7735B_Initialize(void);
void LCD_Reset(void);
void drawRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int color);

BOOL wrap = TRUE;

#include <string.h>
unsigned char colstart = 0, rowstart = 0, _tft_type;
#define _swap(a, b) { signed int t; t = a; a = b; b = t;}
#define _width         128
#define _height        160
#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09
#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13
#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E
#define ST7735_PTLAR   0x30
#define ST7735_VSCRDEF 0x33
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36
#define ST7735_VSCRSADD 0x37
#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6
#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5
#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD
#define ST7735_PWCTR6  0xFC
#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

// Color definitions
#define   ST7735_BLACK   0x0000
#define   ST7735_BLUE    0x001F
#define   ST7735_RED     0xF800
#define   ST7735_GREEN   0x07E0
#define   ST7735_CYAN    0x07FF
#define   ST7735_MAGENTA 0xF81F
#define   ST7735_YELLOW  0xFFE0
#define   ST7735_WHITE   0xFFFF

void delay_ms(int ms)
{
           
#define COUNT_PER_MS    _XTAL_FREQ/4000     // Count per ms = 2000             
#define COUNT_SCALED    COUNT_PER_MS/8     // 

    int count;
    count = COUNT_SCALED * ms;
    count = (0xffff - count);

	T3CON = 0xb8;                       // Timer 2, 16-bit mode, pre-scaler 1:8
        
	TMR0L = count & 0x00ff;             // set the lower byte of TMR
	TMR0H = count >> 8;                 // set the upper byte of TMR

	PIR2bits.TMR3IF = 0;                // clear the Timer 0 flag
	T3CONbits.TMR3ON = 1;               // Turn on the Timer 0

	while (PIR2bits.TMR3IF == 0); 	    // wait for the Timer Flag to be 1 for done
	T3CONbits.TMR3ON = 0;               // turn off the Timer 0
}
    
const char font[] =
{
0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x5F, 0x00, 0x00,
0x00, 0x07, 0x00, 0x07, 0x00,
0x14, 0x7F, 0x14, 0x7F, 0x14,
0x24, 0x2A, 0x7F, 0x2A, 0x12,
0x23, 0x13, 0x08, 0x64, 0x62,
0x36, 0x49, 0x56, 0x20, 0x50,
0x00, 0x08, 0x07, 0x03, 0x00,
0x00, 0x1C, 0x22, 0x41, 0x00,
0x00, 0x41, 0x22, 0x1C, 0x00,
0x2A, 0x1C, 0x7F, 0x1C, 0x2A,
0x08, 0x08, 0x3E, 0x08, 0x08,
0x00, 0x80, 0x70, 0x30, 0x00,
0x08, 0x08, 0x08, 0x08, 0x08,
0x00, 0x00, 0x60, 0x60, 0x00,
0x20, 0x10, 0x08, 0x04, 0x02,
0x3E, 0x51, 0x49, 0x45, 0x3E,
0x00, 0x42, 0x7F, 0x40, 0x00,
0x72, 0x49, 0x49, 0x49, 0x46,
0x21, 0x41, 0x49, 0x4D, 0x33,
0x18, 0x14, 0x12, 0x7F, 0x10,
0x27, 0x45, 0x45, 0x45, 0x39,
0x3C, 0x4A, 0x49, 0x49, 0x31,
0x41, 0x21, 0x11, 0x09, 0x07,
0x36, 0x49, 0x49, 0x49, 0x36,
0x46, 0x49, 0x49, 0x29, 0x1E,
0x00, 0x00, 0x14, 0x00, 0x00,
0x00, 0x40, 0x34, 0x00, 0x00,
0x00, 0x08, 0x14, 0x22, 0x41,
0x14, 0x14, 0x14, 0x14, 0x14,
0x00, 0x41, 0x22, 0x14, 0x08,
0x02, 0x01, 0x59, 0x09, 0x06,
0x3E, 0x41, 0x5D, 0x59, 0x4E,
0x7C, 0x12, 0x11, 0x12, 0x7C,
0x7F, 0x49, 0x49, 0x49, 0x36,
0x3E, 0x41, 0x41, 0x41, 0x22,
0x7F, 0x41, 0x41, 0x41, 0x3E,
0x7F, 0x49, 0x49, 0x49, 0x41,
0x7F, 0x09, 0x09, 0x09, 0x01,
0x3E, 0x41, 0x41, 0x51, 0x73,
0x7F, 0x08, 0x08, 0x08, 0x7F,
0x00, 0x41, 0x7F, 0x41, 0x00,
0x20, 0x40, 0x41, 0x3F, 0x01,
0x7F, 0x08, 0x14, 0x22, 0x41,
0x7F, 0x40, 0x40, 0x40, 0x40,
0x7F, 0x02, 0x1C, 0x02, 0x7F,
0x7F, 0x04, 0x08, 0x10, 0x7F,
0x3E, 0x41, 0x41, 0x41, 0x3E,
0x7F, 0x09, 0x09, 0x09, 0x06,
0x3E, 0x41, 0x51, 0x21, 0x5E,
0x7F, 0x09, 0x19, 0x29, 0x46
};

const char font2[] =
{
0x26, 0x49, 0x49, 0x49, 0x32,
0x03, 0x01, 0x7F, 0x01, 0x03,
0x3F, 0x40, 0x40, 0x40, 0x3F,
0x1F, 0x20, 0x40, 0x20, 0x1F,
0x3F, 0x40, 0x38, 0x40, 0x3F,
0x63, 0x14, 0x08, 0x14, 0x63,
0x03, 0x04, 0x78, 0x04, 0x03,
0x61, 0x59, 0x49, 0x4D, 0x43,
0x00, 0x7F, 0x41, 0x41, 0x41,
0x02, 0x04, 0x08, 0x10, 0x20,
0x00, 0x41, 0x41, 0x41, 0x7F,
0x04, 0x02, 0x01, 0x02, 0x04,
0x40, 0x40, 0x40, 0x40, 0x40,
0x00, 0x03, 0x07, 0x08, 0x00,
0x20, 0x54, 0x54, 0x78, 0x40,
0x7F, 0x28, 0x44, 0x44, 0x38,
0x38, 0x44, 0x44, 0x44, 0x28,
0x38, 0x44, 0x44, 0x28, 0x7F,
0x38, 0x54, 0x54, 0x54, 0x18,
0x00, 0x08, 0x7E, 0x09, 0x02,
0x18, 0xA4, 0xA4, 0x9C, 0x78,
0x7F, 0x08, 0x04, 0x04, 0x78,
0x00, 0x44, 0x7D, 0x40, 0x00,
0x20, 0x40, 0x40, 0x3D, 0x00,
0x7F, 0x10, 0x28, 0x44, 0x00,
0x00, 0x41, 0x7F, 0x40, 0x00,
0x7C, 0x04, 0x78, 0x04, 0x78,
0x7C, 0x08, 0x04, 0x04, 0x78,
0x38, 0x44, 0x44, 0x44, 0x38,
0xFC, 0x18, 0x24, 0x24, 0x18,
0x18, 0x24, 0x24, 0x18, 0xFC,
0x7C, 0x08, 0x04, 0x04, 0x08,
0x48, 0x54, 0x54, 0x54, 0x24,
0x04, 0x04, 0x3F, 0x44, 0x24,
0x3C, 0x40, 0x40, 0x20, 0x7C,
0x1C, 0x20, 0x40, 0x20, 0x1C,
0x3C, 0x40, 0x30, 0x40, 0x3C,
0x44, 0x28, 0x10, 0x28, 0x44,
0x4C, 0x90, 0x90, 0x90, 0x7C,
0x44, 0x64, 0x54, 0x4C, 0x44,
0x00, 0x08, 0x36, 0x41, 0x00,
0x00, 0x00, 0x77, 0x00, 0x00,
0x00, 0x41, 0x36, 0x08, 0x00,
0x02, 0x01, 0x02, 0x04, 0x02
};

void spiwrite(unsigned char spidata)
{
char x;
   PIR1bits.SSPIF = 0;  
   x = SSPBUF;			// dummy read 
   SSPBUF = spidata; 
   while (!SSPSTATbits.BF);	// wait for transmission complete  
}

void write_command(unsigned char cmd_)
{
    TFT_CS = 0;

    TFT_DC = 0;
    spiwrite(cmd_);
 
    TFT_CS = 1;

}

void write_data(unsigned char data_)
{
    TFT_CS = 0;
    TFT_DC = 1;
    spiwrite(data_);
    TFT_CS = 1;
}

void LCD_Reset(void)
{
    TFT_RST = 0;
    delay_ms(100);
    TFT_RST = 1;
}
    
void Bcmd()
{
  write_command(ST7735_SWRESET);
  delay_ms(50);
  write_command(ST7735_SLPOUT);
  delay_ms(500);
  write_command(ST7735_COLMOD);
  write_data(0x05);
  delay_ms(10);
  write_command(ST7735_FRMCTR1);
  write_data(0x00);
  write_data(0x06);
  write_data(0x03);
  delay_ms(10);
  write_command(ST7735_MADCTL);
  write_data(0x08);
  write_command(ST7735_DISSET5);
  write_data(0x15);
  write_data(0x02);
  write_command(ST7735_INVCTR);
  write_data(0x00);
  write_command(ST7735_PWCTR1);
  write_data(0x02);
  write_data(0x70);
  delay_ms(10);
  write_command(ST7735_PWCTR2);
  write_data(0x05);
  write_command(ST7735_PWCTR3);
  write_data(0x01);
  write_data(0x02);
  write_command(ST7735_VMCTR1);
  write_data(0x3C);
  write_data(0x38);
  delay_ms(10);
  write_command(ST7735_PWCTR6);
  write_data(0x11);
  write_data(0x15);
  write_command(ST7735_GMCTRP1);
  write_data(0x09); write_data(0x16); write_data(0x09); write_data(0x20);
  write_data(0x21); write_data(0x1B); write_data(0x13); write_data(0x19);
  write_data(0x17); write_data(0x15); write_data(0x1E); write_data(0x2B);
  write_data(0x04); write_data(0x05); write_data(0x02); write_data(0x0E);
  write_command(ST7735_GMCTRN1);
  write_data(0x0B); write_data(0x14); write_data(0x08); write_data(0x1E);
  write_data(0x22); write_data(0x1D); write_data(0x18); write_data(0x1E);
  write_data(0x1B); write_data(0x1A); write_data(0x24); write_data(0x2B);
  write_data(0x06); write_data(0x06); write_data(0x02); write_data(0x0F);
  delay_ms(10);
  write_command(ST7735_CASET);
  write_data(0x00); write_data(0x02); write_data(0x08); write_data(0x81);
  write_command(ST7735_RASET);
  write_data(0x00); write_data(0x01); write_data(0x08); write_data(0xA0);
  write_command(ST7735_NORON);
  delay_ms(10);
  write_command(ST7735_DISPON);
  delay_ms(500);
}
void Rcmd1()
{
  write_command(ST7735_SWRESET);
  delay_ms(150);
  write_command(ST7735_SLPOUT);
  delay_ms(500);
  write_command(ST7735_FRMCTR1);
  write_data(0x01);
  write_data(0x2C);
  write_data(0x2D);
  write_command(ST7735_FRMCTR2);
  write_data(0x01);
  write_data(0x2C);
  write_data(0x2D);
  write_command(ST7735_FRMCTR3);
  write_data(0x01);
  write_data(0x2C);
  write_data(0x2D);
  write_data(0x01);
  write_data(0x2C);
  write_data(0x2D);
  write_command(ST7735_INVCTR);
  write_data(0x07);
  write_command(ST7735_PWCTR1);
  write_data(0xA2);
  write_data(0x02);
  write_data(0x84);
  write_command(ST7735_PWCTR2);
  write_data(0xC5);
  write_command(ST7735_PWCTR3);
  write_data(0x0A);
  write_data(0x00);
  write_command(ST7735_PWCTR4);
  write_data(0x8A);
  write_data(0x2A);
  write_command(ST7735_PWCTR5);
  write_data(0x8A);
  write_data(0xEE);
  write_command(ST7735_VMCTR1);
  write_data(0x0E);
  write_command(ST7735_INVOFF);
  write_command(ST7735_MADCTL);
  write_data(0xC8);
  write_command(ST7735_COLMOD);
  write_data(0x05); 
}

void Rcmd2red()
{
  write_command(ST7735_CASET);
  write_data(0x00); write_data(0x00);
  write_data(0x00); write_data(0x7F);
  write_command(ST7735_RASET);
  write_data(0x00); write_data(0x00);
  write_data(0x00); write_data(0x9F);
}

void Rcmd3()
{
  write_command(ST7735_GMCTRP1);
  write_data(0x02); write_data(0x1C); write_data(0x07); write_data(0x12);
  write_data(0x37); write_data(0x32); write_data(0x29); write_data(0x2D);
  write_data(0x29); write_data(0x25); write_data(0x2B); write_data(0x39);
  write_data(0x00); write_data(0x01); write_data(0x03); write_data(0x10);
  write_command(ST7735_GMCTRN1);
  write_data(0x03); write_data(0x1D); write_data(0x07); write_data(0x06);
  write_data(0x2E); write_data(0x2C); write_data(0x29); write_data(0x2D);
  write_data(0x2E); write_data(0x2E); write_data(0x37); write_data(0x3F);
  write_data(0x00); write_data(0x00); write_data(0x02); write_data(0x10);
  write_command(ST7735_NORON);
  delay_ms(10);
  write_command(ST7735_DISPON);
  delay_ms(100);
}

void setScrollDefinition(unsigned char top_fix_height, unsigned char bottom_fix_height, BOOL _scroll_direction)
{
  unsigned char scroll_height;
  scroll_height = _height - top_fix_height - bottom_fix_height;
  write_command(ST7735_VSCRDEF);
  write_data(0x00);
  write_data(top_fix_height);
  write_data(0x00);
  write_data(scroll_height);
  write_data(0x00);
  write_data(bottom_fix_height);
  write_command(ST7735_MADCTL);
  if(_scroll_direction){
    if(_tft_type == 0){
      write_data(0xD8);
    }
    if(_tft_type == 1){
      write_data(0xD0);
    }
    if(_tft_type == 2){
      write_data(0x18);
    }
  }
  else{
    if(_tft_type == 0){
      write_data(0xC8);
    }
    if(_tft_type == 1){
      write_data(0xC0);
    }
    if(_tft_type == 2){
      write_data(0x08);
    }
  }
}

void setAddrWindow(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1)
{
  write_command(ST7735_CASET);
  write_data(0);
  write_data(x0 + colstart);
  write_data(0);
  write_data(x1 + colstart);
  write_command(ST7735_RASET);
  write_data(0);
  write_data(y0 + rowstart);
  write_data(0);
  write_data(y1 + rowstart);
  write_command(ST7735_RAMWR); // Write to RAM
}

void drawPixel(unsigned char x, unsigned char y, unsigned int color)
{
  if((x >= _width) || (y >= _height)) 
    return;
  setAddrWindow(x,y,x+1,y+1);
  write_data(color >> 8);
  write_data(color & 0xFF);
}

void drawFastVLine(unsigned char x, unsigned char y, unsigned char h, unsigned int color)
{
  unsigned char hi, lo;
  if((x >= _width) || (y >= _height))
    return;
  if((y + h - 1) >= _height)
    h = _height - y;
  hi = color >> 8; lo = color;
  setAddrWindow(x, y, x, y + h - 1);
  TFT_DC = 1;
  TFT_CS = 0;

  while (h--) {
    spiwrite(hi);
    spiwrite(lo);
  }
  TFT_CS = 1;


}

void fillRect(unsigned char x, unsigned char y, unsigned char w, unsigned char h, unsigned int color)
{
  signed int i;
  // Update in subclasses if desired!
  for (i = x; i < x + w; i++)
  {
    drawFastVLine(i, y, h, color);
  }
}

void drawChar(unsigned char x, unsigned char y, unsigned char c, unsigned int color, unsigned int bg,  unsigned char size)
{
  char i, j;
  if((x >= _width) || (y >= _height))
    return;
  if(size < 1) size = 1;
  if((c < ' ') || (c > '~'))
    c = '?';
  for(i=0; i<5; i++ ) {
    unsigned char line;
    if(c < 'S')
      line = font[(c - 32)*5 + i];
    else
      line = font2[(c - 'S')*5 + i];
    for(j=0; j<7; j++, line >>= 1) {
      if(line & 0x01) {
        if(size == 1) drawPixel(x+i, y+j, color);
        else          fillRect(x+(i*size), y+(j*size), size, size, color);
      }
      else if(bg != color) {
           if(size == 1) drawPixel(x+i, y+j, bg);
           else          fillRect(x+i*size, y+j*size, size, size, bg);
        }
    }
  }
}

void drawtext(unsigned char x, unsigned char y, char *_text, unsigned int color, unsigned int bg, unsigned char size)
{
  unsigned char cursor_x, cursor_y;
  unsigned int textsize, i;
  cursor_x = x, cursor_y = y;
  textsize = strlen(_text);
  for(i = 0; i < textsize; i++){
    if(wrap && ((cursor_x + size * 5) > _width)){
      cursor_x = 0;
      cursor_y = cursor_y + size * 7 + 3 ;
      if(cursor_y > _height) cursor_y = _height;
      if(_text[i] == 0x20) goto _skip; }
    drawChar(cursor_x, cursor_y, _text[i], color, bg, size);
    cursor_x = cursor_x + size * 6;
    if(cursor_x > _width) cursor_x = _width;
    _skip:;}
}

void Rcmd2green()
{
  write_command(ST7735_CASET);
  write_data(0x00); write_data(0x02);
  write_data(0x00); write_data(0x7F + 0x02);
  write_command(ST7735_RASET);
  write_data(0x00); write_data(0x01);
  write_data(0x00); write_data(0x9F + 0x01);
}


void fillRectangle(unsigned char x, unsigned char y, unsigned char w, unsigned char h, unsigned int color)
{
  unsigned char hi, lo;
  if((x >= _width) || (y >= _height))
    return;
  if((x + w - 1) >= _width)  
    w = _width  - x;
  if((y + h - 1) >= _height) 
    h = _height - y;
  setAddrWindow(x, y, x+w-1, y+h-1);
  hi = color >> 8; lo = color;
  TFT_DC = 1;
  TFT_CS = 0;

  for(y=h; y>0; y--) {
    for(x = w; x > 0; x--) {
      spiwrite(hi);
      spiwrite(lo);
    }
  }
  TFT_CS = 1;


}

void drawCircle(signed int x0, signed int y0, signed int r, unsigned int color)
{
  signed int f, ddF_x, ddF_y, x, y;
  f = 1 - r, ddF_x = 1, ddF_y = -2 * r, x = 0, y = r;
  drawPixel(x0  , y0 + r, color);
  drawPixel(x0  , y0 - r, color);
  drawPixel(x0+r, y0    , color);
  drawPixel(x0-r, y0    , color);
  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    drawPixel(x0 + x, y0 + y, color);
    drawPixel(x0 - x, y0 + y, color);
    drawPixel(x0 + x, y0 - y, color);
    drawPixel(x0 - x, y0 - y, color);
    drawPixel(x0 + y, y0 + x, color);
    drawPixel(x0 - y, y0 + x, color);
    drawPixel(x0 + y, y0 - x, color);
    drawPixel(x0 - y, y0 - x, color);
  }
}

void drawCircleHelper(signed int x0, signed int y0, signed int r, unsigned int cornername, unsigned int color)
{
  signed int f, ddF_x, ddF_y, x, y;
  f = 1 - r, ddF_x = 1, ddF_y = -2 * r, x = 0, y = r;
  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
    if (cornername & 0x4) {
      drawPixel(x0 + x, y0 + y, color);
      drawPixel(x0 + y, y0 + x, color);
    }
    if (cornername & 0x2) {
      drawPixel(x0 + x, y0 - y, color);
      drawPixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8) {
      drawPixel(x0 - y, y0 + x, color);
      drawPixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1) {
      drawPixel(x0 - y, y0 - x, color);
      drawPixel(x0 - x, y0 - y, color);
    }
  }
}


void fillCircleHelper(signed int x0, signed int y0, signed int r, unsigned int cornername, signed int delta, unsigned int color)
{
  signed int f, ddF_x, ddF_y, x, y;
  f = 1 - r, ddF_x = 1, ddF_y = -2 * r, x = 0, y = r;
  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;

    if (cornername & 0x1) {
      drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
      drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
    }
    if (cornername & 0x2) {
      drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
      drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
    }
  }
}

void fillCircle(signed int x0, signed int y0, signed int r, unsigned int color)
{
  drawFastVLine(x0, y0 - r, 2 * r + 1, color);
  fillCircleHelper(x0, y0, r, 3, 0, color);
}
void fillScreen(unsigned int color) 
{
  fillRectangle(0, 0, _width, _height, color);
}

void drawFastHLine(unsigned char x, unsigned char y, unsigned char w, unsigned int color)
{
  unsigned char hi, lo;
  if((x >= _width) || (y >= _height))
    return;
  if((x + w - 1) >= _width)
    w = _width - x;
  hi = color >> 8; lo = color;
  setAddrWindow(x, y, x + w - 1, y);
  TFT_DC = 1;
  TFT_CS = 0;

  while (w--) {
    spiwrite(hi);
    spiwrite(lo);
  }
  TFT_CS = 1;

}

void TFT_GreenTab_Initialize()
{
  TFT_CS = 1;
  TFT_DC = 0;

  SSPSTAT=0x80;               // SMP:
  SSPCON1=0x30;               // SSPEN
//    SETUP_SPI(SPI_MASTER | SPI_H_TO_L | SPI_CLK_DIV_4 | SPI_XMIT_L_TO_H);

  Rcmd1();
  Rcmd2red();
  Rcmd3();
  write_command(ST7735_MADCTL);
  write_data(0xC0);
  _tft_type = 1;
}

  void TFT_BlackTab_Initialize()
{
  TFT_CS = 1;
  TFT_DC = 0;
  
  SSPSTAT=0x80;               // SMP:
  SSPCON1=0x30;               // SSPEN

  Rcmd1();
  Rcmd2green();
  Rcmd3();
  colstart = 2;
  rowstart = 1;
  _tft_type = 0;
}

void VerticalScroll(unsigned char _vsp)
{
  write_command(ST7735_VSCRSADD);
  write_data(0x00);
  write_data(_vsp);
}


void drawRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int color)
{
  drawFastHLine(x, y, w, color);
  drawFastHLine(x, y + h - 1, w, color);
  drawFastVLine(x, y, h, color);
  drawFastVLine(x + w - 1, y, h, color);
}

#if defined (DRAW_BMP_FROM_MMCSD_CARD)                       // Additional code for drawing BMP image files from MMC/SD card
#ifndef pixel_buffer
#define pixel_buffer  10
#endif
BOOL bmpDraw(char x, char y, char *bmpname){
  BOOL ec = 0, padding = 0;
  char bmpdata[pixel_buffer*3],
       planes, depth, r, g, b, col, row;
  int i, buffer=pixel_buffer*3, format, width, height, color;
  int32 offset, compression, bmp_size, row_size, padding_factor;
  if((x >= _width) || (y >= _height))
    return 1;
  if(fat16_open_file(bmpname) != 0)
    return 1;
  ec |= sdcard_read_byte(address_pointer + 1, &format);
  format <<= 8;
  ec |= sdcard_read_byte(address_pointer, &format);
  if(format != 0x4D42)                                             // BMP file format signature
    return 1;                                                      // Return error
  ec |= sdcard_read_byte(address_pointer + 0x0D, &offset);
  offset <<= 8;
  ec |= sdcard_read_byte(address_pointer + 0x0C, &offset);
  offset <<= 8;
  ec |= sdcard_read_byte(address_pointer + 0x0B, &offset);
  offset <<= 8;
  ec |= sdcard_read_byte(address_pointer + 0x0A, &offset);
  ec |= sdcard_read_byte(address_pointer + 0x13, &width);
  width <<= 8;
  ec |= sdcard_read_byte(address_pointer + 0x12, &width);
  ec |= sdcard_read_byte(address_pointer + 0x17, &height);
  height <<= 8;
  ec |= sdcard_read_byte(address_pointer + 0x16, &height);
  ec |= sdcard_read_byte(address_pointer + 0x1A, &planes);
  ec |= sdcard_read_byte(address_pointer + 0x1C, &depth);
  ec |= sdcard_read_byte(address_pointer + 0x21, &compression);
  compression <<= 8;
  ec |= sdcard_read_byte(address_pointer + 0x20, &compression);
  compression <<= 8;
  ec |= sdcard_read_byte(address_pointer + 0x1f, &compression);
  compression <<= 8;
  ec |= sdcard_read_byte(address_pointer + 0x1e, &compression);
  if(ec != 0 || compression != 0 || depth != 24 || planes != 1)
    return 1;
  bmp_size = file_size - offset;                                // bmp_size: BMP image raw size
  row_size = bmp_size/height;                                   // row_size: number of bytes per row
  if((x + width  - 1) >=  _width){                              // _width is the TFT screen width
    width = _width  - x;
    padding = 1;                                                //Padding = 1 ==> only upper left part will be displayed
    padding_factor = width / pixel_buffer;
    if(width % pixel_buffer)
      padding_factor++;
    padding_factor *= buffer;
  }
  if((y + height - 1) >= _height){                              // _height is the TFT screen height
    offset += row_size * (height - _height + y);                // Only upper part will be displayed
    height = _height - y;
  }
  file_pointer     = offset;
  address_pointer += offset;
  i = buffer;
  for(row = height; row > 0; row--){
  for(col = 0; col < width; col++){
    if(i >= buffer){
      i = 0;
      if(fat16_read_data(buffer, bmpdata) != 0)
        return 1;
    }
    b = bmpdata[i++];
    g = bmpdata[i++];
    r = bmpdata[i++];
    color = Color565(r, g, b);
    drawPixel(x + col, y + row, color);
  }
    if(padding == 1){
      i = buffer;
      file_pointer    += row_size - padding_factor;
      address_pointer += row_size - padding_factor;
    }
  }
  return 0;
}
#endif



char buffer[31];                        // general buffer for display purpose
char *nbr;                              // general pointer used for buffer
char *txt;

char EW_Count[]     = "00";             // text storage for EW Count
char EWLT_Count[]   = "00";             // text storage for EW Left Turn Count
char NS_Count[]     = "00";             // text storage for NS Count
char NSLT_Count[]   = "00";             // text storage for NS Left Turn Count

char PED_EW_Count[] = "00";             // text storage for EW Pedestrian Count
char PED_NS_Count[] = "00";             // text storage for NS Pedestrian Count

char SW_EWPED_Txt[] = "0";              // text storage for EW Pedestrian Switch
char SW_EWLT_Txt[]  = "0";              // text storage for EW Left Turn Switch
char SW_NSPED_Txt[] = "0";              // text storage for NS Pedestrian Switch
char SW_NSLT_Txt[]  = "0";              // text storage for NS Left Turn Switch

char SW_MODE_Txt[]  = "D";              // text storage for Mode Light Sensor
	
char Act_Mode_Txt[]  = "D";             // text storage for Actual Mode
char FlashingS_Txt[] = "0";             // text storage for Emergency Status
char FlashingR_Txt[] = "0";             // text storage for Flashing Request

char dir;
char Count;                             // RAM variable for Second Count
char PED_Count;                         // RAM variable for Second Pedestrian Count

char SW_EWPED;                          // RAM variable for EW Pedestrian Switch
char SW_EWLT;                           // RAM variable for EW Left Turn Switch
char SW_NSPED;                          // RAM variable for NS Pedestrian Switch
char SW_NSLT;                           // RAM variable for NS Left Turn Switch
char FLASHING;
char FLASHING_REQUEST;
char SW_MODE;                           // RAM variable for Mode Light Sensor
int MODE;
char direction;
float volt;
    
unsigned int get_full_ADC(void);
void Init_ADC(void);
void init_IO(void);


void Set_NS(char color);
void Set_NS_LT(char color);
void Set_EW(char color);
void Set_EW_LT(char color);

void PED_Control( char Direction, char Num_Sec);
void Day_Mode(void);
void Night_Mode(void);


void Wait_One_Second(void);
void Wait_Half_Second(void);
void Wait_N_Seconds (char);
void Wait_One_Second_With_Beep(void);

void Initialize_Screen(void);
void update_LCD_color(char , char );
void update_LCD_PED_Count(char direction, char count);
void update_LCD_misc(void);
void update_LCD_count(char, char);

void Initialize_Screen()
{
  LCD_Reset();
  TFT_GreenTab_Initialize();
  fillScreen(ST7735_BLACK);
  
  /* TOP HEADER FIELD */
  txt = buffer;
  strcpy(txt, "ECE3301L Spring 22-S4");  
  drawtext(2, 2, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  
  /* MODE FIELD */
  strcpy(txt, "Mode:");
  drawtext(2, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(35,10, Act_Mode_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);

  /* FLASHING REQUEST FIELD */
  strcpy(txt, "FR:");
  drawtext(50, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(70, 10, FlashingR_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  
  /* FLASHING STATUS FIELD */
  strcpy(txt, "FS:");
  drawtext(80, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(100, 10, FlashingS_Txt, ST7735_WHITE, ST7735_BLACK, TS_1); 
  
  /* SECOND UPDATE FIELD */
  strcpy(txt, "*");
  drawtext(120, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  
/* EAST/WEST UPDATE FIELD */
  strcpy(txt, "EAST/WEST");
  drawtext  (XTXT, EW_Txt_Y, txt, EW_Color, ST7735_BLACK, TS_1);
  drawRect  (XTXT, EW_Cir_Y-8, 60, 18, EW_Color);
  drawCircle(XRED, EW_Cir_Y, Circle_Size, ST7735_RED);
  drawCircle(XYEL, EW_Cir_Y, Circle_Size, ST7735_YELLOW);
  fillCircle(XGRN, EW_Cir_Y, Circle_Size, ST7735_GREEN);
  drawtext  (XCNT, EW_Count_Y, EW_Count, EW_Color, ST7735_BLACK, TS_2);    


  /* EAST/WEST LEFT TURN UPDATE FIELD */
  strcpy(txt, "E/W LT");
  drawtext  (XTXT, EWLT_Txt_Y, txt, EWLT_Color, ST7735_BLACK, TS_1);
  drawRect  (XTXT, EWLT_Cir_Y-8, 60, 18, EWLT_Color);  
  fillCircle(XRED, EWLT_Cir_Y, Circle_Size, ST7735_RED);
  drawCircle(XYEL, EWLT_Cir_Y, Circle_Size, ST7735_YELLOW);
  drawCircle(XGRN, EWLT_Cir_Y, Circle_Size, ST7735_GREEN);   
  drawtext  (XCNT, EWLT_Count_Y, EWLT_Count, EWLT_Color, ST7735_BLACK, TS_2);

  /* NORTH/SOUTH UPDATE FIELD */
  strcpy(txt, "NORTH/SOUTH");
  drawtext  (XTXT, NS_Txt_Y  , txt, NS_Color, ST7735_BLACK, TS_1);
  drawRect  (XTXT, NS_Cir_Y-8, 60, 18, NS_Color);
  fillCircle(XRED, NS_Cir_Y  , Circle_Size, ST7735_RED);
  drawCircle(XYEL, NS_Cir_Y  , Circle_Size, ST7735_YELLOW);
  drawCircle(XGRN, NS_Cir_Y  , Circle_Size, ST7735_GREEN);
  drawtext  (XCNT, NS_Count_Y, NS_Count, NS_Color, ST7735_BLACK, TS_2);
    
  /* NORTH/SOUTH LEFT TURN UPDATE FIELD */
  strcpy(txt, "N/S LT");
  drawtext  (XTXT, NSLT_Txt_Y, txt, NSLT_Color, ST7735_BLACK, TS_1);
  drawRect  (XTXT, NSLT_Cir_Y-8, 60, 18, NSLT_Color);
  fillCircle(XRED, NSLT_Cir_Y, Circle_Size, ST7735_RED);
  drawCircle(XYEL, NSLT_Cir_Y, Circle_Size, ST7735_YELLOW);
  drawCircle(XGRN, NSLT_Cir_Y, Circle_Size, ST7735_GREEN);  
  drawtext  (XCNT, NSLT_Count_Y, NSLT_Count, NSLT_Color, ST7735_BLACK, TS_2);
  
  /* EAST/WEST PEDESTRIAM UPDATE FIELD */  
  drawtext(2, PED_EW_Count_Y, PED_EW_Count, EW_Color, ST7735_BLACK, TS_2);
  strcpy(txt, "PEW");  
  drawtext(3, EW_Txt_Y, txt, EW_Color, ST7735_BLACK, TS_1);

  /* NORTH/SOUTH PEDESTRIAM UPDATE FIELD */
  strcpy(txt, "PNS");  
  drawtext(3, NS_Txt_Y, txt, NS_Color, ST7735_BLACK, TS_1);
  drawtext(2, PED_NS_Count_Y, PED_NS_Count, NS_Color, ST7735_BLACK, TS_2);
    
  /* MISCELLANEOUS UPDATE FIELD */  
  strcpy(txt, "EWP EWLT NSP NSLT MR");
  drawtext(1,  Switch_Txt_Y, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(6,  Switch_Txt_Y+9, SW_EWPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(32, Switch_Txt_Y+9, SW_EWLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(58, Switch_Txt_Y+9, SW_NSPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(87, Switch_Txt_Y+9, SW_NSLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  
  drawtext(112,Switch_Txt_Y+9, SW_MODE_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
}

void init_UART()
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF &
    USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX &
    USART_BRGH_HIGH, 25);
    OSCCON = 0x60;
}

void putch (char c)
{
    while (!TRMT);
    TXREG = c;
}



void init_IO()
{
    TRISA = 0xE1;
    TRISB = 0x07;
    TRISC = 0x00;
    TRISD = 0x00;
    TRISE = 0x00;                               // 
}

void Init_ADC()
{
    ADCON0 = 0x00;
    ADCON1= 0x0E; 
    ADCON2= 0xA9;
}

unsigned int get_full_ADC()
{
unsigned int result;
   ADCON0bits.GO=1;                     // Start Conversion
   while(ADCON0bits.DONE==1);           // wait for conversion to be completed
   result = (ADRESH * 0x100) + ADRESL;  // combine result of upper byte and
                                        // lower byte into result
   return result;                       // return the result.
}

void Set_NS(char color)
{
    direction = NS;			// Add these two lines here
    update_LCD_color(direction, color);
    
    switch (color)
    {
        case OFF: NS_RED =0;NS_GREEN=0;break;           // Turns off the NS LED
        case RED: NS_RED =1;NS_GREEN=0;break;           // Sets NS LED RED
        case GREEN: NS_RED =0;NS_GREEN=1;break;         // sets NS LED GREEN
        case YELLOW: NS_RED =1;NS_GREEN=1;break;        // sets NS LED YELLOW
    }
}

void Set_NS_LT(char color)
{
    direction = NSLT;			// Add these two lines here
    update_LCD_color(direction, color);
    
    switch (color)
    {
        case OFF: NSLT_RED =0;NSLT_GREEN=0;break;
        case RED: NSLT_RED =1;NSLT_GREEN=0;break;
        case GREEN: NSLT_RED =0;NSLT_GREEN=1;break;
        case YELLOW: NSLT_RED =1;NSLT_GREEN=1;break;
    }
    
    // put code here
}

void Set_EW(char color)
{
    
    direction = EW;			// Add these two lines here
    update_LCD_color(direction, color);
    
    switch (color)
    {
        case OFF: EW_RED =0;EW_GREEN=0;break;
        case RED: EW_RED =1;EW_GREEN=0;break;
        case GREEN: EW_RED =0;EW_GREEN=1;break;
        case YELLOW: EW_RED =1;EW_GREEN=1;break;
    }
    // put code here
}

void Set_EW_LT(char color)
{
    direction = EWLT;			// Add these two lines here
    update_LCD_color(direction, color);
    
    switch (color)
    {
        case OFF: EWLT_RED =0;EWLT_GREEN=0;break;
        case RED: EWLT_RED =1;EWLT_GREEN=0;break;
        case GREEN: EWLT_RED =0;EWLT_GREEN=1;break;
        case YELLOW: EWLT_RED =1;EWLT_GREEN=1;break;
    }
    // put code here 
}

void Activate_Buzzer()
{
    PR2 = 0b11111001 ;
    T2CON = 0b00000101 ;
    CCPR2L = 0b01001010 ;
    CCP2CON = 0b00111100 ; 
    // put code here 
}

void Deactivate_Buzzer()
{
    CCP2CON = 0x0;
    PORTBbits.RB3 = 0; 
    // put code here 
}

void PED_Control( char direction, char Num_Sec)
{ 
    for(char i = Num_Sec-1;i>0; i--)
    {
        update_LCD_PED_Count(direction, i);
        Wait_One_Second_With_Beep();            // hold the number on 7-Segment for 1 second
    }
    
    update_LCD_PED_Count(direction, 0);         //                            //
    Wait_One_Second_With_Beep();                // leaves the 7-Segment off for 1 second
}

void Day_Mode()
{
    MODE = 1;                                   // turns on the MODE_LED
    MODE_LED = 1;
    Act_Mode_Txt[0] = 'D';
    
    MODE_LED = 1;
    
    Set_NS_LT(RED);
    Set_EW(RED);
    Set_EW_LT(RED);
    Set_NS(GREEN);
    if(NS_PED_SW == 1)
    {
        PED_Control(NS, 7);
    }
    Wait_N_Seconds(9);
    Set_NS(YELLOW);
    Wait_N_Seconds(3);
    Set_NS(RED);
    if(EW_LT_SW == 1)
    {
        Set_EW_LT(GREEN);
        Wait_N_Seconds(7);
        Set_EW_LT(YELLOW);
        Wait_N_Seconds(3);
        Set_EW_LT(RED);
    }
    Set_EW(GREEN);
    if(EW_PED_SW == 1)
    {
        PED_Control(EW, 9);
    }
    Wait_N_Seconds(8);
    Set_EW(YELLOW);
    Wait_N_Seconds(3);
    
    if(NS_LT_SW == 1)
    {
        Set_EW(RED);
        Set_NS_LT(GREEN);
        Wait_N_Seconds(7);
        Set_NS_LT(YELLOW);
        Wait_N_Seconds(3);
        
    }
    
    // put code here 
}
void Night_Mode()
{ 
    MODE = 0;                                   // turns on the MODE_LED
    MODE_LED = 0;
    Act_Mode_Txt[0] = 'N';
    
    // put code here
    PORTD = 0xFF;
    PORTC = 0xFF;
    PORTEbits.RE1 = 1;
    MODE_LED = 0;
    Set_NS_LT(RED);
    Set_EW(RED);
    Set_EW_LT(RED);
    Set_NS(GREEN);
    Wait_N_Seconds(9);
    Set_NS(YELLOW);
    Wait_N_Seconds(3);
    Set_NS(RED);
    if(EW_LT_SW == 1)
    {
        Set_EW_LT(GREEN);
        Wait_N_Seconds(7);
        Set_EW_LT(YELLOW);
        Wait_N_Seconds(3);
        Set_EW_LT(RED);
    }
    Set_EW(GREEN);
    Wait_N_Seconds(9);
    Set_EW(YELLOW);
    Wait_N_Seconds(3);
    
    if(NS_LT_SW == 1)
    {
        Set_EW(RED);
        Set_NS_LT(GREEN);
        Wait_N_Seconds(7);
        Set_NS_LT(YELLOW);
        Wait_N_Seconds(3);
        
    }
}

void Old_Wait_One_Second()                      //creates one second delay and blinking asterisk
{
    SEC_LED = 1;
    Wait_Half_Second();                         // Wait for half second (or 500 msec)
    SEC_LED = 0;
    Wait_Half_Second();                         // Wait for half second (or 500 msec)

}

void Wait_One_Second()							//creates one second delay and blinking asterisk
{
    SEC_LED = 1;
    strcpy(txt,"*");
    drawtext(120,10,txt,ST7735_WHITE,ST7735_BLACK,TS_1);
    Wait_Half_Second();                         // Wait for half second (or 500 msec)

    SEC_LED = 0;
    strcpy(txt," ");
    drawtext(120,10,txt,ST7735_WHITE,ST7735_BLACK,TS_1);
    Wait_Half_Second();                         // Wait for half second (or 500 msec)
    update_LCD_misc();
}

void Wait_One_Second_With_Beep()				// creates one second delay as well as sound buzzer
{
    // put code here
    SEC_LED = 1;
    strcpy(txt,"*");
    drawtext(120,10,txt,ST7735_WHITE,ST7735_BLACK,TS_1);
    Activate_Buzzer();
    Wait_Half_Second();
    SEC_LED = 0;
    strcpy(txt," ");
    drawtext(120,10,txt,ST7735_WHITE,ST7735_BLACK,TS_1);
    Deactivate_Buzzer();
    Wait_Half_Second();
    update_LCD_misc();
}
    
void Wait_Half_Second()
{
    T0CON = 0x03;                               // Timer 0, 16-bit mode, prescaler 1:16
    TMR0L = 0xDB;                               // set the lower byte of TMR
    TMR0H = 0x0B;                               // set the upper byte of TMR
    INTCONbits.TMR0IF = 0;                      // clear the Timer 0 flag
    T0CONbits.TMR0ON = 1;                       // Turn on the Timer 0
    while (INTCONbits.TMR0IF == 0);             // wait for the Timer Flag to be 1 for done
    T0CONbits.TMR0ON = 0;                       // turn off the Timer 0
}

void Wait_N_Seconds (char seconds)
{
    char I;
    for (I = seconds; I> 0; I--)
    {
        update_LCD_count(direction, I);  
        Wait_One_Second();                      // calls Wait_One_Second for x number of times
      
    }
    update_LCD_count(direction, 0);   
}

void update_LCD_color(char direction, char color)
{
    char Circle_Y;
    Circle_Y = EW_Cir_Y + direction * 30;    
    
    if (color == Color_Off)     //if Color off make all circles black but leave outline
    {
        fillCircle(XRED, Circle_Y, Circle_Size, ST7735_BLACK);
        fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_BLACK);
        fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_BLACK); 
        drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);            
        drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
        drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);                       
    }    
    else if (color == Color_Red)
    {
        fillCircle(XRED, Circle_Y, Circle_Size, ST7735_BLACK);
        fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_BLACK);
        fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_BLACK); 
        fillCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);            
        drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
        drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);   
    }
    else if (color == Color_Green)
    {
        fillCircle(XRED, Circle_Y, Circle_Size, ST7735_BLACK);
        fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_BLACK);
        fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_BLACK); 
        drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);            
        drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
        fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN); 
    }
    else
    {
        fillCircle(XRED, Circle_Y, Circle_Size, ST7735_BLACK);
        fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_BLACK);
        fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_BLACK); 
        drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);            
        fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
        drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN); 
    }
    
          
    // put code here
}

void update_LCD_count(char direction, char count)
{
   switch (direction)                   //update traffic light no ped time
   {
      case EW:        
        EW_Count[0] = count/10  + '0';
        EW_Count[1] = count%10  + '0';
        drawtext(XCNT, EW_Count_Y, EW_Count, EW_Color, ST7735_BLACK, TS_2);                
        break;
      
       case EWLT:        
        EWLT_Count[0] = count/10  + '0';
        EWLT_Count[1] = count%10  + '0';
        drawtext(XCNT, EWLT_Count_Y, EWLT_Count, EWLT_Color, ST7735_BLACK, TS_2);                
        break;
        
       case NS:        
        NS_Count[0] = count/10  + '0';
        NS_Count[1] = count%10  + '0';
        drawtext(XCNT, NS_Count_Y, NS_Count, NS_Color, ST7735_BLACK, TS_2);                
        break;
        
       case NSLT:        
        NSLT_Count[0] = count/10  + '0';
        NSLT_Count[1] = count%10  + '0';
        drawtext(XCNT, NSLT_Count_Y, NSLT_Count, NSLT_Color, ST7735_BLACK, TS_2);                
        break;
    // put code here
    }  
}

void update_LCD_PED_Count(char direction, char count)
{
   switch (direction)
   {
      case EW:       
        PED_EW_Count[0] = count/10  + '0';          // PED count upper digit
        PED_EW_Count[1] = count%10  + '0';          // PED Lower
        drawtext(PED_Count_X, PED_EW_Count_Y, PED_EW_Count, EW_Color, ST7735_BLACK, TS_2);        
        break;

        
       case NS:       
        PED_NS_Count[0] = count/10  + '0';          // PED count upper digit
        PED_NS_Count[1] = count%10  + '0';          // PED Lower
        drawtext(PED_Count_X, PED_NS_Count_Y, PED_NS_Count, NS_Color, ST7735_BLACK, TS_2);        
        break;
      // put code here  

   }
   
}
void Select_ADC_Channel (char channel)
{
    ADCON0 = channel * 4.88 + 1; //4.8828125
}
void update_LCD_misc()
{
    int nStep = get_full_ADC();         // calculates the # of steps for analog conversion
    volt = nStep * 5 /1024.0;           // gets the voltage in Volts, using 5V as reference s instead of 4, also divide by 1024 
    SW_MODE = volt < 2.5 ? 1:0;         // Mode = 1, Day_mode, Mode = 0 Night_mode
    
    printf( "Voltage = %d/F \r \n", volt);
    
    SW_EWPED = EW_PED_SW;
    SW_EWLT = EW_LT_SW;    
    SW_NSPED = NS_PED_SW;
    SW_NSLT = NS_LT_SW;

    if (SW_EWPED == 0) SW_EWPED_Txt[0] = '0'; else SW_EWPED_Txt[0] = '1';
    if (SW_EWLT == 0) SW_EWLT_Txt[0] = '0'; else SW_EWLT_Txt[0] = '1';
    if (SW_NSPED == 0) SW_NSPED_Txt[0] = '0'; else SW_NSPED_Txt[0] = '1';
    if (SW_NSLT == 0) SW_NSLT_Txt[0] = '0'; else SW_NSLT_Txt[0] = '1';
    if (SW_MODE == 0) SW_MODE_Txt[0] = 'N'; else SW_MODE_Txt[0] = 'D';
        // Set Text at bottom of screen to switch states
    // put code here  
   
    drawtext(35,10, Act_Mode_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);    
    drawtext(6,   Switch_Txt_Y+9, SW_EWPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
    drawtext(32,  Switch_Txt_Y+9, SW_EWLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
    drawtext(58,  Switch_Txt_Y+9, SW_NSPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);       // Show switch and sensor states at bottom of the screen
    drawtext(87,  Switch_Txt_Y+9, SW_NSLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);

    drawtext(112,  Switch_Txt_Y+9, SW_MODE_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
                  
}
void main(void)
{
    init_IO();
    Init_ADC();
    init_UART();
    OSCCON = 0x70; 
    RBPU = 0;    
    // set the system clock to be 1MHz 1/4 of the 4MHz
    Initialize_Screen();                        // Initialize the TFT screen
    Select_ADC_Channel (0);
    while(1)
    {
    int nStep = get_full_ADC();
        float voltage_mv = nStep * 5.0;
        float voltage = voltage_mv/1024;

        if(voltage >= 2.5)
        {
            Night_Mode();
        }
        else
            Day_Mode();
    }
//    int nStep = get_full_ADC();         // calculates the # of steps for analog conversion
//    volt = nStep * 5 /1024.0;       // gets the voltage in Volts, using 5V as reference s instead of 4, also divide by 1024 
//    SW_MODE = volt < 2.5 ? 1:0;        // Mode = 1, Day_mode, Mode = 0 Night_mode
//
//    while(1)                                    // forever loop
//    {
//        if (SW_MODE)    
//        {
//            Day_Mode();                         // calls Day_Mode() function
//        }
//        else
//        {
//            Night_Mode();                       // calls Night_Mode() function
//        }
     
}


