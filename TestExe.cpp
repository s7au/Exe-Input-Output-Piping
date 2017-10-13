#include <iostream>

using namespace std;

int main(int args, char** argv) {
	string input;
	cin >> input;
	int step = 0;
	const char *testName = args > 1 ? argv[1] : "A";
	while (!cin.eof()) {
		step++;
		cout << "Test " << testName << " - Step " << step << ": " << input;
		cin >> input;
	}
}
