#include <ncurses.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

/*
 * Constants
 */

// Size of both players
// [rows]
#define PLAYER_SIZE 5

// Speed of players
// [rows per frame per board_heigth]
#define PLAYER_SPEED 0.008

// X component of ball speed
// [cols per frame per board_width]
#define BALL_SPEED_X 0.0014

// Max of y component of ball speed (y speed is random)
// [rows per frame per board_width]
#define BALL_SPEED_Y_MAX 0.0018

// Delay berween frames
// [µs]
#define FRAME_DELAY 10000

/*
 * Valiables
 */

int HEIGHT, WIDTH;
int GREEN, WHITE, RED, WHITE_TEXT;

/*
 * Code
 */

int my_round(float x); // Return int - rounded 'x'
int random_sign();     // Return either -1 or 1

int main(int argc, char** argv) {
	// Random setup
	srand(time(NULL));

	// Setup
	initscr();  // Init
	cbreak();   // Char mode
	noecho();   // No echo
	timeout(0); // Use non blocking mode
	keypad(stdscr, TRUE); // Catch special chars
	curs_set(0); // No blinking
	getmaxyx(stdscr, HEIGHT, WIDTH); // Get window size

	// Color setup
	start_color();
	init_pair(1, 7, 7);
	init_pair(2, 2, 2);
	init_pair(3, 1, 1);
	init_pair(4, 7, 0);
	WHITE = ' ' | COLOR_PAIR(1);
	GREEN = ' ' | COLOR_PAIR(2);
	RED = ' ' | COLOR_PAIR(3);
	WHITE_TEXT = COLOR_PAIR(4);

	// Game variables
	int game_height = HEIGHT - 5;
	int game_width = WIDTH - 4;
	float p1_pos = (game_height - PLAYER_SIZE) / 2;
	float p2_pos = (game_height - PLAYER_SIZE) / 2;
	float p1_dir = 0;
	float p2_dir = 0;
	int p1_score = 0;
	int p2_score = 0;
	float ball_pos_x = game_width / 2;
	float ball_pos_y = game_height / 2;
	float ball_speed_x = random_sign() * BALL_SPEED_X     * game_width;
	float ball_speed_y = random_sign() * BALL_SPEED_Y_MAX * game_height * ((float)rand() / RAND_MAX);

	// Loop...
	while (1) {
	
		/*
		 * User input
		 */

		switch (getch()) {
			case KEY_HOME: // Exit
				endwin();
				return 0;
			case 'w': // Move p1 up
				if (p1_dir == 1)
					p1_dir = 0;
				else p1_dir = -1;
				break;
			case 's': // Move p1 down
				if (p1_dir == -1)
					p1_dir = 0;
				else p1_dir = 1;
				break;
			case KEY_UP: // Move p2 up
				if (p2_dir == 1)
					p2_dir = 0;
				else p2_dir = -1;
				break;
			case KEY_DOWN: // Move p2 down
				if (p2_dir == -1)
					p2_dir = 0;
				else p2_dir = 1;
				break;
		}

		/*
		 * Players manipulations
		 */

		// Move players and ball
		p1_pos += p1_dir * PLAYER_SPEED * game_height;
		p2_pos += p2_dir * PLAYER_SPEED * game_height;
		ball_pos_x += ball_speed_x;
		ball_pos_y += ball_speed_y;

		// Check wals for players
		if (p1_pos < 0) {
			p1_pos = 0;
			p1_dir = 0;
		} else if (p1_pos > game_height - PLAYER_SIZE) {
			p1_pos = game_height - PLAYER_SIZE;
			p1_dir = 0;
		}
		if (p2_pos < 0) {
			p2_pos = 0;
			p2_dir = 0;
		} else if (p2_pos > game_height - PLAYER_SIZE) {
			p2_pos = game_height - PLAYER_SIZE;
			p2_dir = 0;
		}

		/*
		 * Ball manipulations
		 */

		// Update ball position
		ball_pos_x += ball_speed_x;
		ball_pos_y += ball_speed_y;

		// Check walls for ball
		if (ball_pos_y < 0) {
			ball_pos_y = 0;
			ball_speed_y *= -1;
		} else if (ball_pos_y > game_height - 1) {
			ball_pos_y = game_height - 1;
			ball_speed_y *= -1;
		}

		// Check if ball touches player
		if (ball_pos_x < 2) {
			if (p1_pos <= ball_pos_y && p1_pos + PLAYER_SIZE > ball_pos_y) {
				ball_pos_x = 2;
				ball_speed_x *= -1;
				ball_speed_y += p1_dir * PLAYER_SPEED * game_height / 5;
			} else {
				p1_score -= 10;
				p2_score += 10;
				ball_pos_x = game_width / 2;
				ball_pos_y = game_height / 2;
				ball_speed_x = random_sign() * BALL_SPEED_X     * game_width;
				ball_speed_y = random_sign() * BALL_SPEED_Y_MAX * game_height * ((float)rand() / RAND_MAX);
			}
		} else if (ball_pos_x > game_width - 4) {
			if (p2_pos <= ball_pos_y && p2_pos + PLAYER_SIZE > ball_pos_y) {
				ball_pos_x = game_width - 4;
				ball_speed_x *= -1;
				ball_speed_y += p2_dir * PLAYER_SPEED * game_height / 5;
			} else {
				p1_score += 10;
				p2_score -= 10;
				ball_pos_x = game_width / 2;
				ball_pos_y = game_height / 2;
				ball_speed_x = random_sign() * BALL_SPEED_X     * game_width;
				ball_speed_y = random_sign() * BALL_SPEED_Y_MAX * game_height * ((float)rand() / RAND_MAX);
			}
		}

		/*
		 * Drawing
		 */

		clear();

		// Draw frame
		move(0, 0);
		for (int i = 0; i < WIDTH; i++) {
			addch(GREEN);
		}
		for (int i = 0; i < game_height; i++) {
			move(1+i, 0);
			addch(GREEN);
			addch(GREEN);
			move(1+i, WIDTH-2);
			addch(GREEN);
			addch(GREEN);
		}
		move(game_height + 1, 0);
		for (int i = 0; i < WIDTH; i++) {
			addch(GREEN);
		}

		// Draw players
		int p1_i = my_round(p1_pos);
		int p2_i = my_round(p2_pos);
		for (int i = 0; i < game_height; i++) {
			move(1+i, 3);
			if (i >= p1_i && i < p1_i+PLAYER_SIZE)
				addch(WHITE);
			else
				addch(' ');
		}
		for (int i = 0; i < game_height; i++) {
			move(1+i, WIDTH-4);
			if (i >= p2_i && i < p2_i+PLAYER_SIZE)
				addch(WHITE);
			else
				addch(' ');
		}

		// Print new ball
		move(my_round(ball_pos_y) + 1, my_round(ball_pos_x) + 2);
		addch(RED);
		addch(RED);

		// Print score
		move(HEIGHT-2, WIDTH/2-2);
		printw("SCORE");
		move(HEIGHT-2, WIDTH/4-1);
		printw("%i", p1_score);
		move(HEIGHT-2, WIDTH/4*3-1);
		printw("%i", p2_score);


		// Refresh and whait
		refresh();
		usleep(FRAME_DELAY);
	}

	return 0;
}

int my_round(float x) {
	int n = (int)x;
	if (x - n >= 0.5)
		return n+1;
	return n;
}

int random_sign() {
	return rand() % 2 * 2 - 1;
}
