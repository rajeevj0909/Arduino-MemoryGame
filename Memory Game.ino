#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
#include <EEPROM.h>
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

//Used for menu
int mode = 0;
int menu_state = 0;
int leaderboard_state = 0;
int settings_state = 0;
bool button_press = false;
bool reload_scroll = false;

//Used for gameplay
String s[] = {}; //Sequence to remember
int n = 2; //Length of sequence
String  m[] = {"U", "D", "L", "R"};//Choices in practice mode
int d = 1000; // Time it's shown
int t = 5000; //Seconds to answer
bool game_over = false;

//Used for scroll_top_row function
int index = 0;
String message;
String main_menu = ("L <---Menu---> R");

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  randomSeed(analogRead(0));
  //setup_leaderboard(); //Run this line if it's a new arduino
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
  game_over = false;
  button_press = false;

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
      lcd.clear();
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
    reload_scroll = false;
    button_press = true;
    Serial.println("Mode:" + String(mode) + "Menu:" + String(menu_state) + "Leader:" + String(leaderboard_state) + "Sett:" + String(settings_state));
  }
  else {
    button_press = false;
  }
  if (mode == 0) {
    switch (menu_state) {
      case (0): //HOME
        lcd.setBacklight(GREEN);
        lcd.setCursor(0, 0);
        lcd.print("  Memory Game   ");
        lcd.setCursor(0, 1);
        lcd.print("L <---Menu---> R");
        break;
      case (1): //Practice
        if (reload_scroll == false) {
          lcd.setBacklight(RED);
          message = "Practice Mode- Press SELECT or move on";
          scroll_top_row(message, main_menu);
          lcd.print("Practice Mode   ");
          reload_scroll = true;
        }
        break;
      case (2)://Story
        if (reload_scroll == false) {
          lcd.setBacklight(TEAL);
          message = "Story Mode- Press SELECT or move on";
          scroll_top_row(message, main_menu);
          lcd.print("Story Mode      ");
          reload_scroll = true;
        }
        break;
      case (3)://Leaderboard
        if (reload_scroll == false) {
          lcd.setBacklight(YELLOW);
          message = "  Leaderboard- Press SELECT or move on";
          scroll_top_row(message, main_menu);
          lcd.print("Leaderboard     ");
          reload_scroll = true;
        }
        break;
      case (4)://Settings
        if (reload_scroll == false) {
          lcd.setBacklight(BLUE);
          message = "Settings- Press SELECT or move on";
          scroll_top_row(message, main_menu);
          lcd.print("Settings        ");
          reload_scroll = true;
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
  Serial.println("Practice");
  index = 0;
  String message1 = ("   READ CODE AND REPEAT IT BACK ON ARROWS");
  String message2 = ("   GOOD LUCK!   ");
  scroll_top_row(message1, message2);
  delay(1000);

  //GAMEPLAY
  String s[n];
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
    lcd.print("    " + s[0] + " " + s[1] + " " + s[2] + " " + s[3] + "    ");
    Serial.println("    " + s[0] + " " + s[1] + " " + s[2] + " " + s[3] + "    ");
    delay(d);
    int get_inputs = 0;
    String user_attempt[4];
    while (get_inputs < 4) {
      lcd.clear();
      uint8_t buttons = lcd.readButtons();
      if (buttons) {
        if (button_press == false) {
          if (buttons & BUTTON_UP) {
            user_attempt[get_inputs] = "U";
            get_inputs += 1;
          }
          if (buttons & BUTTON_DOWN) {
            user_attempt[get_inputs] = "D";
            get_inputs += 1;
          }
          if (buttons & BUTTON_LEFT) {
            user_attempt[get_inputs] = "L";
            get_inputs += 1;
          }
          if (buttons & BUTTON_RIGHT) {
            user_attempt[get_inputs] = "R";
            get_inputs += 1;
          }
          button_press = true;
        }
      }
      else {
        button_press = false;
      }
    }
    Serial.println("My GUESS: " + user_attempt[0] + user_attempt[1] + user_attempt[2] + user_attempt[3]);
    if ((user_attempt[0] != s[0]) or (user_attempt[1] != s[1]) or (user_attempt[2] != s[2]) or (user_attempt[3] != s[3])) {
      lcd.setCursor(0, 0);
      lcd.print("WRONG- COPY:" + s[0] + s[1] + s[2] + s[3]);
      lcd.setCursor(0, 1);
      lcd.print("You said: " + user_attempt[0] + user_attempt[1] + user_attempt[2] + user_attempt[3]);
      delay(4000);
      lcd.clear();
      game_over = true;
      mode = 0;
    }
    else {
      lcd.setCursor(0, 0);
      lcd.print("CORRECT!");
      delay(500);
    }
  }
}

void story() {
  Serial.println("Story");
  index = 0;
  String message1 = ("   READ+REPEAT...BE FAST!");
  String message2 = ("  GOOD LUCK!!!  ");
  scroll_top_row(message1, message2);
  delay(1000);

  //GAMEPLAY
  int level = 1;
  int score = 0;
  n = 2;//Length of sequence to start
  String  m[] = {"U", "D", "L", "R"}; //All choices available
  d = 1500; // 1.5 seconds to read
  t = 5000; //5 Seconds to answer
  String s[n];
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
    for (int i = 0; i < n; i++) {
      lcd.print(s[i]);
      Serial.print(s[i]);
    }
    Serial.println(" ");
    lcd.setCursor(0, 1);
    delay(d);
    int get_inputs = 0;
    String user_attempt[n];
    while (get_inputs < n) {
      lcd.clear();
      uint8_t buttons = lcd.readButtons();
      if (buttons) {
        if (button_press == false) {
          if (buttons & BUTTON_UP) {
            user_attempt[get_inputs] = "U";
            get_inputs += 1;
          }
          if (buttons & BUTTON_DOWN) {
            user_attempt[get_inputs] = "D";
            get_inputs += 1;
          }
          if (buttons & BUTTON_LEFT) {
            user_attempt[get_inputs] = "L";
            get_inputs += 1;
          }
          if (buttons & BUTTON_RIGHT) {
            user_attempt[get_inputs] = "R";
            get_inputs += 1;
          }
          button_press = true;
        }
      }
      else {
        button_press = false;
      }
    }
    Serial.print("My GUESS:");
    for (int i = 0; i < n; i++) {
      Serial.print(user_attempt[i]);
      if (user_attempt[i] != s[i]) {
        game_over = true;
        break;
      }
    }
    Serial.println(" ");
    if (game_over) {
      lcd.setBacklight(RED);
      lcd.setCursor(0, 0);
      lcd.print("  COPY:");
      for (int i = 0; i < n; i++) {
        lcd.print(s[i]);
        Serial.print(s[i]);
      }
      Serial.println(" ");
      lcd.setCursor(0, 1);
      lcd.print("Typed: ");
      for (int i = 0; i < n; i++) {
        lcd.print(user_attempt[i]);
        Serial.print(user_attempt[i]);
      }
      Serial.println(" ");
      delay(4000);
      lcd.clear();
      lcd.setBacklight(BLUE);
      lcd.print("SCORE: " + String(score));

      //Update Leaderboard
      int leaderboard_list[20];
      for (int i = 0; i < 20; i++) {
        leaderboard_list[i] = EEPROM.read(i);
      }
      int new_leaderboard[24];
      for (int i = 3; i < 20; i = i + 4) {
        if (score > leaderboard_list[i]) {
          new_leaderboard[i-3]= 68;
          new_leaderboard[i-2]= 73;
          new_leaderboard[i-1]= 75;
          new_leaderboard[i]= score;
          for (int x = i+1; x < 24; x++){
            new_leaderboard[x]= leaderboard_list[x];
          }
          //Write the new leaderboard in
          for (int j = 0; j < 20; j++) {
            EEPROM.update(j, (new_leaderboard[j]));
          }
          break;
        }
        else{//Copy single entry from old leaderboard to new
          new_leaderboard[i-3]= leaderboard_list[i-3];
          new_leaderboard[i-2]= leaderboard_list[i-2];
          new_leaderboard[i-1]= leaderboard_list[i-1];
          new_leaderboard[i]= leaderboard_list[i];
          }
      }
      Serial.println(" ");



      delay(4000);
      lcd.clear();
      mode = 0;
    }
    else {
      level += 1;
      score += 5;
      lcd.setCursor(0, 0);
      lcd.print("    CORRECT!    ");
      lcd.setCursor(0, 1);
      Serial.println(" Level: " + String(level));
      lcd.print("    Level " + String(level));
      delay(1200);

      //INCREASEs DIFFICULTY
      d -= 50; //0.1 less seconds to read
      t -= 50; //0.1 less seconds to answer
      if ((level % 4) == 0) { //Every 4 levels
        n += 1; //Add 1 to sequence
      }
      Serial.println(d);
      Serial.println(t);
      Serial.println(n);
    }
  }
}

void leaderboard() {
  //Only works up to 50 levels as 5x50=250 and
  //I can't store values larger than 255
  int leaderboard_list[20];
  for (int i = 0; i < 20; i++) {
    leaderboard_list[i] = EEPROM.read(i);
  }

  //Buttons for leaderboard
  uint8_t buttons = lcd.readButtons();
  if (buttons) {
    if (buttons & BUTTON_DOWN) {
      leaderboard_state += 1;
      lcd.clear();
      if (leaderboard_state > 5) {
        leaderboard_state = 0;
      }
    }
    if (buttons & BUTTON_UP) {
      leaderboard_state -= 1;
      lcd.clear();
      if (leaderboard_state < 0) {
        leaderboard_state = 5;
      }
    }
    if ((buttons & BUTTON_LEFT) or (buttons & BUTTON_RIGHT)) {
      if (leaderboard_state == 5) {
        lcd.clear();
        mode = 0;
      }
    }
  }
  //Leadeboard menu
  switch (leaderboard_state) {
    case (0)://1st
      lcd.setCursor(0, 0);
      lcd.print("1st: " + String(char(leaderboard_list[0])) + String(char(leaderboard_list[1])) + String(char(leaderboard_list[2])));
      lcd.setCursor(0, 1);
      lcd.print("Score: " + String(leaderboard_list[3]));
      break;
    case (1): //2nd
      lcd.setCursor(0, 0);
      lcd.print("2nd: " + String(char(leaderboard_list[4])) + String(char(leaderboard_list[5])) + String(char(leaderboard_list[6])));
      lcd.setCursor(0, 1);
      lcd.print("Score: " + String(leaderboard_list[7]));
      break;
    case (2)://3rd
      lcd.setCursor(0, 0);
      lcd.print("3rd: " + String(char(leaderboard_list[8])) + String(char(leaderboard_list[9])) + String(char(leaderboard_list[10])));
      lcd.setCursor(0, 1);
      lcd.print("Score: " + String(leaderboard_list[11]));
      break;
    case (3)://4th
      lcd.setCursor(0, 0);
      lcd.print("4th: " + String(char(leaderboard_list[12])) + String(char(leaderboard_list[13])) + String(char(leaderboard_list[14])));
      lcd.setCursor(0, 1);
      lcd.print("Score: " + String(leaderboard_list[15]));
      break;
    case (4)://5th
      lcd.setCursor(0, 0);
      lcd.print("5th: " + String(char(leaderboard_list[16])) + String(char(leaderboard_list[17])) + String(char(leaderboard_list[18])));
      lcd.setCursor(0, 1);
      lcd.print("Score: " + String(leaderboard_list[19]));
      break;
    case (5)://GO BACK
      lcd.setCursor(0, 0);
      lcd.print("     GO BACK");
      lcd.setCursor(0, 1);
      lcd.print("Click left/right");
      break;
  }
}

void settings() {
  Serial.println("Settings");
  //Buttons for settings
  uint8_t buttons = lcd.readButtons();
  if (buttons) {
    if (buttons & BUTTON_DOWN) {
      settings_state += 1;
      lcd.clear();
      if (settings_state > 5) {
        settings_state = 0;
      }
    }
    if (buttons & BUTTON_UP) {
      settings_state -= 1;
      lcd.clear();
      if (settings_state < 0) {
        settings_state = 5;
      }
    }
    if (buttons & BUTTON_RIGHT) {
      if (settings_state == 1) {
        n += 1;
        if (n > 16) {
          n = 16;
        }
      }
      else if (settings_state == 2) {
        n += 1;
        if (n > 4) {
          n = 4;
        }
      }
      else if (settings_state == 3) {
        d += 50;
        if (d > 8000) {
          d = 8000;
        }
      }
      else if (settings_state == 4) {
        t += 50;
        if (t > 8000) {
          t = 8000;
        }
      }
      else if (settings_state == 5) {
        mode = 0;
      }
    }
    if (buttons & BUTTON_LEFT) {
      if (settings_state == 1) {
        n -= 1;
        if (n < 2) {
          n = 2;
        }
      }
      else if (settings_state == 2) {
        n -= 1;
        if (n < 2) {
          n = 2;
        }
      }
      else if (settings_state == 3) {
        d -= 50;
        if (d < 200) {
          d = 200;
        }
      }
      else if (settings_state == 4) {
        t -= 50;
        if (t < 200) {
          t = 200;
        }
      }
      else if (settings_state == 5) {
        mode = 0;
      }
    }
    button_press = true;
  }
  else {
    button_press = false;
  }
  //Settings menu
  switch (settings_state) {
    case (0): //SETTINGS
      lcd.setCursor(0, 0);
      lcd.print("   Settings   ");
      lcd.setCursor(0, 1);
      lcd.print("ALTER PRACTICE");
      break;
    case (1): //Length of S
      lcd.setCursor(0, 0);
      lcd.print("Sequence Length");
      lcd.setCursor(0, 1);
      lcd.print("- <--" + String(n) + "--> +");
      break;
    case (2)://No. Of Choices
      lcd.setCursor(0, 0);
      lcd.print("Choices Length");
      lcd.setCursor(0, 1);
      lcd.print("- <--" + String(n) + "--> +");
      break;
    case (3)://Delay
      lcd.setCursor(0, 0);
      lcd.print("     Delay    ");
      lcd.setCursor(0, 1);
      lcd.print("- <--" + String(d) + "--> +");
      break;
    case (4)://Countdown for user
      lcd.setCursor(0, 0);
      lcd.print("   Countdown  ");
      lcd.setCursor(0, 1);
      lcd.print("- <--" + String(t) + "--> +");
      break;
    case (5)://GO BACK
      lcd.setCursor(0, 0);
      lcd.print("     GO BACK");
      lcd.setCursor(0, 1);
      lcd.print("Click left/right");
      break;
  }
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

void setup_leaderboard() {//Adds example values to view leaderboard as if it's been played before
  char default_leaderboard[] = {'R', 'A', 'J', 80, 'S', 'A', 'M', 60, 'A', 'M', 'Y', 50, 'B', 'E', 'N', 15, 'J', 'O', 'E', 5};
  for (int i = 0; i < 20; i++) {
    EEPROM.update(i, (default_leaderboard[i]));//Change this to write!
  }
}
