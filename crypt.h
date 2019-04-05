#include <curses.h> // Required for exit if wrong version of gcrypt is installed
#include "io.h"
#include <gcrypt.h>

void init_gcrypt();
int init_aes256(gcry_cipher_hd_t *, unsigned char *, unsigned char*);

int decrypt_text_from_file(const char *, char *, char **);
int encrypt_text_to_file(char *, const char *, char *);
