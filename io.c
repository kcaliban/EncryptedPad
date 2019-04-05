#include "io.h"

/* Functions for data writing and reading */
int write_to_file(const char *file, unsigned char *data, size_t size) {
  FILE *f = fopen(file, "wb");

  if (f == NULL) {
    fprintf(stderr, "Error: Unable to open file %s\n", file);
    return 1;
  }

  // Write data to file
  if (fwrite(data, 1, size, f) == 0) {
    fprintf(stderr, "Error in writing to file %s\n", file);
    return 1;
  }

  fclose(f);

  return 0;
}

size_t read_from_file(const char *file, unsigned char **data) {
  long file_size;
  size_t bytes_read;
  FILE *f = fopen(file, "rb");

  if (f == NULL) {
    fprintf(stderr, "Error: Unable to open file %s\n", file);
    return 1;
  }

  // Get file size
  fseek(f, 0, SEEK_END);
  file_size = ftell(f);
  fseek(f, 0, SEEK_SET);

  // Allocate memory
  *data = malloc(file_size + 1);
  if (*data == NULL) {
    fprintf(stderr,
            "Unable to allocate memory for file %s\n! File too large?",
            file);
    fclose(f);
    return 1;
  }

  // Read file into buffer
  if ((bytes_read = fread(*data, 1, file_size, f)) == 0) {
    fprintf(stderr, "Error in reading file %s\n", file);
    fclose(f);
    return 1;
  }

  fclose(f);

  return bytes_read;
}
