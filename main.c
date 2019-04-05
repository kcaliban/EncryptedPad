/* EncryptedPad
 * ------------
 * Jot down short notes, making sure nobody can read them thanks to AES256
 * ------------
 * Encryption inspired by:
 * https://gitlab.tnichols.org/tyler/gcrypt/blob/master/encrypt_decrypt.c
*/
#include "main.h"


void usage() {
  printf("Usage: ./main [-e outf | -d inf]\n");
}

/* This can be done with ncurses instead */
int getpw(char pw[20]) {
  // https://www.gnu.org/software/libc/manual/html_node/getpass.html
  struct termios old, new;

  /* Try to turn off echoing */
  // Standard file descriptor of stdin is 0
  if (tcgetattr(0, &old) != 0)
    return 1;
  new = old;
  new.c_lflag &= ~ECHO;
  if (tcsetattr(0, TCSAFLUSH, &new) != 0)
    return 1;

  /* Read pw */
  fgets(pw, 20 * sizeof(char), stdin);

  /* Restore normal terminal behaviour */
  (void) tcsetattr(0, TCSAFLUSH, &old);

  return 0;
}

void encrypt_dialog(const char *file) {
  char str[300];
  char buf[20];
  char pw[20];
  int j = 0;
  char *line;

  while (1) {
    system("clear");
    printf("Enter whatever pops to your head,"
           " press Enter then CTRL+D to finish:\n");
    printf("-----------------------------------------------------------\n");
    // Read until CTRL+D is received (~ fgets returns NULL)
    while((line = fgets(str + j, sizeof(str) - (j * sizeof(char)), stdin))
            != NULL) {
      j += strlen(line);
      if (j * sizeof(char) >= sizeof(str))
        break;
    }
    printf("-----------------------------------------------------------\n");
    printf("Are you happy with your message?\nEnter yes, yeah, yah,"
           " ... for yes or anything not starting with \"y\" for no: ");
    fgets(buf, sizeof(buf), stdin);
    if (strncmp(buf, "Y", 1) == 0 | strncmp(buf, "y", 1) == 0)
      break;
  }

  printf("-----------------------------------------------------------\n");
  printf("Enter password to encrypt text with (max length: 20): ");
  getpw(pw);
  printf("\nEncrypting message to file %s\n", file);
  if (encrypt_text_to_file(str, file, pw)) {
    printf("An error occured when trying to encrypt text");
    return;
  }
  printf("Message encrypted successfully!\n");
}

void decrypt_dialog(const char *file) {
  char pw[20];
  char *text;

  printf("Enter password for decryption: ");
  getpw(pw);

  if (decrypt_text_from_file(file, pw, &text)) {
    printf("An error occured when trying to decrypt the file\n");
    return;
  }
  printf("\n%s", text);
}

void init_ncurses() {
  initscr();
  // One character at a time input
  cbreak();
  // Suppress automatic echoing
  // noecho();
  // Capture special keystrokes
  keypad(stdscr, TRUE);
}

int main(int argc, const char **argv) {
  // Initialize gcrypt library
  init_gcrypt();
  // Initialize ncurses
  // initscr();

  if (argc != 3) {
    printf("Wrong number of arguments!\n");
    usage();
    // endwin();
    return 1;
  }

  if (strncmp(argv[1], "-e", 2) == 0)
    encrypt_dialog(argv[2]);
  else if (strncmp(argv[1], "-d", 2) == 0)
    decrypt_dialog(argv[2]);
  else {
    printf("Invalid command!\n");
    usage();
    // endwin();
    return 1;
  }

  // endwin();
  return 0;
}
