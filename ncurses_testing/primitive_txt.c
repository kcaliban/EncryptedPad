#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>

#define ALLOWED_CHARS 4000
#define QUIT_KEY      KEY_F(5)
// TODO:
//      * Save entered characters to buffer
//      * Remove backspaced characters from buffer
//      * Cursor movement and newline => pressing key up
//        has to move index of buffer correctly
//        Save position of \n for each line
//      * Better solution with spaces in buffer (move \0 ?)

// Global variable for number of lines
int line_no = 0;

/* Returns position in character buffer from cursor */
int get_pos(int y, int x, int lines[ALLOWED_CHARS]) {
  // Vars: y is line number, x is which character in line
  // Calculate the position in buffer
  int bufpos = 0;
  for (int j = 0; j < y - 1; j++) {
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


/* Main function to call primitive text editor */
int primitive_txt(char in[ALLOWED_CHARS], char out[ALLOWED_CHARS]) {
  int lines[ALLOWED_CHARS]; // Position of "\n"/number of chars per line
                            // for up to ALLOWED_CHARS lines
  int outpos;      // Position of cursor in character buffer
  WINDOW *win;           // Main window

  /* Initialization of variables & ncurses */
  for (int i = 0; i < ALLOWED_CHARS; i++) {
    lines[i] = 0;
  }
  outpos = 0; // If file is loaded this should be placed to the end

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

  // This approach manipulates the underlying data and screen in parallel,
  // maybe it would be smarter to have the screen print data and only modify
  // that?
  int ch;
  while ((ch = getch()) != QUIT_KEY) {
    // Get current position of cursor
    int y, x;
    getyx(win, y, x);
    // Movement of cursor
    if (ch == KEY_UP) {
      if (y > 0) {
        wmove(win, y - 1, x);
      }
    } else if (ch == KEY_RIGHT) {
      if (x < lines[y]) {
        wmove(win, y, x + 1);
      }
    } else if (ch == KEY_DOWN) {
      if (y < line_no) {
        wmove(win, y + 1, x);
      }
    } else if (ch == KEY_LEFT) {
      if (x > 0) {
        wmove(win, y, x - 1);
      }
    }
    // Text removal (BACKSPACE) & next line (ENTER)
    else if (ch == KEY_BACKSPACE) {
      if (x > 0) {
        wmove(win, y, x - 1);
        waddch(win, ' ');
        out[get_pos(y, x - 1, lines)] = ' ';
        wmove(win, y, x - 1); // waddch moves cursor one to the right
        // If at the end of the line, no space should be added, rather
        // all following character should be moved one byte to the left
      } else {
        // Remove a line
        wmove(win, y - 1, lines[y]);
        lines[y] = 0;
        line_no--;
        // Move all following lines one up
      }
    } else if (ch == KEY_ENTER) {
      waddch(win, '\n');
      out[get_pos(y, x + 1, lines)] = '\n';
      wmove(win, y + 1, 0);
      line_no++;
    }
    refresh();
    wrefresh(win);
  }

  endwin();

  return 0;
}

int main(void) {
  char buf[20];
  char *txt;
  txt = malloc(ALLOWED_CHARS * sizeof(char));
  printf("Press \"Enter\" to enter text entry mode, then \"F5\" to finish:\n");
  fgets(buf, sizeof(buf), stdin);

  primitive_txt(NULL, txt);

  printf("Your entered text was: %s\n", txt);
  free(txt);
}
