#include <iostream>
#include <stdlib.h>
#include <string>
#include <limits.h>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <regex>
#include "Record.h"

using namespace std;

void printUsage();
bool validateParams(vector<bool> argsPresent);
bool handleS(string token, string logfile);
bool handleR(string token, string logfile, bool doctor, string name);

int main(int argc, char* argv[]) {
	 // ./logread -K <token> -S -F <log> OR\n./logread -K <token> -R (-D <name> | -N <name>) -F <log>
    Record r = Record();
    vector<bool> argsPresent(NUM_PARAMS, false);
    bool stateMode = false;
    bool modeSet = false;
    string token = "";
    bool doctor = false;
    string name = "";
    bool valid = true;
    string logfile = "";
    int size = argc;
    //printUsage(); //fixme

	try {
		//check for -S and -R, remove if they are present
		for (int i = 0; i < size; i++) {
			if(strlen(argv[i]) > MAX_PARAM_LEN) {
				continue;
			}
			if (strcmp(argv[i], "-S") == 0 && !modeSet) {
				argsPresent.at(S) = true;
				stateMode = true;
				modeSet = true;
				// for (int i = 0; i < size; i++) { //FIXME
				//     cout << argv[i] << " ";
				// }
				// cout << endl; //FIXME
				//remove the A, move everything over
				for (int j = i; j < size - 1; j++) {
					if (strlen(argv[j + 1]) > strlen(argv[j])) {
						argv[j] = new char[strlen(argv[j + 1]) + 1];
					}
					strcpy(argv[j], argv[j + 1]);
				}
				strcpy(argv[size - 1], "\0");
				size--;
			}
			else if (strcmp(argv[i], "-R") == 0 && !modeSet) {
				argsPresent.at(R) = true;
				stateMode = false;
				modeSet = true;
				strcpy(argv[i], "\0");
				//remove the L, move everything over
				strcpy(argv[i], "\0");
				for (int j = i; j < size - 1; j++) {
					if (strlen(argv[j + 1]) > strlen(argv[j])) {
						argv[j] = new char[strlen(argv[j + 1]) + 1];
					}
					strcpy(argv[j], argv[j + 1]);
				}
				strcpy(argv[size - 1], "\0");
				size--;
			}
			else if (((strcmp(argv[i], "-S") == 0) || (strcmp(argv[i], "-R") == 0)) && modeSet) {
				valid = false;
				cout << "invalid" << endl;
				return 255;
			}
		}

		vector<char> args;
		//args.push_back('S');
		args.push_back('K');
		args.push_back('D');
		args.push_back('N');
		// args.push_back('R');
		args.push_back('F');
		
		bool printState = false;
		
		for (int i = 0; i < args.size(); i++) {
			char* optarg = r.parseAppendOpts(size, argv, args.at(i));
			if (optarg == nullptr) {
				continue;
			}
			if ((strlen(optarg) > MAX_PARAM_LEN) || (strlen(optarg) <= 0)) {
				valid = false;
				cout << "invalid" << endl;
				return 255;
			}
			switch(args.at(i)) {
				case 'S':
					if(optarg != nullptr) {
						if ((atoi(optarg) >= 0) && (atoi(optarg) < INT_MAX)) {
							printState = false;
							argsPresent.at(S) = true;
						}
						else {
							valid = false;
							cout << "invalid" << endl;
							return 255;
						}
					}
					break;
				case 'K':
					if(optarg != nullptr) {
						token = string(optarg);
						argsPresent.at(K) = true;
					}
					break;
				case 'D':
					if((optarg != nullptr) && r.lettersOnly(optarg)) {
						doctor = true;
						name = string(optarg);
						argsPresent.at(D) = true;
					}
					else if (!r.lettersOnly(optarg)) {
						valid = false;
						cout << "invalid" << endl;
						return 255;
					}
					break;
				case 'N':
					if((optarg != nullptr) && r.lettersOnly(optarg)) {
						doctor = false;
						name = string(optarg);
						argsPresent.at(N) = true;
					}
					else if (!r.lettersOnly(optarg)) {
						valid = false;
						cout << "invalid" << endl;
						return 255;
					}
					break;
				case 'F':
					if(optarg != nullptr && validFileName(string(optarg))) {
						logfile = string(optarg) + ".txt";
						argsPresent.at(F) = true;
					}
					break;
				default:
					valid = false;
					cout << "invalid" << endl;
					return 255;
					break;
			}
			//cout << "found arg " << args.at(i) << " with thing " << optarg << endl; //FIXME
		}
		// cout << "validating params" << endl; //FIXME
		
		if (!validateParams(argsPresent)) {
			valid = false;
			cout << "invalid" << endl;
			return 255;
		}
		// cout << "starting handle" << endl; //fIXME
		if (argsPresent.at(S)) {
			// cout << "handling S" << endl; //FIXME
			if (!handleS(token, logfile)) {
				// cout << "failure 57" << endl; //fIXME
				cout << "invalid" << endl;
				return 255;
			}
		}
		else if (argsPresent.at(R)) {
			// cout << "handling R" << endl; //FIXME
			if (!handleR(token, logfile, doctor, name)) {
				// cout << "failure 58" << endl; //FIXME
				cout << "invalid" << endl;
				return 255;
			}
		}
		else {
			cout << "invalid" << endl;
			// cout << "failure 59" << endl; //FIXME
			return 255;
		}
	}
	catch(string& e) {
		// cout << "caught" << endl; //fIXME
		cout << e;
		// cout << "CAUGT" << endl; //FIXME
		return 255;
	}
	// cout << "done" << endl; //FIXME

    return 0;
}

void printUsage() {
	cout << "Usage: ./logread -K <token> -S -F <log> OR\n./logread -K <token> -R (-D <name> | -N <name>) -F <log>" << endl;
}

bool validateParams(vector<bool> argsPresent) {
	if (argsPresent.at(S)) {
		for (int i = 0; i < argsPresent.size(); i++) {
			if ((i == K || i == F) && !argsPresent.at(i)) {
				return false;
			}
			else if ((i != K && i != F && i != S) && argsPresent.at(i)) {
				return false;
			}
		}
	}
	else if (argsPresent.at(R)) {
		//either doctor or nurse, but not both
	    if ((argsPresent.at(N) && argsPresent.at(D)) || (!argsPresent.at(N) && !argsPresent.at(D))) {
	        return false;
	    }

	    if (!argsPresent.at(K)) {
	    	return false;
	    }

	    if (!argsPresent.at(F)) {
	    	return false;
	    }
	}
	else {
		return false;
	}

	return true;
}

bool handleS(string token, string logfile) {

	Record r = Record();
	r.setToken(token);
	r.setLogfile(logfile);
	r.setValid(true);
	string contents = "";
	// cout << "getting all names" << endl; //FIXME
	priority_queue<string, vector<string>, greater<string> > names = r.getAllNames(contents);

	if (names.empty() || names.top() == "error") {
		// cout << "failure 39" << endl; //FIXME
		// throw string("integrity error");
		return false;
	}

	string prevName = "";
	vector<State> people;
	priority_queue<Room, vector<Room>, RoomCompare > rooms;
	//get locations of all doctors
	// cout << "getting doctor locations" << endl; //FIXME
	while (!names.empty()) {
		// cout << "name " << names.top() << endl; //FIXME
		//weed out duplicate names
		if (names.top() == prevName) {
			names.pop();
			continue;
		}
		string fileContents = "";
		int nurse = names.top().find("nurse.");
		if (nurse != string::npos) {
			r.setDoctor(false);
			r.setName(names.top().substr(6, names.top().size() - 6));
			// cout << "\tnew name rec" << r.getName() << endl; //FIXME
		}
		else {
			r.setDoctor(true);
			r.setName(names.top().substr(3, names.top().size() - 3));
		}
		vector<Record> nameRecords = r.getLogsForPerson(fileContents,false);
		for (int i = 0; i< nameRecords.size(); i++) { //fixme
			// cout << "\t\trecord " << nameRecords.at(i).toString() << endl;
		}
		State person = r.getState(nameRecords); 

		if (!person.valid) {
			// cout << "failure 60" << endl; //FIXME
			return false;
		}
		people.push_back(person);

		//add room to rooms
		Room personRoom;
		personRoom.roomNum = person.room;
		personRoom.name = person.name;
		personRoom.doctor = person.doctor;
		rooms.push(personRoom);

		prevName = names.top();
		names.pop();
	}

	//print all doctors in hospital
	// cout << "----------------------" << endl; //FIXME
	bool firstprint = true;
	for (int i = 0; i < people.size(); i++) {
		// cout << "in doctor loop " << people.at(i).name << endl; //FIXME
		if (people.at(i).doctor && people.at(i).inHospital) {
			if (((i != 0) && (people.at(i - 1).name != people.at(i).name)) || (i == 0)) {
				if (!firstprint) {
					cout << ",";
				}
				cout << people.at(i).name;
				firstprint = false;
			}
		}
	}
	cout << endl;
	firstprint = true;
	//print all nurses in hospital
	for (int i = 0; i < people.size(); i++) {
		// cout << "in nurse loop " << people.at(i).name << endl; //FIXME
		if (!people.at(i).doctor && people.at(i).inHospital) {
			if (!firstprint) {
				cout << ",";
			}
			cout << people.at(i).name;
			firstprint = false;
		}
	}
	cout << endl;
	// cout << "----------------------" << endl; //FIXME
	bool firstroomprinted = false;
	//print rooms with doctors/nurses inside them
	int lastRoomNum = -2;
	priority_queue<string, vector<string>, greater<string> > namesInRoom;
	while (!rooms.empty()) {
		if (rooms.top().roomNum == -1) {
			rooms.pop();
			continue;
		}
		if(rooms.top().roomNum != lastRoomNum) {
			//get all names associated with this room
			int currRoom = rooms.top().roomNum;
			while (!rooms.empty() && (rooms.top().roomNum == currRoom)) {
				namesInRoom.push(rooms.top().name);
				rooms.pop();
			}
			lastRoomNum = currRoom;

			if (!namesInRoom.empty()) {
				if (firstroomprinted) {
					cout << endl;
				}
				cout << currRoom << ": ";
				firstroomprinted = true;
			}
			firstprint = true;
			while (!namesInRoom.empty()) {
				if (!firstprint) {
					cout << ",";
				}
				cout << namesInRoom.top();
				namesInRoom.pop();
				firstprint = false;
			}
			
		}
	}
	if (!namesInRoom.empty()) {

	}

	return true;
}

bool handleR(string token, string logfile, bool doctor, string name) {
	Record r;
	r.setToken(token);
	r.setLogfile(logfile);
	r.setDoctor(doctor);
	r.setName(name);
	r.setValid(true);

	string fileContents = "";
	vector<Record> nameRecords = r.getLogsForPerson(fileContents, false);
	State person = r.getState(nameRecords); 

	if (!person.valid) {
		return false;
	}

	for (int i = 0; i < person.rooms.size(); i++) {
		cout << person.rooms.at(i) << ",";
	}
	cout << endl;
		
	return true;
}