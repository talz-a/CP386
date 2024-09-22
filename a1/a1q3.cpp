#include <cstring>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

int main() {
  int pipe1[2], pipe2[2];

  if (pipe(pipe1) == -1) {
    cerr << "Pipe1 creation failed!" << endl;
    return 1;
  }

  pid_t pid1 = fork();
  if (pid1 == -1) {
    cerr << "Fork for child 1 failed!" << endl;
    return 1;
  }

  if (pid1 == 0) {
    close(pipe1[0]);

    close(1);
    dup(pipe1[1]);
    close(pipe1[1]);

    execlp("ps", "ps", "axo", "sess", NULL);
    cerr << "Exec for ps failed!" << endl;
    return 1;
  }

  if (pipe(pipe2) == -1) {
    cerr << "Pipe2 creation failed!" << endl;
    return 1;
  }

  pid_t pid2 = fork();
  if (pid2 == -1) {
    cerr << "Fork for child 2 failed!" << endl;
    return 1;
  }

  if (pid2 == 0) {
    close(pipe1[1]);

    close(0);
    dup(pipe1[0]);

    close(pipe1[0]);

    close(pipe2[0]);
    close(1);
    dup(pipe2[1]);

    close(pipe2[1]);

    execlp("sort", "sort", "-u", NULL);
    cerr << "Exec for sort failed!" << endl;
    return 1;
  }

  close(pipe1[0]);
  close(pipe1[1]);
  close(pipe2[1]);

  close(0);
  dup(pipe2[0]);

  close(pipe2[0]);

  execlp("wc", "wc", "-l", NULL);
  cerr << "Exec for wc failed!" << endl;
  return 1;
}
