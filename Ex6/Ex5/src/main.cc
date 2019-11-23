#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string>
#include "division.h"
using std::string;
using std::to_string;

int main() {
  for (int i = 1; i <= 11; i++) {
    char path[20];
    sprintf(path, "./data/%d.bmp", i);
    Division div(string(path), to_string(i));
    div.divide(300, 50, 200, 10, "./output/", "./output/num/");
  }
}
