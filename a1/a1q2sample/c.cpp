#include <iostream>
#include <unistd.h>
#include <stdlib.h>

using namespace std;

int main(){
	int i;
	for(i=1; i<5; i++){
		sleep(rand()%3+1);
		cout << "c" << i << endl; 
	}
return 0;
}
