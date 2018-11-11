#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <queue>
#include <string.h>
#include <unordered_set>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <ctype.h>
#include <sstream>

#define T 0
#define K 1
#define D 2
// #define N 3
#define A 4
#define L 5
#define R 6
#define F 7
#define B 8
#define S 9
#define NUM_PARAMS 10
#define MAX_PARAM_LEN 256

int N = 3;

std::string AVAILABLE_CHARS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 ";

using namespace std;

// encryption library source: https://github.com/philipperemy/easy-encryption.git
static std::string base64_encode(const std::string &in) {

    std::string out;

    int val=0, valb=-6;
    for (int jj = 0; jj < in.size(); jj++) {
    	char c = in[jj];
        val = (val<<8) + c;
        valb += 8;
        while (valb>=0) {
            out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(val>>valb)&0x3F]);
            valb-=6;
        }
    }
    if (valb>-6) out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[((val<<8)>>(valb+8))&0x3F]);
    while (out.size()%4) out.push_back('=');
    return out;
}

static std::string base64_decode(const std::string &in) {

    std::string out;

    std::vector<int> Tt(256,-1);
    for (int i=0; i<64; i++) Tt["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i; 

    int val=0, valb=-8;
    for (int jj = 0; jj < in.size(); jj++) {
    	char c = in[jj];
        if (Tt[c] == -1) break;
        val = (val<<6) + Tt[c];
        valb += 6;
        if (valb>=0) {
            out.push_back(char((val>>valb)&0xFF));
            valb-=8;
        }
    }
    return out;
}

int index(char c) {
	for(int ii = 0; ii < AVAILABLE_CHARS.size(); ii++) {
		if(AVAILABLE_CHARS[ii] == c) {
			// std::cout << ii << " " << c << std::endl;
			return ii;
		}
	}
	return -1;
}

std::string extend_key(std::string& msg, std::string& key) {
	//generating new key
	int msgLen = msg.size();
	std::string newKey(msgLen, 'x');
	int keyLen = key.size(), i, j;
    for(i = 0, j = 0; i < msgLen; ++i, ++j){
        if(j == keyLen)
            j = 0;
 
        newKey[i] = key[j];
    }
    newKey[i] = '\0';
	return newKey;
}


std::string encrypt_vigenere(std::string& msg, std::string& key) {
	int msgLen = msg.size(), keyLen = key.size(), i, j;
 	std::string encryptedMsg(msgLen, 'x');
    // char newKey[msgLen], encryptedMsg[msgLen], decryptedMsg[msgLen];
 
	std::string newKey = extend_key(msg, key);
 
    //encryption
    for(i = 0; i < msgLen; ++i) {
    	// std::cout << msg[i] << " " << isalnum(msg[i]) << std::endl;
    	if(isalnum(msg[i]) or msg[i] == ' ') {
    		encryptedMsg[i] = AVAILABLE_CHARS[((index(msg[i]) + index(newKey[i])) % AVAILABLE_CHARS.size())];
    	} else {
    		encryptedMsg[i] = msg[i];
    	}
    }
	
    encryptedMsg[i] = '\0';
    return encryptedMsg; 
}

std::string decrypt_vigenere(std::string& encryptedMsg, std::string& newKey) {
	// decryption
	int msgLen = encryptedMsg.size();
	std::string decryptedMsg(msgLen, 'x');
	int i;
    for(i = 0; i < msgLen; ++i) {
    	if(isalnum(encryptedMsg[i]) or encryptedMsg[i] == ' ') {
    		decryptedMsg[i] = AVAILABLE_CHARS[(((index(encryptedMsg[i]) - index(newKey[i])) + AVAILABLE_CHARS.size()) % AVAILABLE_CHARS.size())];
    	} else {
    		decryptedMsg[i] = encryptedMsg[i];
    	}
    }
    decryptedMsg[i] = '\0';
	return decryptedMsg;
}


std::string encrypt(std::string& msg, std::string& key) {
	std::string b64_str = base64_encode(msg);
	std::string vigenere_msg = encrypt_vigenere(b64_str, key);
	// std::cout << vigenere_msg << std::endl;
	return vigenere_msg;
}


std::string decrypt(std::string& encrypted_msg, std::string& key) {
	std::string newKey = extend_key(encrypted_msg, key);
	std::string b64_encoded_str = decrypt_vigenere(encrypted_msg, newKey);
	std::string b64_decode_str = base64_decode(b64_encoded_str);
	return b64_decode_str;
}


class State {
public:
	std::vector<int> rooms;
	int room;
	bool inHospital;
	bool inRoom;
	bool valid;
	bool doctor;
	std::string name;
	State() { room = -1; inHospital = false; inRoom = false; valid = false; doctor = false; name = ""; }
};


class Record {
	private:
		int timestamp;
		std::string token;
		bool doctor;
		std::string name;
		bool arrive;
		int room;
		std::string logfile;
		bool batchMode;
		bool valid;
		std::vector<bool> argPresent;
		bool checkParamCombosApp();
		//bool checkParamCombosRead();
		//void parseArg(char c);
		bool writeFile(const std::string& text);
		// bool decrypt(std::string& contents, std::string key);
		// bool encrypt(std::string& contents, std::string key);
		bool relevantRecords(const std::vector<std::string>& recs, std::vector<std::string>& results);
		std::vector<std::string> splitOn(std::string s, char delimeter);
		bool checkConsistency(std::vector<Record>& personRecs);
	public:
		bool lettersOnly(std::string s);
		std::vector<Record> getLogsForPerson(std::string& contents);
		bool readFile(std::string& contents, bool create);
		std::priority_queue<std::string, std::vector<std::string>, std::greater<std::string> > getAllNames(std::string& contents);
		State getState(std::vector<Record> personRecs);
		char* parseAppendOpts(int size, char* argv[], char arg);
		Record(int argc, char* argv[]);
		Record();
		Record(std::string);
		bool append();
		std::string toString();
		bool isValid();
		void setValid(bool v);
		int getTimestamp();
		std::string getToken();
		void setToken(std::string t);
		//void setName(std::string n);
		void setDoctor(bool doc);
		bool isDoctor();
		std::string getName();
		void setName(std::string n);
		bool isArriving();
		int getRoom();
		std::string getLogFile();
		void setLogfile(std::string l);
		bool isBatchMode();
};


class Room {
public:
	int roomNum;
	std::string name;
	bool doctor;
	Room() { roomNum = -1; }
};

struct RoomCompare
{
    bool operator()(const Room& lhs, const Room& rhs)
    {
        return lhs.roomNum < rhs.roomNum;
    }
};

// int encryp(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext);
// int decryp(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext);
//void handleErrors(void);
#include <limits.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <regex>
#include "Record.h"

using namespace std;

Record::Record() {
    timestamp = INT_MAX;
    token = "";
    doctor = false;
    name = "";
    arrive = false;
    room = -1;
    logfile = "";
    batchMode = false;
    valid = false;

}

Record::Record(string s) {
    timestamp = INT_MAX;
    token = "";
    doctor = false;
    name = "";
    arrive = false;
    room = -1;
    logfile = "";
    batchMode = false;
    valid = true;

    vector<string> record = splitOn(s, ' ');
    if ((record.size() != 4) && (record.size() != 3)) {
        valid = false;
		// cout << "wrong record size" << endl; //FIXME
        return;
    }
	// cout << "rec 0" << endl; //FIXME
	// cout << record.at(0) << endl; //FIXME
	// cout << "rec 0 done " << endl; //FIXME
    timestamp = stoi(record.at(0));
    string n = record.at(1);
    int found = n.find("dr.");
    //dr. should be in 0th index
    if (found == 0 ) {
        doctor = true;
        name = n.substr(3, n.size() - 3);
    }
    else if (n.find("nurse.") == 0) {
        //nurse should be in 0th index
        // if (n.find("nurse.") == 0) {
		if (found == 0) {
            doctor = false;
            name = n.substr(6, n.size() - 6);
        }
    }
    else {
        valid = false;
		//cout << "failure 1" << endl; //FIXME
        return;
    }

    found = record.at(2).find("A");
    if (found == 0) {
        arrive = true;
    }
    else {
        found = record.at(2).find("L");
        if (found == 0) {
            arrive = false;
        }
        else {
            valid = false;
			//cout << "failure 2" << endl; //fixme
            return;
        }
    }

    if (record.size() == 4) {
        room = stoi(record.at(3));
    }
    
    logfile = "";
    batchMode = false;
    token = "";


}
Record::Record(int argc, char* argv[]) {
    timestamp = INT_MAX;
    token = "";
    doctor = false;
    name = "";
    arrive = false;
    room = -1;
    logfile = "";
    batchMode = false;
    bool arrivedSet = false;
    valid = true;

    argPresent = vector<bool>(NUM_PARAMS, false);

    //check number of args
    if (!((argc == 3) || (argc == 10) || (argc == 12))) {
        valid = false;
		//cout << "failure 3" << endl; //afixme
        return;
    }
    int size = argc;
    //check for -A and -L, remove if they are present
    for (int i = 0; i < size; i++) {
        if(strlen(argv[i]) > MAX_PARAM_LEN) {
            continue;
        }
        if (strcmp(argv[i], "-A") == 0) {
            argPresent.at(A) = true;
            arrive = true;
            arrivedSet = true;
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
        else if (strcmp(argv[i], "-L") == 0) {
            argPresent.at(L) = true;
            arrive = false;
            arrivedSet = true;
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
        else if (((strcmp(argv[i], "-A") == 0) || (strcmp(argv[i], "-L") == 0)) && arrivedSet) {
            valid = false;
			//cout << "failure 4" << endl; //FIXME
            return;
        }
    }

    int c ;
    vector<char> args;
    args.push_back('T');
    args.push_back('K');
    args.push_back('D');
    args.push_back('N');
    args.push_back('A');
    args.push_back('L');
    args.push_back('R');
    args.push_back('F');
    args.push_back('B');
    for (int i = 0; i < args.size(); i++) {
        char* optarg = parseAppendOpts(size, argv, args.at(i));
        if (optarg == nullptr) {
            continue;
        }
        if ((strlen(optarg) > MAX_PARAM_LEN) || (strlen(optarg) <= 0)) {
            valid = false;
			//cout << "failure 5" << endl; //fIXME
            return;
        }
        switch(args.at(i)) {
            case 'T':
                if(optarg != nullptr) {
                    if ((atoi(optarg) >= 0) && (atoi(optarg) < INT_MAX)) {
                        timestamp = atoi(optarg);
                        argPresent.at(T) = true;
                    }
                    else {
                        valid = false;
						//cout << "failure 6" << endl; //FIXME
                        return;
                    }
                }
                break;
            case 'K':
                if(optarg != nullptr) {
                    token = string(optarg);
                    argPresent.at(K) = true;
                }
                break;
            case 'D':
                if((optarg != nullptr) && lettersOnly(optarg)) {
                    doctor = true;
                    name = string(optarg);
                    argPresent.at(D) = true;
                }
                else if (!lettersOnly(optarg)) {
                    valid = false;
					//cout << "failure 7" << endl;
                    return;
                }
                break;
            case 'N':
                if((optarg != nullptr) && lettersOnly(optarg)) {
                    doctor = false;
                    name = string(optarg);
                    argPresent.at(N) = true;
                }
                 else if (!lettersOnly(optarg)) {
                    valid = false;
					//cout << "failure 8" << endl;
                    return;
                }
                break;
            case 'R':
                if((optarg != nullptr) && (atoi(optarg) >= 0)) {
                    room = atoi(optarg);
                    argPresent.at(R) = true;
                }
                break;
            case 'F':
                if(optarg != nullptr) {
                    logfile = string(optarg) + ".txt";
                    argPresent.at(F) = true;
                }
                break;
            case 'B':
                if(optarg != nullptr) { 
                    batchMode = true;
                    argPresent.at(B) = true;
                }
                break;
            default:
                valid = false;
				//cout << "failure 9" << endl; //FIXME
                return;
                break;
        }
    }
    
    if (!checkParamCombosApp()) {
        valid = false;
		//cout << "failure 10" << endl; //FIXME
        return;
    }
    
}

char* Record::parseAppendOpts(int size, char* argv[], char arg) {
    for (int i = 0; i < size - 1; i++) {
        char argWithDash[3];
        argWithDash[0] = '-';
        argWithDash[1] = arg;
        argWithDash[2] = '\0'; 
        if (strcmp(argv[i], argWithDash) == 0) {
            return argv[i + 1];
        }
    }
	// cout << "failure 11" << endl;
    return nullptr;
}

bool Record::lettersOnly(string s) {
    //check that ascii is 65-90 or 97-122
    if (s.size() < 1) {
		//cout << "failure 12" << endl;
        return false;
    }
    for (int i = 0; i < s.size(); i++) {
        if (((int)s.at(i) < 65) || ((int)s.at(i) > 122)) {
			//cout << "failure 13" << endl;
            return false;
        }
        else if (((int)s.at(i) > 90) && ((int)s.at(i) < 97)) {
			//cout << "failure 14" << endl;
            return false;
        }
    }
    return true;
}

bool Record::checkParamCombosApp() {
    //if batch, -B should be the only parameter
    if (argPresent.at(B)) {
        for (int i = 0; i < argPresent.size(); i++) {
            if (i != B && argPresent.at(i)) {
				//cout << "failure 15" << endl;
                return false;
            }
        }
        return true;
    }

    //timestamp, token, name, logfile name must be present
    if (!argPresent.at(T) || !argPresent.at(K) || !argPresent.at(F)) {
		//cout << "failure 16" << endl;
        return false;
    }

    if (name.size() < 1) {
		//cout << "failure 17" << endl;
        return false;
    }

    //either doctor or nurse, but not both
    if ((argPresent.at(N) && argPresent.at(D)) || (!argPresent.at(N) && !argPresent.at(D))) {
		//cout << "failure 18" << endl;
        return false;
    }

    //either arriving or leaving, but not both
    if((argPresent.at(A) && argPresent.at(L)) || (!argPresent.at(A) && !argPresent.at(L))) {
		//cout << "failure 19" << endl;
        return false;
    }

    return true;

}

//returns logs for the relevant doctor/nurse (doctor name, nurse name)
vector<Record> Record::getLogsForPerson(string& contents) {
    if (!readFile(contents, true) || !valid) {
        vector<Record> error;
        error.push_back(Record());
		// cout << "failure 20" << endl;
        return error;
    }
    if (contents.size() <= 0) {
        return vector<Record>();
    }

    // if (!decrypt(contents, token)) {
    //     vector<Record> error;
    //     error.push_back(Record());
    //     return error;
    // }
	contents = decrypt(contents, this->token);
	// remove "file" from the beginning
	int fi = contents.find("file");
	// cout << contents << endl; //FIXME
	if (fi == string::npos && contents.size() < 5) {
		vector<Record> error;
        error.push_back(Record());
		// cout << "failure 40, integrity problem" << endl;
        return error;
	}
	//if it wasn't in the begnning, we have a problem
	else if (fi != 0 && contents.size() < 5) {
		vector<Record> error;
        error.push_back(Record());
		// cout << "failure 41, integrity problem" << endl;
        return error;
	}
	if (contents.size() < 5) {
		//cout << " old contents: " << contents << endl; //FIXME
		contents = contents.substr(string("file").size(), contents.size() - 1);
		//cout << "new contents: " << contents << endl; //FIXME
	}
	

	// cout << "decrypted contents with token " << this->token << endl; //FIXME
	// cout << contents << endl; //FIXME

    // if (contents.find(logfile) == string::npos) {
    //     vector<Record> error;
    //     error.push_back(Record());
	// 	cout << "failure 21" << endl;
    //     return error;
    // }
	
    vector<string> recs = splitOn(contents, '\n');
	

    vector<string> relevant;
	// cout << "relevatn records" << endl; //FIXME
    if (!relevantRecords(recs, relevant)) {
        vector<Record> error;
        error.push_back(Record());
		// cout << "failure 22" << endl;
        return error;
    }
	// cout << "RECS FROM GETLOGS" << endl; //FIXME
	// for (int i = 0; i < relevant.size(); i++) {
	// 	cout << relevant.at(i) << endl; //FIXME
	// }
	// cout << "done w relevant records" << endl; //FIXME
    //turn lines into records, push them onto vector, return them
    vector<Record> relevantRecs;
    for (int i = 0; i < relevant.size(); i++) {
        relevantRecs.push_back(Record(relevant.at(i)));
    }
	// cout << "RELEVANTRECS from getlong" << endl; //FIXME
	// for (int i = 0; i < relevantRecs.size(); i++) {
	// 	cout << relevantRecs.at(i).toString() << endl; //FIXME
	// }
	// cout << "got relevant recs" << endl; //FIXME
    return relevantRecs;
}

bool Record::append() {
    if (!valid) {
		//cout << "failure 23" << endl;
        return false;
    }
    string contents = "";
    vector<Record> records = getLogsForPerson(contents);
	if (records.size() > 0) {
		if (records.at(0).isValid() == false) {
			cout << "failure 24" << endl;
			return false;
		}
	}
    
   
	// cout << "logs for person done" << endl; //FIXME
    //do sanity checks here
    if (!checkConsistency(records)) {
		//cout << "failure 25" << endl;
        return false;
    }

	// cout << "consistentcy check done" << endl; //FIXME
    
    contents = contents + "\n" + this->toString();
	// cout << "done adding to contents " << endl; //FIXME

    //encrypt contents
    // if (!encrypt(contents, token)) {
    //     return false;
    // }
	// cout << "plaintext: " << contents << endl; //FIXME
	contents = encrypt(contents, this->token);
	// cout << "ciphertext: " << contents << endl; //FIXME
	// cout << "done encrypting with token \"" << token << "\"" << endl; //fixme

    //write new contents into file
    if (!writeFile(contents)) {
		//cout << "failure 26" << endl;
        return false;
    }
    return true;
}

bool Record::checkConsistency(vector<Record>& personRecs) {

    //check timestamp consistency
    for (int i = 1; i < personRecs.size(); i++) {
        if(personRecs.at(i).getTimestamp() <= personRecs.at(i - 1).getTimestamp()) {
			//cout << "failure 27" << endl;
            return false;
        }
    }
    if ((personRecs.size() > 0) && (personRecs.at(personRecs.size() - 1).getTimestamp() >= timestamp)) {
        //cout << "failure 29" << endl;
		return false;
    }

    //add new proposed record to the end of it
    personRecs.push_back(Record(this->toString()));

    int loggedRoom = -1;
    bool inHospital = false;
    bool inRoom = false;
    for (int i = 0; i < personRecs.size(); i++) {
        //if not in the hospital and the next record is not an arrival at the hospital, we have a problem
        if (!inHospital && (!personRecs.at(i).isArriving() || (personRecs.at(i).getRoom() != -1))) {
            //cout << "failure 30" << endl;
			return false;
        }
        //successfully entering hospital
        else if (!inHospital && !inRoom && personRecs.at(i).isArriving() && (personRecs.at(i).getRoom() == -1)) {
            inHospital = true;
            inRoom = false;
            loggedRoom = -1;
            continue;
        }
        else if (inHospital && personRecs.at(i).isArriving() && (personRecs.at(i).getRoom() == -1)) {
            //cout << "failure 31" << endl;
			return false;
        }

        //check for entrance to room 
        //if arriving into an actual room but haven't entered the hospital or left a room, we have a problem
        if (personRecs.at(i).isArriving() && (personRecs.at(i).getRoom() != -1) && (!inHospital || inRoom)) {
            //cout << "failure 32" << endl;
			return false;
        }
        //if the record is arriving, we are not in a room already, we are in the hospital and trying to enter an actual room
        else if (personRecs.at(i).isArriving() && inHospital && !inRoom && (personRecs.at(i).getRoom() != -1)) {
            inRoom = true;
            loggedRoom = personRecs.at(i).getRoom();
            continue;
        }

        //check for leaving room
        //if not arriving, trying to leave actual room, and we are either not in a room or the logged room is not the room we are trying to leave, problem
        if (!personRecs.at(i).isArriving() && (personRecs.at(i).getRoom() != -1) && (!inHospital || !inRoom || (loggedRoom != personRecs.at(i).getRoom()))) {
            //cout << "failure 33" << endl;
			return false;
        }
        else if (!personRecs.at(i).isArriving() && (personRecs.at(i).getRoom() != -1) && inRoom && inHospital && (loggedRoom == personRecs.at(i).getRoom())) {
            inRoom = false;
            loggedRoom = -1;
            continue;
        }

        //check for leaving hospital
        //if we are leaving the hospital, 
        if (!personRecs.at(i).isArriving() && (personRecs.at(i).getRoom() == -1) && (!inHospital || inRoom)) {
            //cout << "failure 34" << endl;
			return false;
        }
        else if (!personRecs.at(i).isArriving() && (personRecs.at(i).getRoom() == -1) && inHospital && !inRoom) {
            loggedRoom = -1;
            inHospital = false;
            continue;
        }
        else {
			//cout << "failure 35" << endl;
            return false;
        }
    }


    return true;

}

bool Record::readFile(string& contents, bool create) {
    if (!valid) {
		//cout << "failure 35" << endl;
        return false;
    }
    contents = "";
    fstream log;
    log.open(logfile, fstream::in);
    if(log.fail() && create) {
        log.open(logfile, fstream::out);
        //log << "";
        string newcont = "file\n";
		// cout << "encrypting " << newcont << endl; //FIXME
        newcont = encrypt(newcont, this->token);
		// cout <<"ciphertext is \"" << newcont << "\"" << endl; //FIXME
        log << newcont << endl;
        log.close();
        log.open(logfile, fstream::in);
    }
    if (log.is_open() && log.good()) {
        string newline;
        int counter = 0;
        while (getline(log, newline) && (counter < contents.max_size())) {
			if (counter == 0) {
				contents = newline;
			}
			else {
				contents = contents + "\n" + newline;
			}
            
			// cout << "new line " << newline << endl; //FIXME
            counter++;
        }
        log.close();
    }
    else {
		//cout << "failure 36" << endl;
        return false;
    }
    return true;
}

bool Record::writeFile(const string& text) {
    if (!valid) {
		//cout << "failure 37" << endl;
        return false;
    }
    fstream log;
    log.open(logfile, fstream::out);
    if (log.is_open() && log.good()) {
        log << text;
        log.close();
    }
    else {
		//cout << "failure 38" << endl;
        return false;
    }
    return true;
}


bool Record::relevantRecords(const vector<string>& allRecs, vector<string>& results) {
	// cout << "getting relevant records of " << this->name << endl; //FIXME
    int i = 0;

    if (allRecs.size() > 0 && allRecs.at(0).size() == 0) {
        i++;
    }
    if ((allRecs.size() > 0) && (allRecs.at(0) == ("test" + logfile))) {
        i++;
    }
    for (i; i < allRecs.size(); i++) {
        if (allRecs.at(i).size() < 1) {
            continue;
        }
        vector<string> record = splitOn(allRecs.at(i), ' ');
        if (record.size() > 0 && record.at(0) == ("file")) {
            continue;
        }
        if ((record.size() != 4) && (record.size() != 3)) {
			// cout << "failure 39" << endl;
			// for (int i = 0; i < record.size(); i++) { //fixme
			// 	cout << record.at(i) << endl;
			// }
            return false;
        }
        string n = record.at(1);
        int found = n.find("dr.");
		// cout << "\t\t\t\tname is " << n << endl; //FIXME
        //dr. should be in 0th index
        if (found == 0 && doctor) {

            string recName = n.substr(3, n.size() - 3);
            if (recName == name) {
                results.push_back(allRecs.at(i));
            }
        }
        else if ((n.find("nurse.") == 0) && !doctor) {
			
            string recName = n.substr(6, n.size() - 6);
			// cout << "\t\t\t\trecname is " << recName << endl; //fixme
            if (recName == name) {
				// cout << "ALLRECS " << allRecs.at(i) << endl; //FIXME
                results.push_back(allRecs.at(i));
            }
        }
        else {
            continue;
        }
    }
	// cout << "RESULTS" << endl; //FIXME
	// for (int i = 0; i < results.size(); i++) {
	// 	cout << results.at(i) << endl; //FIXME
	// }
    return true;
}

//<timestamp> <dr/nurse>.<name> <A|L> ?<room #>
string Record::toString() {
    string rec = "";

    rec = to_string(timestamp) + " ";

    if (doctor) {
        rec = rec + "dr.";
    }
    else {
        rec = rec + "nurse.";
    }

    rec = rec + name + " ";

    if(arrive) {
        rec = rec + "A ";
    }
    else {
        rec = rec + "L ";
    }

    if(room > -1) {
        rec = rec + to_string(room);
    }

    return rec;
}

vector<string> Record::splitOn(string s, char delimeter) {
    vector<string> res;
    string temp = "";
    for (int i = 0; i < s.size(); i++) {
        if (s.at(i) == delimeter) {
            res.push_back(temp);
            temp = "";
            continue;
        }
        temp = temp + s.at(i);
    }
    if (temp != "") {
        res.push_back(temp);
    }
    return res;
}

priority_queue<string, vector<string>, greater<string> > Record::getAllNames(std::string& contents) {
    if (!readFile(contents, false) || !valid) {
		//cout << "failure 37" << endl; //FIXME
        priority_queue<string, vector<string>, greater<string> > error;
        error.push("error");
        return error;
    }
    if (contents.size() <= 0) {
		//cout << "failure 38" << endl; //FIXME
        return priority_queue<string, vector<string>, greater<string> >();
    }
	

    // if (!decrypt(contents, token)) {
    //     priority_queue<string, vector<string>, greater<string> > error;
    //     error.push("error");
    //     return error;
    // }
	// cout << "encrypted contents: \"" << contents << "\"" << endl; //FIXME
	contents = decrypt(contents, this->token);
	// cout << "decrypted contents: " << contents << endl; //FIXME
	// cout << "token is \"" << this->token << "\"" << endl; //FIXME
    // if (contents.find(logfile) == string::npos) {
    //     priority_queue<string, vector<string>, greater<string> > error;
    //     error.push("error");
    //     return error;
    // }
    priority_queue<string, vector<string>, greater<string> > names;
    vector<string> recs = splitOn(contents, '\n');
	// cout << "recs is len " << recs.size() << endl; //FIXME
    for (int i = 0; i < recs.size(); i++) {
        vector<string> record = splitOn(recs.at(i), ' ');
        if (record.size() < 2) {
            continue;
        }
        names.push(record.at(1));
		// cout << "new name " << record.at(1) << endl; //FIXME
    }

    return names;
}

State Record::getState(std::vector<Record> personRecs) {
    //all records must be from the same person
    int loggedRoom = -1;
    bool inHospital = false;
    bool inRoom = false;
    bool valid = true;
    string name = "";
    State s;
    for (int i = 0; i < personRecs.size(); i++) {
        name = personRecs.at(0).getName();
        //if not in the hospital and the next record is not an arrival at the hospital, we have a problem
        if (!inHospital && (!personRecs.at(i).isArriving() || (personRecs.at(i).getRoom() != -1))) {
            valid = false;
            break;
        }
        //successfully entering hospital
        else if (!inHospital && !inRoom && personRecs.at(i).isArriving() && (personRecs.at(i).getRoom() == -1)) {
            inHospital = true;
            inRoom = false;
            loggedRoom = -1;
            continue;
        }
        else if (inHospital && personRecs.at(i).isArriving() && (personRecs.at(i).getRoom() == -1)) {
            valid = false;
            break;
        }

        //check for entrance to room 
        //if arriving into an actual room but haven't entered the hospital or left a room, we have a problem
        if (personRecs.at(i).isArriving() && (personRecs.at(i).getRoom() != -1) && (!inHospital || inRoom)) {
            valid = false;
            break;
        }
        //if the record is arriving, we are not in a room already, we are in the hospital and trying to enter an actual room
        else if (personRecs.at(i).isArriving() && inHospital && !inRoom && (personRecs.at(i).getRoom() != -1)) {
            inRoom = true;
            loggedRoom = personRecs.at(i).getRoom();
            s.rooms.push_back(loggedRoom);
            continue;
        }

        //check for leaving room
        //if not arriving, trying to leave actual room, and we are either not in a room or the logged room is not the room we are trying to leave, problem
        if (!personRecs.at(i).isArriving() && (personRecs.at(i).getRoom() != -1) && (!inHospital || !inRoom || (loggedRoom != personRecs.at(i).getRoom()))) {
            valid = false;
            break;
        }
        else if (!personRecs.at(i).isArriving() && (personRecs.at(i).getRoom() != -1) && inRoom && inHospital && (loggedRoom == personRecs.at(i).getRoom())) {
            inRoom = false;
            loggedRoom = -1;
            continue;
        }

        //check for leaving hospital
        //if we are leaving the hospital, 
        if (!personRecs.at(i).isArriving() && (personRecs.at(i).getRoom() == -1) && (!inHospital || inRoom)) {
            valid = false;
            break;
        }
        else if (!personRecs.at(i).isArriving() && (personRecs.at(i).getRoom() == -1) && inHospital && !inRoom) {
            loggedRoom = -1;
            inHospital = false;
            continue;
        }
        else {
            valid = false;
            break;
        }
    }

    s.room = loggedRoom;
    s.inRoom = inRoom;
    s.inHospital = inHospital;
    s.valid = valid;
    s.doctor = doctor;
    s.name = name;
    return s;

}

bool Record::isValid() {
    return valid;
}

int Record::getTimestamp() {
    return timestamp;
}

string Record::getToken() {
    return token;
}

bool Record::isDoctor() {
    return doctor;
}

string Record::getName() {
    return name;
}

bool Record::isArriving() {
    return arrive;
}

int Record::getRoom() {
    return room;
}

string Record::getLogFile() {
    return logfile;
}

bool Record::isBatchMode() {
    return batchMode;
}

void Record::setToken(string t) {
    token = t;
}

void Record::setLogfile(string l) {
    logfile = l;
}

void Record::setDoctor(bool d) {
    doctor = d;
}

void Record::setName(string n) {
    name = n;
}

void Record::setValid(bool v) {
    valid = v;
}


// void handleErrors(void)
// {
//   ERR_print_errors_fp(stderr);
//   abort();
// }