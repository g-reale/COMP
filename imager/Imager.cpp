#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

fstream open(char * filename, ios::openmode mode){
    fstream file(filename, mode);
    if(!file.is_open()){
        cout << "could not open file: " << filename << endl;
        exit(-1);
    }
    return file;
}

void asserthex(const string& line, const char * filename){
    for(size_t i = 0; i < line.size(); i++){
        if(!(isdigit(line[i]) || 'a' <= line[i] <= 'f')){
            cout << line << " in file " << filename << " is not a hex string" << endl;
            exit(-1);
        }
    }
}

size_t assertnatural(const char * option){
    size_t value;
    if(sscanf(option,"%ld",&value) != 1){
        cout << option << " is not a positive integer " << endl;
        return -1;
    }
    return value;
}

int main(int argc, char ** argv){
    if(argc < 5){
        cout << "usage: " << argv[0] << " <image name> <running> <offset> <binaries...>" << endl;
        return 0;
    }

    //file matrix
    vector<vector<string>> files;
    fstream image = open(argv[1], ios::out | ios::trunc);
    fstream running = open(argv[2], ios::in);
    size_t size = 0;
    string line;

    //copy running file on base address 0
    while (getline(running,line)){
        if(!line.size())
            continue;
        asserthex(line,argv[2]);
        image << line << endl;
        size++;
    }

    //parse offset
    size_t offset = assertnatural(argv[3]);
    if(offset < size){
        cout << "running program(" << size << ") too big for required offset(" << offset << ")" << endl;
        return -1;
    }
    for(size_t i = 0; i < offset - size; i++)
        image << hex << 0 << endl;

    //load each file in a matrix 
    for(size_t i = 4; i < argc; i++){
        
        fstream file = open(argv[i], ios::in);
        vector<string> data;

        while(getline(file,line)){
            if(!line.size())
                continue;
            asserthex(line,argv[i]);
            data.push_back(line);
        }

        files.push_back(data);
    }

    image << hex << files.size() << endl;
    offset += 1 + 2 * files.size();
    for(auto& file : files){
        image << hex << offset << endl << file.size() << endl;
        offset += file.size();
    }
    for(auto& file : files){
    for(auto& line : file){
        image << line << endl;
    }}

    return 0;
}