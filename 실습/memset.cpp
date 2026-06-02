#include<iostream>
#include<string>
#include<string.h>

using namespace std;

int test(char(*out_val)[10]){
    int cnt = 0;
    for(cnt = 0;cnt < 10;cnt++){
        memcpy(out_val[cnt],"123456789",9);
    }
    return 0;
}

int main(){
    int cnt = 0;
    char val[11][10];
    //memset(&val,0x00,sizeof(val));
    test(val);
    for(cnt = 0;cnt < 10;cnt++){
        cout << val[cnt] << endl;
    }
    return 0;
}