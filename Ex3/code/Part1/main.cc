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
	for (int i = 1; i <= 6; i++) {
		string path = itoa(i) + ".bmp";
		std::cout << path << std::endl;
		Hough hough(path);
		hough.hough_process(2.0/*sigma*/, 0.27/*tlow*/, 0.99/*thigh*/, 20/*tvote*/, 30/*tgrad*/, 60/*peakdis*/);
	}
}
