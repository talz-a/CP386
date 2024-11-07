#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sys/wait.h>
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

  int num_binaries;
  try {
    num_binaries = stoi(args[0]);
  } catch (const exception &) {
    cerr << "Could not parse number of binaries" << endl;
    return 1;
  }

  for (int i = 1; i <= num_binaries; i++) {
    const pid_t rc = fork();

    if (rc < 0) {
      cerr << "Fork failed" << endl;
      return 1;
    }

    if (rc == 0) {
      const char *binary_path = args[i].c_str();
      const char *exec_args[] = {binary_path, nullptr};

      execv(binary_path, const_cast<char *const *>(exec_args));
      cerr << "execv failed for " << binary_path << endl;
      return 1;
    }

    wait(NULL);
  }

  return 0;
}
