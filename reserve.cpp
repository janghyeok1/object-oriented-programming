#include <iostream>
#include <string>
#include <cassert>
#include <fstream>
#include <ctime>
#include <cstdio>

using namespace std;

time_t now = time(nullptr);
tm *local = localtime(&now);

class reserve
{
private:
    string name[4] = {"콜라", "사이다", "커피", "물"};
    int price[4] = {1270, 2430, 3590, 530};
    int reserve_left[4] = {10, 10, 10, 10};
    int sale[4] = {0, 0, 0, 0};
    int choice;
    char date[9];
    string data_file;

    void update_filename()
    {
        strftime(date, sizeof(date), "%Y%m%d", local);
        data_file = string(date) + "_data.dat";
    }

    void save_reserve()
    {
        ofstream out("reserve_left.dat", ios::out | ios::binary);
        if (!out.is_open()) { cout << "the file cannot be opened for writing!"; assert(false); }
        for (int i = 0; i < 4; i++)
            out.write(reinterpret_cast<char *>(&reserve_left[i]), sizeof(int));
        out.close();
    }

    void save_sale()
    {
        ofstream out(data_file, ios::out | ios::binary);
        if (!out.is_open()) { cout << "the file cannot be opened for writing!"; assert(false); }
        for (int i = 0; i < 4; i++)
            out.write(reinterpret_cast<char *>(&sale[i]), sizeof(int));
        out.close();
    }

    void load_sale_for_today()
    {
        ifstream in(data_file, ios::in | ios::binary);
        if (in.is_open())
        {
            for (int i = 0; i < 4; i++)
                in.read(reinterpret_cast<char *>(&sale[i]), sizeof(int));
            in.close();
        }
        else
        {
            for (int i = 0; i < 4; i++) sale[i] = 0;
        }
    }

public:
    reserve()
    {
        update_filename();
        ifstream in_r("reserve_left.dat", ios::in | ios::binary);
        if (in_r.is_open())
        {
            for (int i = 0; i < 4; i++)
                in_r.read(reinterpret_cast<char *>(&reserve_left[i]), sizeof(int));
            in_r.close();
        }
        else
        {
            save_reserve();
        }
        load_sale_for_today();
    }

    void getleft()
    {
        cout << "음료 재고" << endl;
        for (int i = 0; i < 4; i++)
            cout << i << ". " << name[i] << " " << reserve_left[i] << "개" << endl;
    }

    void plus()
    {
        cout << "음료 보급" << endl;
        for (int i = 0; i < 4; i++)
            cout << i << ". " << name[i] << endl;
        int i, j;
        cout << "보급할 음료 번호: ";
        cin >> i;
        cout << "보급할 개수: ";
        cin >> j;
        if (i < 0 || i > 3)
        {
            cout << "잘못된 음료 번호" << endl;
            return;
        }
        if (j < 0)
        {
            cout << "보급 음료 개수 음수" << endl;
            return;
        }
        reserve_left[i] += j;
        save_reserve();
        cout << name[i] << "의 재고가 " << reserve_left[i] << "개" << endl;
    }

    void print_day()
    {
        int day_sale[4] = {0, 0, 0, 0};
        ifstream in(data_file, ios::in | ios::binary);
        if (in.is_open())
        {
            for (int i = 0; i < 4; i++)
                in.read(reinterpret_cast<char *>(&day_sale[i]), sizeof(int));
            in.close();
        }
        cout << "일일 매출 현황" << endl;
        cout << date << endl;
        int total = 0;
        for (int i = 0; i < 4; i++)
        {
            cout << name[i] << " " << day_sale[i] << " " << price[i] * day_sale[i] << endl;
            total += price[i] * day_sale[i];
        }
        cout << endl << "총 금액: " << total << "원" << endl;
    }

    void print_month()
    {
        char ym[7];
        strftime(ym, sizeof(ym), "%Y%m", local);
        cout << "월 매출 현황" << endl;
        cout << ym << endl;
        int month_sale[4] = {0, 0, 0, 0};
        for (int d = 1; d <= 31; d++)
        {
            char fname[32];
            snprintf(fname, sizeof(fname), "%s%02d_data.dat", ym, d);
            ifstream in(fname, ios::in | ios::binary);
            if (!in.is_open()) continue;
            int temp[4];
            for (int i = 0; i < 4; i++)
                in.read(reinterpret_cast<char *>(&temp[i]), sizeof(int));
            in.close();
            for (int i = 0; i < 4; i++)
                month_sale[i] += temp[i];
        }
        int total = 0;
        for (int i = 0; i < 4; i++)
        {
            cout << name[i] << " " << month_sale[i] << " " << price[i] * month_sale[i] << endl;
            total += price[i] * month_sale[i];
        }
        cout << endl << "총 금액: " << total << "원" << endl;
    }

    int menu()
    {
        cout << "음료 메뉴" << endl;
        for (int i = 0; i < 4; i++)
            cout << i << ". " << name[i] << ": " << price[i] << "원" << endl;
        cout << "음료 번호를 선택하세요(0, 1, 2, 3): ";
        cin >> choice;
        if (choice < 0 || choice > 3)
        {
            cout << "잘못된 음료 번호" << endl;
            return -1;
        }
        if (reserve_left[choice] <= 0)
        {
            cout << "재고 부족" << endl;
            return -1;
        }
        cout << name[choice] << " 선택 (가격: " << price[choice] << "원)" << endl;
        return price[choice];
    }

    void success()
    {
        reserve_left[choice]--;
        sale[choice]++;
        save_reserve();
        save_sale();

        char timebuf[10];
        strftime(timebuf, sizeof(timebuf), "%H:%M:%S", local);
        ofstream log("transactions.log", ios::app);
        log << date << " " << timebuf << " " << name[choice] << " " << price[choice] << endl;
        log.close();
    }

    void tomorrow()
    {
        local->tm_mday += 1;
        mktime(local);
        update_filename();
        load_sale_for_today();
        cout << "날짜 변경: " << date << endl;
    }
};

// 동전과 지폐를 통합 관리 (거스름돈 계산 시 함께 사용)
class change
{
private:
    // 높은 단위부터 순서대로: 10000, 5000, 1000, 500, 100, 50, 10
    const int coin[7] = {10000, 5000, 1000, 500, 100, 50, 10};
    int exchange[7]   = {0, 0, 0, 0, 0, 0, 0};
    int exist[7]      = {10, 10, 10, 10, 10, 10, 10};
    bool is_locked = false;

    void save()
    {
        ofstream out("money_left.dat", ios::out | ios::binary);
        if (!out.is_open()) { cout << "the file cannot be opened for writing!"; assert(false); }
        for (int i = 0; i < 7; i++)
            out.write(reinterpret_cast<char *>(&exist[i]), sizeof(int));
        out.close();
    }

public:
    change()
    {
        ifstream in("money_left.dat", ios::in | ios::binary);
        if (in.is_open())
        {
            for (int i = 0; i < 7; i++)
                in.read(reinterpret_cast<char *>(&exist[i]), sizeof(int));
            in.close();
        }
        else
        {
            save();
        }
    }

    bool get_locked_status() { return is_locked; }

    // 투입된 화폐 추가 (tt:10000, fk:5000, t:1000, fh:500, h:100, f:50, ten:10)
    void add_exist(int tt, int fk, int t, int fh, int h, int f, int ten)
    {
        exist[0] += tt;
        exist[1] += fk;
        exist[2] += t;
        exist[3] += fh;
        exist[4] += h;
        exist[5] += f;
        exist[6] += ten;
        save();
    }

    // 거스름돈 계산: 동전·지폐 통합 greedy
    // 500 부족 → 100, 100 부족 → 50, 50 부족 → 10 자동 적용
    bool getexchange(unsigned long money)
    {
        int temp_exist[7];
        for (int i = 0; i < 7; i++) temp_exist[i] = exist[i];
        for (int i = 0; i < 7; i++) exchange[i] = 0;
        unsigned long rem = money;
        for (int i = 0; i < 7; i++)
        {
            while (rem >= (unsigned long)coin[i] && temp_exist[i] > 0)
            {
                rem -= coin[i];
                temp_exist[i]--;
                exchange[i]++;
            }
        }
        if (rem > 0)
        {
            cout << "거스름돈 부족 - 사용 중지 상태" << endl;
            is_locked = true;
            return false;
        }
        for (int i = 0; i < 7; i++) exist[i] = temp_exist[i];
        save();
        cout << "거스름돈 반환" << endl;
        for (int i = 0; i < 7; i++)
        {
            if (exchange[i] > 0)
                cout << coin[i] << "원: " << exchange[i] << "개" << endl;
        }
        return true;
    }

    void getexist()
    {
        cout << "자판기 화폐 보유 현황" << endl;
        if (is_locked) cout << "사용 중지 상태" << endl;
        for (int i = 0; i < 7; i++)
            cout << coin[i] << "원: " << exist[i] << "개" << endl;
    }

    // 동전 보급 (500->0, 100->1, 50->2, 10->3)
    void plus_coin()
    {
        cout << "동전 보급(500->0, 100->1, 50->2, 10->3)" << endl;
        int i, j;
        cout << "보급할 동전 종류 번호: ";
        cin >> i;
        cout << "보급할 동전 개수: ";
        cin >> j;
        if (i < 0 || i > 3)
        {
            cout << "잘못된 동전 번호" << endl;
            return;
        }
        if (j < 0)
        {
            cout << "보급 동전 개수 음수" << endl;
            return;
        }
        exist[i + 3] += j; // coin 배열에서 동전은 인덱스 3~6
        save();
        cout << coin[i + 3] << "원 동전 재고가 " << exist[i + 3] << "개" << endl;
        if (is_locked && j > 0)
        {
            is_locked = false;
            cout << "자판기 사용 가능" << endl;
        }
    }

    // 지폐 보급 (10000->0, 5000->1, 1000->2)
    void plus_bill()
    {
        cout << "지폐 보급(10000->0, 5000->1, 1000->2)" << endl;
        int i, j;
        cout << "보급할 지폐 종류 번호: ";
        cin >> i;
        cout << "보급할 지폐 개수: ";
        cin >> j;
        if (i < 0 || i > 2)
        {
            cout << "잘못된 지폐 번호" << endl;
            return;
        }
        if (j < 0)
        {
            cout << "보급 지폐 개수 음수" << endl;
            return;
        }
        exist[i] += j; // coin 배열에서 지폐는 인덱스 0~2
        save();
        cout << coin[i] << "원 지폐 재고가 " << exist[i] << "장" << endl;
        if (is_locked && j > 0)
        {
            is_locked = false;
            cout << "자판기 사용 가능" << endl;
        }
    }
};

int main()
{
    reserve r;
    change c;

    while (true)
    {
        cout << "\n1: 구매\n2: 관리\n3: 종료" << endl;
        if (c.get_locked_status())
            cout << "(현재 사용 중지 상태)" << endl;

        int num = -1;
        cout << "메뉴 선택: ";
        cin >> num;

        if (num == 1)
        {
            if (c.get_locked_status())
            {
                cout << "불가능한 메뉴" << endl;
                continue;
            }
            cout << "화폐 투입" << endl;
            int tt = 0, fk = 0, t = 0, fh = 0, h = 0, f = 0, ten = 0;
            cout << "10000원권 개수: "; cin >> tt;
            cout << "5000원권 개수: ";  cin >> fk;
            cout << "1000원권 개수: ";  cin >> t;
            cout << "500원 개수: ";     cin >> fh;
            cout << "100원 개수: ";     cin >> h;
            cout << "50원 개수: ";      cin >> f;
            cout << "10원 개수: ";      cin >> ten;

            if (tt < 0 || fk < 0 || t < 0 || fh < 0 || h < 0 || f < 0 || ten < 0)
            {
                cout << "투입 금액이나 개수 음수" << endl;
                continue;
            }

            int money = tt * 10000 + fk * 5000 + t * 1000
                      + fh * 500 + h * 100 + f * 50 + ten * 10;
            if (money == 0)
            {
                cout << "투입 금액 없음" << endl;
                continue;
            }

            int price = r.menu();
            if (price == -1) continue;

            if (price > money)
            {
                cout << "투입 금액 부족 - 반환" << endl;
                continue;
            }

            c.add_exist(tt, fk, t, fh, h, f, ten);
            if (c.getexchange(money - price))
            {
                r.success();
            }
            else
            {
                c.add_exist(-tt, -fk, -t, -fh, -h, -f, -ten);
                cout << "반환" << endl;
            }
        }
        else if (num == 2)
        {
            cout << "관리자 메뉴" << endl;
            cout << "1. 음료재고\n2. 음료보급\n3. 화폐재고\n4. 동전보급\n";
            cout << "5. 지폐보급\n6. 일일매출\n7. 월매출\n8. 날짜지남" << endl;
            int mn = -1;
            cout << "관리 선택: ";
            cin >> mn;

            if      (mn == 1) r.getleft();
            else if (mn == 2) r.plus();
            else if (mn == 3) c.getexist();
            else if (mn == 4) c.plus_coin();
            else if (mn == 5) c.plus_bill();
            else if (mn == 6) r.print_day();
            else if (mn == 7) r.print_month();
            else if (mn == 8) r.tomorrow();
            else cout << "잘못된 번호" << endl;
        }
        else if (num == 3)
        {
            cout << "종료" << endl;
            return 0;
        }
        else
        {
            cout << "잘못된 번호" << endl;
        }
    }
}
