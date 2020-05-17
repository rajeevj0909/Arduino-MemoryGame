# Arduino-MemoryGame
Created a memory game where I learnt C++ and the features of an embedded system using an Arduino.

# Instructions

To install, please run program with the setup_leaderboard() function running
inside of the setup function on line 62. Once this has been ran once, 
it can be commented back out again. Enjoy!!!

# Basic Features

* Sequences are displayed on the screen as specified: *YES*
* Backlight flashes (only when correct): *YES*
* Incorrect answer leads to game over: *YES*
* Game supports *N* up to 10: *YES*
* Game supports *M* of size 4: *YES*
* A menu is present: *YES*
* Size of *M* can be changed: *YES*
* *N* can be changed: *YES*
* Message when wrong printed: *YES*

# Additional features

* I have chosen to use delay in some places (and millis() in others) as it 
was appropriate that I didn't want the user to be able to interact with the 
game during the delay.
* I have chosen to make the practice mode endless instead of a single round. 
This is because "practice" takes more than 1 round.
* I have chosen to not allow for difficulty settings in story mode as the 
worth of a "score" is different in each setting and would not be fair to 
users scoring in the leaderboard.
* Handling sequences N that are longer than 10 is present
* A countdown T for inputting the answer is present
* Story mode increases in difficulty
* A full working menu is present
* Selectable difficulty levels present for practice mode
* Custom characters are present (including a 2x2 character- a sad face)
* Suitable displays for winning and losing are present
* A high score table written to the EEPROM is present
* A way for the winner of a high score to enter 3 initials is present
* When the user ends the game in story mode, the settings from the last 
played level is used in practice mode so they can practice that level.
* I have created a function which allows the scrolling of the top line 
while keeping the bottom static whilst also allowing for user interaction.

# Game Storyboard

* Boots into HOME SCREEN: 
	* RIGHT or LEFT for navigation. 
	* SELECT to choose one of the 4 options (Practice, Story, Leaderboard, Settings)
* Practice Mode:
	* Instructions appear
	* Sequence appears
	* User Input
		* Countdown appears
		* User enters either all the asked characters using RIGHT, LEFT, UP, DOWN; decides if it's correct 
		* OR user enters some of the characters, the timer runs out, it's incorrect
	* If Incorrect
		* SAD FACE presented
		* Results presented
		* Returns to menu
	* If Correct
		*Green correct screen
		*Repeats practice mode (without instructions)
* Story Mode:
	* Instructions appear
	* Sequence appears
	* User Input
		* Countdown appears
		* User enters either all the asked characters using RIGHT, LEFT, UP, DOWN; it decides if it's correct 
		* OR user enters some of the characters, the timer runs out, it's incorrect
	* If Incorrect
		* SAD FACE presented
		* Results presented
		* Score presented
		* Checks Leaderboard for top 5 (Reads EEPROM)
			* If Top 5, presents message
			* User can choose username:
			* Letter value changed by LEFT or RIGHT
			* Navigation through UP or DOWN
			* If done, go to done screen and click SELECT
			* Updates EEPROM
		* Returns to menu
	* If Correct
		*Green correct screen with level number
		*Increase difficulty
		*Repeats story mode (without instructions)
* Leaderboard Mode:
	* Reads leaderboard, produces top 5 scores
	* DOWN or UP for navigation. 
	* If done, go to back screen and click SELECT
* Settings Mode:
	* DOWN or UP for navigation
	* LEFT to decrease value, RIGHT to increase value
	* If done, go to back screen and click SELECT
