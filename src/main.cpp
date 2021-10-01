#include <Arduino.h>
#include <M5StickCPlus.h>
#include <LinkedList.h>

void setup() {
  M5.begin();
  m5.Lcd.begin();
}

//basic game settings
int speed = 100; //game cycle speed (100 = 10 c/s, 40 = 25 c/s)

int background = BLACK; //background
int snakeBodyColor = WHITE; //snake body part
int snakeHeadColor = 0x7bef; //snake head color (default is gray)
int wayHintColor = BLUE; //color of hints showing available ways to go
int foodColor = GREEN; //color of food

//Advanced settings       DO NOT CHANGE, game is not able to run properly on other values
int blockSize = 15;
int screenWidth = 135;
int screenHeight = 240;
int startPointX = 60;
int startPointY = 120;

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
snakeHeadPre* snakeHead = new snakeHeadPre(0, 0);
LinkedList<snakeBodyPre>* snakeBodyList = new LinkedList<snakeBodyPre>();
wayHintPre* wayHintList[2] = { new wayHintPre(0, 0), new wayHintPre(0, 0)};
foodPre* food = new foodPre(0, 0);
int score = 0; //num of food eated
bool isAlive = false;
bool addBodyPart = false;
snakeBodyPre* bodyPreset = new snakeBodyPre(0, 0);
int direction = 0; //snake facing direction (0 - up, 1 - right, 2 - down, 3 - left)

//game functions
void start(){ // sets|resets all values
  //snake head
  if(snakeHead != NULL)
    delete(snakeHead);
  snakeHead = new snakeHeadPre(startPointX, startPointY);
  //snake body
  if (snakeBodyList != NULL)
    delete(snakeBodyList);
  snakeBodyList = new LinkedList<snakeBodyPre>();
  //wayhint
  if (wayHintList[0] != NULL)//left
    delete(wayHintList[0]);
  wayHintList[0] = new wayHintPre(startPointX, startPointY);
  if (wayHintList[1] != NULL)//right
    delete(wayHintList[1]);
  wayHintList[1] = new wayHintPre(startPointX, startPointY);
  //food
  if(food != NULL)
    delete(food);
  food = new foodPre(startPointX, startPointY);
  //score
  score = 0;
  //isAlive
  isAlive = true;
  //addBodyPart
  addBodyPart = false;
  //food
  if(food != NULL)
    delete(food);
  direction = startDirection;
}

void Spawn_food(){ //it just changes food position
  bool canPlace = false;
  do
  {
    food->x = random(0, screenWidth / blockSize)*blockSize;
    food->y = random(0, screenHeight / blockSize)*blockSize;

    if(food->x != snakeHead->x || food->y != snakeHead->y){
      for(int bodyPart = snakeBodyList->size()-1; bodyPart <=0; bodyPart--){
        if (food->x == snakeBodyList->get(bodyPart).x && food->y == snakeBodyList->get(bodyPart).y)
        {
          canPlace = false;
          break;
        }
        canPlace = true;
      }
    }
  }while (!canPlace);
}

void Move_body(){
  for (int part = snakeBodyList->size()-1; part >= 0; part--)
  {
    switch (part)
    {
    case 0:
      snakeBodyList->set(part, snakeBodyPre(snakeHead->x, snakeHead->y));

      break;
    default:
      snakeBodyList->set(part, snakeBodyPre(snakeBodyList->get(part-1).x, snakeBodyList->get(part-1).y));
      break;
    }
  }
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
}

void Display_game(){ //thing lower will overdraw higher stuff
  m5.Lcd.fillScreen(background);
  //food
  m5.Lcd.fillRect(food->x, food->y, food->size, food->size, food->color);
  //head
  m5.Lcd.fillRect(snakeHead->x, snakeHead->y, snakeHead->size, snakeHead->size, snakeHead->color);
  //wayHint
  m5.Lcd.fillRect(wayHintList[0]->x, wayHintList[0]->y, wayHintList[0]->size, wayHintList[0]->size, wayHintList[0]->color);
  m5.Lcd.fillRect(wayHintList[1]->x, wayHintList[1]->y, wayHintList[1]->size, wayHintList[1]->size, wayHintList[1]->color);
  //body
  for (int part = snakeBodyList->size()-1; part>=0; part--)
  {
    m5.Lcd.fillRect(snakeBodyList->get(part).x, snakeBodyList->get(part).y, snakeBodyList->get(part).size, snakeBodyList->get(part).size, snakeBodyList->get(part).color);
  }
}

void Is_head_on_body(){
  for (int part = snakeBodyList->size()-1; part>=0; part--)
  {
    if (snakeBodyList->get(part).x == snakeHead->x && snakeBodyList->get(part).y == snakeHead->y)
    {
      isAlive = false;
      break;
    }
  }
}

void Change_direction(bool way){ //f = left, t = right  |  0 - up, 1 - right, 2 - down, 3 - left
  if(!way){ //left
    direction--;
    if(direction<0)
      direction = 3;
  }
  else{ //right
    direction++;
    if(direction>3)
      direction = 0;
  }
}

//game
void loop() {
  start();
  while (isAlive) //SNAKE
  {
    if(food->x == snakeHead->x && food->y == snakeHead->y){
      score++;
      addBodyPart = true;
      bodyPreset = new snakeBodyPre(snakeBodyList->get(snakeBodyList->size()-1).x, snakeBodyList->get(snakeBodyList->size()-1).y);
      Spawn_food();
    }

    Move_head();
    Move_body();

    if(addBodyPart == true) //create snake body if food eated
    {
      snakeBodyList->add(snakeBodyPre(bodyPreset->x, bodyPreset->y));
      delete(bodyPreset);
      addBodyPart = false;
    }
    
    Move_wayHint();

    Display_game();
    Is_head_on_body();
   
    for (int cicle = speed; cicle > 0; cicle--) //get user input and wait for gameSpeed delay
    {
      if (m5.BtnA.isPressed()) //turn left
      {
        Change_direction(false);
      }
      if (m5.BtnB.isPressed()) //turn right
      {
        Change_direction(true);
      }

      delay(1);
    }
    
  }

  m5.Lcd.println("game over"); //game over
  m5.Lcd.println(score);
  delay(1000);
  m5.Lcd.println("press BtnA to restart");
  while(!m5.BtnA.isPressed()){}
}