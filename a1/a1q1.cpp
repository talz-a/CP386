#include <cstdlib>
#include <iostream>
#include <unistd.h>

using namespace std;

int main() {
  int grandparent_id = getpid();
  int rc = fork(); // 1 p -> rc = 1, 2 c rc = 0

  if (rc < 0) {
    cerr << "fork failed" << endl;
    exit(1);
  }

  if (rc == 0) {
    int parent_id = getpid();
    int child_rc = fork();

    if (child_rc < 0) {
      cerr << "fork failed" << endl;
      exit(1);
    }

    if (child_rc == 0) {
      int grandchild_id = getpid();
      cout << "My process id is " << grandchild_id << ", my parent's id is "
           << parent_id << ", and my grandparent's id is " << grandparent_id
           << endl;
    }
  }

  return 0;
}
