#include <iostream>
#include "helper.h"

using namespace std;

int main(){
  time_t start, end;
  time(&start);
  time(&end);
  cout << "process(1) time(" << difftime(end, start) << ") \n";
  sleep(2);
  time(&end);
  cout << "process(1) time(" << difftime(end, start) << ") \n";
  sleep(2);
  time(&end);
  cout << "process(1) time(" << difftime(end, start) << ") \n";
  sleep(2);
  time(&end);
  cout << "process(1) time(" << difftime(end, start) << ") \n";
  sleep(2);
  time(&end);
  cout << "process(1) time(" << difftime(end, start) << ") \n";
  sleep(2);


  return 0;
}
