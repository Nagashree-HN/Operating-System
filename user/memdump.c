#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void memdump(char *fmt, char *data, int len);

int
main(int argc, char *argv[])
{
  if (argc == 1) {
    printf("Example 1:\n");
    int a[2] = {61810, 2025};
    memdump("ii", (char *)a, sizeof(a));

    printf("Example 2:\n");
    memdump("S", "a string", 9);

    printf("Example 3:\n");
    char *s = "another";
    memdump("s", (char *)&s, 8);

    struct sss {
      char *ptr;
      int num1;
      short num2;
      char byte;
      char bytes[8];
    } example;

    example.ptr = "hello";
    example.num1 = 1819438967;
    example.num2 = 100;
    example.byte = 'z';
    strcpy(example.bytes, "xyzzy");

    printf("Example 4:\n");
    memdump("pihcS", (char *)&example, sizeof(example));

    printf("Example 5:\n");
    memdump("sccccc", (char *)&example, sizeof(example));
  } else if (argc == 2) {
    // format in argv[1], up to 512 bytes of data from standard input.
    char data[512];
    int n = 0;
    memset(data, '\0', sizeof(data));
    while (n < sizeof(data)) {
      int nn = read(0, data + n, sizeof(data) - n);
      if (nn <= 0)
        break;
      n += nn;
    }
    memdump(argv[1], data, strlen(data));
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}

void
memdump(char *fmt, char *data, int len)
{
  // your code goes here for task 04

  int pos = 0;

  // Go through each character in the format string
  for (int i = 0; fmt[i] != '\0'; i++) {
    char spec = fmt[i];

    // 'i' reads the next 4 bytes as an integer
    if (spec == 'i') {
      if (pos + 4 > len) {
        printf("memdump: not enough data for '%c'\n", spec);
        return;
      }

      int value;
      memmove(&value, data + pos, 4);

      printf("%d\n", value);
      pos += 4;
    }

    // 'p' reads the next 8 bytes and prints them in hexadecimal
    else if (spec == 'p') {
      if (pos + 8 > len) {
        printf("memdump: not enough data for '%c'\n", spec);
        return;
      }

      uint low;
      uint high;

      memmove(&low, data + pos, 4);
      memmove(&high, data + pos + 4, 4);

      // xv6 printf only prints 32 bits with %x,
      // so print the upper and lower parts separately
      printf("%x", high);
      printf("%x\n", low);

      pos += 8;
    }

    // 'h' reads the next 2 bytes as a short integer
    else if (spec == 'h') {
      if (pos + 2 > len) {
        printf("memdump: not enough data for '%c'\n", spec);
        return;
      }

      short value;
      memmove(&value, data + pos, 2);

      printf("%d\n", value);
      pos += 2;
    }

    // 'c' reads the next byte as a character
    else if (spec == 'c') {
      if (pos + 1 > len) {
        printf("memdump: not enough data for '%c'\n", spec);
        return;
      }

      printf("%c\n", data[pos]);
      pos += 1;
    }

    // 's' reads an 8-byte pointer to a string
    else if (spec == 's') {
      if (pos + 8 > len) {
        printf("memdump: not enough data for '%c'\n", spec);
        return;
      }

      char *str;
      memmove(&str, data + pos, 8);

      printf("%s\n", str);
      pos += 8;
    }

    // 'S' prints the string stored directly in data
    else if (spec == 'S') {

      // Stop at the null character or the end of valid data
      while (pos < len && data[pos] != '\0') {
        printf("%c", data[pos]);
        pos++;
      }

      printf("\n");

      // Move past the null character if one was found
      if (pos < len && data[pos] == '\0') {
        pos++;
      }
    }
  }
}
