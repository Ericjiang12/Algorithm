#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <sstream>
#include "timer.h"
#include "timer.c"
using namespace std;

string content;
vector<bool> output;
int dict_size = 0;
string buf = "";
int file_size=0;



void process(int argc, char* argv[],int * N, int * L, int* S){
// function that handle arguments
    string Ns;
    string Ls;
    string Ss;
    int N1;
    int L1;
    int S1;
    for (int i = 1 ; i!= argc; i++){
        if (argv[i][1] == 'N'){
            Ns = argv[i];
            string value(Ns.begin()+3,Ns.end());
            stringstream geek(value);
            geek>>N1;
            *N = N1;
        }
        if (argv[i][1] == 'L'){
            Ls = argv[i];
            string value(Ls.begin()+3,Ls.end());
            stringstream geek(value);
            geek>>L1;
            *L = L1;
        }
        if (argv[i][1] == 'S'){
            Ss = argv[i];
            string value(Ss.begin()+3,Ss.end());
            stringstream  geek(value);
            geek >> S1;
            *S = S1;
        }

    }
}


void intToString(int num, int len){
// convert our int vector to string in char
    for (int i = len-1; i >= 0; i--) {
        int k = num >> i;
        if (k & 1)
            output.push_back(true);
        else
            output.push_back(false);
    }
    return;
}

void findBestMatch(string dict, string buffer, int N, int L, int S, int max_strlen){
// find the match in our processed dictionary
    int found = -1, length = 0;
    int size = dict.size();
    dict += buffer;
    for (int i=0; i<size;++i){
        if (dict[i] == buffer[0] ){
            int current_length = 1;
            while (current_length < buffer.size()){
                if (dict[i+current_length] != buffer[current_length])
                    break;
                ++current_length;
            }
            if (current_length > length){
                found = i;
                length = current_length;
            }
        }
    }
    if (found == -1 || length == 1) {
        buf += content[dict_size];
        if (buf.size() == max_strlen){
            intToString(0,L);
            intToString(buf.size(),S);
            for (int i=0; i<buf.size(); ++i)
                intToString((int)buf[i],8);
            buf = "";
        }
        ++dict_size;
    }
    else{
        if (buf.size() != 0){
            intToString(0,L);
            intToString(buf.size(),S);
            for (int i=0; i<buf.size(); ++i)
                intToString((int) buf[i], 8);
            buf = "";
        }
        intToString(length-1,L);
        intToString(size-found,N);
        dict_size += length;
    }
    return;
}

void processChunk(int N, int L, int S){
// for different N L S, data get stored in different bits
    int W = (int)pow(2,N);
    int F = (int)pow(2,L);
    int max_length = (int)pow(2,S) - 1;

    if (dict_size == 0){
        buf += content[0];
        ++dict_size;
    }
    while (content.length() >= W){
        findBestMatch(content.substr(0,dict_size), content.substr(dict_size,F), N, L, S, max_length);
        if (dict_size > W-F){
            content.erase(0,dict_size-(W-F));
            dict_size = W-F;
        }
    }
    return;
}

void processLeftOver(int N, int L, int S){
// process leftover chunks 
    int W = (int)pow(2,N);
    int F = (int)pow(2,L);
    int max_length = (int)pow(2,S) - 1;

    while(1){
        findBestMatch(content.substr(0,dict_size), content.substr(dict_size,F), N, L, S, max_length);
        if (content.substr(dict_size,F) == "")
            break;
        if (dict_size > W-F){
            content.erase(0,dict_size-(W-F));
            dict_size = W-F;
        }
    }
    return;
}

void processFile(string filename, int N, int L, int S) {
// process that read a file and call processchunk and do little stuff on output
    ifstream infile;
    infile.open(filename.c_str(), std::ifstream::in);
    content = "";
    intToString(N, 8);
    intToString(L, 8);
    intToString(S, 8);
    while (!infile.eof()) {
        int i = 0;
	string temp;
	
        while (++i <= 1000 && !infile.eof()){
	    temp = infile.get();
            content += temp;
	    file_size += temp.size();
	}
	
        processChunk(N,L,S);
    }
    content.erase(content.size()-1);
    processLeftOver(N,L,S);
    if (buf.size()){
        intToString(0,L);
        intToString(buf.size(),S);
        for (int i=0; i<buf.size(); ++i)
            intToString((int) buf[i], 8);
    }
    intToString(0,L);
    intToString(0,S);
    infile.close();
    return;
}

void encode(std::vector<bool>& map)
{
// encode output the binary data to stdout. which can be > to a file or as stdin for EXPAND
    unsigned char byte = 0;
    int byte_index = 7;
    for (size_t i = 0; i < map.size(); i++)
    {
        if (map[i])
            byte |= (1 << byte_index);
        byte_index--;
        if (byte_index < 0)
        {
            byte_index = 7;
            cout << byte;
            byte = 0;
        }
    }
    if (byte_index != 7)
        cout << byte;
}

int main(int argc, char* argv[])
{
// start the timer. call these functions. and print stuff on error stream.
    start_timer();
    string filename(argv[argc-1]);

    int N = 11;
    int L = 4;
    int S = 3;
    process(argc, argv, &N,&L,&S);

    if(N<9 || N >14 || L<3 || L>4 || S<1 || S>5){
	cerr<<"Parameters invalid. Abort.\n";
	return 0;
    }

    processFile(filename,N,L,S);
    encode(output);
    double time = elapsed_time();
    double size_compress = output.size()/8;
    double ratio = size_compress/ file_size;

    cerr <<endl;
    cerr << "Parameters N="<< N <<" L="<< L <<" S="<<S <<endl;
    cerr << "Time used in LZ.cpp = "<<time <<endl;
    cerr << "Size of original file : "<<file_size <<" bytes"<<endl;	
    cerr << "Size of compressed file : "<< size_compress<<" bytes"<<endl;
    cerr << "Compressed / original ratio : "<<ratio<<endl;
    cerr << endl;
    return 0;
}