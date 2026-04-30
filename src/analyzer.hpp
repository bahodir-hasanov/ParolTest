#pragma once
#include <string>
#include <vector>
#include <cmath>
#include <set>
#include <algorithm>
#include <regex>
#include <random>
#include <map>
#include <sstream>
#include <iomanip>
#include "passwords_db.hpp"

// ════════════════════════════════════════════════════════════════
//  ParolTest — Tahlil va hisoblash moduli
// ════════════════════════════════════════════════════════════════

// ─── Natija strukturalari ────────────────────────────────────────

struct CharsetInfo {
    bool hasLower  = false;
    bool hasUpper  = false;
    bool hasDigit  = false;
    bool hasSymbol = false;
    int  size      = 0;
    int  uniqueChars = 0;
};

struct PatternInfo {
    std::string description;
    double      penaltyMultiplier; // 1.0 = jazo yo'q, 0.001 = juda katta
};

struct CrackEstimate {
    double seconds;
    std::string label;      // "2 daqiqa"
    std::string attackType; // "Dictionary", "Brute-force", "Pattern"
    std::string humor;
    int         severity;   // 0-4
};

struct StrengthResult {
    int    score;       // 0-100
    int    stars;       // 0-5
    std::string label;
    std::string cssClass; // "very-weak","weak","medium","strong","very-strong"
    double entropy;
    CharsetInfo charset;
    std::vector<PatternInfo> patterns;
    std::vector<std::string> issues;
    std::vector<std::string> tips;
    CrackEstimate crack;
    bool isCommon;
    std::vector<std::string> suggestions;
    std::vector<std::string> generated;
};

// ─── Charset tahlili ─────────────────────────────────────────────

CharsetInfo analyzeCharset(const std::string& p) {
    CharsetInfo ci;
    std::set<char> uniq(p.begin(), p.end());
    ci.uniqueChars = (int)uniq.size();
    for (char c : p) {
        if (std::islower((unsigned char)c)) ci.hasLower  = true;
        if (std::isupper((unsigned char)c)) ci.hasUpper  = true;
        if (std::isdigit((unsigned char)c)) ci.hasDigit  = true;
        if (!std::isalnum((unsigned char)c))ci.hasSymbol = true;
    }
    if (ci.hasLower)  ci.size += 26;
    if (ci.hasUpper)  ci.size += 26;
    if (ci.hasDigit)  ci.size += 10;
    if (ci.hasSymbol) ci.size += 32;
    return ci;
}

// ─── Pattern aniqlash ────────────────────────────────────────────

std::vector<PatternInfo> detectPatterns(const std::string& password) {
    std::vector<PatternInfo> result;
    std::string lower = password;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    const int L = (int)password.size();

    // 1. Faqat raqamlar
    if (std::all_of(password.begin(), password.end(), ::isdigit))
        result.push_back({"Faqat raqamlar", 0.0001});

    // 2. Faqat harflar
    if (std::all_of(password.begin(), password.end(), ::isalpha))
        result.push_back({"Faqat harflar", 0.01});

    // 3. Hammasi bir xil
    if (L > 1 && std::all_of(password.begin(), password.end(),
                              [&](char c){ return c == password[0]; }))
        result.push_back({"Barcha belgilar bir xil (aaa, 111)", 0.00001});

    // 4. Ketma-ket raqamlar
    bool seqAsc = true, seqDesc = true;
    for (int i = 1; i < L; i++) {
        if (!isdigit(password[i]) || password[i] != password[i-1]+1) seqAsc  = false;
        if (!isdigit(password[i]) || password[i] != password[i-1]-1) seqDesc = false;
    }
    if ((seqAsc || seqDesc) && L >= 4)
        result.push_back({"Ketma-ket raqamlar (1234 / 9876)", 0.0001});

    // 5. Klaviatura qatori
    static const std::vector<std::string> kbRows = {
        "qwertyuiop","asdfghjkl","zxcvbnm",
        "1234567890","qazwsxedcrfvtgbyhnujmikolp"
    };
    for (auto& row : kbRows) {
        if (L >= 4 && row.find(lower) != std::string::npos) {
            result.push_back({"Klaviatura qatori pattern (qwerty, asdf)", 0.001});
            break;
        }
    }

    // 6. Yil raqami
    std::regex yearRe("(19|20)\\d{2}");
    if (std::regex_search(password, yearRe))
        result.push_back({"Yil raqami mavjud (2024, 1999)", 0.3});

    // 7. Takroriy qism (abab, 1212)
    std::regex repeatRe("(..+)\\1+");
    if (L >= 4 && std::regex_search(password, repeatRe))
        result.push_back({"Takroriy qism (abab, 123123)", 0.01});

    // 8. L33t speak (a→@ etc.)
    std::regex l33t("[@$!][a-z0-9]{2,}|[a-z]{2,}[@$!0-9]");
    if (std::regex_search(lower, l33t) && L < 10)
        result.push_back({"Oddiy l33tspeak (a→@ kabi) — hozir ma'lum", 0.1});

    // 9. Oxirida raqam
    std::regex endNum("[a-zA-Z]+\\d{1,4}$");
    if (L <= 10 && std::regex_match(password, endNum))
        result.push_back({"Harflar + oxirida raqam (salom123)", 0.05});

    return result;
}

// ─── Entropy ─────────────────────────────────────────────────────

double calcEntropy(const std::string& p) {
    CharsetInfo ci = analyzeCharset(p);
    if (ci.size == 0 || p.empty()) return 0.0;

    // Shannon entropy (belgi chastotasi bo'yicha)
    std::map<char,int> freq;
    for (char c : p) freq[c]++;
    double shanEnt = 0.0;
    for (auto& [c, cnt] : freq) {
        double prob = (double)cnt / p.size();
        shanEnt -= prob * log2(prob);
    }
    double posEnt = p.size() * log2(ci.size); // pozitsion
    return (shanEnt * p.size() + posEnt) / 2.0; // o'rtacha
}

// ─── Vaqtni inson tiliga o'girish ────────────────────────────────

std::string humanTime(double secs) {
    if (secs < 0.001)          return "< 1 millisekund";
    if (secs < 1.0)            return "bir necha millisekund";
    if (secs < 60)             return std::to_string((long long)secs) + " soniya";
    if (secs < 3600)           return std::to_string((long long)(secs/60)) + " daqiqa";
    if (secs < 86400)          return std::to_string((long long)(secs/3600)) + " soat";
    if (secs < 2592000)        return std::to_string((long long)(secs/86400)) + " kun";
    if (secs < 31536000)       return std::to_string((long long)(secs/2592000)) + " oy";
    double yr = secs/31536000.0;
    if (yr < 1000)             return std::to_string((long long)yr) + " yil";
    if (yr < 1e6)              return std::to_string((long long)(yr/1000)) + " ming yil";
    if (yr < 1e9)              return std::to_string((long long)(yr/1e6)) + " million yil";
    if (yr < 1e12)             return std::to_string((long long)(yr/1e9)) + " milliard yil";
    return "Koinot umridan uzun";
}

// ─── Crack time hisoblash (Hybrid model) ─────────────────────────

CrackEstimate calcCrackTime(const std::string& password,
                             const std::vector<PatternInfo>& patterns,
                             bool isCommon) {
    // 1) Dictionary attack
    if (isCommon) {
        return {0.001, "< 1 soniya", "Dictionary",
                "Bu parol lug'atda bor — buzish 0 kuch talab qiladi! \U0001F4C4", 0};
    }

    CharsetInfo ci = analyzeCharset(password);
    const int L = (int)password.size();

    // GPU hashcat tezliklari (MD5 uchun)
    const double GPU_RATE   = 1e10; // 10 milliard/s
    // Offline bcrypt
    const double BCRYPT_RATE = 1e4;  // 10 ming/s

    // 2) Brute-force
    double N = ci.size > 0 ? ci.size : 26;
    double combinations = pow(N, L);
    double bruteTime    = combinations / GPU_RATE;

    // 3) Entropy asosida
    double ent = calcEntropy(password);
    double entropyTime = pow(2.0, ent) / GPU_RATE;

    // 4) Pattern jarima
    double multiplier = 1.0;
    for (auto& pt : patterns)
        multiplier *= pt.penaltyMultiplier;
    if (multiplier < 1e-8) multiplier = 1e-8;

    double rawTime = std::min(bruteTime, entropyTime);
    double finalTime = rawTime * multiplier;

    // Humor va daraja
    int sev;
    std::string humor;
    if (finalTime < 1) {
        sev = 0; humor = "Hacker choy ham ichmay buzib oladi \u2615";
    } else if (finalTime < 3600) {
        sev = 0; humor = "Tushlik tanaffusi yetarli \U0001F354";
    } else if (finalTime < 86400) {
        sev = 1; humor = "Bir ish kuni ichida — yaxshilang! \U0001F4BC";
    } else if (finalTime < 2592000) {
        sev = 1; humor = "Hacker bir oy sarflaydi... baribir yaxshilang \U0001F605";
    } else if (finalTime < 31536000) {
        sev = 2; humor = "Bir yillik sabr kerak — o'rtacha \U0001F914";
    } else if (finalTime < 31536000.0*100) {
        sev = 3; humor = "Yaxshi parol! Lekin o'zingiz unutmang \U0001F601";
    } else if (finalTime < 31536000.0*1e6) {
        sev = 4; humor = "Hacker zerikib uxlab qoldi... \U0001F634 Zo'r!";
    } else {
        sev = 4; humor = "Koinot tugasa ham buzmaydi! \U0001F680";
    }

    std::string attackType = patterns.empty() ? "Brute-force (GPU)" : "Pattern+Brute";
    return {finalTime, humanTime(finalTime), attackType, humor, sev};
}

// ─── Kuch bali ───────────────────────────────────────────────────

int calcScore(const std::string& p, const CharsetInfo& ci,
              const std::vector<PatternInfo>& patterns, bool isCommon,
              double entropy) {
    if (p.empty()) return 0;
    int score = 0;

    // Uzunlik (0-35 ball)
    int L = (int)p.size();
    if (L >= 6)  score += 5;
    if (L >= 8)  score += 8;
    if (L >= 10) score += 8;
    if (L >= 12) score += 7;
    if (L >= 16) score += 7;

    // Charset xilma-xilligi (0-30 ball)
    if (ci.hasLower)  score += 5;
    if (ci.hasUpper)  score += 7;
    if (ci.hasDigit)  score += 7;
    if (ci.hasSymbol) score += 11;

    // Noyob belgilar (0-10 ball)
    double uniqueRatio = (double)ci.uniqueChars / std::max(1, L);
    score += (int)(uniqueRatio * 10);

    // Entropy bonus (0-15 ball)
    if (entropy > 30) score += 5;
    if (entropy > 50) score += 5;
    if (entropy > 70) score += 5;

    // Jazo
    if (isCommon)            score -= 70;
    for (auto& pt : patterns)
        score -= (int)((1.0 - pt.penaltyMultiplier) * 15.0 / patterns.size());

    return std::max(0, std::min(100, score));
}

// ─── Parol kuchaytirish ──────────────────────────────────────────

std::string substituteChars(const std::string& base) {
    static const std::map<char,char> subs = {
        {'a','@'},{'A','@'},{'o','0'},{'O','0'},
        {'i','1'},{'I','1'},{'s','$'},{'S','$'},
        {'e','3'},{'E','3'},{'l','!'},{'L','!'},
        {'t','+'},{'T','+'},{'b','6'},{'g','9'}
    };
    std::string result = base;
    int count = 0;
    for (char& c : result) {
        auto it = subs.find(c);
        if (it != subs.end() && count < 3) { c = it->second; count++; }
    }
    return result;
}

std::vector<std::string> suggestUpgrade(const std::string& base) {
    static std::mt19937 rng(std::random_device{}());
    std::vector<std::string> results;

    const std::string syms  = "!@#$%^&*";
    const std::string upper = "ABCDEFGHJKMNPQRSTUVWXYZ";

    // Variant 1: substitution + katta harf + symbol
    {
        std::string v = substituteChars(base);
        if (!v.empty() && islower(v[0])) v[0] = toupper(v[0]);
        v += syms[rng()%8];
        v += std::to_string(rng()%90+10);
        results.push_back(v);
    }

    // Variant 2: o'rtaga symbol + katta harf
    {
        std::string v = base;
        if (!v.empty() && islower(v[0])) v[0] = toupper(v[0]);
        size_t mid = v.size()/2;
        v.insert(mid, 1, syms[rng()%8]);
        v += std::to_string(rng()%900+100);
        results.push_back(v);
    }

    // Variant 3: Uch so'z + raqam (passphrase uslubi)
    {
        static const std::vector<std::string> words = {
            "Tosh","Bahor","Quyosh","Shamol","Daryo",
            "Tog","Yulduz","Oy","Gul","Arzon"
        };
        std::string v  = words[rng()%words.size()];
        v += words[rng()%words.size()];
        v += syms[rng()%8];
        v += std::to_string(rng()%9000+1000);
        results.push_back(v);
    }

    return results;
}

// ─── Kuchli parol generatsiyasi ──────────────────────────────────

std::string generateStrong(int length = 14) {
    static const std::string lowerC  = "abcdefghjkmnpqrstuvwxyz";
    static const std::string upperC  = "ABCDEFGHJKMNPQRSTUVWXYZ";
    static const std::string digits  = "23456789";
    static const std::string symbols = "!@#$%^&*-_+=?";
    static std::mt19937 rng(std::random_device{}());

    std::string pool = lowerC + upperC + digits + symbols;
    std::string result;

    // Kamida bittadan har turdan
    result += lowerC [rng() % lowerC.size()];
    result += upperC [rng() % upperC.size()];
    result += digits [rng() % digits.size()];
    result += symbols[rng() % symbols.size()];

    std::uniform_int_distribution<size_t> dist(0, pool.size()-1);
    while ((int)result.size() < length)
        result += pool[dist(rng)];

    std::shuffle(result.begin(), result.end(), rng);
    return result;
}

// ─── Asosiy tahlil funksiyasi ────────────────────────────────────

StrengthResult analyze(const std::string& password) {
    StrengthResult r;

    if (password.empty()) {
        r.score = 0; r.stars = 0;
        r.label = "Bo'sh"; r.cssClass = "empty";
        return r;
    }

    // Kichik harfga o'tkazib baza tekshiruvi
    std::string lower = password;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    r.isCommon = COMMON_PASSWORDS.count(lower) > 0;

    r.charset  = analyzeCharset(password);
    r.entropy  = calcEntropy(password);
    r.patterns = detectPatterns(password);
    r.crack    = calcCrackTime(password, r.patterns, r.isCommon);
    r.score    = calcScore(password, r.charset, r.patterns, r.isCommon, r.entropy);
    r.stars    = std::min(5, r.score / 20);

    // Label va CSS
    if      (r.score < 20) { r.label = "Juda zaif";  r.cssClass = "very-weak"; }
    else if (r.score < 40) { r.label = "Zaif";        r.cssClass = "weak"; }
    else if (r.score < 60) { r.label = "O'rtacha";    r.cssClass = "medium"; }
    else if (r.score < 80) { r.label = "Kuchli";      r.cssClass = "strong"; }
    else                   { r.label = "Juda kuchli"; r.cssClass = "very-strong"; }

    // Muammolar
    if (r.isCommon)            r.issues.push_back("Mashhur parollar ro'yxatida topildi!");
    for (auto& pt : r.patterns) r.issues.push_back(pt.description);

    // Tavsiyalar
    if (!r.charset.hasUpper)          r.tips.push_back("Katta harf qo'shing (A-Z)");
    if (!r.charset.hasLower)          r.tips.push_back("Kichik harf qo'shing (a-z)");
    if (!r.charset.hasDigit)          r.tips.push_back("Raqam qo'shing (0-9)");
    if (!r.charset.hasSymbol)         r.tips.push_back("Maxsus belgi qo'shing (!@#$%^&*)");
    if ((int)password.size() < 12)    r.tips.push_back("Uzunlikni 12+ belgiga yetkazing");
    if ((int)password.size() < 8)     r.tips.push_back("Kamida 8 belgi bo'lishi kerak!");
    if (r.isCommon)                    r.tips.push_back("Bu paroldan umuman foydalanmang!");

    // Suggestions
    r.suggestions = suggestUpgrade(password);

    // Tasodifiy kuchli parollar
    for (int i = 0; i < 3; i++)
        r.generated.push_back(generateStrong(14 + i*2));

    return r;
}