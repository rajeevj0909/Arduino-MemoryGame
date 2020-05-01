#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

//Used for menu
int mode = 0;
int menu_state = 0;
bool button_press=false;
bool reload_scroll=false;


//Used for scroll_top_row function
int index = 0;
String message;
String main_menu = ("L <---Menu---> R");

//Used for gameplay
String s[] = {}; //Sequence to remember
int n = 0; //Length of sequence
String m[] = {"Up", "Down", "Left", "Right"}; //Up, Down, Left, Right Choices
float t = 5000; //5 Seconds to type in answer
bool game_over = false;


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
  //RESET VARIABLES
  n = 0; //Length of sequence
  t = 5000; //5 Seconds to type in answer
  game_over = false;
  button_press=false;
  
  //Buttons for menu
  uint8_t buttons = lcd.readButtons();
  if (buttons) {
    if (buttons & BUTTON_RIGHT) {
      menu_state += 1;
      Serial.println("Right");
      if (menu_state > 4) {
        menu_state = 0;
      }
    }
    if (buttons & BUTTON_LEFT) {
      menu_state -= 1;
      Serial.println("Left");
      if (menu_state < 0) {
        menu_state = 4;
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
    reload_scroll=false;
    button_press=true;
    Serial.println("Menu State: "+String(menu_state)+"  Mode: "+String(mode));
   }
   else{
    button_press=false;
   }
  if (mode == 0) {
    switch (menu_state) {
      case (0): //HOME
        lcd.setBacklight(GREEN);
        lcd.setCursor((-7), 0);
        lcd.print("  Memory Game   ");
        lcd.setCursor(0, 1);
        lcd.print("L <---Menu---> R");
        break;
      case (1): //Practice
        if (reload_scroll==false){
          lcd.setBacklight(RED);
          message = "Practice Mode- Press SELECT or move on";
          scroll_top_row(message, main_menu);
          lcd.print("Practice Mode   ");
          reload_scroll=true;
        }
        break;
      case (2)://Story
        if (reload_scroll==false){
          lcd.setBacklight(BLUE);
          message = "Story Mode- Press SELECT or move on";
          scroll_top_row(message, main_menu);
          lcd.print("Story Mode      ");
          reload_scroll=true;
        }
        break;
      case (3)://Leaderboard
        if (reload_scroll==false){
          lcd.setBacklight(YELLOW);
          message = "Leaderboard- Press SELECT or move on";
          scroll_top_row(message, main_menu);
          lcd.print("Leaderboard     ");
          reload_scroll=true;
        }
        break;
      case (4)://Settings
        if (reload_scroll==false){
          lcd.setBacklight(TEAL);
          message = "Settings- Press SELECT or move on";
          scroll_top_row(message, main_menu);
          lcd.print("Settings        ");
          reload_scroll=true;
        }
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
  String message1 = ("READ CODE AND REPEAT");
  String message2 = ("BACK ON ARROWS!");
  scroll_top_row(message1, message2);
  delay(3000);

  //GAMEPLAY
  String s[4];
  n = 4;
  t = 5000;
  while (game_over == false) {
    lcd.clear();
    for (int i = 0; i < n; i++) {
      long randNumber = random(300);
      if (randNumber < 75) {
        s[i] = m[0];
      }
      else if (randNumber < 150) {
        s[i] = m[1];
      }
      else if (randNumber < 225) {
        s[i] = m[2];
      }
      else if (randNumber < 300) {
        s[i] = m[3];
      }
    }
    lcd.setCursor(0, 0);
    lcd.print(s[0] + " " + s[1]);
    lcd.setCursor(0, 1);
    lcd.print(s[2] + " " + s[3]);
    Serial.println(s[0] + s[1] + s[2] + s[3]);
    delay(1000);
    int get_inputs = 0;
    String user_attempt[4];
    while (get_inputs < 4) {
      lcd.clear();
      uint8_t buttons = lcd.readButtons();
      if (buttons) {
        if (button_press==false){
          if (buttons & BUTTON_UP) {
            user_attempt[get_inputs]="Up";
            get_inputs += 1;
          }
          if (buttons & BUTTON_DOWN) {
            user_attempt[get_inputs]="Down";
            get_inputs += 1;
          }
          if (buttons & BUTTON_LEFT) {
            user_attempt[get_inputs]="Left";
            get_inputs += 1;
          }
          if (buttons & BUTTON_RIGHT) {
            user_attempt[get_inputs]="Right";
            get_inputs += 1;
          }
          button_press=true;
        }
      }
      else{
        button_press=false;
        }
    }
    Serial.println("My GUESS: "+user_attempt[0] + user_attempt[1] + user_attempt[2] + user_attempt[3]);
    if ((user_attempt[0]!=s[0])or (user_attempt[1]!=s[1])or(user_attempt[2]!=s[2])or(user_attempt[3]!=s[3])){
      lcd.setCursor(0, 0);
      lcd.print("WRONG- COPY:"+s[0] + s[1] + s[2] + s[3]);
      lcd.setCursor(0, 1);
      lcd.print("You said: "+user_attempt[0] + user_attempt[1] + user_attempt[2] + user_attempt[3]);
      delay(3000);
      game_over=true;
      mode=0;
    }
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

void scroll_top_row(String top, String bottom) {
  top = " " + top + " ";
  lcd.setCursor(0, 1);
  lcd.print(bottom);
  lcd.setCursor(0, 0);
  while (index < top.length()) {
    index += 1;
    lcd.setCursor(0, 1);
    lcd.print(bottom);
    lcd.setCursor(0, 0);
    for (int j = index; j < top.length(); j++) {
      lcd.print(top.charAt(j));
    }
  }
}
