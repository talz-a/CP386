#include <cstring>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

int main() {
  int fd[2];
  const string cmds[3] = {"ps axo sess", "sort -u ", "wc -l"};

  if (pipe(fd) == -1) {
    cerr << "Pipe creation failed!" << std::endl;
    return 1;
  }

  pid_t pid = fork();

  if (pid == -1) {
    cerr << "Fork failed!" << std::endl;
    return 1;
  }

  if (pid == 0) {
    close(fd[1]);

    close(0);
    dup(fd[0]);

    close(fd[0]);
    execlp("wc", "wc", NULL);

    cerr << "Exec failed!" << std::endl;
  } else {
    close(fd[0]);
    // do the things in here in parent
    close(fd[1]);
  }

  return 0;
}
