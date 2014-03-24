#include <iostream>
#include <cstdio>

// #include <SPTK/SPTK.h> // if you use installed header
#include "SPTK.h"

using namespace std;

int main()
{
  cout << "Hello SPTK!" << endl;
  for (int i = 0; i < 10; ++i) {
    cout << gexp(2.0, i) << endl;
  }
  return 0;
}
