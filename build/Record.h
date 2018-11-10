#pragma once

#include "State.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <queue>
#include <unordered_set>
#include <functional>
// #include "encrypt.h"


#define T 0
#define K 1
#define D 2
#define N 3
#define A 4
#define L 5
#define R 6
#define F 7
#define B 8
#define S 9
#define NUM_PARAMS 10
#define MAX_PARAM_LEN 256

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
    // bool checkParamCombosRead();
    // void parseArg(char c);
    bool writeFile(const std::string& text);
    bool decrypt(std::string* contents, std::string key);
    bool encrypt(std::string* contents, std::string key);
    bool relevantRecords(std::vector<std::string>* recs, std::vector<std::string>* results, bool addrec);
    std::vector<std::string> splitOn(std::string s, char delimeter);
    bool checkConsistency(std::vector<Record>* personRecs);

 public:
    bool lettersOnly(std::string s);
    std::vector<Record> getLogsForPerson(std::string* contents, bool create);
    bool readFile(std::string* contents, bool create);
    std::priority_queue<std::string, std::vector<std::string>, std::greater<std::string> > getAllNames(std::string* contents);
    State getState(std::vector<Record> personRecs);
    char* parseAppendOpts(int size, char* argv[], char arg);
    Record(int argc, char* argv[]);
    Record();
    explicit Record(std::string s);
    bool append();
    std::string toString();
    bool isValid();
    void setValid(bool v);
    int getTimestamp();
    std::string getToken();
    void setToken(std::string t);
    // void setName(std::string n);
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

bool isValidInt(char* c);
int strlen(char* c);  // overload strlen
int encryp(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext);
int decryp(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext);
// void handleErrors(void);

