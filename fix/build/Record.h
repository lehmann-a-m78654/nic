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
#include <unistd.h>
#include <string>
#include <ctype.h>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <exception>
// #include <filesystem>
// namespace fs = std::filesystem;

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
#define MAX_PARAM_LEN 999999

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

bool isValidInt(char* c);
bool validFileName(string s);
bool validToken(string s);
int strlen(char* c);

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
		
		bool checkConsistency(std::vector<Record>& personRecs);
	public:
		bool lettersOnly(std::string s);
        std::vector<std::string> splitOn(std::string s, char delimeter);
		std::vector<Record> getLogsForPerson(std::string& contents, bool create);
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
        return lhs.roomNum > rhs.roomNum;
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
        // cout << "failure 46" << endl; //FIXME
        valid = false;
		// cout << "wrong record size" << endl; //FIXME
        return;
    }
	// cout << "rec 0" << endl; //FIXME
	// cout << s << endl; //FIXME
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
        if (n.find("nurse.") == 0) {
		// if (found == 0) {
            doctor = false;
            name = n.substr(6, n.size() - 6);
        }
    }
    else {
        valid = false;
		// cout << "failure 1" << endl; //FIXME
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
			// cout << "failure 2" << endl; //fixme
            return;
        }
    }

    if (record.size() == 4) {
		// cout << record.at(3) << endl; //FIXME
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
		// cout << "failure 3" << endl; //afixme
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
			// cout << "failure 4" << endl; //FIXME
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
		else if (strcmp(optarg, "error") == 0) {
			valid = false;
            // cout << "failure 47" << endl; //FIXME
			return;
		}
        if ((strlen(optarg) > MAX_PARAM_LEN) || (strlen(optarg) <= 0)) {
            valid = false;
			// cout << "failure 5" << endl; //fIXME
            return;
        }
        switch(args.at(i)) {
            case 'T':
                if(optarg != nullptr) {
                    if ((atoi(optarg) >= 0) && (atoi(optarg) <= INT_MAX) && isValidInt(optarg)) {
                        timestamp = atoi(optarg);
                        argPresent.at(T) = true;
                    }
                    else {
                        valid = false;
						// cout << "failure 6" << endl; //FIXME
                        return;
                    }
                }
                break;
            case 'K':
                if(optarg != nullptr && validToken(string(optarg))) {
                    token = string(optarg);
                    argPresent.at(K) = true;
                }
				else {
					valid = false;
                    // cout << "failure 48" << endl; //FIXME
					return;
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
					// cout << "failure 7" << endl;
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
					// cout << "failure 8" << endl;
                    return;
                }
                break;
            case 'R':
                if((optarg != nullptr) && (atoi(optarg) >= 0) && isValidInt(optarg)) {
                    room = atoi(optarg);
                    argPresent.at(R) = true;
                }
				else if (!isValidInt(optarg)) {
                    // cout << "failure 49" << endl; //FIXME
					valid = false;
				}
                break;
            case 'F':
                if(optarg != nullptr && validFileName(string(optarg))) {
                    logfile = string(optarg) + ".txt";
                    argPresent.at(F) = true;
                }
				else {
                    // cout << "failure 50" << endl; //FIXME
					valid = false;
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
				// cout << "failure 9" << endl; //FIXME
                return;
                break;
        }
		int numPresent = 0;
		for (int i = 0; i < argPresent.size(); i++) {
			if (argPresent.at(i)) {
				numPresent++;
			}
		}
    }
    
    if (!checkParamCombosApp()) {
        valid = false;
		// cout << "failure 10" << endl; //FIXME
        return;
    }
    
}

char* Record::parseAppendOpts(int size, char* argv[], char arg) {
	char* ret = nullptr;
    for (int i = 0; i < size - 1; i++) {
        char argWithDash[3];
        argWithDash[0] = '-';
        argWithDash[1] = arg;
        argWithDash[2] = '\0'; 
        if (strcmp(argv[i], argWithDash) == 0) {
			// check for double flags
			if(ret != nullptr) {
				char tmp[6] = "error";
				ret = tmp;
				return ret;
			}
            ret = argv[i + 1];
        }
    }
	// cout << "failure 11" << endl;
    return ret;
}

bool Record::lettersOnly(string s) {
    //check that ascii is 65-90 or 97-122
    // cout << s << endl; //FIXME
    if (s.size() < 1) {
		// cout << "failure 12" << endl;
        return false;
    }
    for (int i = 0; i < s.size(); i++) {
        if (((int)s.at(i) < 65) || ((int)s.at(i) > 122)) {
			// cout << "failure 13" << endl;
            return false;
        }
        else if (((int)s.at(i) > 90) && ((int)s.at(i) < 97)) {
			// cout << "failure 14" << endl;
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
				// cout << "failure 15" << endl;
                return false;
            }
        }
        return true;
    }

    //timestamp, token, name, logfile name must be present
    if (!argPresent.at(T) || !argPresent.at(K) || !argPresent.at(F)) {
		// cout << "failure 16" << endl;
        return false;
    }

    if (name.size() < 1) {
		// cout << "failure 17" << endl;
        return false;
    }

    //either doctor or nurse, but not both
    if ((argPresent.at(N) && argPresent.at(D)) || (!argPresent.at(N) && !argPresent.at(D))) {
		// cout << "failure 18" << endl;
        return false;
    }

    //either arriving or leaving, but not both
    if((argPresent.at(A) && argPresent.at(L)) || (!argPresent.at(A) && !argPresent.at(L))) {
		// cout << "failure 19" << endl;
        return false;
    }

    return true;

}

//returns logs for the relevant doctor/nurse (doctor name, nurse name)
vector<Record> Record::getLogsForPerson(string& contents, bool create) {
    if (!readFile(contents, create) || !valid) {
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
    vector<string> pieces = splitOn(logfile, '/');
    string filename = pieces.at(pieces.size() - 1);
    pieces = splitOn(filename, '.');
    filename = pieces.at(0);
    pieces = splitOn(filename, '_');
    filename = pieces.at(0);
    // cout << filename << endl; //FIXME
    string t = filename + this->token;
    // cout << t << endl;
	contents = decrypt(contents, t);
    // cout << "contents " << contents << endl; //FIXME
    if (contents.size() == 0) {
        // cout << "failure integrity 1" << endl; //FIXME
        throw string("integrity violation");
    }
    
	// cout << "contents " << contents << endl; //FIXME
    // cout << "filename to be in contents " << filename << endl; //fixme
	// remove "file" from the beginning
	int fi = contents.find(filename);
	// cout << contents << endl; //FIXME
	if (fi == string::npos && contents.size() < filename.size()) {
		vector<Record> error;
        error.push_back(Record());
		// cout << "failure 40, integrity problem" << endl;
        throw string("integrity violation");
        return error;
	}
	//if it wasn't in the begnning, we have a problem
	else if (fi != 0 && contents.size() < 5) {
		vector<Record> error;
        error.push_back(Record());
		// cout << "failure 41, integrity problem" << endl;
        throw string("integrity violation");
        return error;
	}
	if (contents.size() < 5) {
		//cout << " old contents: " << contents << endl; //FIXME
		contents = contents.substr(filename.size(), contents.size() - 1);
		//cout << "new contents: " << contents << endl; //FIXME
	}
	

	// cout << "decrypted contents with token " << this->token << endl; //FIXME
	// cout << contents << endl; //FIXME

    // if (contents.find(logfile) == string::npos) {
    //     vector<Record> error;
    //     error.push_back(Record());
	// 	// cout << "failure 21" << endl;
    //     return error;
    // }
	
    vector<string> recs = splitOn(contents, '\n');
	

    vector<string> relevant;
	// cout << "relevatn records" << endl; //FIXME
    if (!relevantRecords(recs, relevant)) {
        vector<Record> error;
        error.push_back(Record());
        // cout << "failure 22 integrity violation" << endl;
        throw string("integrity violation");
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
		// cout << "failure 23" << endl;
        return false;
    }
    string contents = "";
    vector<Record> records = getLogsForPerson(contents, true);
	if (records.size() > 0) {
		if (records.at(0).isValid() == false) {
			// cout << "failure 24" << endl;
			return false;
		}
	}
    
    // for(int i = 0; i < records.size(); i++) {
	//     cout << records.at(i).toString() << endl; //FIXME
    // }
	// cout << "logs for person done" << endl; //FIXME
    //do sanity checks here
	//add new record to vector to check for consistency
	// records.push_back(this->toString());
    if (!checkConsistency(records)) {
		// cout << "failure 25" << endl;
        return false;
    }

	// cout << "consistentcy check done" << endl; //FIXME
    
    contents = contents + "\n" + this->toString();
	// cout << "done adding to contents " << endl; //FIXME
    vector<string> pieces = splitOn(logfile, '/');
    string filename = pieces.at(pieces.size() - 1);
    pieces = splitOn(filename, '.');
    filename = pieces.at(0);
    pieces = splitOn(filename, '_');
    filename = pieces.at(0);
    // cout << filename << endl; //FIXME
    //encrypt contents
    // if (!encrypt(contents, token)) {
    //     return false;
    // }
	// cout << "plaintext: " << contents << endl; //FIXME
    string t = filename + this->token;
	contents = encrypt(contents, t);
	// cout << "ciphertext: " << contents << endl; //FIXME
	// cout << "done encrypting with token \"" << token << "\"" << endl; //fixme

    //write new contents into file
    if (!writeFile(contents)) {
		// cout << "failure 26" << endl;
        return false;
    }
    return true;
}

bool Record::checkConsistency(vector<Record>& personRecs) {

    //check timestamp consistency
    for (int i = 1; i < personRecs.size(); i++) {
        if(personRecs.at(i).getTimestamp() <= personRecs.at(i - 1).getTimestamp()) {
			// cout << "failure 27" << endl;
            return false;
        }
    }
	// cout << personRecs.size() << endl; //FIXME
	if (personRecs.size() > 0) {
		// cout << personRecs.at(personRecs.size() - 1).getTimestamp() << endl; //FIXME
		// cout << timestamp << endl; //FIXME
		// cout << personRecs.at(personRecs.size() - 1).toString() << endl; //FIXME
	}
	
    if ((personRecs.size() > 0) && (personRecs.at(personRecs.size() - 1).getTimestamp() >= timestamp)) {
        // cout << "failure 29" << endl;
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
            // cout << "failure 30" << endl;
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
            // cout << "failure 31" << endl;
			return false;
        }

        //check for entrance to room 
        //if arriving into an actual room but haven't entered the hospital or left a room, we have a problem
        if (personRecs.at(i).isArriving() && (personRecs.at(i).getRoom() != -1) && (!inHospital || inRoom)) {
            // cout << "failure 32" << endl;
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
            // cout << "failure 33" << endl;
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
            // cout << "failure 34" << endl;
			return false;
        }
        else if (!personRecs.at(i).isArriving() && (personRecs.at(i).getRoom() == -1) && inHospital && !inRoom) {
            loggedRoom = -1;
            inHospital = false;
            continue;
        }
        else {
			// cout << "failure 35" << endl;
            return false;
        }
    }


    return true;

}

bool Record::readFile(string& contents, bool create) {
    if (!valid) {
		// cout << "failure 35" << endl;
        return false;
    }
    contents = "";
    fstream log;
	//remove leading /es
	if (logfile.find('/') != string::npos && logfile.find('/') < 2) {
		logfile = logfile.substr(logfile.find('/') + 1, logfile.size() - logfile.find('/') - 1);
	}
	// cout << "reading file " << logfile << endl; //FIXME
	// check not accessing parent directory
	int cdcount = 0;
	vector<string> dirs = splitOn(logfile, '/');
	for(int i = 0; i < dirs.size(); i++) {
		if (dirs.at(i) == "..") {
			cdcount++;
		}
		else {
			cdcount--;
		}
        // cannot traverse above directory
        if (cdcount > 0) {
            return false;
        }
	}
	if (cdcount > -1) {
		// cout << "failure 43" << endl; //FIXME
		return false;
	}
	//create the directories
	cdcount = 0; 
	for (int i = 0; i < dirs.size(); i++) {
		//the file
		if (i == dirs.size() - 1) {
			continue;
		}
		// else if (dirs.at(i) == "..") {
		// 	cdcount--;
		// 	cout << "cd .." << endl; //FIXME
		// 	chdir(string("..").c_str());
		// }
		struct stat info;
		// can access it and it is a directory
		if (opendir(dirs.at(i).c_str()) != NULL) {
			// cout << "cd to existing " << dirs.at(i) << endl; //FIXME
			chdir(string(dirs.at(i)).c_str());
			// system("pwd > LOOKATME"); //FIXME
			cdcount++;
		}
		//cannot access it or can access it and it is a file
		else if (create){
			if (dirs.at(i).size() == 0) {
				continue;
			}
			// cout << "making directory " << dirs.at(i) << endl; //FIXME
			system(string("mkdir " + dirs.at(i)).c_str());
			chdir(string(dirs.at(i)).c_str());
			cdcount++;
		}
		else {
			return false;
		}
	}
	//while (cdcount > 0) {
	if (cdcount > 0) {
		chdir(string("..").c_str());
		cdcount--;
		// cout << "pwd is " << endl; //FIXME
		// system("pwd");
		// last directory + filename
		logfile = dirs.at(dirs.size() - 2) + "/" + dirs.at(dirs.size() - 1);
		// cout << "log file is " << logfile << endl; //FIXME
	}
	// cout << "opening logfile " << logfile << endl; //FIXME
    bool success = false;
    log.open(logfile, fstream::in);
    if(log.fail() && create) {
        log.open(logfile, fstream::out);
        //log << "";
        string newcont = "file\n";
        vector<string> pieces = splitOn(logfile, '/');
        string filename = pieces.at(pieces.size() - 1);
        pieces = splitOn(filename, '.');
        filename = pieces.at(0);
        pieces = splitOn(filename, '_');
        filename = pieces.at(0);
        // cout << filename << endl; //FIXME
		// cout << "encrypting " << newcont << endl; //FIXME
        string t = filename + this->token;
        newcont = encrypt(newcont, t);
		// cout <<"ciphertext is \"" << newcont << "\"" << endl; //FIXME
        // log << newcont << endl;
        log.close();
		
        log.open(logfile, fstream::in);
    }
    else if (log.fail() && !create) {
        return false;
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
        success = true;
        log.close();
    }
    else {
		// cout << "failure 36" << endl;
		// perror("36");
		cout << logfile << endl; //FIXME
		// system("pwd > LOOKIE"); // FIXME
        return false;
    }
    if (!create && !success) {
        return false;
    }
    if (contents.length() < 2 && !create) {
        return false;
    }
    return true;
}

bool Record::writeFile(const string& text) {
    if (!valid) {
		// cout << "failure 37" << endl;
        return false;
    }
    fstream log;
	//remove leading /es
	if (logfile.find('/') != string::npos && logfile.find('/') < 2) {
		logfile = logfile.substr(logfile.find('/') + 1, logfile.size() - logfile.find('/') - 1);
	}
	// else {
	// 	cout << "not the case, logfile " << logfile << endl; //FIXME
	// }
	// cout << "writing to " << logfile << endl; //FIXME
    log.open(logfile, fstream::out);
    if (log.is_open() && log.good()) {
        log << text;
        log.close();
    }
    else {
		// cout << "failure 38" << endl;
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
        vector<string> pieces = splitOn(logfile, '/');
        string filename = pieces.at(pieces.size() - 1);
        pieces = splitOn(filename, '.');
        filename = pieces.at(0);
        pieces = splitOn(filename, '_');
        filename = pieces.at(0);
        // cout << filename << endl; //FIXME
        if (record.size() > 0 && record.at(0) == filename) {
            continue;
        }
        if ((record.size() != 4) && (record.size() != 3)) {
			// cout << "failure 39, integrity violation" << endl;
            // cout << "record size " << record.size() << endl; //FIXME
            // cout << record.at(0) << endl; //afixme
			// for (int i = 0; i < record.size(); i++) { //fixme
			// 	cout << record.at(i) << endl;
			// }

            throw string("integrity violation");
            // cout << "life goes on" << endl; //FIXME
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
		// cout << "failure 37" << endl; //FIXME
        priority_queue<string, vector<string>, greater<string> > error;
        error.push("error");
        return error;
    }
    if (contents.size() <= 0) {
		// cout << "failure 38" << endl; //FIXME
        return priority_queue<string, vector<string>, greater<string> >();
    }
	

    // if (!decrypt(contents, token)) {
    //     priority_queue<string, vector<string>, greater<string> > error;
    //     error.push("error");
    //     return error;
    // }
	// cout << "encrypted contents: \"" << contents << "\"" << endl; //FIXME
    vector<string> pieces = splitOn(logfile, '/');
    string filename = pieces.at(pieces.size() - 1);
    pieces = splitOn(filename, '.');
    filename = pieces.at(0);
    pieces = splitOn(filename, '_');
    filename = pieces.at(0);
    // cout << filename << endl; //FIXME
    string t = filename + this->token;
    // cout << t << endl; //FIXME
	contents = decrypt(contents, t);
    // cout << "contents " << contents << endl; //FIXME
    // cout << (int)contents.at(0) << endl;
    if (contents.size() == 0) {
        // cout << "failure integrity 2" << endl; //FIXME
        throw string("integrity violation");
    }

	
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
            // cout << "failure 43" << endl; //FIXME
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
            // cout << "failure 44" << endl; //fixme
            valid = false;
            break;
        }

        //check for entrance to room 
        //if arriving into an actual room but haven't entered the hospital or left a room, we have a problem
        if (personRecs.at(i).isArriving() && (personRecs.at(i).getRoom() != -1) && (!inHospital || inRoom)) {
            // cout << "failure 45" << endl; //FIXme
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
            // cout << "failure 51" << endl; //FIXME
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
            // cout << "failure 52" << endl; //FIXME
            valid = false;
            break;
        }
        else if (!personRecs.at(i).isArriving() && (personRecs.at(i).getRoom() == -1) && inHospital && !inRoom) {
            loggedRoom = -1;
            inHospital = false;
            continue;
        }
        else {
            // cout << "failure 53" << endl; //FIXME
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

bool isValidInt(char* c) {
  uint64_t size = strlen(c);
  if (size > 10) {
    return false;
  }
  uint64_t sum = 0;
  for (int i = 0; i < size; i++) {
    // ascii digit chars are 48-57
    if ((static_cast<int>(c[i]) < 48) || (static_cast<int>(c[i]) > 57)) {
      return false;
    }
    sum = sum + ((static_cast<int>(c[i]) - '0') * (size - i));
    if (sum > INT_MAX) {
      return false;
    }
  }
  return true;
}

bool validFileName(string s) {
	//max size 4096
	if (s.size() > 4096) {
		return false;
	}
	//check that ascii is 65-90, 97-122, 48-57 (numbers), 95 (_), 46 (.), 47 (/)
    if (s.size() < 1) {
		// cout << "failure 12" << endl;
        return false;
    }
    for (int i = 0; i < s.size(); i++) {
        if (((int)s.at(i) >= 97) && ((int)s.at(i) <= 122)) {
			// cout << "failure 13" << endl;
            continue;
        }
        else if (((int)s.at(i) <= 90) && ((int)s.at(i) > 64)) {
			// cout << "failure 14" << endl;
            continue;
        }
		else if (((int)s.at(i) >= 48) && ((int)s.at(i) <= 57)) {
			continue;
		}
		else if (((int)s.at(i) == 95) || ((int)s.at(i) == 46) || ((int)s.at(i) == 47)) {
			continue;
		}
		else {
			// cout << "failed on " << s.at(i) << endl; //FIXME
			return false;
		}
    }
    vector<string> parts = Record().splitOn(s, '/');
    // filename can only be 255 chars
    if (parts.at(parts.size() - 1).size() > 255) {
        return false;
    }
    return true;
	
}

bool validToken(string s) {
	//max size 4096
	// if (s.size() > 4096) {
	// 	return false;
	// }
	//check that ascii is 65-90, 97-122, 48-57 (numbers)
    if (s.size() < 1) {
		// cout << "failure 12" << endl;
        return false;
    }
    for (int i = 0; i < s.size(); i++) {
        if (((int)s.at(i) >= 97) && ((int)s.at(i) <= 122)) {
			// cout << "failure 13" << endl;
            continue;
        }
        else if (((int)s.at(i) <= 90) && ((int)s.at(i) > 64)) {
			// cout << "failure 14" << endl;
            continue;
        }
		else if (((int)s.at(i) >= 48) && ((int)s.at(i) <= 57)) {
			continue;
		}
		else {
			// cout << "failed on " << s.at(i) << endl; //FIXME
			return false;
		}
    }
    return true;
	
}

int strlen(char* c) {
  int64_t size = 0;
  for (int i = 0; i < MAX_PARAM_LEN; i++) { 
    if (c[i] == '\0') {
      return size;
      break;
    }
    size++;
    if (size > INT_MAX) {
      break;
    }
  }
  return -1;
}


// void handleErrors(void)
// {
//   ERR_print_errors_fp(stderr);
//   abort();
// }