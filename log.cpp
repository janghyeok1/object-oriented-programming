#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
using namespace std;

// ─────────────────────────────────────────────
// 로그 범위(레벨) 옵션 : error, warning, event, debug
// 값이 클수록 심각. (범위 필터링에 사용)
// ─────────────────────────────────────────────
enum class LogLevel {
    DEBUG   = 0,   // 디버그용 상세 로그
    EVENT   = 1,   // 일반 동작/이벤트
    WARNING = 2,   // 경고
    ERROR   = 3    // 오류
};

// ─────────────────────────────────────────────
// 출력 옵션 : 파일 및 화면 컨트롤
// ─────────────────────────────────────────────
enum class OutputMode {
    FILE_ONLY,     // 파일 저장만
    CONSOLE_ONLY,  // 화면 출력만
    BOTH           // 화면 출력 및 파일 저장
};

// yyyymmdd 날짜 문자열
string dateString() {
    auto now = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(now);
    tm lt = *localtime(&t);

    ostringstream oss;
    oss << put_time(&lt, "%Y%m%d");
    return oss.str();
}

// ═════════════════════════════════════════════
// 파일 관리 클래스
//  - 로그 관리 클래스에서 생성된 로그 내용을 파일에 저장
//  - 날짜가 바뀌면 현재 날짜로 파일을 새로 생성 (하루 1파일)
// ═════════════════════════════════════════════
class LogFile {
private:
    ofstream ofs;          // 현재 열려 있는 파일 스트림
    string   currentDate;  // 현재 파일의 날짜 (yyyymmdd)
    string   baseName;     // 파일 이름 접두사 (예: "myapp")

    void openForDate(const string& date) {
        if (ofs.is_open()) ofs.close();
        string fileName = baseName + "_" + date + ".txt";
        ofs.open(fileName, ios::app);  // 같은 날 로그는 이어쓰기
        currentDate = date;
    }

public:
    LogFile(const string& baseName = "log") {
        this->baseName = baseName;
    }

    ~LogFile() {
        if (ofs.is_open()) ofs.close();
    }

    void write(const string& line) {
        string today = dateString();
        if (!ofs.is_open() || today != currentDate) {
            openForDate(today);
        }
        if (ofs.is_open()) {
            ofs << line << '\n';
            ofs.flush();
        }
    }
};

// ═════════════════════════════════════════════
// 로그 관리 클래스
//  - 로그 옵션(출력 방식 / 범위) 관리
//  - 로그 내용을 포맷팅해서 파일 관리 클래스로 넘김
// ═════════════════════════════════════════════
class Logger {
private:
    LogFile    file;      // 파일 관리 클래스 (Logger가 소유)
    OutputMode mode;      // 출력 옵션
    LogLevel   minLevel;  // 로그 범위 (이 레벨 미만은 무시)

    string levelToString(LogLevel level) {
        switch (level) {
            case LogLevel::DEBUG:   return "DEBUG";
            case LogLevel::EVENT:   return "EVENT";
            case LogLevel::WARNING: return "WARNING";
            case LogLevel::ERROR:   return "ERROR";
        }
        return "UNKNOWN";
    }

    // yyyymmddhhmmssss (마지막 2자리는 1/100초)
    string timeStamp() {
        auto now = chrono::system_clock::now();
        time_t t = chrono::system_clock::to_time_t(now);
        tm lt = *localtime(&t);

        auto ms = chrono::duration_cast<chrono::milliseconds>(
                      now.time_since_epoch()) % 1000;
        int centi = (int)(ms.count() / 10);

        ostringstream oss;
        oss << put_time(&lt, "%Y%m%d%H%M%S")
            << setw(2) << setfill('0') << centi;
        return oss.str();
    }

public:
    Logger(OutputMode mode = OutputMode::BOTH,
           LogLevel minLevel = LogLevel::DEBUG,
           const string& baseName = "log")
        : file(baseName) {
        this->mode = mode;
        this->minLevel = minLevel;
    }

    void setMode(OutputMode m)  { mode = m; }
    void setLevel(LogLevel l)   { minLevel = l; }
    OutputMode getMode()        { return mode; }      // 현재 설정 확인용
    LogLevel   getLevel()       { return minLevel; }  // 현재 설정 확인용

    // 실제 로그 메소드. 보통 LOG 매크로를 통해 호출.
    // 반환값: 실제로 기록됐으면 true, 범위에 걸려 무시되면 false
    bool log(LogLevel level, const string& msg,
             const char* srcFile, int srcLine) {
        if ((int)level < (int)minLevel) return false;  // 범위 필터

        ostringstream oss;
        oss << timeStamp() << ":"
            << "[" << srcFile << "]"
            << "[" << srcLine << "]"
            << " - [" << levelToString(level) << "] "
            << msg;
        string line = oss.str();

        if (mode == OutputMode::CONSOLE_ONLY || mode == OutputMode::BOTH) {
            cout << line << endl;
        }
        if (mode == OutputMode::FILE_ONLY || mode == OutputMode::BOTH) {
            file.write(line);
        }
        return true;
    }
};

// 호출 위치의 파일명/줄번호를 자동으로 잡아주는 매크로
#define LOG(logger, level, msg) \
    (logger).log((level), (msg), __FILE__, __LINE__)

// ─────────────────────────────────────────────
// 메뉴 표시용 한글 이름 변환 (main에서만 사용)
// ─────────────────────────────────────────────
string modeName(OutputMode m) {
    if (m == OutputMode::FILE_ONLY)    return "파일 저장만";
    if (m == OutputMode::CONSOLE_ONLY) return "화면 출력만";
    return "화면 출력 + 파일 저장";
}

string levelName(LogLevel l) {
    switch (l) {
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::EVENT:   return "EVENT";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR:   return "ERROR";
    }
    return "?";
}

// 한 줄 입력받아 정수로 변환 (잘못 입력하면 -1)
int readInt() {
    string s;
    getline(cin, s);
    try { return stoi(s); }
    catch (...) { return -1; }
}

// ═════════════════════════════════════════════
// 메인 : 메뉴 선택 + 값 입력 방식
// ═════════════════════════════════════════════
int main() {
    // 프로그램 시작 전 로그 객체 생성 (기본: 화면+파일, DEBUG 이상)
    Logger logger(OutputMode::BOTH, LogLevel::DEBUG, "myapp");

    while (true) {
        cout << "\n========== 로그 프로그램 ==========\n";
        cout << " 현재 출력 : " << modeName(logger.getMode()) << "\n";
        cout << " 현재 범위 : " << levelName(logger.getLevel())
             << " 이상\n";
        cout << "-----------------------------------\n";
        cout << " 1. 출력 방식 설정\n";
        cout << " 2. 로그 범위 설정\n";
        cout << " 3. 로그 남기기\n";
        cout << " 0. 종료\n";
        cout << "선택 > ";

        int sel = readInt();

        if (sel == 0) {
            cout << "프로그램을 종료합니다.\n";
            break;
        }
        else if (sel == 1) {
            // 출력 방식 설정
            cout << "\n[출력 방식]\n";
            cout << " 1. 파일 저장만\n";
            cout << " 2. 화면 출력만\n";
            cout << " 3. 화면 + 파일\n";
            cout << "선택 > ";
            int m = readInt();
            if (m == 1) logger.setMode(OutputMode::FILE_ONLY);
            else if (m == 2) logger.setMode(OutputMode::CONSOLE_ONLY);
            else if (m == 3) logger.setMode(OutputMode::BOTH);
            else { cout << "잘못된 입력입니다.\n"; continue; }
            cout << ">> 출력 방식: " << modeName(logger.getMode()) << "\n";
        }
        else if (sel == 2) {
            // 로그 범위 설정
            cout << "\n[로그 범위] 이 레벨 이상만 기록됩니다\n";
            cout << " 1. DEBUG (전부)\n";
            cout << " 2. EVENT\n";
            cout << " 3. WARNING\n";
            cout << " 4. ERROR (오류만)\n";
            cout << "선택 > ";
            int l = readInt();
            if (l == 1) logger.setLevel(LogLevel::DEBUG);
            else if (l == 2) logger.setLevel(LogLevel::EVENT);
            else if (l == 3) logger.setLevel(LogLevel::WARNING);
            else if (l == 4) logger.setLevel(LogLevel::ERROR);
            else { cout << "잘못된 입력입니다.\n"; continue; }
            cout << ">> 로그 범위: " << levelName(logger.getLevel())
                 << " 이상\n";
        }
        else if (sel == 3) {
            // 로그 남기기 : 레벨 선택 + 메시지 입력
            cout << "\n[로그 레벨 선택]\n";
            cout << " 1. DEBUG\n 2. EVENT\n 3. WARNING\n 4. ERROR\n";
            cout << "선택 > ";
            int l = readInt();

            LogLevel level;
            if (l == 1) level = LogLevel::DEBUG;
            else if (l == 2) level = LogLevel::EVENT;
            else if (l == 3) level = LogLevel::WARNING;
            else if (l == 4) level = LogLevel::ERROR;
            else { cout << "잘못된 입력입니다.\n"; continue; }

            cout << "로그 내용 입력 > ";
            string msg;
            getline(cin, msg);

            // LOG 매크로로 기록 (__FILE__, __LINE__ 자동)
            bool recorded = LOG(logger, level, msg);
            if (!recorded) {
                cout << "(현재 범위보다 낮은 레벨이라 기록되지 않았습니다)\n";
            }
        }
        else {
            cout << "메뉴에 없는 번호입니다.\n";
        }
    }

    return 0;
}