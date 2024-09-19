#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <vector>

using namespace std;

int main() {
  ifstream input_file("./a1q2sample/input");
  vector<string> args;
  string line;

  if (!input_file.is_open()) {
    cerr << "Could not open input file" << endl;
    return 1;
  }

  while (getline(input_file, line)) {
    args.push_back(line);
  }

  int num_binaries = stoi(args[0]);

  for (int i = 1; i <= num_binaries; i++) {
    int rc = fork();

    if (rc < 0) {
      cerr << "Fork failed" << endl;
      exit(1);
    }

    if (rc == 0) {
      const char *binary_path = args[i].c_str();
      const char *exec_args[] = {binary_path, nullptr};

      execv(binary_path, const_cast<char *const *>(exec_args));
      cerr << "execv failed for " << binary_path << endl;
      exit(1);
    } else {
      wait(NULL);
    }
  }

  return 0;
}
