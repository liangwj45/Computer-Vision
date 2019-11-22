#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string>
#include "division.h"
using std::string;
using std::to_string;

int main() {
  for (int i = 1; i <= 8; i++) {
    char path[20];
    sprintf(path, "./trainingset/%d.bmp", i);
    Division div(string(path), to_string(i));
    div.divide(300, 40, 200, 10, "./result/trainingset/",
               "./result/trainingset/num/");
  }
  for (int i = 1; i <= 8; i++) {
    char path[20];
    sprintf(path, "./testingset/%d.bmp", i);
    Division div(string(path), to_string(i));
    div.divide(300, 40, 200, 10, "./result/testingset/",
               "./result/testingset/num/");
  }
}
