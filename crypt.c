#include "crypt.h"

#define GCRYPT_VERS "1.8.1"

void init_gcrypt() {
  // Check if the correct version of GCRYPT is used and initialize
  // all subsystems
  if (!gcry_check_version(GCRYPT_VERS)) {
    fputs("Wrong libcrypt version installed.\n", stderr);
    // endwin();
    exit(2);
  }

  // Secure memory initialization of gcrypt
  // Documentation recommends disabling warnings for secure memory allocation
  gcry_control(GCRYCTL_SUSPEND_SECMEM_WARN);
  // Allocate 16k of secure memory
  // (region of memory set aside for sensitive data)
  gcry_control(GCRYCTL_INIT_SECMEM, 16384, 0);
  gcry_control(GCRYCTL_RESUME_SECMEM_WARN);
  // Initialization completed
  gcry_control(GCRYCTL_INITIALIZATION_FINISHED, 0);
}

int init_aes256(gcry_cipher_hd_t *handle, unsigned char *key,
                 unsigned char* iv) {
  gcry_error_t error;

  // Open cipher handle using AES256 w/ ciper-block chaining,
  // usage of secure memory and ciphertext stealing (no padding required)
  error = gcry_cipher_open(handle,
                           GCRY_CIPHER_AES256,
                           GCRY_CIPHER_MODE_CBC,
                           GCRY_CIPHER_SECURE | GCRY_CIPHER_CBC_CTS);
  if (error) {
    fprintf(stderr, "Error in cipher_open: %s/%s\n", gcry_strsource(error),
            gcry_strerror(error));
    return 1;
  }

  // Set key, key size of AES256 is 32
  error = gcry_cipher_setkey(*handle, key, 32);
  if (error) {
    fprintf(stderr, "Error in cipher_setkey: %s/%s\n", gcry_strsource(error),
            gcry_strerror(error));
    gcry_cipher_close(*handle);
    return 1;
  }

  // Set input vector, block size of AES256 is 16
  error = gcry_cipher_setiv(*handle, iv, 16);
  if (error) {
    fprintf(stderr, "Error in cipher_setiv: %s/%s\n", gcry_strsource(error),
            gcry_strerror(error));
    gcry_cipher_close(*handle);
    return 1;
  }

  return 0;
}

int decrypt_text_from_file(const char *infile, char *password, char **outtext) {
  unsigned char kdf_salt[8], init_vector[16], aes_key[32],
                *packed_data;
  size_t file_size, ciphertext_len;
  gcry_cipher_hd_t handle;
  gcry_error_t error;

  /* Reading the file and unpacking its data */
  // Read file
  if ((file_size = read_from_file(infile, &packed_data)) == 0) {
    return 1;
  }

  // Allocate memory for ciphertext
  ciphertext_len = (file_size - 8) - 16;
  *outtext = malloc(ciphertext_len);
  if (*outtext == NULL) {
    fprintf(stderr, "Error: Could not allocate memory for ciphertext!"
                    " File too large?\n");
    free(packed_data);
    return 1;
  }

  // Unpack data
  memcpy(kdf_salt, packed_data, 8);
  memcpy(init_vector, packed_data + 8, 16);
  memcpy(*outtext, packed_data + 8 + 16, ciphertext_len);
  /* End of data unpacking */

  /* Key derivation */
  error = gcry_kdf_derive(password, strlen(password), GCRY_KDF_PBKDF2,
                          GCRY_MD_SHA512, kdf_salt, 8, 10000,
                          32, aes_key);
  if (error) {
    fprintf(stderr, "Error in kdf_derive: %s/%s\n", gcry_strsource(error),
            gcry_strerror(error));
    return 1;
  }
  /* End of key derivation */

  /* Decryption of encrypted text */
  if (init_aes256(&handle, aes_key, init_vector)) {
    return 1;
  }

  error = gcry_cipher_decrypt(handle, *outtext, ciphertext_len, NULL, 0);
  if (error) {
    fprintf(stderr, "Error in cipher_decrypt: %s/%s\n", gcry_strsource(error),
            gcry_strerror(error));
    free(*outtext);
    gcry_cipher_close(handle);
    return 1;
  }
  /* End of decryption */

  gcry_cipher_close(handle);
  return 0;
}

int encrypt_text_to_file(char *text, const char *outfile, char *password) {
  unsigned char kdf_salt[8], init_vector[16], aes_key[32], *ciphertext,
                *packed_data;
  size_t blocks;
  gcry_cipher_hd_t handle;
  gcry_error_t error;

  /* Key derivation */
  // Generate a pseudorandom 64 bit salt for key derivation
  gcry_create_nonce(kdf_salt, 8);
  // Create key using PBKDF2 with SHA512
  // NIST recommends 10000 iterations
  error = gcry_kdf_derive(password, strlen(password), GCRY_KDF_PBKDF2,
                          GCRY_MD_SHA512, kdf_salt, 8, 10000,
                          32, aes_key);
  if (error) {
    fprintf(stderr, "Error in kdf_derive: %s/%s\n", gcry_strsource(error),
            gcry_strerror(error));
    return 1;
  }
  /* End of key derivation */

  /* IV generation and encryption */
  // Generate a pseudorandom 16 byte init vector for AES
  gcry_create_nonce(init_vector, 16);
  // Initialize AES encryption
  if (init_aes256(&handle, aes_key, init_vector)) {
    return 1;
  }
  // Calculate number of needed AES blocks
  blocks = strlen(text) / 16;
  if (strlen(text) % 16 != 0)
    blocks++;
  // Buffer for in-place encryption
  ciphertext = malloc(blocks * 16);
  if (ciphertext == NULL) {
    fprintf(stderr, "Unable to allocate memory for ciphertext!\n");
    return 1;
  }
  // Copy text into buffer
  memcpy(ciphertext, text, blocks * 16);
  // Perform in-place encryption
  error = gcry_cipher_encrypt(handle, ciphertext, blocks * 16, NULL, 0);
  if (error) {
    fprintf(stderr, "Error in cipher_encrypt: %s/%s\n", gcry_strsource(error),
            gcry_strerror(error));
    free(ciphertext);
    return 1;
  }
  /* End of IV generation and encryption */

  /* Data packing for saving */
  // Allocate memory for salt, IV and encrypted text
  packed_data = malloc(8 + 16 + blocks * 16);
  if (packed_data == NULL) {
    fprintf(stderr, "Unable to allocate memory for packed_data!\n");
    gcry_cipher_close(handle);
    return 1;
  }
  // Copy data into allocated space
  memcpy(packed_data, kdf_salt, 8);
  memcpy(packed_data + 8, init_vector, 16);
  memcpy(packed_data + 8 + 16, ciphertext, blocks * 16);
  /* End of data packing */

  // Save packed data to file
  write_to_file(outfile, packed_data, 8 + 16 + blocks * 16);
  gcry_cipher_close(handle);
  return 0;
}
