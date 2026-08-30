#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"
#include "user/user.h"

// Run the command for a file that was found
void
run_exec(char *path, char **cmd, int cmd_count)
{
  char *args[MAXARG];

  // Make sure there is enough space for all arguments
  if (cmd_count + 2 > MAXARG) {
    fprintf(2, "find: too many arguments\n");
    return;
  }

  // Copy the command and its arguments
  for (int i = 0; i < cmd_count; i++) {
    args[i] = cmd[i];
  }

  // Add the matched file path as the last argument
  args[cmd_count] = path;
  args[cmd_count + 1] = 0;

  int pid = fork();

  if (pid < 0) {
    fprintf(2, "find: fork failed\n");
    return;
  }

  // Child process runs the command
  if (pid == 0) {
    exec(args[0], args);

    // This line only runs if exec fails
    fprintf(2, "find: exec %s failed\n", args[0]);
    exit(1);
  }

  // Parent waits until the child is finished
  wait(0);
}

// Search through a directory and its subdirectories
void
find(char *path, char *target, int exec_mode, char **cmd, int cmd_count)
{
  char buf[512];
  char *p;
  int fd;
  struct dirent de;
  struct stat st;
  char name[DIRSIZ + 1];

  // Open the starting directory
  fd = open(path, O_RDONLY);

  if (fd < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  // Get information about the starting path
  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  // Only search if the path is a directory
  if (st.type != T_DIR) {
    close(fd);
    return;
  }

  // Make sure the path fits inside the buffer
  if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
    fprintf(2, "find: path too long\n");
    close(fd);
    return;
  }

  strcpy(buf, path);
  p = buf + strlen(buf);

  // Add '/' before the next file or directory name
  *p++ = '/';

  // Read each entry inside the directory
  while (read(fd, &de, sizeof(de)) == sizeof(de)) {

    // Skip unused directory entries
    if (de.inum == 0) {
      continue;
    }

    // Copy the entry name into a normal C string
    memmove(name, de.name, DIRSIZ);
    name[DIRSIZ] = '\0';

    // Do not search the current or parent directory
    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
      continue;
    }

    // Build the full path for this entry
    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = '\0';

    // Get information about this entry
    if (stat(buf, &st) < 0) {
      fprintf(2, "find: cannot stat %s\n", buf);
      continue;
    }

    // Search inside subdirectories
    if (st.type == T_DIR) {
      find(buf, target, exec_mode, cmd, cmd_count);
    }

    // Check if the file name matches the target
    else if (st.type == T_FILE && strcmp(name, target) == 0) {

      // Run the command when -exec is being used
      if (exec_mode) {
        run_exec(buf, cmd, cmd_count);
      }

      // Normal find behavior from Task 05
      else {
        printf("%s\n", buf);
      }
    }
  }

  close(fd);
}

int
main(int argc, char *argv[])
{
  // A path and file name are required
  if (argc < 3) {
    fprintf(2, "Usage: find <path> <filename> [-exec command]\n");
    exit(1);
  }

  // Normal find command from Task 05
  if (argc == 3) {
    find(argv[1], argv[2], 0, 0, 0);
  }

  // Handle the -exec option for task 06
  else {
    if (argc < 5 || strcmp(argv[3], "-exec") != 0) {
      fprintf(2, "Usage: find <path> <filename> [-exec command]\n");
      exit(1);
    }

    // argv[4] and later contain the command and its arguments
    find(argv[1], argv[2], 1, &argv[4], argc - 4);
  }

  exit(0);
}