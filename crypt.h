#include "io.h"
#include <gcrypt.h>
#include "primitive_txt.h" // ALLOWED_CHARS

void init_gcrypt();
int init_aes256(gcry_cipher_hd_t *, unsigned char *, unsigned char*);

int decrypt_text_from_file(const char *, char *, char (*)[ALLOWED_CHARS]);
int encrypt_text_to_file(char *, const char *, char *);
