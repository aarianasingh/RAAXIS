
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
unsigned long delaytime = 500;
byte shape[16], background[16];

byte s[6]; //help

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
  }
  //0 is top of the screen, set shape at top
  shape[0] = B01000000;
  shape[1] = B11100000;

  //The shapes can all be draw in a 5x2 grid. The first 4 bits represent the top column, last 4 bits represent the bottom column.
  s[0] = B01001110;
  s[1] = B11001100;
  s[2] = B01101100;
  s[3] = B11100010;
  s[4] = B11110000;
  s[5] = B11000110;
  //randomSeed(analogRead(0));//insert pin number that is unconnected!;
}

byte generateShape () {
  int rand = random(0, 6);
  return s[rand];
}

byte topCol (byte rShape) {
  rShape = rShape >> 4;
  rShape = rShape << 4;
  rShape = rShape >> 2;
  return rShape;
}

byte bottomCol (byte rShape) {
  rShape = rShape << 4;
  rShape = rShape >> 2;
  return rShape;
}

void updateGraphics() {
  for (int i = 2; i < 4; ++i) {
    for (int j = 0; j < 8; ++j) {
      byte b = shape[8 * (i - 1) - 1 - j] | background[8 * (i - 1) - 1 - j];
      lc.setColumn(i, j, b);
    }
  }
}
void moveDown() {
  for (int i = 15; i > 0; --i) {
    shape[i] = shape[i - 1];
    //Serial.print(shape[i]);
  }
  shape[0] = 0;
}
void moveRight() {
  byte check_shape_shift = 1;

  //moving to the right
  for (int i = 15; i >= 0; i--)
  {
    if (!(((shape[i])& B00000001) == 00000000)) //checks if shift causes it to reach a wall
    {
      check_shape_shift = 0;
      break;
    }
  }
  if (check_shape_shift == 1)
    for (int i = 15; i >= 0; --i) {
      shape[i] = shape[i] >> 1;
      //Serial.print(shape[i]);
    }
}

void wipeShapeMatrix() {
  for (int i = 15; i > 0; --i) {
    shape[i] = 0;
  }
}

void loop() {
  int addr = 0;
  int counter = 1;
  bool placed = true;

  /*if (counter%16==0)
    {
    byte rShape = generateShape();
    wipeShapeMatrix();
    shape[0] = topCol(rShape);
    shape[1] = bottomCol(rShape);
    delay (1000);
    }*/
  updateGraphics();
  //moveDown();
  //delay(delaytime);
  moveRight();
  delay(delaytime);
  ++counter;
}

/*
  This function lights up a some Leds in a row.
  The pattern will be repeated on every row.
  The pattern will blink along with the row-number.
  row number 4 (index==3) will blink 4 times etc.
*/
/*void rows() {
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
  }*/

/*
  This function lights up a some Leds in a column.
  The pattern will be repeated on every column.
  The pattern will blink along with the column-number.
  column number 4 (index==3) will blink 4 times etc.
*/
/*void columns() {
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
  }*/

/*
  This function will light up every Led on the matrix.
  The led will blink along with the row-number.
  row number 4 (index==3) will blink 4 times etc.
*/
//void single() {
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

/*int r1 = 0, c1 = 7, r2 = 0, c2 = 6, r3 = 1, c3 = 6, r4 = 2, c4 = 6;
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
  }*/

/*void move_shape(byte game_map[], byte shape[], byte display[])
  {
  byte long_tetris = B00111100;
  byte square_tetris[2];
  square_tetris[0] = B00001100;
  square_tetris[1] = B00001100;

  //moving to the right
  for (int i = 7; i > 1; i--)
  {
    if (!(((shape[i - 1] >> 1) && B00000001) == 00000000)) //checks if shift causes it to reach a wall
    {
      check_shape_shift = 0;
      break;
    }
  }
  if (check_shape_shift == 1)
    for (int i = 8; i > 1; i--)
    {
      shape[i] = shape[i - 1] >> 1;
    }
  shape[0] == B00000000;

  //moving to the left
  for (int i = 7; i > 1; i--)
  {
    if (!(shape[i - 1] << 1) && B10000001 == 00000000) //checks if shift causes it to reach a wall
    {
      check_shape_shift = 0;
      break;
    }
  }
  if (check_shape_shift == 1)
    for (int i = 8; i > 1; i--)
    {
      shape[i] = shape[i - 1] << 1;
    }
  shape[0] == B00000000;


  for (int i = 0; i < 8; i++)
  {
    if (game_map[i]&shape[i] != B00000000)
      check_game_move = false;

  }
  if (check_game_move == true)
  {
    for (int i = 0, i < 8; i++)
      display[i] = shape[i] & game_map[i];
  }
  //to check if the shape should be placed onto the game map i.e become static
  for (int

  }*/
