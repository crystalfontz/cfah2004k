//============================================================================
//
// Display is Crystalfontz CFAH2004K
//   https://www.crystalfontz.com/product/cfah2004Ktmijp
//   https://www.crystalfontz.com/product/cfah2004Kyykjp
//
// The controller is a Wuxi AiP31066L
//   https://www.crystalfontz.com/controllers/Wuxi/AiP31066L/
//
//============================================================================
#include <Arduino.h>
// Our dev board, an Arduino UNO R3 runs at 16MHz,
// and this define makes the delays come out correctly.
#define F_CPU 16000000UL
//============================================================================
//
// LCD Data is connected to port D
//   ARD  | Port | LCD
//  #0/D0 |  PD0 | LCD_D0
//  #1/D1 |  PD1 | LCD_D1 
//  #2/D2 |  PD2 | LCD_D2
//  #3/D3 |  PD3 | LCD_D3
//  #4/D4 |  PD4 | LCD_D4
//  #5/D5 |  PD5 | LCD_D5
//  #6/D6 |  PD6 | LCD_D6
//  #7/D7 |  PD7 | LCD_D7
//
#define LCD_PORT  (PORTD)
#define LCD_CTRL  (PORTB)
//
// LCD control lines
//  ARD  | Port | LCD
//  #8   |  PB0 | LCD_RS
//  #9   |  PB1 | LCD_RW
//  #10  |  PB2 | LCD_E
//
#define CLR_RS  (LCD_CTRL &= ~(0x01))
#define SET_RS  (LCD_CTRL |=  (0x01))
#define CLR_RW  (LCD_CTRL &= ~(0x02))
#define SET_RW  (LCD_CTRL |=  (0x02))
#define CLR_E   (LCD_CTRL &= ~(0x04))
#define SET_E   (LCD_CTRL |=  (0x04))
//
//============================================================================
//Instruction Table
#define SPLC780C_ClearDisplay 0x01
#define SPLC780C_ReturnHome   0x02
#define SPLC780C_EntryModeSet(ID, S) (0x04 | (ID << 1) | (S << 0))
#define SPLC780C_DisplayOnOffControl(D, C, B) (0x08 | (D << 2) | (C << 1) | (B << 0))
#define SPLC780C_CursorOrDisplayShift(SC, RL) (0x10 | (SC << 3) | (RL << 2))
#define SPLC780C_FunctionSet(DL, N, F) (0x20 | (DL << 4) | (N << 3) | (F << 2))
//============================================================================
uint8_t checkBusyAndAddress()
{
  //Turn Busy Flag on and turn PORTD to INPUT
	PORTD = 0x80;
	DDRD = 0x00;
	
	CLR_RS;
	SET_RW;

	// Select the LCD controller
	SET_E;

	//Watch Pin 7 for the busy flag
	while (0x80 == (PIND & 0x80))
	{
		delayMicroseconds(80);
		CLR_E;
		delayMicroseconds(80);
		SET_E;
	}
	CLR_E;
	CLR_RW;

  //turn PORTD to OUTPUT
	DDRD = 0xFF;

	return LCD_PORT;
}
//============================================================================
void writeCommand(uint8_t data)
{
  checkBusyAndAddress();
  LCD_PORT = data;    // Press the data onto the port (~250nS setup)

  CLR_RS; // Select the LCD's command register (~125nS setup)

  SET_E;  //
  CLR_E;
}
//----------------------------------------------------------------------------
void writeData(uint8_t data)
{
  checkBusyAndAddress();
  LCD_PORT = data;  // Press the data onto the port (~250nS setup)
  
  SET_RS; // Select the LCD's data register (~125nS setup)

  SET_E;  // 
  CLR_E;  //
}
//----------------------------------------------------------------------------
void lcd_init()
{
  writeCommand(SPLC780C_ClearDisplay);
  delay(15); // Sleep for 15mS
  writeCommand(SPLC780C_ReturnHome);
  delay(15); // Sleep for 15mS
  writeCommand(SPLC780C_DisplayOnOffControl(1, 0, 0));
  delay(15); // Sleep for 15mS
  writeCommand(SPLC780C_FunctionSet(1, 1, 1));
  delay(15); // Sleep for 15mS
}
//============================================================================
void moveCursor(uint8_t y, uint8_t x)
{
  uint8_t cursorPosition = 0x80;
  switch (y)
  {
    case 0:
    {
      cursorPosition += 0x00;
      break;
    }
    case 1:
    {
      cursorPosition += 0x40;
      break;
    }
    case 2:
    {
      cursorPosition += 0x14;
      break;
    }
    case 3:
    {
      cursorPosition += 0x54;
      break;
    }
  }
  writeCommand(cursorPosition + x);
}
//============================================================================
void writeString(char* myString)
{
	uint8_t i = 0;
	do
	{
    //send one character at a time
		writeData((uint8_t)myString[i]);
    i++;
	} while (myString[i] != NULL);
}
//============================================================================
void setup()
{
  //Set the port directions
  // LCD Data is connected to port D
  //   ARD  | Port | LCD
  //  #0/D0 |  PD0 | LCD_D0
  //  #1/D1 |  PD1 | LCD_D1 
  //  #2/D2 |  PD2 | LCD_D2
  //  #3/D3 |  PD3 | LCD_D3
  //  #4/D4 |  PD4 | LCD_D4
  //  #5/D5 |  PD5 | LCD_D5
  //  #6/D6 |  PD6 | LCD_D6
  //  #7/D7 |  PD7 | LCD_D7
  // Set all pins of port D to outputs as default
  DDRD = 0xFF;
  PORTD = 0x00;

  // LCD control lines
  //  ARD  | Port | LCD
  //  #8   |  PB0 | LCD_RS
  //  #9   |  PB1 | LCD_RW
  //  #10  |  PB2 | LCD_E
  DDRB = 0x07;
  PORTB = 0x00;
  
  //Drive the ports to a reasonable starting state.
  CLR_RW; // We can set this and forget it because there's no read functionality built into this test.
  SET_RS;
  CLR_E;
}
//============================================================================
void loop()
{
  //Initialize the LCD controller
  lcd_init();
  delay(500); // Sleep for 1/2 a second

  //set DDRAM address to first row first column
  moveCursor(0, 0);
  writeString("**  CRYSTALFONTZ  **");
  
  moveCursor(1, 0);
  writeString("**    CFAH2004K   **");

  moveCursor(2, 0);
  writeString("**  20x4 Display  **");

  moveCursor(3, 0);
  writeString("www.crystalfontz.com");

  delay(5000);
} // void loop()
//============================================================================
