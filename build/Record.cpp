
#include <limits.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
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
    // cout << "building record for " << s << endl; //FIXME
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
        return;
    }
    timestamp = stoi(record.at(0));
    string n = record.at(1);
    int found = n.find("dr.");
    // dr. should be in 0th index
    if (found == 0) {
        doctor = true;
        name = n.substr(3, n.size() - 3);
    } else if (n.find("nurse.") == 0) {
        // nurse should be in 0th index
        if (n.find("nurse.") == 0) {
            doctor = false;
            name = n.substr(6, n.size() - 6);
            // cout << "nurse name is " << name <<  endl; // FIXME
        } 
    } else {
        // cout << "not valid " << endl; // FIXME
        valid = false;
        return;
    }

    found = record.at(2).find("A");
    if (found == 0) {
        arrive = true;
    } else {
        found = record.at(2).find("L");
        if (found == 0) {
            arrive = false;
        } else {
            valid = false;
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

    // check number of args
    if (!((argc == 3) || (argc == 10) || (argc == 12))) {
        valid = false;
        return;
    }
    int size = argc;
    // check for -A and -L, remove if they are present
    for (int i = 0; i < size; i++) {
        if (strlen(argv[i]) > MAX_PARAM_LEN && (strlen(argv[i]) > 0)) {
            continue;
        }
        if (strcmp(argv[i], "-A") == 0) {
            argPresent.at(A) = true;
            arrive = true;
            arrivedSet = true;
            // remove the A, move everything over
            for (int j = i; j < size - 1; j++) {
                if (strlen(argv[j + 1]) > strlen(argv[j])) {
                    argv[j] = new char[strlen(argv[j + 1]) + 1];
                }
                // cout << "copying " << argv[j + 1] << endl; //FIXME
                strcpy(argv[j], argv[j + 1]);
                // snprintf(argv[j], strlen(argv[j + 1]), argv[j + 1]);
            }
            strcpy(argv[size - 1], "\0");
            // snprintf(argv[size - 1], 1, "\0");
            size--;
        } else if (strcmp(argv[i], "-L") == 0) {
            argPresent.at(L) = true;
            arrive = false;
            arrivedSet = true;
            strcpy(argv[i], "\0");
            // remove the L, move everything over
            strcpy(argv[i], "\0");
            for (int j = i; j < size - 1; j++) {
                if (strlen(argv[j + 1]) > strlen(argv[j])) {
                    argv[j] = new char[strlen(argv[j + 1]) + 1];
                }
                strcpy(argv[j], argv[j + 1]);
            }
            strcpy(argv[size - 1], "\0");
            size--;
        } else if (((strcmp(argv[i], "-A") == 0) || 
                (strcmp(argv[i], "-L") == 0)) && arrivedSet) {
            valid = false;
            return;
        }
    }

    int c;
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
    // cout << "parsed a and l, looking onward now" << endl; //FIXME
    for (int i = 0; i < args.size(); i++) {
        char* optarg = parseAppendOpts(size, argv, args.at(i));
        // cout << args.at(i) << endl; //FIXME
        if (optarg == nullptr) {
            continue;
        }
        //  cout << "testing length of  " << optarg << endl; //FIXME
        if ((strlen(optarg) > MAX_PARAM_LEN) || (strlen(optarg) <= 0)) {
            // cout << strlen(optarg) << endl; //FIXME
            valid = false;
            return;
        }
        // cout << "switch on " << args.at(i) << endl;  // FIXME 
        switch (args.at(i)) {
            case 'T':
                if ((optarg != nullptr) && isValidInt(optarg)) {
                    if ((atoi(optarg) >= 0) && (atoi(optarg) < INT_MAX))  {
                        // cout << "T is " << endl;  // FIXME
                        timestamp = atoi(optarg);
                        argPresent.at(T) = true;
                    } else {
                        // cout << "T messed up" << endl; //FIXME
                        valid = false;
                        return;
                    }
                }
                break;
            case 'K':
                if (optarg != nullptr) {
                    // cout << "K is " << optarg << endl; //FIXME
                    token = string(optarg);
                    argPresent.at(K) = true;
                }
                break;
            case 'D':
                // cout << "case D" << endl; //FIXME
                if ((optarg != nullptr) && lettersOnly(optarg)) {
                    // cout << "D is fine" << optarg << endl; //FIXME
                    doctor = true;
                    name = string(optarg);
                    argPresent.at(D) = true;
                } else if (!lettersOnly(optarg)) {
                    // cout << "D messed up " << optarg <<endl; //FIXME
                    valid = false;
                    return;
                }
                break;
            case 'N':
                if ((optarg != nullptr) && lettersOnly(optarg)) {
                    doctor = false;
                    name = string(optarg);
                    argPresent.at(N) = true;
                } else if (!lettersOnly(optarg)) {
                     // cout << "N messed up" <<endl; //FIXME
                    valid = false;
                    return;
                }
                break;
            case 'R':
                if ((optarg != nullptr) && isValidInt(optarg) && (atoi(optarg) >= 0)) {
                    room = atoi(optarg);
                    argPresent.at(R) = true;
                }
                break;
            case 'F':
                if (optarg != nullptr) {
                    logfile = string(optarg) + ".txt";
                    // cout << "f is " << logfile << endl;  // FIXME
                    argPresent.at(F) = true;
                }
                break;
            case 'B':
                if (optarg != nullptr) {
                    batchMode = true;
                    argPresent.at(B) = true;
                }
                break;
            default:
                valid = false;
                // cout << "default messed up" <<endl; //FIXME
                return;
                break;
        }
    }
    // cout << "now checking param combos" << endl; //FIXME
    if (!checkParamCombosApp()) {
        valid = false;
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
    return nullptr;
}

bool Record::lettersOnly(string s) {
    // check that ascii is 65-90 or 97-122
    if (s.size() < 1) {
        return false;
    }
    for (int i = 0; i < s.size(); i++) {
        if ((static_cast<int>(s.at(i)) < 65) || (static_cast<int>(s.at(i)) > 122)) {
            return false;
        } else if ((static_cast<int>(s.at(i)) > 90) && (static_cast<int>(s.at(i)) < 97)) {
            return false;
        }
    }
    return true;
}

bool Record::checkParamCombosApp() {
    // if batch, -B should be the only parameter
    if (argPresent.at(B)) {
        for (int i = 0; i < argPresent.size(); i++) {
            if (i != B && argPresent.at(i)) {
                return false;
            }
        }
        return true;
    }

    // timestamp, token, name, logfile name must be present
    if (!argPresent.at(T) || !argPresent.at(K) || !argPresent.at(F)) {
        // cout << "missing T, K or F" << endl;  // FIXME
        return false;
    }

    if (name.size() < 1) {
        // cout << "name size too smal" << endl; // FIXME
        return false;
    }

    // either doctor or nurse, but not both
    if ((argPresent.at(N) && argPresent.at(D)) || 
            (!argPresent.at(N) && !argPresent.at(D))) {
        return false;
    }

    // either arriving or leaving, but not both
    if ((argPresent.at(A) && argPresent.at(L)) || 
            (!argPresent.at(A) && !argPresent.at(L))) {
        return false;
    }
    return true;
}

// returns logs for the relevant doctor/nurse (doctor name, nurse name)
vector<Record> Record::getLogsForPerson(string* contents, bool create) {
    if (!readFile(contents, create) || !valid) {
        vector<Record> error;
        error.push_back(Record());
        return error;
    }
    if (contents->size() <= 0) {
        return vector<Record>();
    }

    if (!decrypt(contents, token)) {
        vector<Record> error;
        error.push_back(Record());
        return error;
    }

    if (contents->find("test" + logfile) == string::npos) {
        vector<Record> error;
        error.push_back(Record());
        return error;
    }
    vector<string> recs = splitOn(*contents, '\n');

    vector<string> relevant;
    if (!relevantRecords(&recs, &relevant, create)) {
        vector<Record> error;
        error.push_back(Record());
        return error;
    }

    // turn lines into records, push them onto vector, return them
    vector<Record> relevantRecs;
    for (int i = 0; i < relevant.size(); i++) {
        // cout << "relevant.at(i) " << relevant.at(i) << endl; // FIXME
        relevantRecs.push_back(Record(relevant.at(i)));
    }
    return relevantRecs;
}

bool Record::append() {
  if (!valid) {
    return false;
  }
  string contents = "";
  vector<Record> records = getLogsForPerson(&contents, true);
  if ((records.size() == 1) && (records.at(0).isValid() == false)) {
    return false;
  }
  // if (records.size() > 0) {
  //   records.push_back(Record(this->toString()));
  // }
  
  // do sanity checks here
  if (!checkConsistency(&records)) {
    return false;
  }
  // add to contents
  contents = contents + "\n" + this->toString();
  // encrypt contents
  if (!encrypt(&contents, token)) {
    return false;
  }
  // write new contents into file
  if (!writeFile(contents)) {
    return false;
  }
  return true;
}

bool Record::checkConsistency(vector<Record>* personRecs) {
  // cout << "checking consistency" << endl; // FIXME
  // check timestamp consistency
  for (int i = 1; i < personRecs->size(); i++) {
    if (personRecs->at(i).getTimestamp() <= personRecs->at(i - 1).getTimestamp()) {
      // cout << "this tiemstamp is too early" << personRecs->at(i).getTimestamp() << endl; // fIXME
      return false;
    }
    // cout << "timestamps " << personRecs->at(i - 1).getTimestamp() << " < " << personRecs->at(i).getTimestamp() << endl; // FIXME
  }
  // if ((personRecs->size() > 0) && 
  //     (personRecs->at(personRecs->size() - 1).getTimestamp() >= timestamp)) {
  //   cout << ""
  //   return false;
  // }
  // add new proposed record to the end of it
  //personRecs->push_back(Record(this->toString()));
  // cout << "timestamp consistent" << endl; // FIXME
  int loggedRoom = -1;
  bool inHospital = false;
  bool inRoom = false;
  for (int i = 0; i < personRecs->size(); i++) {
    // cout << "checking rec " << personRecs->at(i).toString() << endl; // FIXME
    // if not in the hospital and the next record is not an arrival at the hospital, we have a problem
    if (!inHospital && (!personRecs->at(i).isArriving() || (personRecs->at(i).getRoom() != -1))) {
      // cout << "not in hospital and not arriving or already in a room" << endl;  // FIXME
      return false;
    } else if (!inHospital && !inRoom && personRecs->at(i).isArriving() && 
        (personRecs->at(i).getRoom() == -1)) {
      // successfully entering hospital
      inHospital = true;
      inRoom = false;
      loggedRoom = -1;
      // cout << "loggedRoom not in hospital set to " << loggedRoom << endl; // fIXME
      continue;
    } else if (inHospital && personRecs->at(i).isArriving() && (personRecs->at(i).getRoom() == -1)) {
      // cout << "inhospital and arriving and room is -1" << endl;  // FIXME
      return false;
    }
    // check for entrance to room 
    // if arriving into an actual room but haven't entered the hospital or left a room, we have a problem
    if (personRecs->at(i).isArriving() && 
        (personRecs->at(i).getRoom() != -1) && (!inHospital || inRoom)) {
      // cout << "arriving and already in a room and either not in a hospital or already in a room" << endl; // FIXME
      return false;
    } else if (personRecs->at(i).isArriving() && inHospital && !inRoom && (personRecs->at(i).getRoom() != -1)) {
      // if the record is arriving, we are not in a room already, we are in the hospital and trying to enter an actual room
      inRoom = true;
      loggedRoom = personRecs->at(i).getRoom();
      // cout << "loggedRoom set to " << loggedRoom << endl; // fIXME
      continue;
    }
    // check for leaving room
    // if not arriving, trying to leave actual room, and we are either not in a room or the logged room is not the room we are trying to leave, problem
    if (!personRecs->at(i).isArriving() && (personRecs->at(i).getRoom() != -1) && 
        (!inHospital || !inRoom || (loggedRoom != personRecs->at(i).getRoom()))) {
      // cout << loggedRoom << " vs " << personRecs->at(i).getRoom() << endl; // FIXME
      // cout << personRecs->at(i).toString() << endl; // FIXME
      return false;
    } else if (!personRecs->at(i).isArriving() && (personRecs->at(i).getRoom() != -1) && 
        inRoom && inHospital && (loggedRoom == personRecs->at(i).getRoom())) {
      inRoom = false;
      loggedRoom = -1;
      // cout << "loggedRoom leaving room set to " << loggedRoom << endl; // fIXME
      // cout << personRecs->at(i).toString() << endl; // FIXME
      continue;
    }
    // cout << "done checking for leaving room" << endl; // FIXME
    // check for leaving hospital
    // if we are leaving the hospital, 
    if (!personRecs->at(i).isArriving() && 
        (personRecs->at(i).getRoom() == -1) && (!inHospital || inRoom)) {
      // cout << "B" << endl; // FIXME
      return false;
    } else if (!personRecs->at(i).isArriving() && 
        (personRecs->at(i).getRoom() == -1) && inHospital && !inRoom) {
      loggedRoom = -1;
      inHospital = false;
      continue;
    } else {
      // cout << "C" << endl; // FIXME
      return false;
    }
  }
  // cout << "consistent" << endl;  // FIXME
  return true;
}

bool Record::readFile(string* contents, bool create) {
  if (!valid) {
    return false;
  }
  *contents = "";
  fstream log;
  log.open(logfile, fstream::in);
  if (log.fail() && create) {
    log.open(logfile, fstream::out);
    string newcont = "test" + logfile;
    encrypt(&newcont, "01234567890123456789012345678901");
    log << newcont << endl;
    log.close();
    log.open(logfile, fstream::in);
  }
  if (log.is_open() && log.good()) {
    string newline;
    int counter = 0;
    while (getline(log, newline) && (counter < contents->max_size())) {
      *contents = *contents + "\n" + newline;
      counter++;
    }
    log.close();
  } else {
    return false;
  }
  return true;
}

bool Record::writeFile(const string& text) {
  if (!valid) {
    return false;
  }
  fstream log;
  log.open(logfile, fstream::out);
  if (log.is_open() && log.good()) {
    log << text;
    log.close();
  } else {
    return false;
  }
  return true;
}

bool Record::decrypt(string* contents, string k) {
  if (contents->size() == 0) {
    return true;
  }

  if (k != "01234567890123456789012345678901") {
    cout << "integrity violation" << endl;
    return false;
  }
  return true;
}

bool Record::relevantRecords(vector<string>* allRecs, vector<string>* results, bool addrec) {
  int i = 0;
  int timestamp = 0; // check timestamp consistency
  // vector<string> allRecs = *alRecs;
  //add this record to them ix
  if (addrec) {
    // cout << "VALID" << endl; // fixme
    allRecs->push_back(this->toString());
  }  
  if (allRecs->size() > 0 && allRecs->at(0).size() == 0) {
    i++;
  }
  if ((allRecs->size() > 0) && (allRecs->at(0) == ("test" + logfile))) {
    i++;
  }
  for (i; i < allRecs->size(); i++) {
    if (allRecs->at(i).size() < 1) {
      continue;
    }
    vector<string> record = splitOn(allRecs->at(i), ' ');
    if (record.size() > 0 && record.at(0) == ("test" + logfile)) {
      continue;
    }
    if ((record.size() != 4) && (record.size() != 3)) {
      return false;
    }

    // cout << "old timestamp " << timestamp << " vs " << atoi((char*)record.at(0).c_str()) << endl; // FIXME
    
    // check timestamp consistency
    if (isValidInt((char*)record.at(0).c_str()) && (atoi((char*)record.at(0).c_str()) <= timestamp)) {
      return false;
    } else if (isValidInt((char*)record.at(0).c_str()) && (atoi((char*)record.at(0).c_str()) > timestamp)) {
      timestamp = atoi((char*)record.at(0).c_str());
    } else  {
      // cout << "invalid int " << atoi((char*)record.at(0).c_str()) << endl; // FIXME
      return false;
    }
    
    string n = record.at(1);
    int found = n.find("dr.");
    // dr. should be in 0th index
    if (found == 0 && doctor) {
      string recName = n.substr(3, n.size() - 3);
      if (recName == name) {
        results->push_back(allRecs->at(i));
      }
    } else if ((n.find("nurse.") == 0) && !doctor) {
      string recName = n.substr(6, n.size() - 6);
      if (recName == name) {
        results->push_back(allRecs->at(i));
      }
    } else {
      continue;
    }
  }
  // cout << "relevant records all good" << endl; // FIXME
  return true;
}

// <timestamp> <dr/nurse>.<name> <A|L> ?<room #>
string Record::toString() {
  string rec = "";

  rec = to_string(timestamp) + " ";

  if (doctor) {
    rec = rec + "dr.";
  } else {
    rec = rec + "nurse.";
  }

  rec = rec + name + " ";

  if (arrive) {
    rec = rec + "A ";
  } else {
    rec = rec + "L ";
  }

  if (room > -1) {
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

priority_queue<string, vector<string>, greater<string> > Record::getAllNames(std::string* contents) {
  if (!readFile(contents, false) || !valid) {
    priority_queue<string, vector<string>, greater<string> > error;
    error.push("error");
    return error;
  }
  if (contents->size() <= 0) {
    return priority_queue<string, vector<string>, greater<string> >();
  }

  if (!decrypt(contents, token)) {
    priority_queue<string, vector<string>, greater<string> > error;
    error.push("error");
    return error;
  }
  if (contents->find("test" + logfile) == string::npos) {
    priority_queue<string, vector<string>, greater<string> > error;
    error.push("error");
    return error;
  }
  priority_queue<string, vector<string>, greater<string> > names;
  vector<string> recs = splitOn(*contents, '\n');
  for (int i = 0; i < recs.size(); i++) {
    vector<string> record = splitOn(recs.at(i), ' ');
    if (record.size() < 2) {
      continue;
    }
    names.push(record.at(1));
  }

  return names;
}

State Record::getState(std::vector<Record> personRecs) {
  // all records must be from the same person
  int loggedRoom = -1;
  bool inHospital = false;
  bool inRoom = false;
  bool valid = true;
  string name = "";
  State s;
  // cout << "personrecs.size() " << personRecs.size() << endl; // FIXME
  for (int i = 0; i < personRecs.size(); i++) {
    name = personRecs.at(0).getName();
    // cout << "set name to " << personRecs.at(0).getName() << endl; // FIXME
    // if not in the hospital and the next record is not an arrival at the hospital, we have a problem
    if (!inHospital && (!personRecs.at(i).isArriving() || (personRecs.at(i).getRoom() != -1))) {
      valid = false;
      break;
    } else if (!inHospital && !inRoom && personRecs.at(i).isArriving() && (personRecs.at(i).getRoom() == -1)) {
      // successfully entering hospital
      inHospital = true;
      inRoom = false;
      loggedRoom = -1;
      continue;
    } else if (inHospital && personRecs.at(i).isArriving() && (personRecs.at(i).getRoom() == -1)) {
      valid = false;
      break;
    }
    // check for entrance to room. if arriving into an actual room but haven't entered the hospital or left a room, we have a problem
    if (personRecs.at(i).isArriving() && (personRecs.at(i).getRoom() != -1) && (!inHospital || inRoom)) {
      valid = false;
      break;
    } else if (personRecs.at(i).isArriving() && inHospital && !inRoom && (personRecs.at(i).getRoom() != -1)) {
      // if the record is arriving, we are not in a room already, we are in the hospital and trying to enter an actual room
      inRoom = true;
      loggedRoom = personRecs.at(i).getRoom();
      s.rooms.push_back(loggedRoom);
      continue;
    }

    // check for leaving room
    // if not arriving, trying to leave actual room, and we are either not in a room or the logged room is not the room we are trying to leave, problem
    if (!personRecs.at(i).isArriving() && (personRecs.at(i).getRoom() != -1) && (!inHospital || !inRoom || (loggedRoom != personRecs.at(i).getRoom()))) {
      valid = false;
      break;
    } else if (!personRecs.at(i).isArriving() && (personRecs.at(i).getRoom() != -1) && inRoom && inHospital && (loggedRoom == personRecs.at(i).getRoom())) {
      inRoom = false;
      loggedRoom = -1;
      continue;
    }

    // check for leaving hospital
    // if we are leaving the hospital, 
    if (!personRecs.at(i).isArriving() && (personRecs.at(i).getRoom() == -1) && (!inHospital || inRoom)) {
      valid = false;
      break;
    } else if (!personRecs.at(i).isArriving() && (personRecs.at(i).getRoom() == -1) && inHospital && !inRoom) {
      loggedRoom = -1;
      inHospital = false;
      continue;
    } else {
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

bool Record::encrypt(string* contents, string k) {
  if (k != "01234567890123456789012345678901") {
    cout << "integrity violation" << endl;
    return false;
  }
  return true;
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

int encryp(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext) {
  EVP_CIPHER_CTX *ctx;

  int len;

  int ciphertext_len;

  /* Create and initialise the context */
  if (!(ctx = EVP_CIPHER_CTX_new())) return false;

  /* Initialise the encryption operation. IMPORTANT - ensure you use a key
  * and IV size appropriate for your cipher
  * In this example we are using 256 bit AES (i.e. a 256 bit key). The
  * IV size for *most* modes is the same as the block size. For AES this
  * is 128 bits */
  if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
  return false;

  /* Provide the message to be encrypted, and obtain the encrypted output.
  * EVP_EncryptUpdate can be called multiple times if necessary
  */
  if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
  return false;
  ciphertext_len = len;

  /* Finalise the encryption. Further ciphertext bytes may be written at
  * this stage.
  */
  if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) return false;
  ciphertext_len += len;

  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  return ciphertext_len;
}

int decryp(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext) {
  EVP_CIPHER_CTX *ctx;

  int len;

  int plaintext_len;

  /* Create and initialise the context */
  if (!(ctx = EVP_CIPHER_CTX_new())) return false;

  /* Initialise the decryption operation. IMPORTANT - ensure you use a key
  * and IV size appropriate for your cipher
  * In this example we are using 256 bit AES (i.e. a 256 bit key). The
  * IV size for *most* modes is the same as the block size. For AES this
  * is 128 bits */
  if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
  return false;

  /* Provide the message to be decrypted, and obtain the plaintext output.
  * EVP_DecryptUpdate can be called multiple times if necessary
  */
  if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
  return false;
  plaintext_len = len;

  /* Finalise the decryption. Further plaintext bytes may be written at
  * this stage.
  */
  if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) return false;
  plaintext_len += len;

  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  return plaintext_len;
}

// void handleErrors(void)
// {
//   ERR_print_errors_fp(stderr);
//   abort();
// }
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
