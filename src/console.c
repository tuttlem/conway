#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ncurses.h>

#define UV_WIDTH 64
#define UV_HEIGHT 32

unsigned char *universe;

void universe_clear(unsigned char *u, int width, int height);
void universe_seed(unsigned char *u, int width, int height);
void universe_permute(unsigned char *u, int width, int height);
void universe_render(unsigned char *u, int width, int height);
void universe_stats(unsigned char *u, int width, int height, int *alive, int *dead);

/** Program entry point */
int main(int argc, char *argv[]) {
  int a, d;

  initscr();
  cbreak();
  noecho();
  nodelay(stdscr, TRUE);

  srand(time(NULL));
  universe = (unsigned char *) malloc(UV_HEIGHT * UV_WIDTH * sizeof(unsigned char));

  universe_clear(universe, UV_WIDTH, UV_HEIGHT);
  universe_seed(universe, UV_WIDTH, UV_HEIGHT);
  universe_render(universe, UV_WIDTH, UV_HEIGHT);

  while (1) {
    if (getch() == 27) {
      break;
    }

    universe_permute(universe, UV_WIDTH, UV_HEIGHT);
    universe_render(universe, UV_WIDTH, UV_HEIGHT);
    universe_stats(universe, UV_WIDTH, UV_HEIGHT, &a, &d);
    
    mvprintw(0, 0, "Alive: %d    Dead: %d", a, d);
    sleep(1);
  }

  free(universe);
  
  refresh();
  endwin();

  return 0;
}


void universe_clear(unsigned char *u, int width, int height) {
  memset(u, 0, width * height * sizeof(unsigned char));
}

void universe_seed(unsigned char *u, int width, int height) {
  int i;

  for (i = 0; i < (width * height); i ++) {
    u[i] = rand() % 7 == 0;
  }
}

void universe_render(unsigned char *u, int width, int height) {
  int x, y;

  erase();

  for (y = 0; y < height; y ++) {
    for (x = 0; x < width; x ++) {
      if (u[x + (y * width)]) {
        mvprintw(y + 1, x, "*");
      } else {
        mvprintw(y + 1, x, ".");
      }
    }
  }
}

int count_live_neighbours(unsigned char *u, int width, int height, int x, int y) {
  int x1 = (x - 1) % width;
  int x2 = (x + 1) % width;
  int y1 = (y - 1) % height;
  int y2 = (y + 1) % height;

  return u[x1 + (y1 * width)] +
         u[x1 + (y2 * width)] +
         u[x2 + (y1 * width)] +
         u[x2 + (y2 * width)] +
         u[x  + (y1 * width)] +
         u[x  + (y2 * width)] +
         u[x1 + (y  * width)] +
         u[x2 + (y  * width)];
}

void universe_permute(unsigned char *u, int width, int height) {
  int x, y, n, l;

  for (y = 0; y < height; y ++) {
    for (x = 0; x < width; x ++) {
      l = u[x + (y * width)];
      n = count_live_neighbours(u, width, height, x, y);

      if (l & ((n < 2) || (n > 3))) {
        u[x + (y * width)] = 0;
      } else if (!l & (n == 3)) {
        u[x + (y * width)] = 1;
      }
    }
  }
}

void universe_stats(unsigned char *u, int width, int height, int *alive, int *dead) {
  int i;

  *alive = 0;
  *dead = 0;

  for (i = 0; i < (width * height); i ++) {
    *alive += u[i];
  }

  *dead = (width * height) - *alive;
}
