#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int main() {
  int fd;
  char text[] = "Hello Linux!\n";
  char buff[50];
  
  //Create and write to file
  fd = open("test.txt", O_CREAT | O_WRONLY, 0644);
  write(fd, text, sizeof(text));
  close(fd);

  //Read file
  fd = open("test.txt", O_RDONLY);
  read(fd, buf, sizeof(buf));
  printf("File content: %s", buf);
  close(fd);

  //Change file permisisons
  chmod("test.txt", 0600);

  printf("\nFile permissions changed to 600.\n");

  return 0;
