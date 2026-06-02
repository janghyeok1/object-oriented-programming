#include <iostream>
#include <string>
using namespace std;

class menu
{
private:
    string juice[4] = {"콜라", "사이다", "커피", "물"};
    int price[4] = {1270, 2400, 3520, 520};
    int num;
    int reserve[4] = {10, 10, 10, 10};
    int sale[4] = {0, 0, 0, 0};
public:
    int charge(int pay) { return pay - price[num - 1]; }
    int get_price() { return price[num - 1]; }
    int get_reserve() { return reserve[num - 1]; }
    void print()
    {
        cout << "\n[배출] " << juice[num - 1] << "이(가) 나왔습니다. (" << price[num - 1] << "원)" << endl;
        reserve[num - 1] -= 1;
        sale[num - 1]++;
    }
    void print_menu()
    {
        cout << "\n=== 음료 메뉴 ===" << endl;
        for (int i = 0; i < 4; i++)
        {
            cout << i + 1 << ". " << juice[i] << ": " << price[i] << "원" << endl;
        }
        cout << "음료 번호를 선택하세요: ";
        cin >> num;
        if (num < 1 || num > 4)
        {
            cout << "잘못된 음료 번호입니다." << endl;
            return;
        }
    }
    int get_num() { return num; }
    void leave()
    {
        cout << "\n=== 음료 현황 ===" << endl;
        for (int i = 0; i < 4; i++)
        {
            cout << juice[i] << ": " << reserve[i] << "개" << endl;
        }
    }
    void supply()
    {
        cout << "\n보급할 음료의 번호(콜라->1 사이다->2 커피->3 물->4): ";
        int num1;
        cin >> num1;
        if (num1 < 1 || num1 > 4)
        {
            cout << "잘못된 음료 번호입니다." << endl;
            return;
        }
        cout << "투입할 음료의 개수: ";
        int num2;
        cin >> num2;
        if (num2 < 0)
        {
            cout << "개수가 음수일 수 없습니다." << endl;
            return;
        }
        reserve[num1 - 1] += num2;
        cout << juice[num1 - 1] << " 보급이 완료되었습니다." << endl;
    }
    void daily(){
        cout << "\n=== 일일 매출 현황 ===" << endl;
        int total = 0;
        for(int i = 0; i < 4; i++) {
            int current_sale = sale[i] * price[i];
            cout << juice[i] << ": " << sale[i] << "개, " << current_sale << "원" << endl;
            total += current_sale;
        }
        cout << "총 매출액: " << total << "원" << endl;
    }
    void next(){
        for(int i = 0; i < 4; i++){
            sale[i] = 0;
        }
        cout << "\n날짜가 변경되어 일일 매출 기록이 초기화되었습니다." << endl;
    }
};

class change
{
private:
    int c[4] = {};
    unsigned long money;
    int m[4] = {10, 10, 10, 10};
    bool sufficient;
    void search()
    {
        if(500*m[0]+100*m[1]+50*m[2]+10*m[3] < money){
            cout << "잔돈 부족" << endl;
            sufficient = false;
            return;
        }
        for (int i = 0; i < 4; i++)
            c[i] = 0;

        int temp_m[4];
        for (int i = 0; i < 4; i++)
            temp_m[i] = m[i];
        int temp_money = money;

        while (temp_money >= 500 && temp_m[0] > 0)
        {
            temp_money -= 500;
            c[0]++;
            temp_m[0]--;
        }
        while (temp_money >= 100 && temp_m[1] > 0)
        {
            temp_money -= 100;
            c[1]++;
            temp_m[1]--;
        }
        while (temp_money >= 50 && temp_m[2] > 0)
        {
            temp_money -= 50;
            c[2]++;
            temp_m[2]--;
        }
        while (temp_money >= 10 && temp_m[3] > 0)
        {
            temp_money -= 10;
            c[3]++;
            temp_m[3]--;
        }

        sufficient = (temp_money == 0);

        if (sufficient)
        {
            for (int i = 0; i < 4; i++)
                m[i] = temp_m[i];
        }
    }

public:
    change() : money(0), sufficient(true) {}

    void calculate_balance(int m)
    {
        money = m;
        search();
    }
    int print()
    {
        if (!sufficient)
        {
            cout << "\n[오류] 사용중지 (자판기 잔돈 부족)" << endl;
            return 0;
        }
        cout << "\n=== 거스름돈 반환 ===" << endl;
        cout << "500원: " << c[0] << "개" << endl
             << "100원: " << c[1] << "개" << endl
             << "50원: " << c[2] << "개" << endl
             << "10원: " << c[3] << "개" << endl;
        return 1;
    }
    void leave()
    {
        cout << "\n=== 동전 현황 ===" << endl;
        cout << "500원: " << m[0] << "개" << endl
             << "100원: " << m[1] << "개" << endl
             << "50원: " << m[2] << "개" << endl
             << "10원: " << m[3] << "개" << endl;
    }
    void supply()
    {
        cout << "\n보급할 동전의 금액(500->0 100->1 50->2 10->3): ";
        int num1;
        cin >> num1;
        if (num1 < 0 || num1 > 3)
        {
            cout << "잘못된 동전 번호입니다." << endl;
            return;
        }
        cout << "투입할 동전의 개수: ";
        int num2;
        cin >> num2;
        if (num2 < 0)
        {
            cout << "개수가 음수일 수 없습니다." << endl;
            return;
        }
        m[num1] += num2;
        cout << "동전 보급이 완료되었습니다." << endl;
    }
};

int main()
{
    menu m;
    change c;

    while (true)
    {
        cout << "\n[메인 메뉴] 1: 구매 | 2: 관리 | 3: 종료\n선택: ";
        int num = -1;
        cin >> num;

        if (num == 1)
        {
            int t = 0, fh = 0, h = 0, f = 0, ten = 0;
            cout << "\n=== 화폐 투입 ===" << endl;
            cout << "1000원권 개수: "; cin >> t;
            cout << "500원권 개수: ";  cin >> fh;
            cout << "100원권 개수: ";  cin >> h;
            cout << "50원권 개수: ";   cin >> f;
            cout << "10원권 개수: ";   cin >> ten;
            if(t < 0 || fh < 0 || h < 0 || f < 0 || ten < 0){
                cout << "투입금액 음수" << endl;
                return 0;
            }
            int money = t * 1000 + fh * 500 + h * 100 + f * 50 + ten * 10;
            if (money < 0)
            {
                cout << "금액이 0 미만입니다. 처음부터 다시 하십시오." << endl;
                continue;
            }
            
            m.print_menu();
            if (m.get_num() < 1 || m.get_num() > 4)
            {
                cout << "메뉴 번호가 잘못되었습니다. 처음부터 다시 하십시오." << endl;
                continue;
            }

            if (m.get_reserve() <= 0)
            {
                cout << "선택하신 음료의 재고가 부족합니다" << endl;
                continue;
            }

            int cha = m.charge(money);
            if (cha < 0)
            {
                cout << "투입 금액이 부족합니다. 처음부터 다시 하십시오." << endl;
                continue;
            }
            
            c.calculate_balance(cha);
            if (c.print() == 0)
            {
                return 0;
            }

            m.print();
        }
        else if (num == 2)
        {
            cout << "\n[관리 메뉴] 1: 음료재고 | 2: 동전재고 | 3: 음료보급 | 4: 동전보급 | 5: 일일매출 | 6: 날짜지남\n선택: ";
            int manage_num = -1;
            cin >> manage_num;

            if (manage_num == 1) m.leave();
            else if (manage_num == 2) c.leave();
            else if (manage_num == 3) m.supply();
            else if (manage_num == 4) c.supply();
            else if (manage_num == 5) m.daily();
            else if (manage_num == 6) m.next();
            else cout << "잘못된 관리 번호입니다." << endl;
        }
        else if (num == 3)
        {
            cout << "자판기 프로그램을 종료합니다." << endl;
            break;
        }
        else
        {
            cout << "잘못된 메뉴 선택입니다." << endl;
        }
    }
    return 0;
}