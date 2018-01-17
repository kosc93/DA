#include <iostream>


#include "win.h"

using namespace std;


int main(){
  int a,b,c;
  a=1;
  b=2;
  c=a+b;
  cout << c;
  win my_window;
  my_window.set_background_color(200,200,200);
  my_window.wait_until_closed();
  return 0;
}