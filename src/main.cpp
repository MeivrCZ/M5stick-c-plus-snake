#include <Arduino.h>
#include <M5StickCPlus.h>

void setup() {
  M5.begin();
  Serial.begin(38400);
  M5.Lcd.begin();
  delay(1000);
  Serial.println("snake begin");
}

//basic game settings
int speed = 15; //100 + (10*speed) miliseconds

int background = BLACK; //background
int snakeBodyColor = WHITE; //snake body part
int snakeHeadColor = 0x7bef; //snake head color (default is gray)
int wayHintColor = BLUE; //color of hints showing available ways to go
int foodColor = GREEN; //color of food

//Advanced settings       DO NOT CHANGE, game is not able to run properly on other values
int blockSize = 15;
int screenWidth = 120; //135
int screenHeight = 225; //240
int startPointX = 60;
int startPointY = 120;
const int numOfBlocks = 144; // (sHeight/bSize)*(sWidth/bSize) 

//ingame start var., different values won't change gameplay much
int startDirection = 0; //snake facing direction (0 - up, 1 - right, 2 - down, 3 - left)


////////////////////////////////////////////////////
//any change after this point will brake this game//
////////////////////////////////////////////////////






//game block prop. 
class block{
  public:
  int x; //position on x axis
  int y; //position on y axis
  int size = blockSize;
};

class snakeBodyPre: public block {
  public:
  snakeBodyPre(int xPos, int yPos){
    x = xPos;
    y = yPos;
  }
  int color = snakeBodyColor;
};

class snakeHeadPre: public block {
  public:
  snakeHeadPre(int xPos, int yPos){
    x = xPos;
    y = yPos;
  }

  int color = snakeHeadColor;
};

class foodPre: public block {
  public:
  foodPre(int xPos, int yPos){
    x = xPos;
    y = yPos;
  }

  int color = foodColor;
};

class wayHintPre: public block {
  public:
  wayHintPre(int xPos, int yPos){
    x = xPos;
    y = yPos;
  }

  int color = wayHintColor;
};







//game declarations
snakeHeadPre* snakeHead = NULL;
snakeBodyPre* snakeBodyList[numOfBlocks];
wayHintPre* wayHintList[2];
foodPre* food = NULL;
int score = 0; //num of food eated
bool isAlive = false;
bool addBodyPart = false;
snakeBodyPre* bodyPreset = NULL;
int direction = 0; //snake facing direction (0 - up, 1 - right, 2 - down, 3 - left)
int turn = 0; //0 - do not turn, 1 - left, 2 right

//game functions
void start(){ // sets|resets all values
  Serial.println("start");
  //snake head
  if(snakeHead != NULL){
    delete(snakeHead);
    snakeHead = NULL;
    }
  snakeHead = new snakeHeadPre(startPointX, startPointY);
  Serial.println("start - snake head");
  //snake body
  for (int part = 0; part <= numOfBlocks-1; part++)
  {
    if (snakeBodyList[part] != NULL){
      delete(snakeBodyList[part]);
      snakeBodyList[part] = NULL;
    }
  }
  Serial.println("start - snake body");
  //wayhint
  if (wayHintList[0] != NULL){//left
    delete(wayHintList[0]);
    wayHintList[0] = NULL;
  }
  wayHintList[0] = new wayHintPre(startPointX, startPointY);
  if (wayHintList[1] != 0){//right
    delete(wayHintList[1]);
    wayHintList[1] = NULL;
  }
  wayHintList[1] = new wayHintPre(startPointX, startPointY);
  Serial.println("start - wayHint");
  //food
  if(food != NULL){
    delete(food);
    food = NULL;
  }
  food = new foodPre(startPointX, startPointY);
  Serial.println("start - food");
  //score
  score = 0;
  Serial.println("start - score");
  //isAlive
  isAlive = true;
  Serial.println("start - isAlive");
  //addBodyPart
  addBodyPart = false;
  Serial.println("start - bodypart");
  //bodyPreset
  if(bodyPreset != NULL){
    delete(bodyPreset);
    bodyPreset = NULL;
  }
  Serial.println("satrt - bodyPreset");
  //direction
  direction = startDirection;
  Serial.println("start - direction");
  //turn
  turn = 0;
  Serial.println("start - turn");
}

void Spawn_food(){ //it just changes food position
  Serial.println("Spawn_food");
  bool canPlace = false;
  do
  {
    food->x = random(0, screenWidth / blockSize)*blockSize;
    food->y = random(0, screenHeight / blockSize)*blockSize;

    if(food->x != snakeHead->x || food->y != snakeHead->y){
      Serial.println("Spawn_food - food is not on head");
      for(int bodyPart = numOfBlocks-1; bodyPart >=0; bodyPart--){
        if (snakeBodyList[bodyPart] != NULL)
        {
          if (food->x == snakeBodyList[bodyPart]->x && food->y == snakeBodyList[bodyPart]->y)
          {
            canPlace = false;
            Serial.println("Spawn_food - cannot place");
            break;
          }
        }
        canPlace = true;
        //Serial.println("Spawn_food - can place");
      }
    }
    else{
      Serial.println("Spawn_food - food is on head");
    }
  }while (!canPlace);
  Serial.println("Spawn_food - placed");
}

void Move_body(){
  for (int part = numOfBlocks-1; part >= 0; part--)
  {
    if (snakeBodyList[part] != NULL)
    {
      switch (part)
      {
      case 0:
        snakeBodyList[part]-> x = snakeHead->x;
        snakeBodyList[part]-> y = snakeHead->y;

        break;
      default:
        snakeBodyList[part]->x = snakeBodyList[part-1]->x;
        snakeBodyList[part]->y = snakeBodyList[part-1]->y;
        break;
      }
    }
  }
  Serial.println("Move_body - body moved");
}

void Move_head(){
  switch (direction) //0 - up, 1 - right, 2 - down, 3 - left
  {
  case 0:
    snakeHead->y-=blockSize;
    break;
  case 1:
    snakeHead->x+=blockSize;
    break;
  case 2:
    snakeHead->y+=blockSize;
    break;
  case 3:
    snakeHead->x-=blockSize;
    break;
  }

  if(snakeHead->x > screenWidth) //if snake is behind "wall", it will be moved on other side of screen
    snakeHead->x = 0;
  else if(snakeHead->x < 0){
    snakeHead->x = screenWidth;
  }
  else if(snakeHead->y > screenHeight)
    snakeHead->y = 0;
  else if (snakeHead->y < 0){
    snakeHead->y = screenHeight;
  }
  Serial.println("Move_head - head moved");
}

void Move_wayHint(){
  switch (direction) //0 - up, 1 - right, 2 - down, 3 - left
  {
  case 0:
    wayHintList[0]->x = snakeHead->x+blockSize;
    wayHintList[1]->x = snakeHead->x-blockSize;

    wayHintList[0]->y = snakeHead->y;
    wayHintList[1]->y = snakeHead->y;
    break;
  case 1:
    wayHintList[0]->y = snakeHead->y+blockSize;
    wayHintList[1]->y = snakeHead->y-blockSize;

    wayHintList[0]->x = snakeHead->x;
    wayHintList[1]->x = snakeHead->x;
    break;
  case 2:
    wayHintList[0]->x = snakeHead->x+blockSize;
    wayHintList[1]->x = snakeHead->x-blockSize;

    wayHintList[0]->y = snakeHead->y;
    wayHintList[1]->y = snakeHead->y;
    break;
  case 3:
    wayHintList[0]->y = snakeHead->y+blockSize;
    wayHintList[1]->y = snakeHead->y-blockSize;

    wayHintList[0]->x = snakeHead->x;
    wayHintList[1]->x = snakeHead->x;
    break;
  }
  Serial.println("Move_wayHint - hints moved");
}

void Display_game(){ //thing lower will overdraw higher stuff
  Serial.println("Display_game");
  M5.Lcd.fillScreen(background);
  Serial.println("Display_game - background");
  //food
  M5.Lcd.fillRect(food->x, food->y, food->size, food->size, food->color);
  Serial.println("Display_game - food");
  //head
  M5.Lcd.fillRect(snakeHead->x, snakeHead->y, snakeHead->size, snakeHead->size, snakeHead->color);
  Serial.println("Display_game - head");
  //wayHint
  M5.Lcd.fillRect(wayHintList[0]->x, wayHintList[0]->y, wayHintList[0]->size, wayHintList[0]->size, wayHintList[0]->color);
  M5.Lcd.fillRect(wayHintList[1]->x, wayHintList[1]->y, wayHintList[1]->size, wayHintList[1]->size, wayHintList[1]->color);
  Serial.println("Display_game - hint");
  //body
  for (int part = numOfBlocks-1; part>=0; part--)
  {
    if(snakeBodyList[part] != NULL){
      M5.Lcd.fillRect(snakeBodyList[part]->x, snakeBodyList[part]->y, snakeBodyList[part]->size, snakeBodyList[part]->size, snakeBodyList[part]->color);
    }
  }
  Serial.println("Display_game - body");
}

void Is_head_on_body(){
  for (int part = numOfBlocks-1; part>=0; part--)
  {
    if(snakeBodyList[part] != NULL)
    {
      if (snakeBodyList[part]->x == snakeHead->x && snakeBodyList[part]->y == snakeHead->y)
      {
        isAlive = false;
        Serial.println("Is_head_on_body - head is on body");
        break;
      }
    }
  }
}

void Change_direction(){ //0 - up, 1 - right, 2 - down, 3 - left
  switch (turn)  //0 - do not turn, 1 - left, 2 right
  {
  case 1: 
    direction--;
    if(direction<0)
      direction = 3;
    Serial.println("Change_direction - turned left");
    break;

  case 2:
    direction++;
    if(direction>3)
      direction = 0;
    Serial.println("Change_direction - turned right");
    break;
  }
}

void Add_snake_body(){ //create snake body if food eated
  Serial.println("Add_snake_body");
  for (int part = 0; addBodyPart; part++)
  {
    if (snakeBodyList[part] == NULL)
    {
      snakeBodyList[part] = new snakeBodyPre(bodyPreset->x, bodyPreset->y);
      delete(bodyPreset);
      bodyPreset = NULL;
      addBodyPart = false;
      Serial.println("Add_snake_body - body added");
    }
  }
}

void Make_preset(){
  if (snakeBodyList[0] == NULL) //if snakeBodyList is empety, it will get data from snakeHead
  {
    bodyPreset = new snakeBodyPre(snakeHead->x, snakeHead->y);
  }
  else{ //get data from last body instance in list
    bool presetIsMade = false;
    for(int part = numOfBlocks-1; !presetIsMade; part--){
      if (snakeBodyList[part] != NULL)
      {
        bodyPreset = new snakeBodyPre(snakeBodyList[part]->x, snakeBodyList[part]->y);
        presetIsMade = true;
        Serial.println("Make_preset - preset was made");
      }
    }
  }
}

//game
void loop() {
  start();
  Serial.println("loop - start");
  while (isAlive) //SNAKE
  {
    Serial.println("loop - snake is alive");
    if(food->x == snakeHead->x && food->y == snakeHead->y){
      Serial.println("loop - food eaten - if works");
      score++;
      addBodyPart = true;
      Make_preset();
      Serial.println("loop - food eaten - new bodyPreset");
      Spawn_food();
      Serial.println("loop - food eaten - food spawned");
    }
    Serial.println("loop - food");

    Move_body();
    Move_head();
    Serial.println("loop - head and body moved");

    Add_snake_body();
    Serial.println("loop - add_body");
    
    Move_wayHint();
    Serial.println("loop - move hint");

    Display_game();
    Serial.println("loop - display");
    Is_head_on_body();
    Serial.println("loop - check if head is on body");

   //get user input and wait for gameSpeed delay
    delay(100); //time to unpress buttons
    for (int cicle = speed; cicle > 0; cicle--) 
    {
      if (M5.BtnA.read() == 1) //turn left
      {
        turn = 1;
        Serial.println("loop - btnA pressed");
      }
      if (M5.BtnB.read() == 1) //turn right
      {
        turn = 2;
        Serial.println("loop - btnB pressed");
      }
      delay(10);
      //Serial.println("loop - input check cicle passed");
    }
    Change_direction();
    turn = 0;

    Serial.println("loop - end of cicle");
  }
  Serial.println("loop - snake is dead");
  M5.Lcd.fillScreen(background); //game over
  M5.Lcd.println("game over"); 
  M5.Lcd.println(score);
  delay(1000);
  M5.Lcd.println("press BtnA to restart");
  while(M5.BtnA.read() != 1){}
  Serial.println("loop - game over - btnA pressed in");
}