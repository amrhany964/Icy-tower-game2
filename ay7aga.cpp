#include <Icy_tower_SP\ay7aga\include\SFML\Graphics>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <Windows.h> // included in order to apply the window Maximize
#include <iostream>
#include<cstdlib>
using namespace std;
using namespace sf;

struct Character {
	Sprite sprite;
	Texture tx;
	int jump_power = 13.7f;
};

struct Platfrom {
	Sprite sprite;
	Texture tx;
} platform[1000];

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;
const float SCALE_CONSTANT = 1.5238F;

double xforwindow = 1920.0;  // Window Dimensions
double yforwindow = 1080.0;

double xformenu = 500.0; // Main Menu Photo
double yformenu = 393.0;

double yforicon = 400;  // The First Icon
double xforicon = 333;
bool onlyone = 0;
int opened_game_sound = 0;
int opened_menu_sound = 0;

void endless_run(Sprite lwall[], Sprite rwall[], Sprite bg[], float);
bool is_touching(RectangleShape, Sprite);
void main_menu_generator(Text arr[], Sprite&, Sprite&, Font&, Texture&, Texture&);
void pause_menu_generator(Text arr[], Font&);
void game_over_generator(Text arr[], Font&);
void options_generator(Text arr[], Font&);
void main_menu_logic(Text arr[], RenderWindow&, Music&, Music&, Music&, int&, int&, bool&, bool&, bool&, bool&, bool&, bool&);
void options_logic(Text arr[], Music&, Music&, Music&, int&, bool&, bool&);
void lose_logic(Text high, Text over[], Text&, RenderWindow&, Sprite&, Music&, Music&, Music&, int&, bool&, bool&, bool&, bool&, bool&, bool&);
void pause_logic(Text buttons[], Music&, Music&, int&, int&, bool&, bool&, bool&, bool&, bool&);
void game_reset(RenderWindow&, Text&, Texture&, Texture&, Texture&, Texture&, Texture&, Sprite lwall[], Sprite rwall[], Sprite bg[], Sprite&, Character&, Platfrom plat[], RectangleShape&, int&, int&, int&, int&, int&, bool&, bool&);
void player_movement(Character&, int&, float&, float&, int&, int&, bool&, bool&, bool&, bool&, Music&);

int main() {
	srand(static_cast<unsigned>(time(NULL)));

	RenderWindow window(VideoMode(xforwindow, yforwindow, Style::Fullscreen), "ICY TOWER");  //create main window
	::ShowWindow(window.getSystemHandle(), SW_MAXIMIZE); // Maximize the RenderWindow to take the full screen
	window.setFramerateLimit(6);

	// Main Menu
	Text write[6];  // 5 instructions   // 6 exit from inst
	int keyboardclick = -1; // Move Between Icons
	bool play = false;
	bool inst = false; //open instructions frame
	bool back = false; // color for back
	Sprite mainmenu;
	Sprite instructions;
	Font font; //icons
	Texture menutexture; //upload main menu
	Texture insttexture; // upload background for instructions
	main_menu_generator(write, mainmenu, instructions, font, menutexture, insttexture);

	// Pause Menu
	Text buttons[3];
	int selected = 0;
	Font pauseFont;
	pause_menu_generator(buttons, pauseFont);

	// All the Sprite and Variables that relates to the Main Game Logic
	int delay = 0, fast_cnt = 0; // Edit Frames

	Sprite left_wall[4], right_wall[4], background[3];
	Texture tx_lwall, tx_rwall, tx_bg;
	int index = 0;

	int x = 0; // Control Sprite Animation Frames
	float world_speed = 0; // Control the Speed of Walls, Background and Tiles
	float velocity_y = 0; // Control the Jump of the Character (Based on Projectile Motion)

	Texture start_ground_texture;
	Sprite ground;
	Character icys;

	// Game Setup
	bool is_grounded = false, is_jumping = false;

	bool firstjump = 0; // Adjust The Tiles to Start Getting Down When Player Start Jumping of Ground
	int counter = 0; // Detect if the Character is Grounded or not
	icys.jump_power = 13.7f;

	Texture platformtext; // Upload Platforms (Tiles)
	int y = 500; // Start Of The First Tile
	int x1 = 150;

	RectangleShape rectangle;

	bool isPaused = false;   //pause menu
	bool reset_the_game = false;

	int score = 0, highscore = 0;
	bool islose = false;
	bool vis[1000] = { 0 };
	bool soundgame = true;
	bool options = false;

	// Game Over
	Text over[3];
	int buttonlose = 0;
	game_over_generator(over, font);

	// Options
	Text optionsfont[3];
	int soundbutton = 0;
	options_generator(optionsfont, font);

	//// changing items sound /////
	Music menu_change;
	menu_change.openFromFile("menu_change.ogg");

	////// choose menu ////
	Music menu_choose;
	menu_choose.openFromFile("menu_choose.ogg");

	////// sound menu ////
	Music menu_sound;
	menu_sound.openFromFile("ori music.ogg");
	menu_sound.setLoop(true);

	////// sound game ////
	Music game_sound;
	game_sound.openFromFile("gameplay sound.ogg");
	game_sound.setLoop(true);

	////// gameover sound ////
	Music died;
	died.openFromFile("gameover.ogg");

	////// jump sound ////
	Music jump_sound;
	jump_sound.openFromFile("jump.ogg");

	// Score Counter
	Text text;

	while (window.isOpen()) {
		Event event;
		while (window.pollEvent(event))
			if (event.type == Event::Closed) window.close();

		// Main Menu
		if (!play && !options)
			main_menu_logic(write, window, menu_sound, menu_choose, menu_change, soundbutton, keyboardclick, inst, play, options, reset_the_game, soundgame, back);

		// Options
		if (options) {
			options_logic(optionsfont, menu_sound, menu_choose, menu_change, soundbutton, soundgame, options);
		}

		if (!play) {
			if (soundgame and opened_menu_sound == 0) {
				opened_menu_sound = 1;
				menu_sound.play();
			}
		}
		////////////////////////////////////////////////////////////////////////
		/////////////////////////// GamePlay Logic /////////////////////////////
		////////////////////////////////////////////////////////////////////////
		if (play) {
			if (opened_menu_sound == 1) {
				menu_sound.stop();
			}
			if (soundgame and opened_game_sound == 0) {
				opened_game_sound = 1;
				game_sound.play();
			}
			if (soundgame and opened_game_sound == 2) {
				opened_game_sound = 1;
				game_sound.play();
			}
			text.setString("Score : " + to_string(score)); // Score
			text.setFont(font);
			text.setFillColor(Color(255, 255, 255));
			text.setCharacterSize(100);

			Text high;

			high.setString("high score : " + to_string(highscore)); // Score
			high.setFont(font);
			high.setFillColor(Color(255, 255, 255));
			high.setCharacterSize(100);

			if (score >= highscore) highscore = score;
			for (int i = 0; i < 1000; i++) {
				if (platform[i].sprite.getPosition().y >= 1080 && vis[i] == false) {
					score += 3;
					vis[i] = true;
				}
			}
			if (score % 11 == 10) {
				world_speed += .002f;
			}
			if (reset_the_game) {
				keyboardclick = -1;
				score = 0;
				for (int i = 0; i < 1000; i++)
					vis[i] = 0;
				delay = 0;
				fast_cnt = 0;
				index = 0;
				x = 0;
				world_speed = 0;
				velocity_y = 0;
				is_grounded = false;
				is_jumping = false;
				firstjump = 0;
				counter = 0;
				y = 420;
				x1 = 650;

				game_reset(window, text, tx_lwall, tx_rwall, tx_bg, start_ground_texture, platformtext, left_wall, right_wall, background, ground, icys, platform, rectangle, index, x1, y, delay, fast_cnt, firstjump, reset_the_game);
			}

			window.setFramerateLimit(90);
			// GameOver Checker
			if (icys.sprite.getPosition().y > WINDOW_HEIGHT) {
				islose = true;
			}
			// Pause Logic //
			if (!isPaused && Keyboard::isKeyPressed(Keyboard::Escape)) {
				if (soundgame) {
					menu_choose.play();
				}
				isPaused = true;
			}

			if (isPaused) {
				window.setFramerateLimit(6);
				// Pause Logic
				if (soundgame and opened_game_sound == 1) {
					game_sound.pause();
					opened_game_sound = 2;
				}
				pause_logic(buttons, menu_choose, menu_change, soundbutton, selected, soundgame, isPaused, options, play, inst);

				if (options)
					options_logic(optionsfont, menu_sound, menu_choose, menu_change, soundbutton, soundgame, options);
			}

			// The GamePlay
			if (!options && !islose && !isPaused) {
				// The Game
				rectangle.setPosition(icys.sprite.getPosition().x + 10.f, icys.sprite.getPosition().y + icys.sprite.getGlobalBounds().height);
				icys.sprite.setTextureRect(IntRect(0, 0, 42.5, 49)); // Reset Player Texture Rectangle

				// Check if the Player is Collide with Tiles
				for (int w = 0; w < 1000; w++)
					if (is_touching(rectangle, platform[w].sprite) && velocity_y >= 0) { /////////
						is_grounded = true;
						is_jumping = false;
						delay++;
						counter = 0;

						if (firstjump == false) {
							firstjump = true;
							world_speed = 2.65f;
							icys.jump_power = 13.7f;
						}

						// Make the Character on the Top of the Tile
						icys.sprite.setPosition(icys.sprite.getPosition().x, platform[w].sprite.getPosition().y - icys.sprite.getGlobalBounds().height);
						// Handle the Jump Glitch
						icys.sprite.move(0, world_speed + 2.f);
					}
					else counter++;

				// Check if the Player Collide with the Ground
				if (is_touching(rectangle, ground)) {
					counter = 0;
					is_grounded = true;
					is_jumping = false;
					delay++;

					// Make the Player on the Top of the Ground
					icys.sprite.setPosition(icys.sprite.getPosition().x, ground.getPosition().y - icys.sprite.getGlobalBounds().height);
					icys.sprite.move(0, world_speed + 2.f);
				}
				else counter++;

				// if the Player Doesn't Touch Any Tile or Ground
				// Then Him Not Grounded
				if (counter > 1001) {
					is_grounded = false;
				}

				if (icys.sprite.getPosition().y <= 0)
					velocity_y = 0.35f;

				// Player Movement
				player_movement(icys, x, velocity_y, world_speed, delay, fast_cnt, is_grounded, is_jumping, firstjump, soundgame, jump_sound);

				// Make the World Speed faster if the Character Jumps
				if (is_jumping && firstjump && fast_cnt < 20) {
					for (int i = 0; i < 1000; i++) {
						platform[i].sprite.move(0, world_speed + .0000002f); // Increase the Difficulty of the Game
					}

					endless_run(left_wall, right_wall, background, world_speed + 1.5f);
					fast_cnt++;
				}

				// Make the Tiles Move Down When the Player Jump for the First Time
				if (firstjump) {
					for (int j = 0; j < 1000; j++) {
						platform[j].sprite.move(0, world_speed + 0.45f);

						if (platform[j].sprite.getPosition().y > WINDOW_HEIGHT)
							platform[j].sprite.setScale(0, 0);
					}

					ground.move(0, world_speed);
					if (ground.getPosition().y > WINDOW_HEIGHT)
						ground.setScale(0, 0);
				}

				// The Function that Contains the Logic of Walls and Background
				endless_run(left_wall, right_wall, background, world_speed);
			}

			window.clear(Color::Black);
			// Draw Walls and Background
			for (int l = 0; l < 3; l++)
				window.draw(background[l]);
			for (int z = 0; z < 4; z++) {
				window.draw(left_wall[z]);
				window.draw(right_wall[z]);
			}

			// Draw the Main Character(icys), the initial Ground and the Tiles
			window.draw(icys.sprite);
			window.draw(ground);
			for (int k = 0; k < 1000; k++)
				window.draw(platform[k].sprite);
			window.draw(text);
			high.setPosition(Vector2f(0, 100));
			if (score == highscore) {
				high.setFillColor(Color::Red);
			}
			window.draw(high);

			// GameOver (Lose)
			if (islose) {
				if (soundgame and opened_game_sound == 1) {
					game_sound.stop();
				}
				lose_logic(high, over, text, window, instructions, died, menu_choose, menu_change, buttonlose, soundgame, isPaused, islose, reset_the_game, play, inst);
			}

			//////////////////////////////////////////////////////

			if (isPaused && !options) {
				window.draw(instructions);
				for (int i = 0; i < 3; ++i)
					window.draw(buttons[i]);
			}

			if (options && isPaused) {
				window.clear();
				window.draw(instructions);
				window.draw(optionsfont[0]);
				window.draw(optionsfont[1]);
				window.draw(optionsfont[2]);
			}

			window.display();
		}
		else if (inst) {
			window.clear();
			window.draw(instructions);
			window.draw(write[4]);
			window.draw(write[5]);
			window.display();
		}
		else if (options) {
			window.clear();
			window.draw(instructions);
			window.draw(optionsfont[0]);
			window.draw(optionsfont[1]);
			window.draw(optionsfont[2]);
			window.display();
		}
		else {
			window.clear();
			window.draw(mainmenu);
			window.draw(write[0]);
			window.draw(write[1]);
			window.draw(write[2]);
			window.draw(write[3]);
			window.display();
		}
	}
}

void endless_run(Sprite lwall[], Sprite rwall[], Sprite bg[], float speed = 0.0f) { //Move Wall and Background
	for (int i = 0; i < 4; i++) {
		lwall[i].move(0, speed);
		rwall[i].move(0, speed);
		if (i < 3)
			bg[i].move(0, speed);

		if (lwall[i].getPosition().y >= SCALE_CONSTANT * 720) {
			lwall[i].setPosition(0, lwall[((i - 1 == -1) ? 3 : (i - 1))].getPosition().y - (SCALE_CONSTANT * 360 - speed));
		}

		if (rwall[i].getPosition().y >= SCALE_CONSTANT * 720) {
			rwall[i].setPosition(rwall[0].getPosition().x, rwall[((i - 1 == -1) ? 3 : (i - 1))].getPosition().y - (SCALE_CONSTANT * 360 - speed));
		}

		if (bg[i].getPosition().y >= SCALE_CONSTANT * 720) {
			bg[i].setPosition(lwall[0].getGlobalBounds().width - SCALE_CONSTANT * 5, bg[((i - 1 == -1) ? 2 : (i - 1))].getPosition().y - (SCALE_CONSTANT * 720 - speed));
		}
	}
}

bool is_touching(RectangleShape rect, Sprite ground) {
	return rect.getGlobalBounds().intersects(ground.getGlobalBounds());
}

void main_menu_generator(Text write[], Sprite& mainmenu, Sprite& instructions, Font& font, Texture& menutexture, Texture& insttexture) {
	font.loadFromFile("font.otf");

	write[0].setFont(font); //play font
	write[0].setString("play");
	write[0].setCharacterSize(200);
	write[0].setPosition(Vector2f(xforicon, yforicon));

	write[1].setFont(font);	//options font
	write[1].setString("options");
	write[1].setCharacterSize(200);
	write[1].setPosition(Vector2f(xforicon, yforicon + 150));

	write[2].setFont(font); //exit font
	write[2].setString("exit");
	write[2].setCharacterSize(200);
	write[2].setPosition(Vector2f(xforicon, yforicon + 300));

	write[3].setFont(font); //instructions font
	write[3].setString("instructions");
	write[3].setCharacterSize(90);
	write[3].setPosition(Vector2f(100, 100));
	write[3].setFillColor(Color(255, 255, 255));

	write[4].setFont(font); // instructions deyails
	write[4].setString("Press the [Left Arrow Key] to go to the Left\nPress the [Right Arrow Key] to go to the Right\nPress the [Up Arrow Key] to Jump");
	write[4].setCharacterSize(50);
	write[4].setPosition(Vector2f(100, 100));
	write[4].setFillColor(Color(30, 144, 255));

	write[5].setFont(font); //back font
	write[5].setString("back");
	write[5].setCharacterSize(100);
	write[5].setPosition(Vector2f(xforwindow - 400, 100));
	write[5].setFillColor(Color(255, 255, 255));

	menutexture.loadFromFile("main menu.jpg");
	mainmenu.setTexture(menutexture);
	mainmenu.setScale(xforwindow / xformenu, yforwindow / yformenu);

	insttexture.loadFromFile("instructions.jpg");
	instructions.setTexture(insttexture);
	instructions.setScale(xforwindow / 1628, yforwindow / 1080);
}

void pause_menu_generator(Text buttons[], Font& pauseFont) {
	pauseFont.loadFromFile("font.otf");
	buttons[0].setFillColor(Color::White);
	buttons[0].setFont(pauseFont);
	buttons[0].setString("Continue");
	buttons[0].setCharacterSize(75);
	buttons[0].setPosition(788, 360);

	buttons[1].setFillColor(Color::White);
	buttons[1].setFont(pauseFont);
	buttons[1].setString("Option");
	buttons[1].setCharacterSize(75);
	buttons[1].setPosition(828, 470);

	buttons[2].setFillColor(Color::White);
	buttons[2].setFont(pauseFont);
	buttons[2].setString("Exit");
	buttons[2].setCharacterSize(75);
	buttons[2].setPosition(858, 580);
}

void game_over_generator(Text over[], Font& font) {
	over[0].setString("game over");// game over
	over[0].setFont(font);
	over[0].setFillColor(Color(1000, 0, 0));
	over[0].setCharacterSize(300);
	over[0].setPosition(WINDOW_WIDTH / 2 - 450, WINDOW_HEIGHT / 2 - 450);

	over[1].setString("restart");// restart
	over[1].setFont(font);
	over[1].setFillColor(Color(255, 255, 255));
	over[1].setCharacterSize(100);
	over[1].setPosition(WINDOW_WIDTH / 2 - 450, WINDOW_HEIGHT / 2 + 150);

	over[2].setString("exit");// exit
	over[2].setFont(font);
	over[2].setFillColor(Color(255, 255, 255));
	over[2].setCharacterSize(100);
	over[2].setPosition(WINDOW_WIDTH / 2 + 250, WINDOW_HEIGHT / 2 + 150);
}

void options_generator(Text optionsfont[], Font& font) {
	optionsfont[0].setString("sound");// exit
	optionsfont[0].setFont(font);
	optionsfont[0].setFillColor(Color(255, 255, 255));
	optionsfont[0].setCharacterSize(200);
	optionsfont[0].setPosition(WINDOW_WIDTH / 2 - 200, 100);

	optionsfont[1].setString("on");// exit
	optionsfont[1].setFont(font);
	optionsfont[1].setFillColor(Color(255, 255, 255));
	optionsfont[1].setCharacterSize(100);
	optionsfont[1].setPosition(WINDOW_WIDTH / 2 - 300, WINDOW_HEIGHT / 2 + 200);


	optionsfont[2].setString("off");// exit
	optionsfont[2].setFont(font);
	optionsfont[2].setFillColor(Color(255, 255, 255));
	optionsfont[2].setCharacterSize(100);
	optionsfont[2].setPosition(WINDOW_WIDTH / 2 + 300, WINDOW_HEIGHT / 2 + 200);
}

void main_menu_logic(Text write[], RenderWindow& window, Music& gamesound, Music& keyboardchoose, Music& keyboardclic, int& soundbutton, int& keyboardclick, bool& inst, bool& play, bool& options, bool& reset_the_game, bool& soundgame, bool& back)
{
	if (!play) {
		window.setFramerateLimit(6);
		// Main Menu Logic
		// Instructions
		if (!inst && !options) {
			if (Keyboard::isKeyPressed(Keyboard::Down) && keyboardclick == -1) {
				if (soundgame) keyboardclic.play();   keyboardclick = 0;
			}
			else if (Keyboard::isKeyPressed(Keyboard::Down) && keyboardclick == 3) {
				keyboardclick = 0; if (soundgame)keyboardclic.play();
			}
			else if (Keyboard::isKeyPressed(Keyboard::Up) && keyboardclick == 1) {
				keyboardclick = 0; if (soundgame)keyboardclic.play();
			}
			// Play
			else if (Keyboard::isKeyPressed(Keyboard::Down) && keyboardclick == 0) {
				keyboardclick = 1; if (soundgame)keyboardclic.play();
			}
			else if (Keyboard::isKeyPressed(Keyboard::Up) && keyboardclick == 2) {
				if (soundgame)keyboardclic.play(); 	keyboardclick = 1;
			}
			// Options
			else if (Keyboard::isKeyPressed(Keyboard::Down) && keyboardclick == 1) {
				if (soundgame)keyboardclic.play(); 	keyboardclick = 2;
			}
			else if (Keyboard::isKeyPressed(Keyboard::Up) && keyboardclick == 3) {
				keyboardclick = 2; if (soundgame)keyboardclic.play();
			}
			// Exit
			else if (Keyboard::isKeyPressed(Keyboard::Down) && keyboardclick == 2) {
				keyboardclick = 3; if (soundgame)keyboardclic.play();
			}
			else if (Keyboard::isKeyPressed(Keyboard::Up) && keyboardclick == 0) {
				keyboardclick = 3; if (soundgame)keyboardclic.play();
			}
		}

		if (keyboardclick == 1) {
			write[0].setFillColor(Color(255, 255, 0));
			write[1].setFillColor(Color(255, 255, 255));
			write[2].setFillColor(Color(255, 255, 255));
			write[3].setFillColor(Color(255, 255, 255));
		}
		else if (keyboardclick == 2) {
			write[0].setFillColor(Color(255, 255, 255));
			write[1].setFillColor(Color(255, 255, 0));
			write[2].setFillColor(Color(255, 255, 255));
			write[3].setFillColor(Color(255, 255, 255));
		}
		else if (keyboardclick == 3) {
			write[0].setFillColor(Color(255, 255, 255));
			write[1].setFillColor(Color(255, 255, 255));
			write[2].setFillColor(Color(255, 255, 0));
			write[3].setFillColor(Color(255, 255, 255));
		}
		else if (keyboardclick == 0) {
			write[0].setFillColor(Color(255, 255, 255));
			write[1].setFillColor(Color(255, 255, 255));
			write[2].setFillColor(Color(255, 255, 255));
			write[3].setFillColor(Color(255, 255, 0));
		}
		//exit
		if (keyboardclick == 3 && Keyboard::isKeyPressed(Keyboard::Enter)) {
			if (soundgame) {
				keyboardchoose.play();
			}
			window.close();
		}
		//option
		if (keyboardclick == 2 && Keyboard::isKeyPressed(Keyboard::Enter)) {
			if (soundgame) {
				keyboardchoose.play();
				soundbutton = 0;
			}
			else
				soundbutton = 1;

			options = true;
		}
		//open instructiosns
		if (keyboardclick == 0 && Keyboard::isKeyPressed(Keyboard::Enter)) {
			if (soundgame) {
				keyboardchoose.play();
			}
			inst = true;
		}
		//back
		if (Keyboard::isKeyPressed(Keyboard::Down) && inst)  back = true;

		if (back) write[5].setFillColor(Color(30, 144, 255));
		else write[5].setFillColor(Color(255, 255, 255));

		if (Keyboard::isKeyPressed(Keyboard::Enter) && back)
		{
			if (soundgame) {
				keyboardchoose.play();
			}
			keyboardclick = -1;
			back = false;
			inst = false;
			write[0].setFillColor(Color(255, 255, 255));
			write[3].setFillColor(Color(255, 255, 255));
		}

		if (keyboardclick == -1) {
			write[0].setFillColor(Color(255, 255, 255));
			write[1].setFillColor(Color(255, 255, 255));
			write[2].setFillColor(Color(255, 255, 255));
			write[3].setFillColor(Color(255, 255, 255));
		}

		if (keyboardclick == 1 && Keyboard::isKeyPressed(Keyboard::Enter)) {
			if (soundgame) {
				keyboardchoose.play();
			}
			play = true;
			reset_the_game = true;
		}
	}
}

void options_logic(Text optionsfont[], Music& menu_sound, Music& keyboardchoose, Music& keyboardclic, int& soundbutton, bool& soundgame, bool& options) {
	if (options) {
		if (soundbutton == 0) {
			optionsfont[1].setFillColor(Color(255, 255, 0));
			optionsfont[2].setFillColor(Color(255, 255, 255));
		}
		if (soundbutton == 1) {
			optionsfont[1].setFillColor(Color(255, 255, 255));
			optionsfont[2].setFillColor(Color(255, 255, 0));
		}
		if (soundbutton == 0 && Keyboard::isKeyPressed(Keyboard::Right)) {
			soundbutton = 1; if (soundgame)keyboardclic.play();
		}
		if (soundbutton == 1 && Keyboard::isKeyPressed(Keyboard::Left)) {
			soundbutton = 0; if (soundgame)keyboardclic.play();
		}
		if (soundbutton == 0 && Keyboard::isKeyPressed(Keyboard::Enter)) {
			if (soundgame) {
				keyboardchoose.play();
			}
			soundgame = true;
			menu_sound.setVolume(100.00f);
		}
		if (soundbutton == 1 && Keyboard::isKeyPressed(Keyboard::Enter)) {
			if (soundgame) {
				keyboardchoose.play();
			}
			soundgame = false;
			menu_sound.setVolume(0.00f);
		}
		if (Keyboard::isKeyPressed(Keyboard::Escape)) {
			if (soundgame) {
				keyboardchoose.play();
			}
			options = false;
		}
	}
}

void lose_logic(Text high, Text over[], Text& text, RenderWindow& window, Sprite& instructions, Music& game_over, Music& keyboardchoose, Music& keyboardclic, int& buttonlose, bool& soundgame, bool& isPaused, bool& islose, bool& reset_the_game, bool& play, bool& inst) {
	if (islose) {
		if (soundgame and !onlyone) {
			game_over.play();
			onlyone = 1;
		}
		window.setFramerateLimit(6);
		window.draw(instructions);
		text.setPosition(WINDOW_WIDTH / 2 - 190, WINDOW_HEIGHT / 2);
		high.setPosition(WINDOW_WIDTH / 2 - 290, WINDOW_HEIGHT / 2 - 100); ////////////////////////////
		high.setFillColor(Color::Cyan);
		if (Keyboard::isKeyPressed(Keyboard::Key::Escape)) {
			isPaused = false;
		}
		if (buttonlose == 0 && Keyboard::isKeyPressed(Keyboard::Key::Right)) {
			buttonlose = 1; if (soundgame)keyboardclic.play();
		}
		if (buttonlose == 1 && Keyboard::isKeyPressed(Keyboard::Key::Left)) {
			buttonlose = 0; if (soundgame)keyboardclic.play();
		}
		if (buttonlose == 0) {
			over[1].setFillColor(Color(255, 255, 0));
			over[2].setFillColor(Color(255, 255, 255));
		}
		if (buttonlose == 1) {
			over[1].setFillColor(Color(255, 255, 255));
			over[2].setFillColor(Color(255, 255, 0));
		}
		window.draw(text);
		for (int i = 0; i < 3; i++) {
			window.draw(over[i]);
			window.draw(high);
		}
		if (buttonlose == 0 && Keyboard::isKeyPressed(Keyboard::Key::Enter)) {
			if (soundgame) {
				keyboardchoose.play();
			}
			isPaused = false;
			islose = false;
			reset_the_game = true;
		}

		if (buttonlose == 1 && Keyboard::isKeyPressed(Keyboard::Key::Enter)) {
			if (soundgame) {
				keyboardchoose.play();
			}
			play = false;
			inst = false;
			isPaused = false;
			islose = false;
		}
	}
}

void pause_logic(Text buttons[], Music& keyboardchoose, Music& keyboardclic, int& soundbutton, int& selected, bool& soundgame, bool& isPaused, bool& options, bool& play, bool& inst) {
	if (isPaused && Keyboard::isKeyPressed(Keyboard::Down)) {
		if (soundgame)keyboardclic.play();
		buttons[selected].setFillColor(Color::White);
		selected++;
		selected %= 3;
		buttons[selected].setFillColor(Color::Yellow);

	}
	else if (isPaused && Keyboard::isKeyPressed(Keyboard::Up)) {
		if (soundgame)keyboardclic.play();
		buttons[selected].setFillColor(Color::White);
		if (selected >= 1)
			selected--;
		buttons[selected].setFillColor(Color::Yellow);
	}
	else if (Keyboard::isKeyPressed(Keyboard::Enter)) {
		if (soundgame)keyboardchoose.play();
		if (selected == 0) {
			isPaused = false;
		}
		else if (selected == 1 && Keyboard::isKeyPressed(Keyboard::Enter)) {
			if (soundgame) {
				keyboardchoose.play();
				soundbutton = 0;
			}
			else
				soundbutton = 1;

			options = true;
		}
		else if (isPaused && selected == 2) {
			play = false;
			inst = false;
			isPaused = false;
		}
	}
}

void game_reset(RenderWindow& window, Text& text, Texture& tx_lwall, Texture& tx_rwall, Texture& tx_bg, Texture& start_ground_texture, Texture& platformtext, Sprite left_wall[], Sprite right_wall[], Sprite background[], Sprite& ground, Character& icys, Platfrom platform[], RectangleShape& rectangle, int& index, int& x1, int& y, int& delay, int& fast_cnt, bool& firstjump, bool& reset_the_game) {
	text.setPosition(Vector2f(0, 0));

	tx_lwall.loadFromFile("wallLeft.png");
	tx_rwall.loadFromFile("wallRight.png");
	tx_bg.loadFromFile("Background-1.png");

	// Apply Textures to Walls and Background
	for (int x = 0; x < 4; x++) {
		left_wall[x].setTexture(tx_lwall);
		left_wall[x].setScale(SCALE_CONSTANT, SCALE_CONSTANT);

		right_wall[x].setTexture(tx_rwall);
		right_wall[x].setScale(SCALE_CONSTANT, SCALE_CONSTANT);

		if (x < 3) {
			background[x].setTexture(tx_bg);
			background[x].setScale(SCALE_CONSTANT, SCALE_CONSTANT);
		}
	}

	// Set the Initial Position of the Walls and Background
	for (int y = 1; y > -3; y--) {
		left_wall[index].setPosition(0, y * SCALE_CONSTANT * 360);
		right_wall[index].setPosition(window.getSize().x - right_wall[index].getGlobalBounds().width, y * SCALE_CONSTANT * 360);

		if (index < 3)
			background[index].setPosition(left_wall[index].getGlobalBounds().width - SCALE_CONSTANT * 5, (y - 1) * SCALE_CONSTANT * 720);
		index++;
	}


	// Main Character
	icys.tx.loadFromFile("cape character.png");
	icys.sprite.setTexture(icys.tx);
	icys.sprite.setTextureRect(IntRect(0, 0, 42.5, 49));
	icys.sprite.setPosition(550, 1000);
	icys.sprite.setScale(1.5, 1.5);

	//The Ground
	start_ground_texture.loadFromFile("block_1.png");
	ground.setTexture(start_ground_texture);
	ground.setPosition(110.f, 1025.f); // Set Ground Position
	ground.setScale(5.88f, 1.22f);

	platformtext.loadFromFile("block_1.png");
	// Generate Tiles
	for (int i = 0; i < 1000; i++) {
		platform[i].sprite.setTexture(platformtext);
		platform[i].sprite.setScale((rand() % 3 + 2) / 2.f, 1);
		platform[i].sprite.setPosition(x1, y + ((firstjump) ? 300 : 170));
		int yran = rand() % 175;
		x1 = 200 + rand() % (1000);
		if (yran < 150) {
			yran += 150;
		}
		y -= yran;
	}

	rectangle.setSize(Vector2f(icys.sprite.getGlobalBounds().width - 30.f, 3.f));
	// Create a Small Delay in Order to Handle the Huge Framerate
	delay = 0, fast_cnt = 0;

	reset_the_game = false;
	onlyone = 0;
	opened_game_sound = 0;
	opened_menu_sound = 0;
}

void player_movement(Character& icys, int& x, float& velocity_y, float& world_speed, int& delay, int& fast_cnt, bool& is_grounded, bool& is_jumping, bool& firstjump, bool& soundgame, Music& jump_sound) {
	// Player Movement Based on The Keyboard Input
	// Jump Animation
	if (is_grounded == false) {
		icys.sprite.setTextureRect(IntRect(0, 3 * 49, 42.5, 49));
	}

	// Move Right
	if (Keyboard::isKeyPressed(Keyboard::Key::Right) && icys.sprite.getPosition().x < 1745) {
		icys.sprite.move(7, 0);
		if (is_grounded) {
			x++;
			x %= 4;
			icys.sprite.setTextureRect(IntRect(x * 42.5, 49, 42.5, 49));
		}
		else {
			x++; // increment animation frame
			x %= 4; // wrap animation frames
			icys.sprite.setTextureRect(IntRect((x * 42.5), 4 * 49, 42.5, 49)); // set texture rectangle for animation
		}
	}

	// Move Left
	if (Keyboard::isKeyPressed(Keyboard::Key::Left) && icys.sprite.getPosition().x > 118.f) {
		icys.sprite.move(-7, 0);
		if (is_grounded) {
			x++;
			x %= 4;
			icys.sprite.setTextureRect(IntRect((x * 42.5) + 42, 49, -42.5, 49));
		}
		else {
			x++; // increment animation frame
			x %= 4; // wrap animation frames
			icys.sprite.setTextureRect(IntRect((x * 42.5) + 42, 4 * 49, -42.5, 49)); // set texture rectangle for animation
		}
	}

	// Jump
	if (is_grounded) {
		// If the Player is Grounded
		// Neglect the Effect of Gravity
		velocity_y = 0;

		// -------------- Normal Jump -------------- //
		if (Keyboard::isKeyPressed(Keyboard::Key::Up) && delay > 10) {
			if (soundgame) {
				jump_sound.play();
			}
			// If the User Press UP ARROW
			// Move the Character Upwards
			velocity_y -= icys.jump_power;
			if (firstjump == false)
				velocity_y -= 4.5f;

			is_grounded = false;
			is_jumping = true; fast_cnt = 0; // Reset the Delay Handler

			if (firstjump) world_speed += 0.005;
			delay = 0;
		}
	}
	else {
		velocity_y += 0.325f;  // Simulate the Effect of Gravity
	}

	// Apply the Effect of Gravity
	icys.sprite.move(0, velocity_y);
}