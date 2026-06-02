#include <iostream>
#include <string>
#include <cassert>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <cstdio>
#include <vector>
using namespace std;

// 전역 시뮬레이션 날짜
tm current_tm;

// 화폐 단위 (change 클래스와 동일 순서)
static const int DENOM[7] = {10000, 5000, 1000, 500, 100, 50, 10};

// ─────────────────────────────────────────────
// 날짜 저장 / 불러오기
// ─────────────────────────────────────────────
void load_date()
{
    ifstream in("current_date.txt");
    if (in.is_open())
    {
        int y, m, d; char sep;
        in >> y >> sep >> m >> sep >> d;
        in.close();
        current_tm = {};
        current_tm.tm_year  = y - 1900;
        current_tm.tm_mon   = m - 1;
        current_tm.tm_mday  = d;
        current_tm.tm_isdst = -1;
        mktime(&current_tm);
    }
    else
    {
        time_t now = time(nullptr);
        current_tm = *localtime(&now);
        char buf[11];
        strftime(buf, sizeof(buf), "%Y-%m-%d", &current_tm);
        ofstream out("current_date.txt");
        out << buf << "\n";
        out.close();
    }
}

void save_date()
{
    char buf[11];
    strftime(buf, sizeof(buf), "%Y-%m-%d", &current_tm);
    ofstream out("current_date.txt");
    out << buf << "\n";
    out.close();
}

// ─────────────────────────────────────────────
// 로그 레벨 / 출력 옵션
// ─────────────────────────────────────────────
enum class LogLevel    { DEBUG = 0, EVENT = 1, WARNING = 2, ERROR_LV = 3 };
enum class OutputOption { FILE_ONLY, SCREEN_ONLY, BOTH };

string level_str(LogLevel lv)
{
    switch (lv)
    {
        case LogLevel::DEBUG:    return "[DEBUG]  ";
        case LogLevel::EVENT:    return "[EVENT]  ";
        case LogLevel::WARNING:  return "[WARNING]";
        case LogLevel::ERROR_LV: return "[ERROR]  ";
    }
    return "[???]    ";
}

// ─────────────────────────────────────────────
// FileManager: 날짜 기반 로그 파일 관리
//   - 로그 관리 클래스에서 생성된 내용을 파일에 저장
//   - 날짜 변경 시 새 날짜 파일로 전환
// ─────────────────────────────────────────────
class FileManager
{
private:
    string filename;

public:
    FileManager() : filename("transactions.log") {}

    string get_filename() const { return filename; }

    // 로그 한 줄 파일에 추가
    void write_line(const string &line)
    {
        ofstream out(filename, ios::app);
        if (out.is_open()) out << line << "\n";
        out.close();
    }

    // 파일 전체 내용 읽기
    vector<string> read_all() const
    {
        vector<string> lines;
        ifstream in(filename);
        string line;
        while (getline(in, line))
            lines.push_back(line);
        in.close();
        return lines;
    }
};

// ─────────────────────────────────────────────
// LogManager: 출력 옵션·레벨 관리 + 포맷 생성
//   포맷: [LEVEL] yyyymmddhhmmsscc:[파일][라인] – 내용
//   출력 옵션: 파일만 / 화면만 / 둘다
//   레벨 옵션: DEBUG / EVENT / WARNING / ERROR
// ─────────────────────────────────────────────
class LogManager
{
private:
    FileManager  file_mgr;
    OutputOption output_opt;
    LogLevel     min_level;

    // 포맷 생성: [LEVEL] yyyymmddhhmmsscc:[파일][라인] – 내용
    // yyyymmdd = 시뮬레이션 날짜, hhmmsscc = 실제 시각+센티초(2자리)
    string make_entry(LogLevel lv, const string &src_file, int line, const string &msg)
    {
        char datebuf[9];
        strftime(datebuf, sizeof(datebuf), "%Y%m%d", &current_tm);

        auto now_tp = chrono::system_clock::now();
        time_t now_t = chrono::system_clock::to_time_t(now_tp);
        long cs = chrono::duration_cast<chrono::milliseconds>(
                      now_tp.time_since_epoch()).count() % 1000 / 10; // 센티초
        tm *rt = localtime(&now_t);
        char timebuf[7];
        strftime(timebuf, sizeof(timebuf), "%H%M%S", rt);

        ostringstream oss;
        oss << level_str(lv) << " "
            << datebuf << timebuf
            << setw(2) << setfill('0') << cs
            << ":" << src_file
            << "[" << line << "]"
            << " - " << msg;
        return oss.str();
    }

public:
    LogManager(OutputOption opt = OutputOption::BOTH,
               LogLevel     lv  = LogLevel::DEBUG)
        : output_opt(opt), min_level(lv) {}

    void set_output(OutputOption opt) { output_opt = opt; }
    void set_level (LogLevel     lv)  { min_level  = lv;  }
    OutputOption get_output() const   { return output_opt; }
    LogLevel     get_level()  const   { return min_level;  }

    // 로그 기록 (레벨 필터 적용)
    void log(LogLevel lv, const string &src_file, int line, const string &msg)
    {
        if (lv < min_level) return;

        string entry = make_entry(lv, src_file, line, msg);

        if (output_opt == OutputOption::FILE_ONLY || output_opt == OutputOption::BOTH)
            file_mgr.write_line(entry);
        if (output_opt == OutputOption::SCREEN_ONLY || output_opt == OutputOption::BOTH)
            cout << "[LOG] " << entry << "\n";
    }

    // 관리 메뉴 9번: 현재 날짜 로그 파일 출력
    void print_logs() const
    {
        auto lines = file_mgr.read_all();
        cout << "===== 로그 (" << file_mgr.get_filename() << ") =====" << endl;
        if (lines.empty()) cout << "(로그 없음)" << endl;
        for (auto &l : lines) cout << l << "\n";
        cout << "=========================" << endl;
        cout << "총 " << lines.size() << "건" << endl;
    }

    // 관리 메뉴 10번: 로그 설정
    void configure()
    {
        cout << "── 로그 설정 ──" << endl;

        const char *opt_name = (output_opt == OutputOption::FILE_ONLY)   ? "파일만" :
                               (output_opt == OutputOption::SCREEN_ONLY) ? "화면만" : "둘다";
        cout << "1. 출력 방식 (현재: " << opt_name << ")" << endl;
        cout << "2. 로그 레벨 (현재: " << level_str(min_level) << ")" << endl;
        cout << "설정 선택: ";
        int sel; cin >> sel;

        if (sel == 1)
        {
            cout << "출력 방식 선택" << endl;
            cout << "  1. 파일만    - 로그를 파일에만 저장 (화면 출력 없음)" << endl;
            cout << "  2. 화면만    - 로그를 화면에만 출력 (파일 저장 없음)" << endl;
            cout << "  3. 둘다      - 파일 저장 + 화면 출력 동시 수행" << endl;
            cout << "선택: ";
            int o; cin >> o;
            if      (o == 1) { output_opt = OutputOption::FILE_ONLY;   cout << "파일만으로 변경" << endl; }
            else if (o == 2) { output_opt = OutputOption::SCREEN_ONLY; cout << "화면만으로 변경" << endl; }
            else if (o == 3) { output_opt = OutputOption::BOTH;        cout << "둘다로 변경"    << endl; }
            else cout << "잘못된 입력" << endl;
        }
        else if (sel == 2)
        {
            cout << "로그 레벨 선택 (선택한 레벨 이상만 기록)" << endl;
            cout << "  1. DEBUG   - 재고/매출 조회 등 모든 동작 기록" << endl;
            cout << "  2. EVENT   - 구매, 보급, 날짜 변경 등 주요 이벤트만 기록" << endl;
            cout << "  3. WARNING - 잘못된 입력, 재고 부족 등 경고만 기록" << endl;
            cout << "  4. ERROR   - 거스름돈 부족 잠금 등 오류만 기록" << endl;
            cout << "선택: ";
            int o; cin >> o;
            if      (o == 1) { min_level = LogLevel::DEBUG;    cout << "DEBUG 이상 출력" << endl; }
            else if (o == 2) { min_level = LogLevel::EVENT;    cout << "EVENT 이상 출력" << endl; }
            else if (o == 3) { min_level = LogLevel::WARNING;  cout << "WARNING 이상 출력" << endl; }
            else if (o == 4) { min_level = LogLevel::ERROR_LV; cout << "ERROR만 출력" << endl; }
            else cout << "잘못된 입력" << endl;
        }
        else cout << "잘못된 입력" << endl;
    }
};

// 전역 로거
LogManager g_logger;

// LOG 매크로: 레벨·파일명·라인 자동 삽입
#define LOG(level, msg) g_logger.log((level), "reserve.cpp", __LINE__, (msg))

// ─────────────────────────────────────────────
// reserve 클래스
// ─────────────────────────────────────────────
class reserve
{
private:
    string name[4]         = {"콜라", "사이다", "커피", "물"};
    int    price[4]        = {1270, 2430, 3590, 530};
    int    reserve_left[4] = {0, 0, 0, 0};
    int    sale[4]         = {0, 0, 0, 0};
    int    choice;
    char   date[9];
    string data_file;

    void update_filename()
    {
        strftime(date, sizeof(date), "%Y%m%d", &current_tm);
        data_file = string(date) + "_data.txt";
    }

    void save_reserve()
    {
        ofstream out("reserve_left.txt");
        if (!out.is_open()) { cout << "파일 열기 실패(reserve_left.txt)"; assert(false); }
        for (int i = 0; i < 4; i++) out << i << "," << reserve_left[i] << "\n";
        out.close();
    }

    void save_sale()
    {
        ofstream out(data_file);
        if (!out.is_open()) { cout << "파일 열기 실패(" << data_file << ")"; assert(false); }
        for (int i = 0; i < 4; i++) out << i << "," << sale[i] << "\n";
        out.close();
    }

    void load_sale_for_today()
    {
        ifstream in(data_file);
        if (in.is_open())
        {
            string line;
            while (getline(in, line))
            {
                stringstream ss(line);
                string idx_s, val_s;
                getline(ss, idx_s, ','); getline(ss, val_s);
                int idx = stoi(idx_s), val = stoi(val_s);
                if (idx >= 0 && idx < 4) sale[idx] = val;
            }
            in.close();
        }
        else
            for (int i = 0; i < 4; i++) sale[i] = 0;
    }

public:
    reserve()
    {
        update_filename();
        ifstream in_r("reserve_left.txt");
        if (in_r.is_open())
        {
            string line;
            while (getline(in_r, line))
            {
                stringstream ss(line);
                string idx_s, val_s;
                getline(ss, idx_s, ','); getline(ss, val_s);
                int idx = stoi(idx_s), val = stoi(val_s);
                if (idx >= 0 && idx < 4) reserve_left[idx] = val;
            }
            in_r.close();
        }
        else
            save_reserve();
        load_sale_for_today();
    }

    void getleft()
    {
        cout << "음료 재고" << endl;
        for (int i = 0; i < 4; i++)
            cout << i << ". " << name[i] << " " << reserve_left[i] << "개" << endl;
        LOG(LogLevel::DEBUG, "음료 재고 확인");
    }

    void plus()
    {
        cout << "음료 보급" << endl;
        for (int i = 0; i < 4; i++) cout << i << ". " << name[i] << endl;
        int i, j;
        cout << "보급할 음료 번호: "; cin >> i;
        cout << "보급할 개수: ";      cin >> j;
        if (i < 0 || i > 3) { cout << "잘못된 음료 번호" << endl; return; }
        if (j < 0)           { cout << "보급 개수 음수"   << endl; return; }
        reserve_left[i] += j;
        save_reserve();
        cout << name[i] << "의 재고가 " << reserve_left[i] << "개" << endl;
        LOG(LogLevel::EVENT, "음료 보급 index=" + to_string(i) +
                             " name=" + name[i] + " count=" + to_string(j));
    }

    void print_day()
    {
        int day_sale[4] = {0, 0, 0, 0};
        ifstream in(data_file);
        if (in.is_open())
        {
            string line;
            while (getline(in, line))
            {
                stringstream ss(line);
                string idx_s, val_s;
                getline(ss, idx_s, ','); getline(ss, val_s);
                int idx = stoi(idx_s), val = stoi(val_s);
                if (idx >= 0 && idx < 4) day_sale[idx] = val;
            }
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
        cout << "\n총 금액: " << total << "원" << endl;
        LOG(LogLevel::DEBUG, "일일 매출 확인 date=" + string(date) +
                             " total=" + to_string(total));
    }

    void print_month()
    {
        char ym[7];
        strftime(ym, sizeof(ym), "%Y%m", &current_tm);
        cout << "월 매출 현황" << endl;
        cout << ym << endl;
        int month_sale[4] = {0, 0, 0, 0};
        for (int d = 1; d <= 31; d++)
        {
            char fname[32];
            snprintf(fname, sizeof(fname), "%s%02d_data.txt", ym, d);
            ifstream in(fname);
            if (!in.is_open()) continue;
            string line;
            while (getline(in, line))
            {
                stringstream ss(line);
                string idx_s, val_s;
                getline(ss, idx_s, ','); getline(ss, val_s);
                int idx = stoi(idx_s), val = stoi(val_s);
                if (idx >= 0 && idx < 4) month_sale[idx] += val;
            }
            in.close();
        }
        int total = 0;
        for (int i = 0; i < 4; i++)
        {
            cout << name[i] << " " << month_sale[i] << " " << price[i] * month_sale[i] << endl;
            total += price[i] * month_sale[i];
        }
        cout << "\n총 금액: " << total << "원" << endl;
        LOG(LogLevel::DEBUG, "월 매출 확인 month=" + string(ym) +
                             " total=" + to_string(total));
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
            LOG(LogLevel::WARNING, "잘못된 음료 번호 입력 index=" + to_string(choice));
            return -1;
        }
        if (reserve_left[choice] <= 0)
        {
            cout << "재고 부족" << endl;
            LOG(LogLevel::WARNING, "재고 부족 index=" + to_string(choice) +
                                   " name=" + name[choice]);
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
        LOG(LogLevel::EVENT, "구매 완료 index=" + to_string(choice) +
                             " name=" + name[choice] +
                             " price=" + to_string(price[choice]));
    }

    void tomorrow()
    {
        current_tm.tm_mday += 1;
        mktime(&current_tm);
        save_date();
        update_filename();
        load_sale_for_today();
        cout << "날짜 변경: " << date << endl;
        LOG(LogLevel::EVENT, "날짜 변경 date=" + string(date));
    }
};

// ─────────────────────────────────────────────
// change 클래스
// ─────────────────────────────────────────────
class change
{
private:
    const int coin[7]  = {10000, 5000, 1000, 500, 100, 50, 10};
    int       exist[7] = {0, 0, 0, 0, 0, 0, 0};
    bool      is_locked = false;

    void save()
    {
        ofstream out("money_left.txt");
        if (!out.is_open()) { cout << "파일 열기 실패(money_left.txt)"; assert(false); }
        for (int i = 0; i < 7; i++) out << i << "," << exist[i] << "\n";
        out.close();
    }

public:
    change()
    {
        ifstream in("money_left.txt");
        if (in.is_open())
        {
            string line;
            while (getline(in, line))
            {
                stringstream ss(line);
                string idx_s, val_s;
                getline(ss, idx_s, ','); getline(ss, val_s);
                int idx = stoi(idx_s), val = stoi(val_s);
                if (idx >= 0 && idx < 7) exist[idx] = val;
            }
            in.close();
        }
        else
            save();
    }

    bool get_locked_status() { return is_locked; }

    // input[i]: 투입한 화폐 종류별 개수
    bool getexchange(unsigned long change_amount, int input[7])
    {
        int temp_exist[7];
        for (int i = 0; i < 7; i++) temp_exist[i] = exist[i] + input[i];

        int exchange[7] = {0, 0, 0, 0, 0, 0, 0};
        unsigned long rem = change_amount;
        for (int i = 0; i < 7; i++)
        {
            while (rem >= (unsigned long)coin[i] && temp_exist[i] > 0)
            {
                rem -= coin[i]; temp_exist[i]--; exchange[i]++;
            }
        }
        if (rem > 0)
        {
            cout << "거스름돈 부족 - 사용 중지 상태" << endl;
            is_locked = true;
            LOG(LogLevel::ERROR_LV, "거스름돈 부족 잠금 change_amount=" +
                                    to_string(change_amount));
            return false;
        }
        for (int i = 0; i < 7; i++) exist[i] = temp_exist[i];
        save();
        cout << "거스름돈 반환" << endl;
        for (int i = 0; i < 7; i++)
            if (exchange[i] > 0)
                cout << coin[i] << "원: " << exchange[i] << "개" << endl;
        return true;
    }

    void getexist()
    {
        cout << "자판기 화폐 보유 현황" << endl;
        if (is_locked) cout << "사용 중지 상태" << endl;
        for (int i = 0; i < 7; i++)
            cout << coin[i] << "원: " << exist[i] << "개" << endl;
        LOG(LogLevel::DEBUG, "화폐 재고 확인");
    }

    void plus_coin()
    {
        cout << "동전 보급(500->0, 100->1, 50->2, 10->3)" << endl;
        int i, j;
        cout << "보급할 동전 종류 번호: "; cin >> i;
        cout << "보급할 동전 개수: ";      cin >> j;
        if (i < 0 || i > 3) { cout << "잘못된 동전 번호" << endl; return; }
        if (j < 0)           { cout << "보급 개수 음수"   << endl; return; }
        exist[i + 3] += j;
        save();
        cout << coin[i + 3] << "원 동전 재고가 " << exist[i + 3] << "개" << endl;
        LOG(LogLevel::EVENT, "동전 보급 denom=" + to_string(coin[i + 3]) +
                             " count=" + to_string(j));
        if (is_locked && j > 0)
        {
            is_locked = false;
            cout << "자판기 사용 가능" << endl;
            LOG(LogLevel::EVENT, "거스름돈 보급으로 잠금 해제");
        }
    }

    void plus_bill()
    {
        cout << "지폐 보급(10000->0, 5000->1, 1000->2)" << endl;
        int i, j;
        cout << "보급할 지폐 종류 번호: "; cin >> i;
        cout << "보급할 지폐 개수: ";      cin >> j;
        if (i < 0 || i > 2) { cout << "잘못된 지폐 번호" << endl; return; }
        if (j < 0)           { cout << "보급 개수 음수"   << endl; return; }
        exist[i] += j;
        save();
        cout << coin[i] << "원 지폐 재고가 " << exist[i] << "장" << endl;
        LOG(LogLevel::EVENT, "지폐 보급 denom=" + to_string(coin[i]) +
                             " count=" + to_string(j));
        if (is_locked && j > 0)
        {
            is_locked = false;
            cout << "자판기 사용 가능" << endl;
            LOG(LogLevel::EVENT, "거스름돈 보급으로 잠금 해제");
        }
    }
};

// ─────────────────────────────────────────────
// main
// ─────────────────────────────────────────────
int main()
{
    load_date();

    reserve r;
    change  c;

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
                LOG(LogLevel::WARNING, "잠금 상태에서 구매 시도 차단");
                continue;
            }

            cout << "화폐 투입" << endl;
            int input[7] = {0, 0, 0, 0, 0, 0, 0};
            const char *label[7] = {
                "10000원권 개수: ", "5000원권 개수: ", "1000원권 개수: ",
                "500원 개수: ", "100원 개수: ", "50원 개수: ", "10원 개수: "
            };

            bool valid = true;
            for (int i = 0; i < 7; i++)
            {
                cout << label[i];
                int cnt; cin >> cnt;
                if (cnt < 0)
                {
                    cout << "투입 개수가 음수입니다." << endl;
                    LOG(LogLevel::WARNING, "음수 투입 시도 denom_index=" + to_string(i));
                    valid = false;
                    break;
                }
                input[i] = cnt;
            }
            if (!valid) continue;

            unsigned long money = 0;
            for (int i = 0; i < 7; i++)
                money += (unsigned long)input[i] * DENOM[i];

            if (money == 0)
            {
                cout << "투입 금액 없음" << endl;
                LOG(LogLevel::WARNING, "투입 금액 없음");
                continue;
            }

            int price = r.menu();
            if (price == -1) continue;

            if ((unsigned long)price > money)
            {
                cout << "투입 금액 부족 - 반환" << endl;
                LOG(LogLevel::WARNING, "투입 금액 부족 반환 inserted=" +
                                       to_string(money) + " price=" + to_string(price));
                continue;
            }

            if (c.getexchange(money - (unsigned long)price, input))
                r.success();
            else
            {
                cout << "반환" << endl;
                LOG(LogLevel::WARNING, "거스름돈 부족으로 전액 반환 inserted=" +
                                       to_string(money));
            }
        }
        else if (num == 2)
        {
            cout << "관리자 메뉴" << endl;
            cout << "1. 음료재고\n2. 음료보급\n3. 화폐재고\n4. 동전보급\n";
            cout << "5. 지폐보급\n6. 일일매출\n7. 월매출\n8. 날짜지남\n";
            cout << "9. 로그출력\n10. 로그설정" << endl;
            int mn = -1;
            cout << "관리 선택: ";
            cin >> mn;
            if      (mn == 1)  r.getleft();
            else if (mn == 2)  r.plus();
            else if (mn == 3)  c.getexist();
            else if (mn == 4)  c.plus_coin();
            else if (mn == 5)  c.plus_bill();
            else if (mn == 6)  r.print_day();
            else if (mn == 7)  r.print_month();
            else if (mn == 8)  r.tomorrow();
            else if (mn == 9)  g_logger.print_logs();
            else if (mn == 10) g_logger.configure();
            else               cout << "잘못된 번호" << endl;
        }
        else if (num == 3)
        {
            cout << "종료" << endl;
            return 0;
        }
        else
            cout << "잘못된 번호" << endl;
    }
}