#include<iostream>
#include<string>
using namespace std;

string format(int n) {
    string s = to_string(n);
    int pos = s.length() - 3;
    while (pos > 0) {
        s.insert(pos, ",");
        pos -= 3;
    }
    return s;
}
class ticket{
protected:
    int price;
public:
    virtual int getprice(int price) = 0;
    virtual void print() = 0;
};

class normalticket : public ticket{
private:
    int price;
public:
    int getprice(int price){
        this->price = price;
        return price;
    }
    void print(){
        
        cout << "일반권 선택" << endl << "가격: 1,450원" << endl << "거스름돈: " << format(price - 1450) << "원" << endl;
    }
};
class youthticket : public ticket{
private:
    int price;
public:
    int getprice(int price){
        this->price = price;
        return price;
    }
    void print(){
        cout << "청소년권 선택" << endl << "가격: 1,000원" << endl << "거스름돈: " << format(price - 1000) << "원" << endl;
    }
};
class childticket : public ticket{
private:
    int price;
public:
    int getprice(int price){
        this->price = price;
        return price;
    }
    void print(){
        cout << "어린이권 선택" << endl << "가격: 500원" << endl << "거스름돈: " << format(price - 500) << "원" << endl;
    }
};



int main(){
    int num;
    ticket* t;
    cout << "1. 일반권" << endl << "2. 청소년권" << endl << "3. 어린이권" << endl << endl;
    cout << "선택: ";
    cin >> num;
    if(num == 1) t = new normalticket();
    else if(num == 2) t = new youthticket();
    else if(num == 3) t = new childticket();
    else return 0;
    cout << "투입 금액: ";
    int price;
    cin >> price;
    t->getprice(price);
    cout << endl;
    t->print();
    delete t;
    return 0;
}