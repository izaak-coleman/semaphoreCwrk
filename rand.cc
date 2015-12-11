#include <iostream>
#include <cstdlib>

using namespace std;

int main(){
  char l = 'Y';
  srand( time(NULL) );
  int stall;

  while( l == 'Y' ){
    stall = (rand() % 3) + 2;
    cout << "Stall val: " << stall << "\n";
    cout << "Again? ";
    cin >> l;
    cout << "\n";
  }
}
