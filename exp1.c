#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

int main() {
    char buf[10];

    int fd = creat("a.txt", 0644);   // create a file
    write(fd, "Hello", 5);           // write to it
    close(fd);

    fd = open("a.txt", O_RDONLY);
    read(fd, buf, 5);                // read from it
    buf[5] = '\0';
    close(fd);
    printf("File says: %s\n", buf);

    int pid = fork();                // make a child process
    printf("Child PID: %d\n",getpid());
    if (pid == 0) {
        sleep(100);                  // child just sleeps
    }
    kill(pid, SIGKILL);              // parent kills the child
    printf("Child killed\n");
    return 0;
}
