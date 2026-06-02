#include<iostream>
#include<string>
#include<fstream>
#include<cassert>

using namespace std;

int main(){
    int int1 = 12345;
    double double1 = 45.78;
    ofstream strmout ("Sample", ios :: out | ios :: binary);
    if(!strmout.is_open()){
        cout << "the file sample cannot be opened for writing!";
        assert(false);
    }
    strmout.write(reinterpret_cast<char*>(&int1), sizeof(int));
    strmout.write(reinterpret_cast<char*>(&double1), sizeof(double));
    strmout.close();

    int int2;
    double double2;
    ifstream strmin("Sample", ios :: in | ios :: binary);
    if(!strmin.is_open()){
        cout << "the file sample cannot be opened for reading!";
        assert(false);
    }
    strmin.read(reinterpret_cast<char*>(&int2), sizeof(int));
    strmin.read(reinterpret_cast<char*>(&double2),sizeof(double));
    strmin.close();
    cout << int2 << endl << double2 << endl;
    return 0;
}