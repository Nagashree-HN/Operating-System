#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{

  // Check if the user entered the number of ticks or forgot to pass the argument
  if (argc != 2) {
    fprintf(2, "Usage: sleep <ticks>\n");
    exit(1);
  }

  // Convert the ticks argument from a string to an integer
  int ticks = atoi(argv[1]);

  // Pause the process for the given number of ticks
  pause(ticks);

  // Exit the program after sleep is completed
  exit(0);
}