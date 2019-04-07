/* A very primitive text editor with limited amount of characters */
#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>

#define ALLOWED_CHARS 5000
#define QUIT_KEY      KEY_F(5)

// Global variable for number of lines
int line_no = 0;

/* Returns position in character buffer from cursor */
int get_pos(int y, int x, int lines[ALLOWED_CHARS]) {
  // Vars: y is line number, x is which character in line
  // Calculate the position in buffer
  int bufpos = 0;
  for (int j = 0; j < y; j++) {
    // Add for each line their number of characters
    bufpos += lines[j];
  }
  // It should not be possible to move the cursor beyond the number of lines
  // But the cursor can go beyond the number of characters in a line
  if (x > lines[y]) {
    bufpos += lines[y];
  } else {
    bufpos += x;
  }
  return bufpos;
}

/* Primitive text editor, using txt as input and output */
int primitive_txt(char txt[ALLOWED_CHARS]) {
  int chars[ALLOWED_CHARS]; // Position of "\n"/number of chars per line
                            // for up to ALLOWED_CHARS lines
  WINDOW *win;              // Main window

  /* Initialization of variables & ncurses */
  for (int i = 0; i < ALLOWED_CHARS; i++) {
    chars[i] = 0;
  }

  // Initialization of ncurses
  initscr();
  // Each character is read, no wait for newline
  cbreak();
  // New window
  win = newwin(LINES - 1, COLS - 1, 0, 0);
  // No automatic echoing of characters
  noecho();
  // Special characters enabled
  keypad(win, TRUE);
  keypad(stdscr, TRUE);
  /* End of init */

  /* Main loop */
  int ch;
  while ((ch = getch()) != QUIT_KEY) {
    // Get current position of cursor
    int y, x;
    getyx(win, y, x);
    /* Movement of cursor */
    if (ch == KEY_UP) {
      if (y > 0) {
        y--;
        if (x > chars[y]) {
          x = chars[y] - 1;
        }
      }
    } else if (ch == KEY_RIGHT) {
      if (x < chars[y] - 1) {
        x++;
      }
    } else if (ch == KEY_DOWN) {
      if (y < line_no) {
        y++;
        if (x > chars[y]) {
          x = chars[y] - 1;
        }
      }
    } else if (ch == KEY_LEFT) {
      if (x > 0) {
        x--;
      }
    /* End of cursor movement */
    /* Backspace & Enter */
    } else if (ch == KEY_BACKSPACE) {
      if (chars[y] > 0 & x > 0) {
        // Removing one char ~ moving all following chars one to the left
        for (int i = get_pos(y, --x, chars); i < ALLOWED_CHARS - 1; i++) {
          txt[i] = txt[i + 1];
        }
        chars[y]--;
      } else {
        // Either remove a line or do nothing (if at beginning of document)
        if (y > 0) {
          // Move all characters to the left
          for (int i = get_pos(y, --x, chars); i < ALLOWED_CHARS - 1; i++) {
            txt[i] = txt[i + 1];
          }
          // Update chars array
          for (int i = y; i < ALLOWED_CHARS - 1; i++) {
            chars[i] = chars[i + 1];
          }
          line_no--;
        }
      }
    } else if (ch == 10) { // KEY_ENTER defaults to Numpad enter
                           // 10 is ASCII code for \n
      int dif = 0; // How many characters are copied into new line
      // Move all following characters to the right to make space for a new \n
      for (int i = ALLOWED_CHARS - 1; i >= get_pos(y, x, chars); i--) {
        txt[i] = txt[i - 1];
      }
      txt[get_pos(y, x, chars)] = '\n';
      // If the cursor is not at the end of line we need to get the number
      // of characters that will be in the new line
      if (x != chars[y]) {
        dif = chars[y] - x;
      }
      /* Update of chars array */
      chars[y] = chars[y] + 1 - dif; // + 1 for '\n'
      chars[y + 1] = dif;
      // All following values of chars have to be modified
      for (int i = y + 2; i < ALLOWED_CHARS - 1; i++) {
        chars[i] = chars[i + 1];
      }
      /* End of update */
      line_no++;
      y++;
      x = 0;
    } else {
      // Make space for a new character
      for (int i = ALLOWED_CHARS - 1; i > get_pos(y, x, chars); i--) {
        txt[i] = txt[i - 1];
      }
      txt[get_pos(y, x, chars)] = ch;
      chars[y]++;
      x++;
    }

    // Clear window
    wclear(win);
    // Print text
    wprintw(win, txt);
    // Move cursor
    wmove(win, y, x);
    // Refresh changes
    refresh();
    wrefresh(win);
  }
  /* End of main loop */

  endwin();

  return 0;
}

/*
int main(void) {
  char buf[20];
  char *txt;
  txt = malloc(ALLOWED_CHARS * sizeof(char));
  printf("Press \"Enter\" to enter text entry mode, then \"F5\" to finish:\n");
  fgets(buf, sizeof(buf), stdin);

  primitive_txt(txt);

  printf("Your entered text was: %s\n", txt);
  free(txt);
}
*/
