#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string>
#include "division.h"
using std::string;
using std::to_string;

int main() {
  Division::num_cnt = 0;
  for (int i = 1; i <= 8; i++) {
    char path[20];
    sprintf(path, "./train/%d.bmp", i);
    Division div(string(path), to_string(i));
    div.divide(300, 50, 200, 10, "./output/train/", "./output/train/num/");
  }
  Division::num_cnt = 0;
  for (int i = 1; i <= 8; i++) {
    char path[20];
    sprintf(path, "./test/%d.bmp", i);
    Division div(string(path), to_string(i));
    div.divide(300, 50, 200, 10, "./output/test/", "./output/test/num/");
  }
}
