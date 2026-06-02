#include <iostream>
using namespace std;

int quotient(int n1, int n2);

int main()
{
    int n1, n2, result;
    for (int i = 0; i < 5; i++)
    {
        try
        {
            cout << "first: ";
            cin >> n1;
            cout << "second: ";
            cin >> n2;
            if (quotient(n1, n2) == -1) throw 0;
            result = n1 / n2;
            cout << "result: " << result << endl;
        }
        catch (int x)
        {
            cout << "0으로 나눌 수 없음" << endl;
        }
    }
    return 0;
}

int quotient(int n1, int n2){
    if(n2 == 0) return -1;
    else return 0;
}