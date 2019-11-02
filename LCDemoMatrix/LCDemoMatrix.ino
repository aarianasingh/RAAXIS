//We always have to include the library
#include "LedControl.h"
const int rightButton = 4;
const int leftButton = 3;
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
int player_score = 0; //keeps track of the players score
byte game_state = 1; //check if game is over
byte shape[17], background[17];
byte s[6]; //help

void setup() {
  /*
    The MAX72XX is in power-saving mode on startup,
    we have to do a wakeup call
  */
  background[16] = B11111111; // initialised to 1 to indicate the end row of the matrix
  for (int i = 0; i < 4; ++i) {
    lc.shutdown(i, false);
    /* Set the brightness to a medium values */
    lc.setIntensity(i, 8);
    /* and clear the display */
    lc.clearDisplay(i);
  }

  pinMode (rightButton, INPUT);
  pinMode (leftButton, INPUT);
  for (int i = 0; i < 16; ++i) {
    shape[i] = 0;
    background[i] = 0;
  }
  //0 is top of the screen, set shape at top
  shape[0] = B00010000;
  shape[1] = B00111000;

  //The shapes can all be draw in a 5x2 grid. The first 4 bits represent the top column, last 4 bits represent the bottom column.
  s[0] = B01001110;
  s[1] = B11001100;
  s[2] = B01101100;
  s[3] = B11100010;
  s[4] = B11110000;
  s[5] = B11000110;
  randomSeed(analogRead(0));//insert pin number that is unconnected!;
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
  //delay(delaytime);
}
void moveLeft() {
  byte check_shape_shift = 1;

  //moving to the left
  for (int i = 15; i >= 0; i--)
  {
    if ((((shape[i])&B10000000) != 00000000) || (((shape[i] << 1)&background[i + 1]) != B00000000)) //checks if shift causes it to reach a wall change + to minus if array is reversed
    {
      check_shape_shift = 0;
      break;
    }
  }
  if (check_shape_shift == 1)
    for (int i = 15; i >= 0; --i) {
      shape[i] = shape[i] << 1;
    }
}//end of method

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
}//end of method

void wipeShapeMatrix() {
  for (int i = 15; i >= 0; --i) {
    shape[i] = 0;
  }
}//end of method

void wipeGameMatrix() {
  for (int i = 15; i >= 0; --i) {
    background[i] = 0;
  }
}

bool bottomDetection() {
  for (int i = 15; i >= 0; i--)
  {
    if (((shape[i]& background[i + 1]) != B00000000)) //checks if shift causes it to reach a shape or the end row
    {
      return 0;
    }
  }
  return 1;
}
void placeShape() {
  for (int i = 15; i >= 0; --i) {
    background[i] = background[i] | shape[i];
  }
}
void clearRows() {
  int i = 15;
  while (background[i] == B11111111) {
    --i;
  }
  int diff = 15 - i;
  for (int j = i; j >= 0; --j) {
    background[j + diff] = background[j];
  }
  for (int j = 0; j < diff; ++j) {
    background[j] = 0;
  }

  /*for (int i = 15; i >= 0; --i) {
    if (background[i] == B11111111)
    {
      for (int k = i; k > 0; k--) //moves the rows above cleard line down
      {
        background[k] = background[k - 1];
      }
      background[0] = 0;
      player_score++;
    }
    }*/
}
//clears the shape matrix and places the newly generated shape at the begining of the shape matrix
//should call random generate shape function and change the shape
void Place_Shape()
{

  bool check_place_shape = bottomDetection();
  if (check_place_shape == 0)
  {
    placeShape();
    clearRows();
    wipeShapeMatrix();

    if (background[0] == B00000000 || background[1] == B00000000)
    {
      byte rShape = generateShape();
      shape[0] = topCol(rShape);
      shape[1] = bottomCol(rShape);
    }
    else {
      game_state = 0;
      wipeGameMatrix();
    }
  }
}//end of method
void loop() {

  bool placed = true;
  lc.setRow(0, 0, B00000000);

  int counter = 0;

  while (game_state) {
      updateGraphics();
    int rightButtonState = digitalRead(rightButton);
    int leftButtonState = digitalRead(leftButton);

    if (rightButtonState == LOW) {
      //lc.setRow(0, 0, B10000000);
      moveRight();
    }
    if (leftButtonState == LOW) {
      //lc.setRow(0, 0, B00000001);
      moveLeft();
    }
    if (counter % 5 == 0) {
      moveDown();
    }
    Place_Shape();

    counter ++;
  }
  wipeGameMatrix();
  wipeShapeMatrix();
  updateGraphics();
}
