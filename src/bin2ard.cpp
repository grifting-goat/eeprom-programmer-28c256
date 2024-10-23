#include<iostream>
#include<fstream>

using namespace std;

const int romSize = 0x8000;
const char* filePath = "a.out";

int main () {
    pair<int, short> code;

    //load the file, change to whatever
    ifstream inputFile(filePath, std::ios::binary);

    //check important stuff
    if (!inputFile) {
        cout << "Failed to load file!" << endl;
        return 1;
    }

    //determine size of file
    inputFile.seekg(0, std::ios::end);
    std::streamsize size = inputFile.tellg();
    
    // Check if tellg() succeeded
    if (size == -1) {
        cout << "could not determine size" << endl;
        inputFile.close();
        return 1;
    }

    //reset file
    inputFile.seekg(0, std::ios::beg);
    if (size != romSize) {cout << "wrong rom size";}

    //read the file
    int k = 0;
    int flag =0;
    int counter = 0;
    unsigned char buffer[romSize];
    pair<int, short> out[romSize];
    inputFile.read(reinterpret_cast<char*>(buffer), romSize);

    for (int i = 0; i < romSize; i++) {
        if (buffer[i] == 0x00) {
            if (counter == 0) {flag = k;}
            counter++;
            }
        else {counter = 0;}
        out[k].first = i;
        out[k].second = buffer[i];
        k++;
        if (counter > 4) {break;}
    }

    //copy to new array
    pair<int, short>* output = new pair<int, short>[flag];
    for(int i = 0; i < flag; i++) {
        output[i] = out[i];
    }

    //out to a file
    std::ofstream outputFile("output.txt");

    if (!outputFile.is_open()) {
        std::cerr << "Failed to open file!" << std::endl;
        return 1;
    }

    outputFile << "{";
    for (int i = 0; i < flag; i++) {
        outputFile << output[i].first << ",";
        outputFile << output[i].second;
        if (i != (flag-1)) {outputFile << ",";}
        if((i%16) == 15) {outputFile << "\n";}
    }
    outputFile << "}";

    delete[] output;
    inputFile.close();
    outputFile.close();
    return 0;
}