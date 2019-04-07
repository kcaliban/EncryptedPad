/* EncryptedPad
 * ------------
 * Jot down short notes, making sure nobody can read them thanks to AES256.
 * Number of characters is limited in primitve_txt.h
 *
 * Press F5 to cancel note editing mode.
 * ------------
 * Encryption inspired by:
 * https://gitlab.tnichols.org/tyler/gcrypt/blob/master/encrypt_decrypt.c
*/
#include "main.h"

void usage() {
  printf("Usage: ./encrn [-e file | -d file]\n");
  printf("\t\t-e file\t\t-\topens a new blank document in primitive text"
         " editor, saved to file with F5\n");
  printf("\t\t-d file\t\t-\topens file in primitive text editor,"
         " save changes with F5\n");
}

/* This could be done with ncurses instead */
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
  char str[ALLOWED_CHARS];
  char pw[20];

  primitive_txt(str);

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
  char text[ALLOWED_CHARS];
  char pw[20];

  printf("Enter password for decryption: ");
  getpw(pw);

  if (decrypt_text_from_file(file, pw, &text)) {
    printf("An error occured when trying to decrypt the file\n");
    return;
  }
  primitive_txt(text);
  if (encrypt_text_to_file(text, file, pw)) {
    printf("An error occured when trying to re-encrypt text");
    return;
  }
  printf("Message (possibly) edited and encrypted again successfully!\n");
}

int main(int argc, const char **argv) {
  // Initialize gcrypt library
  init_gcrypt();
  if (argc != 3) {
    printf("Wrong number of arguments!\n");
    usage();
    return 1;
  }

  if (strncmp(argv[1], "-e", 2) == 0)
    encrypt_dialog(argv[2]);
  else if (strncmp(argv[1], "-d", 2) == 0)
    decrypt_dialog(argv[2]);
  else {
    printf("Invalid command!\n");
    usage();
    return 1;
  }

  return 0;
}
