#include<iostream>
#include<string>
#include<cstring>
#include<assert.h>
using namespace std;

class MyArray{
    friend ostream &operator<<(ostream&, const MyArray&);
private:
    int *data;
    int size;
public:
    MyArray(int s = 10){
        size = (s > 0 ? s : 10);
        data = new int[size];
        for(int i = 0;i < size;i++) data[i] = 0;
    }
    //생성자
    ~MyArray(){
        delete []data;
        data = NULL;
    }
    //소멸자
    int getSize() const{
        return size;
    }
    //배열 크기 반환
    MyArray &operator=(const MyArray &a){
        if(&a != this){
            delete []data;
            size = a.size;
            data = new int[size];
            for(int i = 0;i < size;i++) data[i] = a.data[i];
        }
        return *this;
    }
    // = 연산자 중복
    int &operator[](int index){
        assert(0 <= index && index < size);
        return data[index];
    }
    //[] 연산자 중복
};
ostream &operator<<(ostream &output, const MyArray &a){
    for(int i = 0;i < a.size;i++) output << a.data[i] << ' ';
    output << endl;
    return output;
}
//출력 중복
int main(){
    MyArray a1(10);
    a1[0] = 1;
    a1[1] = 2;
    a1[2] = 3;
    a1[3] = 4;
    cout << a1;
    return 0;
}