#include <iostream>
#include <string>
#include <cstring>
using namespace std;

class MyString {
    friend ostream &operator<<(ostream &, const MyString &);
private:
    char *pBuf;
public:
    MyString(const char *s = NULL) {
        if (s == NULL) {
            pBuf = new char[1];
            pBuf[0] = NULL;
        }
        else {
            pBuf = new char[::strlen(s) + 1];
            memset(pBuf, 0x00, ::strlen(s) + 1);
            memcpy(pBuf, s, ::strlen(s));
        }
    }
    //생성자, 없으면 null, 있으면 한글자씩 copy하고 마지막은 null로 종료
    MyString(const MyString &s) {
        pBuf = new char[s.getSize() + 1];
        memset(pBuf, 0x00, s.getSize() + 1);
        memcpy(pBuf, s.pBuf, s.getSize());
    }
    //생성자(s를 복사함)
    ~MyString() {
        if (pBuf) delete[] pBuf;
    }
    //소멸자, 메모리 해제
    void print(){
        cout << pBuf << endl;
    }
    //출력 함수
    int getSize() const{
        return strlen(pBuf);
    }
    //문자열 길이 반환
    MyString operator+(MyString& s){
        char *temp = new char[getSize() + s.getSize() + 1];
        memset(temp, 0x00, getSize() + s.getSize() + 1);
        memcpy(temp, pBuf, getSize());
        memcpy(&temp[getSize()], s.pBuf, s.getSize());
        MyString r(temp);
        delete[] temp;
        return r;
    }
    //+라는 이름의 함수, 문자열 2개를 연결
};

ostream &operator<<(ostream &output, const MyString &a) {
    output << a.pBuf << endl;
    return output;
}
//friend 함수

int main() {
    MyString s1("Hello ");
    MyString s2("World!");
    s1.print();
    s2.print();
    cout << "s1+s2=>" << (s1+s2) << endl;
    return 0;
}