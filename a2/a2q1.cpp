#include <iostream>
#include <pthread.h>
#include <vector>

using namespace std;

struct threadargs {
  int row;
  vector<vector<int>> *m1;
  vector<vector<int>> *m2;
  vector<vector<int>> *res;
  int y;
  int y1;
};

void *multiply_row(void *arg) {
  threadargs *args = (threadargs *)arg;
  int i = args->row;
  for (int j = 0; j < args->y1; j++) {
    (*args->res)[i][j] = 0;
    for (int k = 0; k < args->y; k++) {
      (*args->res)[i][j] += (*args->m1)[i][k] * (*args->m2)[k][j];
    }
  }
  return nullptr;
}

int main() {
  int x, y, x1, y1;

  cout << "enter the dimensions of matrix a (rows and columns): ";
  cin >> x >> y;

  cout << "enter the dimensions of matrix b (rows and columns): ";
  cin >> x1 >> y1;

  if (y != x1) {
    cout << "error: enter valid matrix dimensions" << endl;
    return 1;
  }

  vector<vector<int>> m1(x, vector<int>(y));
  vector<vector<int>> m2(x1, vector<int>(y1));
  vector<vector<int>> res(x, vector<int>(y1));

  cout << "enter the elements of matrix a:" << endl;
  for (int i = 0; i < x; i++)
    for (int j = 0; j < y; j++)
      cin >> m1[i][j];

  cout << "enter the elements of matrix b:" << endl;
  for (int i = 0; i < x1; i++)
    for (int j = 0; j < y1; j++)
      cin >> m2[i][j];

  pthread_t threads[x];
  threadargs args[x];

  for (int i = 0; i < x; i++) {
    args[i].row = i;
    args[i].m1 = &m1;
    args[i].m2 = &m2;
    args[i].res = &res;
    args[i].y = y;
    args[i].y1 = y1;

    int rc =
        pthread_create(&threads[i], nullptr, multiply_row, (void *)&args[i]);
    if (rc) {
      cout << "Error: unable to create thread," << rc << endl;
      return 1;
    }
  }

  for (int i = 0; i < x; i++) {
    pthread_join(threads[i], nullptr);
  }

  cout << "resulting matrix c:" << endl;
  for (int i = 0; i < x; i++) {
    for (int j = 0; j < y1; j++) {
      cout << res[i][j] << " ";
    }
    cout << endl;
  }

  return 0;
}
