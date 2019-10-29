
//We always have to include the library
#include "LedControl.h"

/*
  Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
  pin 12 is connected to the DataIn
  pin 11 is connected to the CLK
  pin 10 is connected to LOAD
  We have only a single MAX72XX.
*/
LedControl lc = LedControl(12, 11, 10, 4);

/* we always wait a bit between updates of the display */
unsigned long delaytime = 1000;
byte shape[16], background[16], graphics[16];

void setup() {
  /*
    The MAX72XX is in power-saving mode on startup,
    we have to do a wakeup call
  */
  for (int i = 0; i < 4; ++i) {
    lc.shutdown(i, false);
    /* Set the brightness to a medium values */
    lc.setIntensity(i, 8);
    /* and clear the display */
    lc.clearDisplay(i);
  }
  for (int i = 0; i < 16; ++i) {
    shape[i] = 0;
    background[i] = 0;
    graphics[i] = 0;
  }
  //0 is top of the screen
  shape[0] = B01000000;
  shape[1] = B11100000;
}

void updateGraphics() {
  for (int i = 2; i < 4; ++i) {
    for (int j = 0; j < 8; ++j) {
      byte b = shape[8 * (i - 1) - 1 - j] | background[8 * (i - 1) - 1 - j];
      lc.setColumn(i, j, b);
    }
  }
}

/*
  This function lights up a some Leds in a row.
  The pattern will be repeated on every row.
  The pattern will blink along with the row-number.
  row number 4 (index==3) will blink 4 times etc.
*/
void rows() {
  for (int row = 0; row < 8; row++) {
    delay(delaytime);
    lc.setRow(0, row, B10100000);
    delay(delaytime);
    lc.setRow(0, row, (byte)0);
    for (int i = 0; i < row; i++) {
      delay(delaytime);
      lc.setRow(0, row, B10100000);
      delay(delaytime);
      lc.setRow(0, row, (byte)0);
    }
  }
}

/*
  This function lights up a some Leds in a column.
  The pattern will be repeated on every column.
  The pattern will blink along with the column-number.
  column number 4 (index==3) will blink 4 times etc.
*/
void columns() {
  for (int col = 0; col < 8; col++) {
    delay(delaytime);
    lc.setColumn(0, col, B10100000);
    delay(delaytime);
    lc.setColumn(0, col, (byte)0);
    for (int i = 0; i < col; i++) {
      delay(delaytime);
      lc.setColumn(0, col, B10100000);
      delay(delaytime);
      lc.setColumn(0, col, (byte)0);
    }
  }
}

/*
  This function will light up every Led on the matrix.
  The led will blink along with the row-number.
  row number 4 (index==3) will blink 4 times etc.
*/
void single() {
  /*for(int row=0;row<8;row++) {
    for(int col=0;col<8;col++) {
    delay(delaytime);
    lc.setLed(1,row,col,true);
    delay(delaytime);
    for(int i=0;i<col;i++) {
      lc.setLed(1,row,col,false);
      delay(delaytime);
      lc.setLed(1,row,col,true);
      delay(delaytime);
    }
    }
    }*/

  int r1 = 0, c1 = 7, r2 = 0, c2 = 6, r3 = 1, c3 = 6, r4 = 2, c4 = 6;
  for (int i = 0; i < 4; ++i) {
    while (c1 >= 0)
    { lc.setLed(i, r1, c1, true);
      lc.setLed(i, r2, c2, true);
      delay(delaytime);
      --c1;
      --c2;
      lc.setLed(i, r1, c1 + 1, false);
    }
    if (c1 < 0) {
      lc.setLed(i, r1, c1 + 1, false);
      c1 = 7;
      continue;
    }
  }
}
void shiftShape() {
  for (int i = 15; i > 0; --i) {
    shape[i] = shape[i - 1];
    //Serial.print(shape[i]);
  }
  shape[0] = 0;
}
void loop() {
  //writeArduinoOnMatrix();
  //rows();
  //columns();
  updateGraphics();
  shiftShape();
  delay(delaytime);
  //single();
}
