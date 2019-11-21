//We always have to include the library
#include "LedControl.h"
const int rightButton = 7;
const int leftButton = 6;
const int rotateButton = 5;
const int moveFaster = 4;
/*
  Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
  pin 12 is connected to the DataIn
  pin 11 is connected to the CLK
  pin 10 is connected to LOAD
  We have only a single MAX72XX.
*/
LedControl lc = LedControl(12, 11, 10, 4);

/*
   todo: CHANGE RANDOM SHAPE GEN
         CHANGE GRAPHICS DISPLAY
         CHANGE INITIAL SHAPE STORAGE
         SQUASH WEIRD MERGING BUG
*/

/* we always wait a bit between updates of the display */
unsigned long delaytime = 500;
int player_score = 0; //keeps track of the players score
int x = 0, y = 0, rotation = 0, typeShape = 0;
byte game_state = 1; //check if game is over
byte shape[17], background[17];
byte s[7][8];

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

  //for debugging
  Serial.begin(9600);
  pinMode (rightButton, INPUT);
  pinMode (leftButton, INPUT);
  pinMode (rotateButton, INPUT);
  pinMode (moveFaster, INPUT);


  //shape initialization
  for (int i = 0; i < 16; ++i) {
    shape[i] = 0;
    background[i] = 0;
  }
  //0 is top of the screen, set shape at top
  shape[0] = B00110000;
  shape[1] = B00110000;
  x = 3;
  y = 1;
  rotation  = 0;

  //square/smashboy initialization -1
  for (int i = 0; i < 8; i += 2) {
    s[0][i] = B11001100;
    s[0][i + 1] = B00000000;
  }

  //cleveland z initialization -5

  s[1][0] = B00001100;
  s[1][1] = B01100000;
  s[1][2] = B01001100;
  s[1][3] = B10000000;
  s[1][4] = B11000110;
  s[1][5] = B00000000;
  s[1][6] = B00100110;
  s[1][7] = B01000000;

  //rhode island z initialization

  s[2][0] = B00000110;
  s[2][1] = B11000000;
  s[2][2] = B10001100;
  s[2][3] = B01000000;
  s[2][4] = B01101100;
  s[2][5] = B00000000;
  s[2][6] = B01000110;
  s[2][7] = B00100000;

  //orange ricky
  s[3][0] = B00101110;
  s[3][1] = B00000000;
  s[3][2] = B01000100;
  s[3][3] = B01100000;
  s[3][4] = B00001110;
  s[3][5] = B10000000;
  s[3][6] = B11000100;
  s[3][7] = B01000000;

  //blue ricky
  s[4][0] = B10001110;
  s[4][1] = B00000000;
  s[4][2] = B01100100;
  s[4][3] = B01000000;
  s[4][4] = B00001110;
  s[4][5] = B00100000;
  s[4][6] = B01000100;
  s[4][7] = B11000000;

  //hero
  s[5][0] = B00001111;
  s[5][1] = B00000000;
  s[5][2] = B01000100;
  s[5][3] = B01000100;
  s[5][4] = B00001111;
  s[5][5] = B00000000;
  s[5][6] = B01000100;
  s[5][7] = B01000100;

  //teewee
  s[6][0] = B01001110;
  s[6][1] = B00000000;
  s[6][2] = B01000110;
  s[6][3] = B01000000;
  s[6][4] = B00001110;
  s[6][5] = B01000000;
  s[6][6] = B01001100;
  s[6][7] = B01000000;

  randomSeed(analogRead(0));//insert pin number that is unconnected!;
}

void rotate() {
  if (typeShape != 0) {
    //change rotation type
    ++rotation;
    //make sure the range is within 0-3
    rotation = rotation % 4;
    int count = 0;
    //if the pivot point is at the corners,
    //move it over by one
    if (x == 0) x = 1;
    else if (x == 7) x = 6;
    int shift = x - 3;
    int canRotate = 1;
    byte row = B00000000;

    //check if can rotate
    for (int i = y - 1; (i <= y + 2) && (i < 16); ++i) {
      int num = rotation * 2 + (count / 2);
      if (count % 2 == 0) {
        row = bitshiftRotate(topCol(s[typeShape][num]), shift, shift);
      } else {
        row = bitshiftRotate(bottomCol(s[typeShape][num]), shift, shift);
      }
      if (row & background[i] != 0) {
        canRotate = 0;
        break;
      }
      ++count;
    }

    count = 0;
    //if can rotate, then change shape[i]
    if (canRotate == 1) {
      for (int i = y - 1; (i <= y + 2) && (i < 16); ++i) {
        int num = rotation * 2 + (count / 2);
        if (count % 2 == 0) {
          shape[i] = bitshiftRotate(topCol(s[typeShape][num]), shift, shift);
        } else {
          shape[i] = bitshiftRotate(bottomCol(s[typeShape][num]), shift, shift);
        }
        ++count;
      }
    }
  }
}

//helper method for rotate()
byte bitshiftRotate(byte row, int howmuch, int leftOrRight) {
  if (leftOrRight == 0) {
    return row;
  }
  int shifty = howmuch;
  if (howmuch < 0)
    shifty = shifty * -1;
  for (int i = 0; i < shifty; ++i) {
    //left shift
    if (leftOrRight < 0) {
      row = row << 1;
      //right shift
    } else if (leftOrRight > 0) {
      row = row >> 1;
    }
  }
  return row;
}


void generateShape () {
  int rand = 5;//random(0, 7);
  int orientation = random(0, 4);
  shape[0] = topCol(s[rand][orientation * 2]);
  shape[1] = bottomCol(s[rand][orientation * 2]);
  shape[2] = topCol(s[rand][orientation * 2 + 1]);
  shape[3] = bottomCol(s[rand][orientation * 2 + 1]);
  x = 3;
  y = 1;
  rotation  = orientation;
  typeShape = rand;
  Serial.print("generated\n");
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
  ++y;
  //delay(delaytime);
}
void moveLeft() {
  byte check_shape_shift = 1;

  //moving to the left
  for (int i = 15; i >= 0; i--)
  {

    //try removing plus 1
    if ((((shape[i])&B10000000) != 00000000) || (((shape[i] << 1)&background[i]) != B00000000)) //checks if shift causes it to reach a wall change + to minus if array is reversed
    {
      check_shape_shift = 0;
      break;
    }
  }
  if (check_shape_shift == 1) {
    for (int i = 15; i >= 0; --i) {
      shape[i] = shape[i] << 1;
    }
    --x;
  }//end of method
}
void moveRight() {
  byte check_shape_shift = 1;

  //moving to the right
  for (int i = 15; i >= 0; i--)
  {
    if ((((shape[i])&B00000001) != 00000000) || (((shape[i] >> 1)&background[i]) != B00000000)) //checks if shift causes it to reach a wall
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
  ++x;
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
  int rowsCleared = 0;
  for (int i = 15; i >= 0; --i) {
    if (background[i] == B11111111)
    {
      for (int k = i; k > 0; k--) //moves the rows above cleard line down
      {
        background[k] = background[k - 1];
      }
      ++i;
      background[0] = 0;
      ++rowsCleared;
      Serial.print("incremented rowsCleared\n");
    }
  }
  addScore(rowsCleared);
}
void addScore(int rowsCleared){
  if(rowsCleared == 1){
    player_score += 1;
  }else if(rowsCleared == 2){
    player_score +=2;
  }else if(rowsCleared ==3){
    player_score += 6;
  }else if(rowsCleared == 4){
    player_score += 24;
  }
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
    Serial.print("place_shape called\n");
    if (background[0] == B00000000 || background[1] == B00000000)
    {
      generateShape();
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
    int rotateButtonState = digitalRead(rotateButton);
    int moveFasterState = digitalRead(moveFaster);
    Serial.begin(9600);
    if (rightButtonState == LOW) {
      //lc.setRow(0, 0, B10000000);
      moveRight();
    }
    if (leftButtonState == LOW) {
      //lc.setRow(0, 0, B00000001);
      moveLeft();
    }
    if (moveFasterState == LOW) {
      moveDown();
    }
    else {
      if (counter % 5 == 0) {
        moveDown();
      }
    }
    if (rotateButtonState == LOW) {
      rotate();
    }
    Place_Shape();
    
    counter ++;
  }
  wipeGameMatrix();
  wipeShapeMatrix();
  updateGraphics();
}
