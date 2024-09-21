#include <cstdlib>
#include <iostream>
#include <unistd.h>

using namespace std;

int main() {
  const pid_t grandparent_id = getpid();
  const pid_t rc = fork();

  if (rc < 0) {
    cerr << "fork failed" << endl;
    return 1;
  }

  if (rc == 0) {
    const pid_t parent_id = getpid();
    const pid_t child_rc = fork();

    if (child_rc < 0) {
      cerr << "fork failed" << endl;
      return 1;
    }

    if (child_rc == 0) {
      const pid_t grandchild_id = getpid();
      cout << "My process id is " << grandchild_id << ", my parent's id is "
           << parent_id << ", and my grandparent's id is " << grandparent_id
           << endl;
    }
  }

  return 0;
}
