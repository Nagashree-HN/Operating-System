#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "user/user.h"

// Search through a directory and its subdirectories
void
find(char *path, char *target)
{
  char buf[512];
  char *p;
  int fd;
  struct dirent de;
  struct stat st;
  char name[DIRSIZ + 1];

  // Open the starting path
  fd = open(path, O_RDONLY);

  if (fd < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  // Get information about the path
  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  // Make sure the path is a directory
  if (st.type != T_DIR) {
    close(fd);
    return;
  }

  // Make sure the new path will fit in the buffer
  if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
    fprintf(2, "find: path too long\n");
    close(fd);
    return;
  }

  strcpy(buf, path);
  p = buf + strlen(buf);

  // Add '/' before the file or directory name
  *p++ = '/';

  // Read each directory entry
  while (read(fd, &de, sizeof(de)) == sizeof(de)) {

    // Skip empty entries
    if (de.inum == 0) {
      continue;
    }

    // Copy the entry name into a normal C string
    memmove(name, de.name, DIRSIZ);
    name[DIRSIZ] = '\0';

    // Do not search "." or ".."
    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
      continue;
    }

    // Build the full path
    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = '\0';

    // Get information about this entry
    if (stat(buf, &st) < 0) {
      fprintf(2, "find: cannot stat %s\n", buf);
      continue;
    }

    // Search inside subdirectories
    if (st.type == T_DIR) {
      find(buf, target);
    }

    // Print the path when the file name matches
    else if (st.type == T_FILE && strcmp(name, target) == 0) {
      printf("%s\n", buf);
    }
  }

  close(fd);
}

int
main(int argc, char *argv[])
{
  // find needs a starting directory and a file name
  if (argc != 3) {
    fprintf(2, "Usage: find <path> <filename>\n");
    exit(1);
  }

  find(argv[1], argv[2]);

  exit(0);
}