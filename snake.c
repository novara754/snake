#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <stdint.h>
#include <curses.h>
#include <unistd.h>

typedef enum direction {
	DIR_UP,
	DIR_DOWN,
	DIR_LEFT,
	DIR_RIGHT,
	DIR_NONE,
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
snake_body *new_body(snake_body *prev, int x, int y);
void append_body(snake *s);
snake make_snake(int x, int y);
void draw_snake(snake *s);
void move_snake(snake *s, direction new_dir);
void delete_snake(snake *s);

const char SNAKE_BODY = '#';
const char APPLE = 'O';

int main(void) {
	initscr();
	cbreak();
	noecho();
	nodelay(stdscr, TRUE);
	keypad(stdscr, TRUE);
	curs_set(FALSE);

	const int scr_width = getmaxx(stdscr);
	const int scr_height = getmaxy(stdscr);
	snake s = make_snake(scr_width / 2, scr_height / 2);
	direction dir = DIR_UP;
	int score = 0;
	int apple_x = -1;
	int apple_y = -1;

	while (1) {
		if (apple_x == -1) {
			apple_x = rand() % scr_width;
			apple_y = rand() % scr_height;
		}

		direction new_dir = get_input();
		if (new_dir != DIR_NONE) dir = new_dir;

		move_snake(&s, dir);
		if (s.head->x == apple_x && s.head->y == apple_y) {
			score++;
			apple_x = -1;
			append_body(&s);
		}

		clear();
		draw_snake(&s);
		mvaddch(apple_y, apple_x, APPLE);
		mvprintw(0, 0, "Score: %d", score);
		refresh();

		usleep(70000);
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
		default:
			return DIR_NONE;
	}
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
	for (snake_body *node = s->head; node != NULL; node = node->next) {
		mvaddch(node->y, node->x, SNAKE_BODY);
	}
	move(0, 0);
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
