#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <curses.h>
#include <unistd.h>

typedef enum direction {
	DIR_UP,
	DIR_DOWN,
	DIR_LEFT,
	DIR_RIGHT,
	DIR_NONE,
	QUIT,
} direction;

typedef struct snake_body {
	int x, y;
	struct snake_body *prev;
	struct snake_body *next;
} snake_body;

typedef struct snake {
	snake_body *head;
	snake_body *last;
} snake;

direction get_input(void);
bool opposite(direction dir1, direction dir2);
snake_body *new_body(snake_body *prev, int x, int y);
void append_body(snake *s);
snake make_snake(int x, int y);
void draw_snake(snake *s);
void move_snake(snake *s, direction new_dir);
void delete_snake(snake *s);

const char SNAKE_BODY = '#';
const short SNAKE_COLOR_I = 1;
const short SNAKE_COLOR = COLOR_GREEN;
const char APPLE = 'O';
const short APPLE_COLOR_I = 2;
const short APPLE_COLOR = COLOR_RED;

int SCR_WIDTH, SCR_HEIGHT;

int main(void) {
	initscr();
	cbreak();
	noecho();
	nodelay(stdscr, TRUE);
	keypad(stdscr, TRUE);
	curs_set(FALSE);
	start_color();

	init_pair(SNAKE_COLOR_I, SNAKE_COLOR, COLOR_BLACK);
	init_pair(APPLE_COLOR_I, APPLE_COLOR, COLOR_BLACK);

	SCR_WIDTH = getmaxx(stdscr);
	SCR_HEIGHT = getmaxy(stdscr);

	snake s = make_snake(SCR_WIDTH / 2, SCR_HEIGHT / 2);
	direction dir = DIR_UP;
	int score = 0;
	int apple_x = -1;
	int apple_y = -1;

	while (1) {
		if (apple_x == -1) {
			apple_x = 1 + rand() % (SCR_WIDTH - 2);
			apple_y = 1 + rand() % (SCR_HEIGHT - 2);
		}

		direction new_dir = get_input();

		if (new_dir == QUIT) {
			break;
		}

		if (new_dir != DIR_NONE && !(score > 1 && opposite(dir, new_dir))) {
			dir = new_dir;
		}

		move_snake(&s, dir);
		if (s.head->x == apple_x && s.head->y == apple_y) {
			score++;
			apple_x = -1;
			append_body(&s);
		}

		clear();

		draw_snake(&s);

		attron(COLOR_PAIR(APPLE_COLOR_I));
		mvaddch(apple_y, apple_x, APPLE);
		attroff(COLOR_PAIR(APPLE_COLOR_I));

		mvprintw(0, 0, "Score: %d", score);
		refresh();

		usleep(90000 - (score * 700));
	}

	delete_snake(&s);

	endwin();
}

direction get_input(void) {
	switch (getch()) {
		case 'w':
		case KEY_UP:
			return DIR_UP;
		case 's':
		case KEY_DOWN:
			return DIR_DOWN;
		case 'a':
		case KEY_LEFT:
			return DIR_LEFT;
		case 'd':
		case KEY_RIGHT:
			return DIR_RIGHT;
		case 'q':
			return QUIT;
		default:
			return DIR_NONE;
	}
}

bool opposite(direction dir1, direction dir2) {
	if (dir1 == DIR_UP && dir2 == DIR_DOWN) return true;
	else if (dir1 == DIR_DOWN && dir2 == DIR_UP) return true;
	else if (dir1 == DIR_LEFT && dir2 == DIR_RIGHT) return true;
	else if (dir1 == DIR_RIGHT && dir2 == DIR_LEFT) return true;
	else return false;
}

snake_body* new_body(snake_body *prev, int x, int y) {
	snake_body *node = malloc(sizeof(*node));
	node->x = x;
	node->y = y;
	node->prev = prev;
	node->next = NULL;
	return node;
}

snake make_snake(int x, int y) {
	snake_body *head = new_body(NULL, x, y);
	return (snake){
		.head = head,
		.last = NULL,
	};
}

void append_body(snake *s) {
	if (s->last != NULL) {
		s->last->next = new_body(s->last, s->last->x, s->last->y);
		s->last = s->last->next;
	} else {
		s->last = new_body(s->head, s->head->x, s->head->y);
	}
}

void draw_snake(snake *s) {
	attron(COLOR_PAIR(SNAKE_COLOR_I));
	for (snake_body *node = s->head; node != NULL; node = node->next) {
		mvaddch(node->y, node->x, SNAKE_BODY);
	}
	attroff(COLOR_PAIR(SNAKE_COLOR_I));
}

void move_snake(snake *s, direction dir) {
	if (s->last != NULL) {
		snake_body *last = s->last;
		snake_body *second_last = last->prev;
		second_last->next = NULL;
		last->prev = NULL;

		switch (dir) {
			case DIR_UP:
				last->x = s->head->x;
				last->y = s->head->y - 1;
				break;
			case DIR_DOWN:
				last->x = s->head->x;
				last->y = s->head->y + 1;
				break;
			case DIR_LEFT:
				last->x = s->head->x - 1;
				last->y = s->head->y;
				break;
			case DIR_RIGHT:
				last->x = s->head->x + 1;
				last->y = s->head->y;
				break;
			default: // unreachable
				break;
		}

		if (last->x < 0) last->x = SCR_WIDTH;
		else if (last->x >= SCR_WIDTH) last->x = 0;
		if (last->y < 0) last->y = SCR_HEIGHT;
		else if (last->y >= SCR_HEIGHT) last->y = 0;

		last->next = s->head;
		s->head->prev = last;
		s->head = last;
		s->last = second_last;
	} else {
		switch (dir) {
			case DIR_UP:
				s->head->y -= 1;
				break;
			case DIR_DOWN:
				s->head->y += 1;
				break;
			case DIR_LEFT:
				s->head->x -= 1;
				break;
			case DIR_RIGHT:
				s->head->x += 1;
				break;
			default: // unreachable
				break;
		}
	}
}

void delete_snake(snake *s) {
	for (snake_body *node = s->head; node != NULL; node = node->next) {
		free(node);
	}
}
