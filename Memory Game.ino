#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
#include <EEPROM.h>
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

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
int c = 2; //Length of choices
int d = 1000; // Time it's shown
int t = 5000; //Seconds to answer
bool game_over = false;//Game finished
int username_state = 0; //Letter chosen to edit
int username[] = {65, 65, 65}; //{A,A,A}

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
      if (menu_state > 4) {
        menu_state = 0;
      }
    }
    if (buttons & BUTTON_LEFT) {
      menu_state -= 1;
      if (menu_state == (-1)) {
        menu_state = 4;
      }
    }
    if (buttons & BUTTON_SELECT) {
      lcd.clear();
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
          lcd.print(" Practice Mode  ");
          reload_scroll = true;
        }
        break;
      case (2)://Story
        if (reload_scroll == false) {
          lcd.setBacklight(TEAL);
          message = "Story Mode- Press SELECT or move on";
          scroll_top_row(message, main_menu);
          lcd.print("   Story Mode   ");
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
      Serial.println(randNumber);
      if (randNumber < (300/c)) {
        s[i] = m[0];
      }
      else if (randNumber < ((300/c)*2)) {
        s[i] = m[1];
      }
      else if (randNumber < ((300/c)*3)) {
        s[i] = m[2];
      }
      else if (randNumber < ((300/c)*4)) {
        s[i] = m[3];
      }
    }
    lcd.setCursor(0, 0);
    for (int i = 0; i < n; i++) {
      lcd.print(s[i]);
      Serial.print(s[i]);
    }
    Serial.println(" ");
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
      lcd.setBacklight(BLUE);
      lcd.setCursor(0, 0);
      lcd.print(" COPY:");
      for (int i = 0; i < n; i++) {
        lcd.print(s[i]);
        Serial.print(s[i]);
      }
      Serial.println(" ");
      lcd.setCursor(0, 1);
      lcd.print("Typed:");
      for (int i = 0; i < n; i++) {
        lcd.print(user_attempt[i]);
        Serial.print(user_attempt[i]);
      }
      Serial.println(" ");
      delay(4000);
      lcd.setBacklight(RED);
      lcd.clear();
      mode = 0;
    }
    else {
      lcd.setBacklight(GREEN);
      lcd.setCursor(0, 0);
      lcd.print("CORRECT!");
      delay(500);
      lcd.setBacklight(RED);
    }
  }
}

void story() {
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
      lcd.print(" COPY:");
      for (int i = 0; i < n; i++) {
        lcd.print(s[i]);
        Serial.print(s[i]);
      }
      Serial.println(" ");
      lcd.setCursor(0, 1);
      lcd.print("Typed:");
      for (int i = 0; i < n; i++) {
        lcd.print(user_attempt[i]);
        Serial.print(user_attempt[i]);
      }
      Serial.println(" ");
      delay(4000);
      lcd.clear();
      lcd.setBacklight(BLUE);
      lcd.setCursor(0, 0);
      lcd.print("SCORE: " + String(score));
      delay(1500);

      //Update Leaderboard
      int leaderboard_list[20];
      for (int i = 0; i < 20; i++) {
        leaderboard_list[i] = EEPROM.read(i);
      }
      int new_leaderboard[24];
      for (int i = 3; i < 20; i = i + 4) {
        if (score > leaderboard_list[i]) {
          lcd.setCursor(0, 1);
          lcd.print("TOP 5 SCORE!!!");
          delay(1500);
          username_state = 0;
          while (username_state < 5) {
            uint8_t buttons = lcd.readButtons();
            if (buttons) {
              if (buttons & BUTTON_DOWN) {
                username_state += 1;
                lcd.clear();
                if (username_state == 4) {
                  username_state = 0;
                }
              }
              if (buttons & BUTTON_UP) {
                username_state -= 1;
                lcd.clear();
                if (username_state == 0) {
                  username_state = 4;
                }
              }
              if (buttons & BUTTON_RIGHT) {
                lcd.clear();
                if (username_state == 0) {
                  username[0] += 1;
                  if (username[0] > 90) {
                    username[0] = 65;
                  }
                }
                else if (username_state == 1) {
                  username[1] += 1;
                  if (username[1] > 90) {
                    username[1] = 65;
                  }
                }
                else if (username_state == 2) {
                  username[2] += 1;
                  if (username[2] > 90) {
                    username[2] = 65;
                  }
                }
                else if (username_state == 3) {
                  username_state = 5;
                }
              }
              if (buttons & BUTTON_LEFT) {
                lcd.clear();
                if (username_state == 0) {
                  username[0] -= 1;
                  if (username[0] < 65) {
                    username[0] = 90;
                  }
                }
                else if (username_state == 1) {
                  username[1] += 1;
                  if (username[1] > 65) {
                    username[1] = 90;
                  }
                }
                else if (username_state == 2) {
                  username[2] -= 1;
                  if (username[2] < 65) {
                    username[2] = 90;
                  }
                }
                else if (username_state == 3) {
                  username_state = 5;
                }
              }
              button_press = true;
            }
            else {
              button_press = false;
            }

            switch (username_state) {
              case (0):
                lcd.setCursor(0, 0);
                lcd.print("Username Letter1");
                lcd.setCursor(0, 1);
                lcd.print("- <--" + String(char(username[0])) + "--> +");
                break;
              case (1):
                lcd.setCursor(0, 0);
                lcd.print("Username Letter2");
                lcd.setCursor(0, 1);
                lcd.print("- <--" + String(char(username[1])) + "--> +");
                break;
              case (2):
                lcd.setCursor(0, 0);
                lcd.print("Username Letter3");
                lcd.setCursor(0, 1);
                lcd.print("- <--" + String(char(username[2])) + "--> +");
                break;
              case (3)://GO BACK
                lcd.setCursor(0, 0);
                lcd.print("    DONE?");
                lcd.setCursor(0, 1);
                lcd.print("Click left/right");
                break;
            }
          }
          new_leaderboard[i - 3] = username[0];
          new_leaderboard[i - 2] = username[1];
          new_leaderboard[i - 1] = username[2];
          new_leaderboard[i] = score;
          for (int x = i - 3; x < 24; x++) {
            new_leaderboard[x + 4] = leaderboard_list[x];
          }
          //Write the new leaderboard in
          for (int j = 0; j < 20; j++) {
            EEPROM.update(j, (new_leaderboard[j]));
          }
          break;
        }
        else { //Copy single entry from old leaderboard to new
          new_leaderboard[i - 3] = leaderboard_list[i - 3];
          new_leaderboard[i - 2] = leaderboard_list[i - 2];
          new_leaderboard[i - 1] = leaderboard_list[i - 1];
          new_leaderboard[i] = leaderboard_list[i];
        }
      }
      lcd.clear();
      mode = 0;
    }
    else {
      lcd.setBacklight(GREEN);
      level += 1;
      score += 5;
      lcd.setCursor(0, 0);
      lcd.print("    CORRECT!    ");
      lcd.setCursor(0, 1);
      lcd.print("    Level " + String(level));
      delay(1200);
      lcd.setBacklight(BLUE);

      //INCREASES DIFFICULTY
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
      lcd.print("    GO BACK");
      lcd.setCursor(0, 1);
      lcd.print("Click left/right");
      break;
  }
}

void settings() {
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
        c += 1;
        if (c > 4) {
          c = 4;
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
        c -= 1;
        if (c < 2) {
          c = 2;
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
      lcd.print("- <--" + String(c) + "--> +");
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
      lcd.print("    GO BACK");
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

void setup_leaderboard() {//Adds example values to view leaderboard if it's not been played before
  char default_leaderboard[] = {'R', 'A', 'J', 80, 'S', 'A', 'M', 60, 'A', 'M', 'Y', 50, 'B', 'E', 'N', 15, 'J', 'O', 'E', 5};
  for (int i = 0; i < 20; i++) {
    EEPROM.update(i, (default_leaderboard[i]));//Change this to write!
  }
}
