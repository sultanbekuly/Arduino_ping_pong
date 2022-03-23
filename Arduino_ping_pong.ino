//oled libraries:
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//oled vars:
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Button pins:
const int buttonUP = 6;
const int buttonDOWN = 5;
//button vars:
int lastButtonStateUP = LOW;   // the previous reading from the input pin
int lastButtonStateDOWN = LOW;   // the previous reading from the input pin
unsigned long debounceDelay = 10;    // the debounce time; increase if the output flickers

//GAME vars:
//scores:
int player_score = 0;
int enemy_score = 0;

//player:
int player_position_X = 19; // static
int player_position_Y = 0;
int player_width = 16;
int player_thickness = 4;

//enemy:
int enemy_position_X = 104; // static
int enemy_position_Y = 47;
int enemy_width = 16;
int enemy_thickness = 4;
long enemy_last_move_time = 0;
long enemy_speed_of_moving = 2000;//update time in ms


//ball:
//void fillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);
int ball_position_X = 63;
int ball_position_Y = 31;
int ball_radius = 1;
int ball_direction_X = 3;
int ball_direction_Y = 3;
int ball_speed = 8;//9,8,7...1
long ball_last_move_time = 0;


void setup() {
  // put your setup code here, to run once:
  pinMode(buttonUP, INPUT_PULLUP);
  pinMode(buttonDOWN, INPUT_PULLUP);

  Serial.begin(9600);
  Serial.println("Start");

  //initiate random
  randomSeed(analogRead(0));
  ball_direction_X = -3;
  ball_direction_Y = random(-5, 5);
  //ball_direction_Y = -5;//test

  

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  

  // Clear the buffer
  display.clearDisplay();
  
  
  //draw lines:
  display.drawLine(16,  0,  16, 63, SSD1306_WHITE);
  display.drawLine(111, 0, 111, 63, SSD1306_WHITE);
  display.display();
  
  //scores field init:
  display.setTextSize(2);    
  display.setTextColor(SSD1306_WHITE); // Draw white text

  player_score = 8888; // test
  enemy_score = 8888; // test
  print_score(player_score, 0);
  print_score(enemy_score, 115);
  
  display.setTextSize(3);
  display.setCursor(28, 0);
  display.write("Ping");
  display.setCursor(28, 31);
  display.write("Pong");
  display.display();
  
  display.setTextSize(2);
  delay(2000); // Pause for 2 seconds
  

  //NEW GAME:
  
  // Clear the buffer
  display.clearDisplay();
  
  //draw lines:
  display.drawLine(16,  0,  16, 63, SSD1306_WHITE);
  display.drawLine(111, 0, 111, 63, SSD1306_WHITE);
  display.display();
  
  //Write scores:
  player_score = 0; //reset player_score
  enemy_score = 0;  //reset enemy_score
  print_score(player_score, 0);
  print_score(enemy_score, 115);
  
  //Display players:
  //void fillRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color);
  display.fillRect(player_position_X, player_position_Y, player_thickness, player_width, SSD1306_WHITE);
  display.fillRect(enemy_position_X, enemy_position_Y, enemy_thickness, enemy_width, SSD1306_WHITE);
  display.display(); // Update screen with each newly-drawn rectangle

  
  //Display the ball:
  display.fillCircle(ball_position_X, ball_position_Y, ball_radius, SSD1306_WHITE);
  display.display();

  
  
  delay(500); // Pause for 0.5 second  
}

void loop() {
  buttons_check();

  move_the_ball_and_check_for_collisions();

  move_enemy();
}

void move_enemy(){
  //enemy:
  if(millis() > enemy_speed_of_moving+enemy_last_move_time){
    display.fillRect(enemy_position_X, enemy_position_Y, enemy_thickness, enemy_width, SSD1306_BLACK);
    
    if(ball_position_Y < enemy_position_Y+enemy_width/2){
      enemy_position_Y = enemy_position_Y - 3;
    }else{
      enemy_position_Y = enemy_position_Y + 3;
    }

    //checking if enemy is within the wall:
    if(enemy_position_Y > 64-player_width) enemy_position_Y = 64-player_width;
    if(enemy_position_Y < 0) enemy_position_Y = 0;
      
//    Serial.print("enemy_position_Y: "); Serial.println(enemy_position_Y);
    display.fillRect(enemy_position_X, enemy_position_Y, enemy_thickness, enemy_width, SSD1306_WHITE);
    display.display(); // Update screen with each newly-drawn rectangle
    enemy_last_move_time = millis();
  }
  
}

void move_the_ball_and_check_for_collisions(){
  //move th ball:
  if(millis() > ball_speed*20+ball_last_move_time){
    //erase ball on old position:
    display.fillCircle(ball_position_X, ball_position_Y, ball_radius, SSD1306_BLACK);
    display.display();
    
    //set new posion of the ball:
    ball_position_X = ball_position_X + ball_direction_X;
    
    if(ball_position_Y + ball_direction_Y < -1) ball_direction_Y = ball_direction_Y * -1;
    if(ball_position_Y + ball_direction_Y > 64) ball_direction_Y = ball_direction_Y * -1;
    
    ball_position_Y = ball_position_Y + ball_direction_Y;
    
    //draw ball on new position:
    display.fillCircle(ball_position_X, ball_position_Y, ball_radius, SSD1306_WHITE);
    display.display();
//    Serial.print("ball_position_Y: "); Serial.println(ball_position_Y);
    ball_last_move_time = millis();

    //Check for player loose:
    if(ball_position_X < player_position_X){
      Serial.println("Player lose!");
      newRound("enemy");//player
    }
    //check for collision of the ball and the player:
    if(player_position_X <= ball_position_X && player_position_X+player_thickness >= ball_position_X
    && player_position_Y <= ball_position_Y && player_position_Y+player_width >= ball_position_Y){
      Serial.println("Collision of the ball and the player");
      //send the ball to enemy with random values:
      ball_direction_X = 3;
      ball_direction_Y = random(-5, 5);
      display.fillRect(player_position_X, player_position_Y, player_thickness, player_width, SSD1306_WHITE);
      display.display(); // Update screen with each newly-drawn rectangle
      
    }
    //check for enemy loose:
    if(ball_position_X > enemy_position_X+enemy_thickness){
      Serial.println("Enemy lose!");
      newRound("player");//enemy
    }
    //check for collision of the ball and the enemy:
    if(enemy_position_X <= ball_position_X && enemy_position_X+enemy_thickness >= ball_position_X
    && enemy_position_Y <= ball_position_Y && enemy_position_Y+enemy_width >= ball_position_Y){
      Serial.println("Collision of the ball and the enemy");
      //send the ball to player with random values:
      ball_direction_X = -3;
      ball_direction_Y = random(-5, 5);
      display.fillRect(enemy_position_X, enemy_position_Y, enemy_thickness, enemy_width, SSD1306_WHITE);
      display.display(); // Update screen with each newly-drawn rectangle
    }
  }
  
}

void newRound(String winner){
  // Clear the buffer
  display.clearDisplay();
  
  //draw lines:
  display.drawLine(16,  0,  16, 63, SSD1306_WHITE);
  display.drawLine(111, 0, 111, 63, SSD1306_WHITE);
  display.display();
  
  //Update scores:
  if(winner == "enemy"){
    enemy_score++;
  }else{
    player_score++;
  }
  print_score(player_score, 0);
  print_score(enemy_score, 115);

  //reset gaming vars:
  //player:
  player_position_X = 19; // static
  player_position_Y = 0;
  player_width = 16;
  player_thickness = 4;
  
  //ball:
  ball_position_X = 63;
  ball_position_Y = 31;
  ball_radius = 1;
  //set random direction for th ball:
  
  ball_direction_X = -3;
  ball_direction_Y = random(-5, 5);
  //ball_direction_Y = -5;//test
  ball_last_move_time = 0;
  
  //Display players:
  //void fillRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t color);
  display.fillRect(player_position_X, player_position_Y, player_thickness, player_width, SSD1306_WHITE);
  display.display(); // Update screen with each newly-drawn rectangle

  //enemy:
  enemy_position_X = 104; // static
  enemy_position_Y = 47;
  enemy_width = 16;
  enemy_thickness = 4;
  enemy_last_move_time = 0;

  //checking for if we need to update enemy_speed_of_moving and ball_speed
  if((player_score+enemy_score)%5 == 0){
    //5,10,15 and so on
    if(ball_speed > 3) ball_speed = ball_speed - 1; //10,9,8...
    Serial.print("ball_speed: ");Serial.println(ball_speed);
  }
  if((player_score+enemy_score)%10 == 0){
    //10,20,30 and so on
    if(enemy_speed_of_moving > 1) enemy_speed_of_moving = enemy_speed_of_moving * 0.9; //2000,1800,1620,1458...
    Serial.print("enemy_speed_of_moving: ");Serial.println(enemy_speed_of_moving);
  }
  
  delay(500); // Pause for 0.5 seconds
}

void print_score(int temp_num, int X){ //0/115
  for(int i=48; i>=0; i-=16){
    
    int num = temp_num % 10;
    char cstr[16];
    itoa(num, cstr, 10);
    
    display.setCursor(X, i);
    display.write(cstr);
    display.display();
//    Serial.println(cstr);
    
    temp_num = temp_num/10;
    if(temp_num==0){
      break;
    }
  }
}

void buttons_check(){ 
  if (!digitalRead(buttonUP) && !lastButtonStateUP) {
    lastButtonStateUP = true;
//    Serial.println("UP pressed");
    if(player_position_Y > 0){
      display.fillRect(player_position_X, player_position_Y, player_thickness, player_width, SSD1306_BLACK);
      player_position_Y = player_position_Y-3;
      display.fillRect(player_position_X, player_position_Y, player_thickness, player_width, SSD1306_WHITE);
      display.display(); // Update screen with each newly-drawn rectangle
    }
  }
  if (digitalRead(buttonUP) && lastButtonStateUP) {
    lastButtonStateUP = false;
  }


  if (!digitalRead(buttonDOWN) && !lastButtonStateDOWN) {
    lastButtonStateDOWN = true;
//    Serial.println("DOWN pressed");
    if(player_position_Y < 64-player_width){
      display.fillRect(player_position_X, player_position_Y, player_thickness, player_width, SSD1306_BLACK);
      player_position_Y = player_position_Y+3;
      display.fillRect(player_position_X, player_position_Y, player_thickness, player_width, SSD1306_WHITE);
      display.display(); // Update screen with each newly-drawn rectangle
    }
  }
  if (digitalRead(buttonDOWN) && lastButtonStateDOWN) {
    lastButtonStateDOWN = false;
  }
}


/*
oled figures:
https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives




*/
