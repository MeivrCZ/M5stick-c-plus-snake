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

//Advanced settings
int blockSize = 15; //DO NOT CHANGE, game is not able to handle other values
int screenWidth = 135;
int screenHeight = 240;

//ingame var., different values won't change gameplay much
int direction = 0; //snake facing direction (0 - up, 1 - left, 2 - down, 3 - right)
int startPointX = 68;
int startPointY = 120;


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
LinkedList<snakeBodyPre>* snakeBodyList = NULL;
wayHintPre* wayHintList[2] = {NULL, NULL};
foodPre* food = NULL;
int score = NULL; //num of food eated
bool isAlive = NULL;
bool addBodyPart = NULL;
snakeBodyPre* bodyPreset = NULL;

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
}

void Spawn_food(){ //it just changes food position
  bool canPlace = false;
  do
  {
    food->x = random(0, screenWidth);
    food->y = random(0, screenWidth);

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

    //move body

    if(addBodyPart == true)
    {
      snakeBodyList->add(snakeBodyPre(bodyPreset->x, bodyPreset->y));
      delete(bodyPreset);
      addBodyPart = false;
    }
    
    //move head
    //move wayHint
    //display game
    //check if head is not on body
    //wait and capture data for another cicle
  }

  m5.Lcd.println("game over"); //game over
  m5.Lcd.println(score);
  delay(1000);
  m5.Lcd.println("press BtnA to restart");
  while(!m5.BtnA.isPressed()){}
}