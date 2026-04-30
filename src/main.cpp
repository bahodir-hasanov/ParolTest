/*
 * ParolTest - Parol kuchini tahlil qiluvchi dastur
 * C++17 | Terminal UI (ANSI colors)
 * Kompilyatsiya: g++ -std=c++17 -O2 -o paroltest main.cpp
 */

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <cmath>
#include <random>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <limits>
#include <regex>
#include <map>

#ifdef _WIN32
  #include <conio.h>
  #include <windows.h>
#else
  #include <termios.h>
  #include <unistd.h>
#endif

// ─────────────────────────────────────────────
//  ANSI rang kodlari
// ─────────────────────────────────────────────
namespace Color {
    const std::string RESET   = "\033[0m";
    const std::string BOLD    = "\033[1m";
    const std::string DIM     = "\033[2m";
    const std::string RED     = "\033[31m";
    const std::string GREEN   = "\033[32m";
    const std::string YELLOW  = "\033[33m";
    const std::string BLUE    = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN    = "\033[36m";
    const std::string WHITE   = "\033[37m";
    const std::string BRED    = "\033[1;31m";
    const std::string BGREEN  = "\033[1;32m";
    const std::string BYELLOW = "\033[1;33m";
    const std::string BCYAN   = "\033[1;36m";
    const std::string BWHITE  = "\033[1;37m";
}

// ─────────────────────────────────────────────
//  Umumiy mashhur parollar bazasi
// ─────────────────────────────────────────────
const std::set<std::string> COMMON_PASSWORDS = {
    // Global
    "123456","12345678","123456789","password","qwerty","qwerty123","111111",
    "000000","123123","abc123","letmein","monkey","dragon","master","sunshine",
    "princess","football","baseball","shadow","superman","batman","trustno1",
    "hello","charlie","iloveyou","love123","azerty","qwertyuiop","asdfgh",
    "zxcvbnm","1234qwer","qazwsx","1q2w3e","987654","654321","159753","258456",
    "741852","369258","147258","112233","223344","556677","qwerty1","qwerty12",
    "qwerty1234","qwerty2024","qwerty777","admin2024","admin777","password1",
    "password123","pass2024","1234","12345","1234567","admin","admin123","root",
    "user","test123","welcome","welcome123","login","guest","pass123",
    // O'zbekcha
    "salom","salom123","salomjon","salom1234","salomjon123","salom2024",
    "salom2000","salom01","salom777","salom7777","uzbek","uzbek123","toshkent",
    "tashkent","uzb123","uzbekiston","uzb777","uzb2024","uzb2000","uzb01",
    "diyor","diyor123","ali123","vali123","aziz123","umar123","islom123",
    "muslim123","bek123","bekzod123","telefon","nomer123","raqam123","parol123",
    "kod123","pin123","pin2024","pin0000","0000","1111","parol","kirish",
    "foydalanuvchi","maxsus","maxfiy","sirli","yashirin"
};

// ─────────────────────────────────────────────
//  Pattern aniqlash
// ─────────────────────────────────────────────
struct PatternResult {
    bool found;
    std::string description;
};

std::vector<PatternResult> detectPatterns(const std::string& password) {
    std::vector<PatternResult> results;

    // Faqat raqamlar
    if (std::all_of(password.begin(), password.end(), ::isdigit)) {
        results.push_back({true, "Faqat raqamlar"});
    }

    // Faqat harflar
    if (std::all_of(password.begin(), password.end(), ::isalpha)) {
        results.push_back({true, "Faqat harflar (belgi yo'q)"});
    }

    // Takroriy belgilar (aaa, 111)
    bool allSame = password.length() > 1 &&
        std::all_of(password.begin(), password.end(),
            [&](char c){ return c == password[0]; });
    if (allSame) {
        results.push_back({true, "Barcha belgilar bir xil"});
    }

    // Ketma-ket raqamlar (123456)
    bool seqNum = true;
    for (size_t i = 1; i < password.size() && seqNum; i++) {
        if (!isdigit(password[i]) || password[i] != password[i-1]+1)
            seqNum = false;
    }
    if (seqNum && password.size() >= 3) {
        results.push_back({true, "Ketma-ket raqamlar (1234...)"});
    }

    // Klaviatura qatori (qwerty, asdf, zxcv)
    std::vector<std::string> kbRows = {"qwertyuiop","asdfghjkl","zxcvbnm","1234567890"};
    std::string lower = password;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    for (auto& row : kbRows) {
        if (row.find(lower) != std::string::npos && password.size() >= 3) {
            results.push_back({true, "Klaviatura qatori pattern"});
            break;
        }
    }

    // Yil (19xx, 20xx)
    std::regex yearRe("(19|20)\\d{2}");
    if (std::regex_search(password, yearRe)) {
        results.push_back({true, "Yil raqami mavjud"});
    }

    // Ko'p takroriy qism
    std::regex repeatRe("(..+)\\1+");
    if (std::regex_search(password, repeatRe) && password.size() >= 4) {
        results.push_back({true, "Takroriy qism (abab, 1212)"});
    }

    return results;
}

// ─────────────────────────────────────────────
//  Charset va entropy
// ─────────────────────────────────────────────
struct CharsetInfo {
    bool hasLower, hasUpper, hasDigit, hasSymbol;
    int size;
};

CharsetInfo analyzeCharset(const std::string& p) {
    CharsetInfo ci{false,false,false,false,0};
    for (char c : p) {
        if (islower(c))  ci.hasLower  = true;
        if (isupper(c))  ci.hasUpper  = true;
        if (isdigit(c))  ci.hasDigit  = true;
        if (!isalnum(c)) ci.hasSymbol = true;
    }
    if (ci.hasLower)  ci.size += 26;
    if (ci.hasUpper)  ci.size += 26;
    if (ci.hasDigit)  ci.size += 10;
    if (ci.hasSymbol) ci.size += 32;
    return ci;
}

double calcEntropy(const std::string& p) {
    CharsetInfo ci = analyzeCharset(p);
    if (ci.size == 0 || p.empty()) return 0.0;
    return p.size() * log2(ci.size);
}

// ─────────────────────────────────────────────
//  Buzilish vaqtini hisoblash (hybrid model)
// ─────────────────────────────────────────────
struct CrackResult {
    double seconds;
    std::string timeText;
    std::string humorText;
    int level; // 0=juda zaif, 1=zaif, 2=o'rtacha, 3=kuchli, 4=juda kuchli
};

std::string humanTime(double secs) {
    if (secs < 1)            return "< 1 soniya";
    if (secs < 60)           return std::to_string((long long)secs) + " soniya";
    if (secs < 3600)         return std::to_string((long long)(secs/60)) + " daqiqa";
    if (secs < 86400)        return std::to_string((long long)(secs/3600)) + " soat";
    if (secs < 2592000)      return std::to_string((long long)(secs/86400)) + " kun";
    if (secs < 31536000)     return std::to_string((long long)(secs/2592000)) + " oy";
    double years = secs / 31536000.0;
    if (years < 1e6) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(0) << years;
        return oss.str() + " yil";
    }
    std::ostringstream oss;
    oss << std::scientific << std::setprecision(1) << years;
    return oss.str() + " yil";
}

CrackResult calcCrackTime(const std::string& password) {
    // Mashhur parollar bazasida bo'lsa
    std::string lower = password;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    if (COMMON_PASSWORDS.count(lower)) {
        return {0.001, "< 1 soniya", "Bu parol lug'atda bor — buzish 0 kuch talab qiladi! \U0001F4C4", 0};
    }

    auto patterns = detectPatterns(password);
    CharsetInfo ci = analyzeCharset(password);
    double entropy = calcEntropy(password);

    // Brute force: GPU-level (10^10 urinish/s)
    const double GPU_RATE = 1e10;
    // Online attack (10^3 urinish/s)
    const double ONLINE_RATE = 1e3;

    // N^L / R
    double bruteTime = 0;
    if (ci.size > 0) {
        double combinations = pow(ci.size, (double)password.size());
        bruteTime = combinations / GPU_RATE;
    }

    // Entropy asosida
    double entropyTime = pow(2.0, entropy) / GPU_RATE;

    // Pattern topilsa — vaqtni kamaytir
    double multiplier = 1.0;
    if (!patterns.empty()) multiplier = 0.001;

    double finalTime = std::min(bruteTime, entropyTime) * multiplier;

    // Level aniqlash
    int level;
    std::string humor;
    if (finalTime < 1) {
        level = 0;
        humor = "Hacker choy ham ichmay buzib oladi \u2615";
    } else if (finalTime < 86400) {
        level = 1;
        humor = "Bir ish kuni yetarli \U0001F4BC";
    } else if (finalTime < 31536000) {
        level = 2;
        humor = "O'rtacha kuchli, lekin yaxshilang \U0001F504";
    } else if (finalTime < 31536000.0 * 100) {
        level = 3;
        humor = "Yaxshi parol! Lekin o'zingiz unutmang \U0001F601";
    } else {
        level = 4;
        humor = "Hacker zerikib uxlab qoldi... \U0001F634 Ajoyib parol!";
    }

    return {finalTime, humanTime(finalTime), humor, level};
}

// ─────────────────────────────────────────────
//  Kuch darajasi (0-100)
// ─────────────────────────────────────────────
struct StrengthInfo {
    int score;       // 0-100
    int stars;       // 0-5
    std::string label;
    std::string color;
};

StrengthInfo calcStrength(const std::string& p) {
    if (p.empty()) return {0, 0, "Bo'sh", Color::DIM};

    int score = 0;
    CharsetInfo ci = analyzeCharset(p);
    auto patterns = detectPatterns(p);
    std::string lower = p;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    bool isCommon = COMMON_PASSWORDS.count(lower) > 0;

    // Uzunlik bali
    int len = (int)p.size();
    if (len >= 6)  score += 10;
    if (len >= 8)  score += 10;
    if (len >= 10) score += 10;
    if (len >= 12) score += 10;
    if (len >= 16) score += 10;

    // Charset xilma-xilligi
    if (ci.hasLower)  score += 10;
    if (ci.hasUpper)  score += 10;
    if (ci.hasDigit)  score += 10;
    if (ci.hasSymbol) score += 15;

    // Entropy bonus
    double ent = calcEntropy(p);
    if (ent > 50) score += 5;
    if (ent > 70) score += 5;

    // Jazo
    if (isCommon)          score -= 60;
    score -= (int)(patterns.size() * 15);
    if (score < 0) score = 0;
    if (score > 100) score = 100;

    int stars = score / 20;
    if (stars > 5) stars = 5;

    std::string label;
    std::string color;
    if (score < 20)      { label = "Juda zaif";  color = Color::BRED; }
    else if (score < 40) { label = "Zaif";        color = Color::RED; }
    else if (score < 60) { label = "O'rtacha";    color = Color::BYELLOW; }
    else if (score < 80) { label = "Kuchli";      color = Color::BGREEN; }
    else                 { label = "Juda kuchli"; color = Color::BCYAN; }

    return {score, stars, label, color};
}

// ─────────────────────────────────────────────
//  Tavsiya parollar yaratish
// ─────────────────────────────────────────────
std::string generateStrong(int length = 14) {
    static const std::string lower  = "abcdefghjkmnpqrstuvwxyz";
    static const std::string upper  = "ABCDEFGHJKMNPQRSTUVWXYZ";
    static const std::string digits = "23456789";
    static const std::string syms   = "!@#$%^&*-_+=?";
    static std::mt19937 rng(std::random_device{}());

    std::string pool = lower + upper + digits + syms;
    std::string result;
    // Kamida bittadan har turdan
    result += lower[rng()%lower.size()];
    result += upper[rng()%upper.size()];
    result += digits[rng()%digits.size()];
    result += syms[rng()%syms.size()];

    std::uniform_int_distribution<size_t> dist(0, pool.size()-1);
    while ((int)result.size() < length) result += pool[dist(rng)];

    std::shuffle(result.begin(), result.end(), rng);
    return result;
}

// Parolni kuchaytirish (substitution + qo'shimchalar)
std::vector<std::string> suggestUpgrade(const std::string& base) {
    static std::mt19937 rng(std::random_device{}());
    std::vector<std::string> results;

    // Substitution qoidalari
    std::map<char,std::string> subs = {
        {'a',"@"},{'A',"@"},{'o',"0"},{'O',"0"},
        {'i',"1"},{'I',"1"},{'s',"$"},{'S',"$"},
        {'e',"3"},{'E',"3"},{'l',"!"}, {'L',"!"}
    };

    // Variant 1: substitution + katta harf + symbol oxirida
    std::string v1 = base;
    for (auto& [from, to] : subs)
        for (char& c : v1) if (c == from) { c = to[0]; break; }
    // Birinchi harfni kattalashtir
    if (!v1.empty() && islower(v1[0])) v1[0] = toupper(v1[0]);
    v1 += "!7";
    results.push_back(v1);

    // Variant 2: o'rtaga symbol + raqam
    std::string v2 = base;
    std::string syms = "!@#$";
    size_t mid = v2.size()/2;
    v2.insert(mid, 1, syms[rng()%4]);
    v2.insert(mid+1, std::to_string(rng()%90+10));
    if (!v2.empty()) v2[0] = toupper(v2[0]);
    results.push_back(v2);

    // Variant 3: to'liq tasodifiy kuchli parol
    results.push_back(generateStrong(14));
    results.push_back(generateStrong(16));

    return results;
}

// ─────────────────────────────────────────────
//  Ko'rsatish yordamchilari
// ─────────────────────────────────────────────
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    std::cout << "\033[2J\033[1;1H";
#endif
}

void printLine(char c = '-', int len = 60) {
    std::cout << Color::DIM;
    for (int i = 0; i < len; i++) std::cout << c;
    std::cout << Color::RESET << "\n";
}

void printHeader() {
    clearScreen();
    printLine('=');
    std::cout << Color::BCYAN
              << "  ██████╗  █████╗ ██████╗  ██████╗ ██╗  ████████╗███████╗███████╗████████╗\n"
              << "  ██╔══██╗██╔══██╗██╔══██╗██╔═══██╗██║  ╚══██╔══╝██╔════╝██╔════╝╚══██╔══╝\n"
              << "  ██████╔╝███████║██████╔╝██║   ██║██║     ██║   █████╗  ███████╗   ██║   \n"
              << "  ██╔═══╝ ██╔══██║██╔══██╗██║   ██║██║     ██║   ██╔══╝  ╚════██║   ██║   \n"
              << "  ██║     ██║  ██║██║  ██║╚██████╔╝███████╗██║   ███████╗███████║   ██║   \n"
              << "  ╚═╝     ╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝ ╚══════╝╚═╝   ╚══════╝╚══════╝   ╚═╝   \n"
              << " Salom! mamnunman sizni ko'rib turganimdan. #dev_boha\n\n"
              << Color::RESET;
    printLine('=');
    std::cout << Color::DIM << "  Parol kuchini tahlil qiluvchi va kuchaytirishchi dastur\n" << Color::RESET;
    printLine('-');
    std::cout << "\n";
}

void printStrengthBar(const StrengthInfo& si) {
    int filled = si.score / 5; // 0-20 segment
    std::cout << "  Kuch: [";
    for (int i = 0; i < 20; i++) {
        if (i < filled) {
            if (si.score < 40)      std::cout << Color::RED    << "█";
            else if (si.score < 60) std::cout << Color::YELLOW << "█";
            else if (si.score < 80) std::cout << Color::GREEN  << "█";
            else                    std::cout << Color::CYAN   << "█";
        } else {
            std::cout << Color::DIM << "░";
        }
    }
    std::cout << Color::RESET << "] ";
    std::cout << si.color << si.label << Color::RESET;
    std::cout << Color::DIM << " (" << si.score << "/100)\n" << Color::RESET;

    // Yulduzlar
    std::cout << "  ";
    for (int i = 0; i < 5; i++) {
        if (i < si.stars) std::cout << Color::BYELLOW << "★ " << Color::RESET;
        else              std::cout << Color::DIM     << "☆ " << Color::RESET;
    }
    std::cout << "\n";
}

void printAnalysis(const std::string& password) {
    if (password.empty()) return;

    CharsetInfo ci = analyzeCharset(password);
    auto patterns  = detectPatterns(password);
    auto crack     = calcCrackTime(password);
    auto strength  = calcStrength(password);
    double entropy = calcEntropy(password);
    std::string lower = password;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    bool isCommon = COMMON_PASSWORDS.count(lower) > 0;

    // ── Kuch paneli ──────────────────────────────
    std::cout << "\n";
    printLine('-');
    std::cout << Color::BWHITE << "  TAHLIL NATIJALARI\n" << Color::RESET;
    printLine('-');
    printStrengthBar(strength);

    // ── Asosiy metrikalar ─────────────────────────
    std::cout << "\n";
    std::cout << "  " << Color::DIM << "Uzunlik     : " << Color::RESET
              << Color::BWHITE << password.size() << " belgi\n" << Color::RESET;

    std::cout << "  " << Color::DIM << "Charset     : " << Color::RESET;
    if (ci.hasLower)  std::cout << Color::GREEN  << "[a-z] " << Color::RESET;
    if (ci.hasUpper)  std::cout << Color::CYAN   << "[A-Z] " << Color::RESET;
    if (ci.hasDigit)  std::cout << Color::YELLOW << "[0-9] " << Color::RESET;
    if (ci.hasSymbol) std::cout << Color::MAGENTA<< "[!@#] " << Color::RESET;
    std::cout << Color::DIM << "(" << ci.size << " belgi to'plami)\n" << Color::RESET;

    std::cout << "  " << Color::DIM << "Entropy     : " << Color::RESET;
    if (entropy < 28)      std::cout << Color::BRED;
    else if (entropy < 50) std::cout << Color::BYELLOW;
    else if (entropy < 70) std::cout << Color::BGREEN;
    else                   std::cout << Color::BCYAN;
    std::cout << std::fixed << std::setprecision(1) << entropy << " bit\n" << Color::RESET;

    // ── Buzilish vaqti ───────────────────────────
    std::cout << "\n  " << Color::DIM << "Buzilish vaqti (GPU brute-force):\n" << Color::RESET;
    std::cout << "  ";
    switch(crack.level) {
        case 0: std::cout << Color::BRED;    break;
        case 1: std::cout << Color::RED;     break;
        case 2: std::cout << Color::BYELLOW; break;
        case 3: std::cout << Color::BGREEN;  break;
        case 4: std::cout << Color::BCYAN;   break;
    }
    std::cout << "  ⏱  " << crack.timeText << "\n" << Color::RESET;
    std::cout << "  " << Color::DIM << "  " << crack.humorText << "\n" << Color::RESET;

    // ── Mashhur bazalar ──────────────────────────
    std::cout << "\n  " << Color::DIM << "Mashhur parollar bazasi: " << Color::RESET;
    if (isCommon) std::cout << Color::BRED << "⚠  TOPILDI! Bu parol bazada bor\n" << Color::RESET;
    else          std::cout << Color::BGREEN << "✓  Bazada yo'q\n" << Color::RESET;

    // ── Pattern muammolari ───────────────────────
    if (!patterns.empty()) {
        std::cout << "\n  " << Color::BYELLOW << "⚠  Aniqlanganmuammolar:\n" << Color::RESET;
        for (auto& pt : patterns) {
            std::cout << Color::YELLOW << "     • " << pt.description << "\n" << Color::RESET;
        }
    }

    // ── Tavsiyalar ───────────────────────────────
    std::cout << "\n  " << Color::BWHITE << "Yaxshilash uchun tavsiyalar:\n" << Color::RESET;
    if (!ci.hasUpper)  std::cout << Color::DIM << "     → Katta harf qo'shing (A-Z)\n" << Color::RESET;
    if (!ci.hasLower)  std::cout << Color::DIM << "     → Kichik harf qo'shing (a-z)\n" << Color::RESET;
    if (!ci.hasDigit)  std::cout << Color::DIM << "     → Raqam qo'shing (0-9)\n" << Color::RESET;
    if (!ci.hasSymbol) std::cout << Color::DIM << "     → Maxsus belgi qo'shing (!@#$)\n" << Color::RESET;
    if ((int)password.size() < 12) std::cout << Color::DIM << "     → Uzunlikni 12+ belgiga yetkazing\n" << Color::RESET;
    if (isCommon)      std::cout << Color::DIM << "     → Bu paroldan umuman foydalanmang!\n" << Color::RESET;
    if (patterns.empty() && ci.hasUpper && ci.hasLower && ci.hasDigit && ci.hasSymbol && (int)password.size()>=12)
        std::cout << Color::BGREEN << "     ✓  Parol juda kuchli, barakalla!\n" << Color::RESET;

    // ── Kuchaytirish variantlari ─────────────────
    auto suggestions = suggestUpgrade(password);
    std::cout << "\n";
    printLine('-');
    std::cout << Color::BWHITE << "  KUCHLIROQ PAROL VARIANTLARI\n" << Color::RESET;
    printLine('-');
    int idx = 1;
    for (auto& sug : suggestions) {
        std::string tag = (idx <= 2) ? " (asosiy parolingizdan)" : " (tasodifiy kuchli)";
        std::cout << "  " << Color::BCYAN << idx++ << ") " << Color::RESET
                  << Color::BWHITE << sug << Color::RESET
                  << Color::DIM << tag << "\n" << Color::RESET;
    }

    printLine('-');
}

// ─────────────────────────────────────────────
//  Parol maxfiy kiritish (yulduz ko'rsatish)
// ─────────────────────────────────────────────
std::string readPassword(const std::string& prompt) {
    std::cout << prompt;
    std::string pass;

#ifdef _WIN32
    char ch;
    while ((ch = _getch()) != '\r') {
        if (ch == '\b') {
            if (!pass.empty()) { pass.pop_back(); std::cout << "\b \b"; }
        } else if (ch >= 32) {
            pass += ch;
            std::cout << '*';
        }
    }
#else
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    char ch;
    while (read(STDIN_FILENO, &ch, 1) == 1 && ch != '\n') {
        if (ch == 127 || ch == '\b') {
            if (!pass.empty()) { pass.pop_back(); std::cout << "\b \b" << std::flush; }
        } else if (ch >= 32) {
            pass += ch;
            std::cout << '*' << std::flush;
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif

    std::cout << "\n";
    return pass;
}

// ─────────────────────────────────────────────
//  Parol generator menyu
// ─────────────────────────────────────────────
void generatorMenu() {
    std::cout << "\n";
    printLine('-');
    std::cout << Color::BWHITE << "  PAROL GENERATOR\n" << Color::RESET;
    printLine('-');
    std::cout << Color::DIM << "  Nechta belgi? (8-32, standart 14): " << Color::RESET;
    std::string lenStr;
    std::getline(std::cin, lenStr);
    int len = 14;
    if (!lenStr.empty()) {
        try { len = std::stoi(lenStr); }
        catch (...) { len = 14; }
        if (len < 8)  len = 8;
        if (len > 32) len = 32;
    }
    std::cout << "\n";
    for (int i = 1; i <= 5; i++) {
        std::string p = generateStrong(len);
        std::cout << "  " << Color::BCYAN << i << ") " << Color::RESET
                  << Color::BWHITE << p << Color::RESET << "\n";
    }
    printLine('-');
}

// ─────────────────────────────────────────────
//  Asosiy menyu
// ─────────────────────────────────────────────
void mainMenu() {
    while (true) {
        printHeader();
        std::cout << "  " << Color::BWHITE << "1" << Color::RESET << Color::DIM << "  Parolni tekshirish\n" << Color::RESET;
        std::cout << "  " << Color::BWHITE << "2" << Color::RESET << Color::DIM << "  Kuchli parol yaratish\n" << Color::RESET;
        std::cout << "  " << Color::BWHITE << "3" << Color::RESET << Color::DIM << "  Dastur haqida\n" << Color::RESET;
        std::cout << "  " << Color::BWHITE << "0" << Color::RESET << Color::DIM << "  Chiqish\n" << Color::RESET;
        std::cout << "\n  " << Color::DIM << "Tanlov: " << Color::RESET;

        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "1") {
            printHeader();
            std::string password = readPassword("  Parolni kiriting: ");
            if (password.empty()) {
                std::cout << Color::RED << "  Parol bo'sh bo'lishi mumkin emas!\n" << Color::RESET;
            } else {
                printAnalysis(password);
            }
            std::cout << "\n  " << Color::DIM << "Davom etish uchun Enter bosing..." << Color::RESET;
            std::string tmp; std::getline(std::cin, tmp);

        } else if (choice == "2") {
            printHeader();
            generatorMenu();
            std::cout << "\n  " << Color::DIM << "Davom etish uchun Enter bosing..." << Color::RESET;
            std::string tmp; std::getline(std::cin, tmp);

        } else if (choice == "3") {
            printHeader();
            printLine('-');
            std::cout << Color::BWHITE << "  ParolTest v1.0\n\n" << Color::RESET;
            std::cout << Color::DIM
                      << "  Ishlatilgan algoritmlar:\n"
                      << "    • Hybrid model (Dictionary + Brute-force + Entropy)\n"
                      << "    • Shannon entropy: H = L * log2(N)\n"
                      << "    • GPU-level brute-force: 10^10 urinish/soniya\n"
                      << "    • O'rta Osiyo + global mashhur parollar bazasi\n"
                      << "    • Pattern detection (keyboard, sequential, repeat)\n"
                      << "    • Smart substitution suggestion engine\n\n"
                      << "  Muallif: Dev_boha | C++17\n"
                      << Color::RESET;
            printLine('-');
            std::cout << "\n  " << Color::DIM << "Davom etish uchun Enter bosing..." << Color::RESET;
            std::string tmp; std::getline(std::cin, tmp);

        } else if (choice == "0") {
            std::cout << "\n  " << Color::BCYAN << "Xayr! Xavfsiz paro'llar ishlating!\n\n" << Color::RESET;
            break;
        } else {
            std::cout << Color::RED << "  Noto'g'ri tanlov!\n" << Color::RESET;
            std::string tmp; std::getline(std::cin, tmp);
        }
    }
}

// ─────────────────────────────────────────────
//  main
// ─────────────────────────────────────────────
int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hOut, &mode);
    SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif

    // Command line mode: ./paroltest <parol>
    if (argc > 1) {
        std::string password = argv[1];
        printHeader();
        printAnalysis(password);
        return 0;
    }

    mainMenu();
    return 0;
}