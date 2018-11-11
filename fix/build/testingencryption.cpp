#include <iostream>
#include <string>
#include "Record.h"

using namespace std;

int main() {
    string t = "testing";
    string secret = "secret";
    string enc = encrypt(t, secret);
    cout << enc << endl; 
    string dec = decrypt(enc, secret);
    cout << dec<< endl;
    cout << "done" << endl; 
    return 0;
}