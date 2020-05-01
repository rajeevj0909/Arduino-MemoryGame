#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

int menu_state = 0;
int mode = 0;
bool game_over = false;

//String s[] = {}; //Sequence to remember
int n = 0; //Length of sequence
//String m[] = {"U", "D", "L", "R"}; //Up, Down, Left, Right Choices
float t = 5000; //5 Seconds to type in answer

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  randomSeed(analogRead(0));
}

void loop() {
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7
  menu();
}

void menu() {
  uint8_t buttons = lcd.readButtons();
  if (buttons) {
    Serial.println("Menu State: "+String(menu_state)+"  Mode: "+String(mode));
    
    if (buttons & BUTTON_LEFT) {
      menu_state -= 1;
      int current_time = millis();
      Serial.println("Left");
      if (menu_state < 0) {
        menu_state = 4;
      }
    }
    if (buttons & BUTTON_RIGHT) {
      menu_state += 1;
      Serial.println("Right");
      if (menu_state > 4) {
        menu_state = 0;
      }
    }
    if (buttons & BUTTON_SELECT) {
      Serial.println("Select");
      if (menu_state == 1) { //Practice
        mode = 1; //practe_state
      }
      else if (menu_state == 2) { //Story
        mode = 2; //story_state
      }
      else if (menu_state == 3) { //Leaderboard
        mode = 3; //leaderboard_state
      }
      else if (menu_state == 4) { //Settings
        mode = 4; //settings_state
      }
    }
  }
  if (mode == 0) {
    switch (menu_state) {
      case (0): //HOME
        /*byte charName[] = {
          B11011,
          B11011,
          B00000,
          B00100,
          B00100,
          B10001,
          B10001,
          B01110};
          lcd.write(charName);*/
        lcd.setBacklight(GREEN);
        lcd.setCursor((-7), 0);
        lcd.print("  Memory Game   ");
        lcd.setCursor(0, 1);
        lcd.print("L <---Menu---> R");
        break;
      case (1): //Practice
        lcd.setBacklight(RED);
        lcd.setCursor(0, 0);
        lcd.print("Practice Mode- Press SELECT or move on");
        lcd.setCursor(0, 1);
        lcd.print("L <---Menu---> R");
        break;
      /*String message="Practice Mode- Press SELECT or move on";
        String end="NOPE";
        int i=0;
        while (end=="NOPE"){
        int current=millis();
        if (current+30000>millis()){
          i=i+1;
          lcd.print(message);
          Serial.println(i);
        }
        if (i>30){
          end="True";
        }
        }*/


      case (2)://Story
        lcd.setBacklight(BLUE);
        lcd.setCursor(0, 0);
        lcd.print("Story Mode- Press SELECT or move on");
        lcd.setCursor(0, 1);
        lcd.print("L <---Menu---> R");
        break;
      case (3)://Settings
        lcd.setBacklight(YELLOW);
        lcd.setCursor(0, 0);
        lcd.print("Leaderboard- Press SELECT or move on");
        lcd.setCursor(0, 1);
        lcd.print("L <---Menu---> R");
        break;
      case (4)://Leaderboard
        lcd.setBacklight(TEAL);
        lcd.setCursor(0, 0);
        lcd.print("Settings- Press SELECT or move on");
        lcd.setCursor(0, 1);
        lcd.print("L <---Menu---> R");
        break;
    }
  }
  else {
    switch (mode) {
      case 1:
        practice();
        break;
      case 2:
        story();
        break;
      case 3:
        leaderboard();
        break;
      case 4:
        settings();
        break;
    }
  }
}

void practice() {
  lcd.clear();
  Serial.println("Practice");
  
  //GAMEPLAY
  String s[4];
  n = 4;
  String m[]= {"U","D","L","R"};
  t = 5000;
  while (game_over == false) {
    lcd.clear();
    for (int i = 0; i < n; i++) {
      long randNumber = random(300);
      if (randNumber<75){
        s[i] = m[0];
      }
      else if (randNumber<150){
        s[i] = m[1];
      }
      else if (randNumber<225){
        s[i] = m[2];
      }
      else if (randNumber<300){
        s[i] = m[3];
      }
    }
    lcd.print(s[0]+s[1]+s[2]+s[3]);
    delay(3000);
  }
}

void story() {
  lcd.clear();
  Serial.println("Story");
}

void leaderboard() {
  lcd.clear();
  Serial.println("Leaderboard");
}

void settings() {
  lcd.clear();
  Serial.println("Settings");
}
