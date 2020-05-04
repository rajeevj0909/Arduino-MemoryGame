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

//Custom Characters
byte up_arrow[] = { B00100, B01110, B10101, B00100, B00100, B00100, B00100, B00100 };
byte down_arrow[] = { B00100, B00100, B00100, B00100, B00100, B10101, B01110, B00100 };
byte left_arrow[] = { B00000, B00100, B01000, B11111, B11111, B01000, B00100, B00000 };
byte right_arrow[] = { B00000, B00100, B00010, B11111, B11111, B00010, B00100, B00000 };
//4 Quadrants for the sad face; top left, top right, bottom left, bottom right
byte wrong_TL[] = { B11110, B00000, B01101, B01101, B00001, B00001, B00111, B00101 };
byte wrong_TR[] = { B01111, B00000, B10110, B10110, B10000, B10000, B11100, B10100 };
byte wrong_BL[] = { B00000, B00000, B00111, B01000, B10000, B10000, B00000, B00000 };
byte wrong_BR[] = { B00000, B00000, B11100, B00010, B00001, B00001, B00000, B00000 };

//Used for menu interaction
int mode = 0;
int menu_state = 0;
int leaderboard_state = 0;
int settings_state = 0;
bool button_press = false;

//Used for gameplay
byte s[] = {}; //Sequence to remember
int n = 4; //Length of sequence
byte  m[] = {byte(0), byte(1), byte(2), byte(3)};//Choices available
int c = 2; //Length of choices
int d = 1000; // Time it's shown
int t = 5000; //Seconds to answer
bool game_over = false;//Game finished
int username_state = 0; //Letter chosen to edit
int username[] = {65, 65, 65}; //{A,A,A}
long current;//For the countdown

//Used for scroll_top_row function
int index;
String message;
String main_menu = ("L <---Menu---> R");

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.createChar(0, up_arrow);
  lcd.createChar(1, down_arrow);
  lcd.createChar(2, left_arrow);
  lcd.createChar(3, right_arrow);
  lcd.createChar(4, wrong_TL);
  lcd.createChar(5, wrong_TR);
  lcd.createChar(6, wrong_BL);
  lcd.createChar(7, wrong_BR);
  randomSeed(analogRead(0));
  //setup_leaderboard(); //Run this line if it's a new arduino
}

void loop() {
  menu();
}

void menu() {
  //Reset game
  game_over = false;
  //Buttons for menu
  uint8_t buttons = lcd.readButtons(); //Button for home screen
  if (buttons && (mode == 0) && (menu_state == 0)) {
    if (button_press == false) {
      if (buttons & BUTTON_RIGHT) {
        menu_state = 1;
      }
      if (buttons & BUTTON_LEFT) {
        menu_state = 4;
      }
      button_press = true;
    }
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
        lcd.print("L <---HOME---> R");
        break;
      case (1): //Practice
        lcd.setBacklight(RED);
        message = "Practice Mode- Press SELECT or move on";
        scroll_top_row(message, main_menu);
        break;
      case (2)://Story
        lcd.setBacklight(TEAL);
        message = "Story Mode- Press SELECT or move on";
        scroll_top_row(message, main_menu);
        break;
      case (3)://Leaderboard
        lcd.setBacklight(YELLOW);
        message = "Leaderboard- Press SELECT or move on";
        scroll_top_row(message, main_menu);
        break;
      case (4)://Settings
        lcd.setBacklight(BLUE);
        message = "Settings- Press SELECT or move on";
        scroll_top_row(message, main_menu);
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

void practice() { //Instructions
  String message1 = ("READ CODE AND REPEAT IT BACK ON ARROWS");
  String message2 = ("   GOOD LUCK!   ");
  scroll_top_row(message1, message2);
  delay(1000);

  //GAMEPLAY
  byte s[n];
  while (game_over == false) {
    lcd.clear(); //Adds random choices to sequence
    for (int i = 0; i < n; i++) {
      long randNumber = random(300);
      if (randNumber < (300 / c)) {
        s[i] = m[0];
      }
      else if (randNumber < ((300 / c) * 2)) {
        s[i] = m[1];
      }
      else if (randNumber < ((300 / c) * 3)) {
        s[i] = m[2];
      }
      else if (randNumber < ((300 / c) * 4)) {
        s[i] = m[3];
      }
    }//Displays code to remember
    lcd.setCursor(0, 0);
    for (int i = 0; i < n; i++) {
      lcd.write(s[i]);
    }
    delay(d);
    lcd.clear();
    int get_inputs = 0;
    byte user_attempt[n];
    current = millis();
    while (get_inputs < n) {//Allows user to type answer
      uint8_t buttons = lcd.readButtons();
      if (buttons && (mode == 1)) {
        if (button_press == false) {
          if (buttons & BUTTON_UP) {
            user_attempt[get_inputs] = byte(0);
            get_inputs += 1;
          }
          if (buttons & BUTTON_DOWN) {
            user_attempt[get_inputs] = byte(1);
            get_inputs += 1;
          }
          if (buttons & BUTTON_LEFT) {
            user_attempt[get_inputs] = byte(2);
            get_inputs += 1;
          }
          if (buttons & BUTTON_RIGHT) {
            user_attempt[get_inputs] = byte(3);
            get_inputs += 1;
          }
          button_press = true;
        }
      }
      else {
        button_press = false;
      }//Displays time left
      lcd.setCursor(8, 1);
      lcd.print("TIMER: ");
      lcd.setCursor(15, 1);
      lcd.print(((current + t) - millis()) / 1000);
      if ((current + t) < millis()) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("RAN OUT OF TIME");
        delay(1000);
        break;
      }
    }//If the 2 lists don't match, game over
    for (int i = 0; i < n; i++) {
      if (user_attempt[i] != s[i]) {
        game_over = true;
        break;
      }
    }
    if (game_over) {
      lcd.setBacklight(BLUE);
      lcd.clear();//Draw sad face
      lcd.setCursor(7, 0);
      lcd.write(byte(4));
      lcd.write(byte(5));
      lcd.setCursor(7, 1);
      lcd.write(byte(6));
      lcd.write(byte(7));
      delay(2000);
      lcd.clear();//Show results
      lcd.setCursor(0, 0);
      lcd.print(" Copy:");
      for (int i = 0; i < n; i++) {
        lcd.write(s[i]);
      }
      lcd.setCursor(0, 1);
      lcd.print("Typed:");
      for (int i = 0; i < n; i++) {
        if ((user_attempt[i] == byte(0)) or (user_attempt[i] == byte(1)) or (user_attempt[i] == byte(2)) or (user_attempt[i] == byte(3))) {
          lcd.write(user_attempt[i]);
        }//Else statement ends for loop when it detects no more characters in list
        else {
          break;
        }
      }
      delay(4000);
      lcd.setBacklight(RED);
      lcd.clear();
      mode = 0;
    }
    else {//CORRECT ANSWER
      lcd.setBacklight(GREEN);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("    CORRECT!    ");
      delay(500);
      lcd.setBacklight(RED);
    }
  }
}

void story() {
  String message1 = ("READ+REPEAT...BE FAST!");
  String message2 = ("  GOOD LUCK!!!  ");
  scroll_top_row(message1, message2);
  delay(1000);

  //GAMEPLAY
  int level = 1;
  int score = 0;
  n = 2;//Length of sequence to start
  d = 1500; // 1.5 seconds to read
  t = 5100; //5.1 Seconds to answer
  while (game_over == false) {
    lcd.clear();// Story mode always has 4 choices
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
    }//Displays message to remember
    lcd.setCursor(0, 0);
    for (int i = 0; i < n; i++) {
      lcd.write(s[i]);
    }
    delay(d);
    lcd.clear();
    int get_inputs = 0;
    byte user_attempt[n];
    current = millis();
    while (get_inputs < n) {
      uint8_t buttons = lcd.readButtons();
      if (buttons && (mode == 2)) {
        if (button_press == false) {
          if (buttons & BUTTON_UP) {
            user_attempt[get_inputs] = byte(0);
            get_inputs += 1;
          }
          if (buttons & BUTTON_DOWN) {
            user_attempt[get_inputs] = byte(1);
            get_inputs += 1;
          }
          if (buttons & BUTTON_LEFT) {
            user_attempt[get_inputs] = byte(2);
            get_inputs += 1;
          }
          if (buttons & BUTTON_RIGHT) {
            user_attempt[get_inputs] = byte(3);
            get_inputs += 1;
          }
          button_press = true;
        }
      }
      else {
        button_press = false;
      }//Displays time left
      lcd.setCursor(8, 1);
      lcd.print("TIMER: ");
      lcd.setCursor(15, 1);
      lcd.print(((current + t) - millis()) / 1000);
      if ((current + t) < millis()) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("RAN OUT OF TIME");
        delay(1000);
        break;
      }
    }
    for (int i = 0; i < n; i++) {
      if (user_attempt[i] != s[i]) {
        game_over = true;
        break;
      }
    }
    if (game_over) {
      lcd.setBacklight(RED);
      lcd.clear();//Draw sad face
      lcd.setCursor(7, 0);
      lcd.write(byte(4));
      lcd.write(byte(5));
      lcd.setCursor(7, 1);
      lcd.write(byte(6));
      lcd.write(byte(7));
      delay(2000);
      lcd.clear();//Show result
      lcd.setCursor(0, 0);
      lcd.print(" Copy:");
      for (int i = 0; i < n; i++) {
        lcd.write(s[i]);
      }
      lcd.setCursor(0, 1);
      lcd.print("Typed:");
      for (int i = 0; i < n; i++) {
        if ((user_attempt[i] == byte(0)) or (user_attempt[i] == byte(1)) or (user_attempt[i] == byte(2)) or (user_attempt[i] == byte(3))) {
          lcd.write(user_attempt[i]);
        }
        else {
          break;
        }
      }//Shows the score
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
      }//Creates a temporary leaderboard for 6 people
      int new_leaderboard[24];
      for (int i = 3; i < 20; i = i + 4) {
        if (score > leaderboard_list[i]) {//When the correct placement is found
          lcd.setCursor(0, 1);
          lcd.print("TOP 5 SCORE!!!");
          delay(1500);//Allows user to insert name
          username_state = 0;
          while (username_state < 5) {
            uint8_t buttons = lcd.readButtons();
            if (buttons && (mode == 2)) {
              if (button_press == false) {
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
                  if (username_state == (-1)) {
                    username_state = 3;
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
                    username[1] -= 1;
                    if (username[1] < 65) {
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
            }
            else {
              button_press = false;
            }
            switch (username_state) {
              case (0)://First letter
                lcd.setCursor(0, 0);
                lcd.print("User Letter1");
                lcd.setCursor(0, 1);
                lcd.print("- <--" + String(char(username[0])) + "--> +");
                lcd.setCursor(15, 0);//Add navigation arrows
                lcd.write(byte(0));
                lcd.setCursor(15, 1);
                lcd.write(byte(1));
                break;
              case (1)://Second letter
                lcd.setCursor(0, 0);
                lcd.print("User Letter2");
                lcd.setCursor(0, 1);
                lcd.print("- <--" + String(char(username[1])) + "--> +");
                lcd.setCursor(15, 0);//Add navigation arrows
                lcd.write(byte(0));
                lcd.setCursor(15, 1);
                lcd.write(byte(1));
                break;
              case (2)://Third letter
                lcd.setCursor(0, 0);
                lcd.print("User Letter3");
                lcd.setCursor(0, 1);
                lcd.print("- <--" + String(char(username[2])) + "--> +");
                lcd.setCursor(15, 0);//Add navigation arrows
                lcd.write(byte(0));
                lcd.setCursor(15, 1);
                lcd.write(byte(1));
                break;
              case (3)://Save answer
                lcd.setCursor(0, 0);
                lcd.print("    DONE?");
                lcd.setCursor(0, 1);
                lcd.print("Click left/right");
                break;
            }
          }//Insert old members back into leaderboard
          new_leaderboard[i - 3] = username[0];
          new_leaderboard[i - 2] = username[1];
          new_leaderboard[i - 1] = username[2];
          new_leaderboard[i] = score;
          for (int x = i - 3; x < 24; x++) {
            new_leaderboard[x + 4] = leaderboard_list[x];
          }
          //Write the new leaderboard in memory
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
      mode = 0;//Go home
    }
    else {//Correct answer
      level += 1;
      score += 5;
      lcd.clear();
      lcd.setBacklight(GREEN);
      lcd.setCursor(0, 0);
      lcd.print("    CORRECT!    ");
      lcd.setCursor(0, 1);
      lcd.print("    Level " + String(level));
      delay(1200);
      lcd.setBacklight(BLUE);

      //INCREASES DIFFICULTY
      d -= 50; //Less seconds to read
      t -= 50; //Less seconds to answer
      if ((level % 4) == 0) { //Every 4 levels
        n += 1; //Add 1 to sequence
      }
    }
  }
}

void leaderboard() {
  //Only works up to 50 levels as 5x50=250 and
  //I can't store score values larger than 255
  int leaderboard_list[20];//Stores the 5 players info
  for (int i = 0; i < 20; i++) {
    leaderboard_list[i] = EEPROM.read(i);
  }
  //Buttons for leaderboard
  uint8_t buttons = lcd.readButtons();
  if (buttons && (mode == 3)) {
    if (button_press == false) {
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
      button_press = true;
    }
  }
  else {
    button_press = false;
  }
  //Leadeboard menu
  switch (leaderboard_state) {
    case (0)://1st
      lcd.setCursor(0, 0); //Prints the 3-letter name
      lcd.print("1st: " + String(char(leaderboard_list[0])) + String(char(leaderboard_list[1])) + String(char(leaderboard_list[2])));
      lcd.setCursor(0, 1); //Prints the score they got
      lcd.print("Score: " + String(leaderboard_list[3]));
      lcd.setCursor(15, 0);//Add navigation arrows
      lcd.write(byte(0));
      lcd.setCursor(15, 1);
      lcd.write(byte(1));
      break;
    case (1): //2nd
      lcd.setCursor(0, 0);
      lcd.print("2nd: " + String(char(leaderboard_list[4])) + String(char(leaderboard_list[5])) + String(char(leaderboard_list[6])));
      lcd.setCursor(0, 1);
      lcd.print("Score: " + String(leaderboard_list[7]));
      lcd.setCursor(15, 0);//Add navigation arrows
      lcd.write(byte(0));
      lcd.setCursor(15, 1);
      lcd.write(byte(1));
      break;
    case (2)://3rd
      lcd.setCursor(0, 0);
      lcd.print("3rd: " + String(char(leaderboard_list[8])) + String(char(leaderboard_list[9])) + String(char(leaderboard_list[10])));
      lcd.setCursor(0, 1);
      lcd.print("Score: " + String(leaderboard_list[11]));
      lcd.setCursor(15, 0);//Add navigation arrows
      lcd.write(byte(0));
      lcd.setCursor(15, 1);
      lcd.write(byte(1));
      break;
    case (3)://4th
      lcd.setCursor(0, 0);
      lcd.print("4th: " + String(char(leaderboard_list[12])) + String(char(leaderboard_list[13])) + String(char(leaderboard_list[14])));
      lcd.setCursor(0, 1);
      lcd.print("Score: " + String(leaderboard_list[15]));
      lcd.setCursor(15, 0);//Add navigation arrows
      lcd.write(byte(0));
      lcd.setCursor(15, 1);
      lcd.write(byte(1));
      break;
    case (4)://5th
      lcd.setCursor(0, 0);
      lcd.print("5th: " + String(char(leaderboard_list[16])) + String(char(leaderboard_list[17])) + String(char(leaderboard_list[18])));
      lcd.setCursor(0, 1);
      lcd.print("Score: " + String(leaderboard_list[19]));
      lcd.setCursor(15, 0);//Add navigation arrows
      lcd.write(byte(0));
      lcd.setCursor(15, 1);
      lcd.write(byte(1));
      break;
    case (5)://GO BACK
      lcd.setCursor(0, 0);
      lcd.print(" TO GO BACK");
      lcd.setCursor(0, 1);
      lcd.print("Click left/right");
      lcd.setCursor(15, 0);//Add navigation arrow
      lcd.write(byte(0));
      break;
  }
}

void settings() {
  //Includes a lot of validation for values changing
  uint8_t buttons = lcd.readButtons();
  if (buttons && (mode == 4)) {
    if (button_press == false) {
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
  }
  else {
    button_press = false;
  }
  //Viewing the menu
  switch (settings_state) {
    case (0): //SETTINGS
      lcd.setCursor(0, 0);
      lcd.print("   Settings");
      lcd.setCursor(0, 1);
      lcd.print("ALTER PRACTICE");
      lcd.setCursor(15, 0);//Add navigation arrows
      lcd.write(byte(0));
      lcd.setCursor(15, 1);
      lcd.write(byte(1));
      break;
    case (1): //Length of S
      lcd.setCursor(0, 0);
      lcd.print("Sequence Length");
      lcd.setCursor(0, 1);
      lcd.print("- <---" + String(n) + "---> +");
      lcd.setCursor(15, 0);//Add navigation arrows
      lcd.write(byte(0));
      lcd.setCursor(15, 1);
      lcd.write(byte(1));
      break;
    case (2)://No. Of Choices
      lcd.setCursor(0, 0);
      lcd.print("Choices Length");
      lcd.setCursor(0, 1);
      lcd.print("- <---" + String(c) + "---> + ");
      lcd.setCursor(15, 0);//Add navigation arrows
      lcd.write(byte(0));
      lcd.setCursor(15, 1);
      lcd.write(byte(1));
      break;
    case (3)://Delay to view in milliseconds
      lcd.setCursor(0, 0);
      lcd.print("     Delay    ");
      lcd.setCursor(0, 1);
      lcd.print("- <--" + String(d) + "--> + ");
      lcd.setCursor(15, 0);//Add navigation arrows
      lcd.write(byte(0));
      lcd.setCursor(15, 1);
      lcd.write(byte(1));
      break;
    case (4)://Countdown for user in milliseconds
      lcd.setCursor(0, 0);
      lcd.print("   Countdown  ");
      lcd.setCursor(0, 1);
      lcd.print("- <--" + String(t) + "--> + ");
      lcd.setCursor(15, 0);//Add navigation arrows
      lcd.write(byte(0));
      lcd.setCursor(15, 1);
      lcd.write(byte(1));
      break;
    case (5)://GO BACK
      lcd.setCursor(0, 0);
      lcd.print(" TO GO BACK");
      lcd.setCursor(0, 1);
      lcd.print("Click left/right");
      lcd.setCursor(15, 0);//Add navigation arrow
      lcd.write(byte(0));
      break;
  }
}
/*This function scrolls text on the top row
  while keeping the bottom row static*/
void scroll_top_row(String top, String bottom) {
  top = "   " + top + " ";
  lcd.setCursor(0, 1);
  lcd.print(bottom);
  lcd.setCursor(0, 0);
  index = 0;
  while (index < top.length()) {
    index += 1;
    lcd.setCursor(0, 1);
    lcd.print(bottom);
    lcd.setCursor(0, 0);
    for (int j = index; j < top.length(); j++) {
      lcd.print(top.charAt(j));
      //CHECK BUTTONS
      uint8_t buttons = lcd.readButtons();
      if (buttons && (mode == 0) && (menu_state != 0)) {
        if (button_press == false) {
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
              leaderboard_state = 0;
            }
            else if (menu_state == 4) { //Settings
              mode = 4; //settings_state
              settings_state = 0;
            }
          }
          button_press = true;
          return;
        }
      }
      else {
        button_press = false;
      }
    }
  }
}

//Run this function when setting up a new device
void setup_leaderboard() {//Adds example values to view leaderboard if it's not been played before
  char default_leaderboard[] = {'R', 'A', 'J', 80, 'S', 'A', 'M', 60, 'A', 'M', 'Y', 50, 'B', 'E', 'N', 15, 'D', 'A', 'N', 5};
  for (int i = 0; i < 20; i++) {
    EEPROM.write(i, (default_leaderboard[i]));
  }
}
