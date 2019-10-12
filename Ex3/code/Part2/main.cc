#include "Hough.h"

#include <iostream>
#include <sstream>
#include <string>
using std::string;

string itoa(int i) {
	stringstream ss;
	string s;
	ss << i;
	ss >> s;
	return s;
}

int main(int argc, char const* argv[]) {
	for (int i = 1; i <= 9; i++) {
		string path = itoa(i) + ".bmp";
		std::cout << path << std::endl;
		Hough hough(path);
		hough.hough_process(2 /*sigma*/, 0.56 /*tlow*/, 0.92 /*thigh*/,
			50 /*tgrad*/, 20 /*minR*/, 140 /*maxR*/, 180 /*tvote*/,
			40 /*center_dis*/);
	}
}
