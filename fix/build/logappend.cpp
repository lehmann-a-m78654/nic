#include <iostream>
#include <stdlib.h>
#include <string>
#include "Record.h"

void printUsage();


using namespace std;
int main(int argc, char* argv[]) {
	 // -T <timestamp> -K <token> (-D <doctor-name> | -N <nurse-name>) (-A | -L) [-R <room-id>] -F <log>
    //printUsage(); //FIXME
	try {
		Record p = Record(argc, argv);
		if (!p.isValid()) {
			cout << "invalid" << endl;
			return 255;
		}

		if(!p.append()) {
			cout << "invalid" << endl;
			return 255;
		}
	}
	catch(string& e) {
		cout << e;
		return 255;
	}

	return 0;
}

void printUsage() {
	cout << "Usage: ./logappend -T <timestamp> -K <token> (-D <doctor-name> | -N <nurse-name>) (-A | -L) [-R <room-id>] -F <log>" << endl;
}