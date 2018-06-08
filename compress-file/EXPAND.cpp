#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include "timer.h"
#include "timer.c"
using namespace std;
vector<bool> code;
int pos = 0;

void read_stdin(istream& in)
{
// read from stdin when there is no arugment for file
    unsigned char byte = 0;
    while (!in.eof()){
        in.read((char *)&byte, sizeof(byte));
        int num = (int) byte;
        for (int i = 7; i >= 0; i--) {
            int k = num >> i;
            if (k & 1)
                code.push_back(true);
            else
                code.push_back(false);
        }
    }
    return;
}


void decode(string filename)
{
// decode: get original char data input. and generate our data structure to store these 1, 0
    ifstream fin(filename.c_str(), ios::binary | ios::in);
    unsigned char byte;
    while (!fin.eof()){
        fin.read((char *)&byte, sizeof(byte));
        int num = (int) byte;
        for (int i = 7; i >= 0; i--) {
            int k = num >> i;
            if (k & 1)
                code.push_back(true);
            else
                code.push_back(false);
        }
    }
    fin.close();
    return;
}

int binaryToInt(int len){
// convert 1, 0 to actual integers for our further process
    int result = 0;
    for (int i=pos; i<pos+len; ++i){
        if (code.at(i))
            result |= 1 << (pos+len-i-1);
    }
    pos += len;
    return result;
}

void recover_file(){
// take those integers and decode from them to get the actual char.
    string window = "";
    // read first three byte
    int N = binaryToInt(8);
    int L = binaryToInt(8);
    int S = binaryToInt(8);
    int W = (int)pow(2,N);
    while(1){
        int len = binaryToInt(L);
        if (len == 0){
            int str_len = binaryToInt(S);
            if (str_len == 0)
                break;
            else{
                for (int i=0; i<str_len; ++i){
                    char c = binaryToInt(8);
                    window += c;
                    cout << c;
                }
            }
        }
        else{
            len += 1;
            int lookback = binaryToInt(N);
            int window_pos = window.size() - lookback;
            for (int i=0; i<len; ++i){
                window += window[window_pos];
                cout << window[window_pos++];
            }
        }
        if (window.size() > W)
            window.erase(0, window.size()-W);
    }
}



int main(int argc, char * argv[]){
// start the timer. call these functions. little error handling. and error stream output.
    start_timer();
    string filename = argv[argc-1];
    //cout << "argv[argc-1] : "<<argv[argc-1]<<endl;
    //cout.flush();
    if (argc == 1){
        read_stdin(cin);
	
	}
    else
        decode(filename);
    if (code.size()==8)
	return 0;
    recover_file();
    double time = elapsed_time();
    cerr << "Time used in EXPAND.cpp = "<<time <<endl;
    cerr << endl;
    return 0;
}