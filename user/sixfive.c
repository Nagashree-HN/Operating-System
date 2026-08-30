#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int fd;
  char ch;
  char number[32];
  int index;
  int valid;

  // Check if the user provided at least one input file
  if (argc < 2) {
    fprintf(2, "Usage: sixfive <file>\n");
    exit(1);
  }

  // Process each input file given on the command line
  for (int file = 1; file < argc; file++) {

    // Open the input file
    fd = open(argv[file], O_RDONLY);

    // Check if the file was opened successfully
    if (fd < 0) {
      fprintf(2, "Error: Cannot open file %s\n", argv[file]);
      continue;
    }

    index = 0;
    valid = 1;

    // Read the input file one character at a time
    while (read(fd, &ch, 1) == 1) {

      // Check if the character is one of the separators
      if (strchr(" -\r\t\n./,", ch)) {

        // Process the token when a separator is reached
        if (index > 0 && valid) {
          number[index] = '\0';

          int value = atoi(number);

          // Print numbers that are multiples of 5 or 6
          if (value % 5 == 0 || value % 6 == 0) {
            printf("%d\n", value);
          }
        }

        // Reset for the next token
        index = 0;
        valid = 1;

      } else {

        // Store the character if it is a digit
        if (ch >= '0' && ch <= '9') {

          if (index < sizeof(number) - 1) {
            number[index++] = ch;
          } else {
            // Mark the token invalid if the number is too long
            valid = 0;
          }

        } else {
          // Mark the token invalid if it contains a non-digit character
          valid = 0;
        }
      }
    }

    // End of file acts like a separator
    if (index > 0 && valid) {
      number[index] = '\0';

      int value = atoi(number);

      if (value % 5 == 0 || value % 6 == 0) {
        printf("%d\n", value);
      }
    }

    // Close the input file
    close(fd);
  }

  exit(0);
}