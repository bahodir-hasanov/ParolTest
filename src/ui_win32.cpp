/*
 * ParolTest — Windows (Win32) GUI
 * Tahlil moduli Linux GTK bilan bir xil: analyzer.hpp + passwords_db.hpp
 *
 * MSVC:
 *   cmake -S . -B build -A x64
 *   cmake --build build --config Release
 *
 * MinGW:
 *   cmake -S . -B build -G "MinGW Makefiles"
 *   cmake --build build
 */

#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>
#include <commctrl.h>
#include <cstring>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "analyzer.hpp"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "comctl32.lib")
#ifdef _MSC_VER
#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

namespace {

constexpr int ID_EDIT      = 101;
constexpr int ID_TOGGLE    = 102;
constexpr int ID_GENERATE  = 103;
constexpr int ID_COPY_S0   = 200;
constexpr int ID_COPY_G0   = 210;

constexpr COLORREF COL_BG     = RGB(15, 17, 23);
constexpr COLORREF COL_SIDE   = RGB(22, 27, 39);
constexpr COLORREF COL_LINE   = RGB(42, 46, 61);
constexpr COLORREF COL_TEXT   = RGB(226, 232, 240);
constexpr COLORREF COL_MUTED  = RGB(74, 85, 104);
constexpr COLORREF COL_ACCENT = RGB(100, 212, 247);
constexpr COLORREF COL_GOOD   = RGB(74, 222, 128);
constexpr COLORREF COL_WARN   = RGB(251, 191, 36);
constexpr COLORREF COL_BAD    = RGB(248, 113, 113);
constexpr COLORREF COL_EDIT   = RGB(22, 27, 39);

HINSTANCE gInst;
HWND gWnd;
HWND hEdit, hToggle, hGenerate;
HWND hStrength, hStars, hScore;
HWND hLength, hCharset, hEntropy;
HWND hCrackTime, hCrackType, hHumor, hBadge, hStatus;
HWND hIssues, hTips;
HWND hSugPass[3], hSugTag[3], hSugCopy[3];
HWND hGenPass[3], hGenTag[3], hGenCopy[3];
HWND hStrBar, hEntBar;

HFONT hFontTitle, hFontUi, hFontMono, hFontSmall;
HBRUSH brBg, brSide, brEdit, brLine;
bool passwordVisible = false;
std::string sugStore[3];
std::string genStore[3];
int lastScore = 0;
bool lastCommon = false;
bool hasAnalysis = false;

std::wstring utf8ToWide(const std::string& s) {
    if (s.empty()) return L"";
    int n = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), nullptr, 0);
    std::wstring w(n, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), w.data(), n);
    return w;
}

std::string wideToUtf8(const std::wstring& w) {
    if (w.empty()) return "";
    int n = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(), nullptr, 0, nullptr, nullptr);
    std::string s(n, '\0');
    WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(), s.data(), n, nullptr, nullptr);
    return s;
}

void setText(HWND h, const std::string& utf8) {
    std::wstring w = utf8ToWide(utf8);
    SetWindowTextW(h, w.c_str());
}

void copyUtf8(const std::string& u8) {
    std::wstring w = utf8ToWide(u8);
    if (!OpenClipboard(gWnd)) return;
    EmptyClipboard();
    SIZE_T bytes = (w.size() + 1) * sizeof(wchar_t);
    HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (!mem) { CloseClipboard(); return; }
    void* p = GlobalLock(mem);
    memcpy(p, w.c_str(), bytes);
    GlobalUnlock(mem);
    SetClipboardData(CF_UNICODETEXT, mem);
    CloseClipboard();
}

HWND makeLabel(HWND parent, int x, int y, int w, int h, const wchar_t* text, HFONT font) {
    HWND hwnd = CreateWindowExW(0, L"STATIC", text,
                             WS_CHILD | WS_VISIBLE | SS_LEFT | SS_NOPREFIX,
                             x, y, w, h, parent, nullptr, gInst, nullptr);
    SendMessageW(hwnd, WM_SETFONT, (WPARAM)font, TRUE);
    return hwnd;
}

HWND makeBtn(HWND parent, int id, int x, int y, int w, int h, const wchar_t* text) {
    HWND hwnd = CreateWindowExW(0, L"BUTTON", text,
                             WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                             x, y, w, h, parent, (HMENU)(INT_PTR)id, gInst, nullptr);
    SendMessageW(hwnd, WM_SETFONT, (WPARAM)hFontUi, TRUE);
    return hwnd;
}

COLORREF scoreColor(int score) {
    if (score < 20) return COL_BAD;
    if (score < 40) return RGB(251, 146, 60);
    if (score < 60) return COL_WARN;
    if (score < 80) return COL_GOOD;
    return RGB(52, 211, 153);
}

void setBar(HWND bar, int percent) {
    percent = std::max(0, std::min(100, percent));
    SendMessageW(bar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
    SendMessageW(bar, PBM_SETPOS, percent, 0);
}

void resetEmptyUi() {
    setText(hStrength, "—");
    setText(hStars, "☆☆☆☆☆");
    setText(hScore, "0 / 100");
    setText(hCharset, "—");
    setText(hEntropy, "—");
    setText(hCrackTime, "—");
    setText(hCrackType, "GPU Brute-force");
    setText(hHumor, "");
    setText(hBadge, "—");
    setText(hIssues, "Parol kiriting...");
    setText(hTips, "");
    setBar(hStrBar, 0);
    setBar(hEntBar, 0);
    hasAnalysis = false;
    lastScore = 0;
    lastCommon = false;
    for (int i = 0; i < 3; i++) {
        sugStore[i].clear();
        genStore[i].clear();
        setText(hSugPass[i], "—");
        setText(hSugTag[i], "");
        setText(hGenPass[i], "—");
        setText(hGenTag[i], "");
    }
    setText(hStatus, "ParolTest v2.0 | Parol kiriting...");
}

void applyResult(const StrengthResult& r) {
    setText(hStrength, r.label);
    std::string stars;
    for (int i = 0; i < 5; i++) stars += (i < r.stars) ? "★" : "☆";
    setText(hStars, stars);
    setText(hScore, std::to_string(r.score) + " / 100");
    setText(hLength, std::to_string(r.length) + " belgi");

    std::string cs;
    if (r.charset.hasLower)  cs += "a-z ";
    if (r.charset.hasUpper)  cs += "A-Z ";
    if (r.charset.hasDigit)  cs += "0-9 ";
    if (r.charset.hasSymbol) cs += "!@# ";
    if (cs.empty()) cs = "—";
    setText(hCharset, cs);

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << r.entropy << " bit";
    setText(hEntropy, oss.str());

    setText(hCrackTime, r.crack.label);
    setText(hCrackType, r.crack.attackType);
    setText(hHumor, r.crack.humor);
    setText(hBadge, r.isCommon ? "Bazada topildi!" : "Bazada yo'q");

    std::string issues;
    if (r.issues.empty()) issues = "Muammo aniqlanmadi";
    else {
        for (size_t i = 0; i < r.issues.size(); i++) {
            if (i) issues += "\r\n";
            issues += "- " + r.issues[i];
        }
    }
    setText(hIssues, issues);

    std::string tips;
    if (r.tips.empty()) tips = "Parol barcha talablarga javob beradi";
    else {
        for (size_t i = 0; i < r.tips.size(); i++) {
            if (i) tips += "\r\n";
            tips += "> " + r.tips[i];
        }
    }
    setText(hTips, tips);

    static const char* genTags[] = {"14 belgi", "16 belgi", "18 belgi"};
    for (int i = 0; i < 3; i++) {
        if (i < (int)r.suggestions.size()) {
            sugStore[i] = r.suggestions[i];
            setText(hSugPass[i], r.suggestions[i]);
            setText(hSugTag[i], i < 2 ? "Asosiy paroldan" : "Passphrase uslubi");
        } else {
            sugStore[i].clear();
            setText(hSugPass[i], "—");
            setText(hSugTag[i], "");
        }
        if (i < (int)r.generated.size()) {
            genStore[i] = r.generated[i];
            setText(hGenPass[i], r.generated[i]);
            setText(hGenTag[i], std::string(genTags[i]) + " — Kriptografik");
        } else {
            genStore[i].clear();
            setText(hGenPass[i], "—");
            setText(hGenTag[i], "");
        }
    }

    setBar(hStrBar, r.score);
    setBar(hEntBar, (int)std::min(100.0, r.entropy));
    lastScore = r.score;
    lastCommon = r.isCommon;
    hasAnalysis = true;

    std::ostringstream sb;
    sb << "Parol tahlil qilindi  |  "
       << r.score << "/100 ball  |  "
       << std::fixed << std::setprecision(1) << r.entropy << " bit  |  "
       << r.crack.label;
    setText(hStatus, sb.str());

    InvalidateRect(gWnd, nullptr, FALSE);
}

std::string currentPassword() {
    int n = GetWindowTextLengthW(hEdit);
    if (n <= 0) return "";
    std::wstring w(n + 1, L'\0');
    GetWindowTextW(hEdit, w.data(), n + 1);
    w.resize(n);
    return wideToUtf8(w);
}

void refreshAnalysis() {
    std::string password = currentPassword();
    setText(hLength, std::to_string(password.size()) + " belgi");
    if (password.empty()) {
        resetEmptyUi();
        setText(hLength, "0 belgi");
        return;
    }
    applyResult(analyze(password));
}

void createUi(HWND wnd) {
    const int L = 16;
    const int LW = 268;

    makeLabel(wnd, L, 16, LW, 28, L"PAROLTEST", hFontTitle);
    makeLabel(wnd, L, 44, LW, 18, L"Parol kuchini tahlil qilish", hFontSmall);
    makeLabel(wnd, L, 78, LW, 16, L"PAROL", hFontSmall);

    hEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
                            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_PASSWORD,
                            L, 98, LW - 44, 28, wnd, (HMENU)ID_EDIT, gInst, nullptr);
    SendMessageW(hEdit, WM_SETFONT, (WPARAM)hFontMono, TRUE);
    SendMessageW(hEdit, EM_SETCUEBANNER, TRUE, (LPARAM)L"Parolni kiriting...");

    hToggle = makeBtn(wnd, ID_TOGGLE, L + LW - 40, 98, 40, 28, L"Ko'r");
    hGenerate = makeBtn(wnd, ID_GENERATE, L, 134, LW, 28, L"Kuchli parol yaratish");

    makeLabel(wnd, L, 176, LW, 16, L"KUCH DARAJASI", hFontSmall);
    hStrength = makeLabel(wnd, L, 196, 160, 20, L"—", hFontUi);
    hScore = makeLabel(wnd, L + 160, 196, 108, 20, L"0 / 100", hFontSmall);

    hStrBar = CreateWindowExW(0, PROGRESS_CLASSW, L"",
                              WS_CHILD | WS_VISIBLE,
                              L, 220, LW, 14, wnd, nullptr, gInst, nullptr);
    SendMessageW(hStrBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
    SendMessageW(hStrBar, PBM_SETBARCOLOR, 0, COL_ACCENT);
    SendMessageW(hStrBar, PBM_SETBKCOLOR, 0, RGB(30, 36, 53));

    hStars = makeLabel(wnd, L, 238, LW, 22, L"☆☆☆☆☆", hFontUi);

    makeLabel(wnd, L, 270, 120, 14, L"UZUNLIK", hFontSmall);
    makeLabel(wnd, L + 134, 270, 134, 14, L"CHARSET", hFontSmall);
    hLength = makeLabel(wnd, L, 286, 120, 20, L"0 belgi", hFontUi);
    hCharset = makeLabel(wnd, L + 134, 286, 134, 20, L"—", hFontUi);

    makeLabel(wnd, L, 316, LW, 14, L"ENTROPY", hFontSmall);
    hEntropy = makeLabel(wnd, L, 332, 120, 20, L"—", hFontUi);
    hEntBar = CreateWindowExW(0, PROGRESS_CLASSW, L"",
                              WS_CHILD | WS_VISIBLE,
                              L + 128, 336, 140, 12, wnd, nullptr, gInst, nullptr);
    SendMessageW(hEntBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
    SendMessageW(hEntBar, PBM_SETBARCOLOR, 0, COL_ACCENT);
    SendMessageW(hEntBar, PBM_SETBKCOLOR, 0, RGB(30, 36, 53));

    makeLabel(wnd, L, 364, LW, 16, L"BUZILISH VAQTI", hFontSmall);
    hCrackTime = makeLabel(wnd, L, 384, LW, 24, L"—", hFontTitle);
    hCrackType = makeLabel(wnd, L, 410, LW, 16, L"GPU Brute-force", hFontSmall);
    hHumor = makeLabel(wnd, L, 428, LW, 48, L"", hFontSmall);
    hBadge = makeLabel(wnd, L, 480, LW, 22, L"—", hFontUi);

    const int RX = 310;
    makeLabel(wnd, RX, 16, 480, 16, L"MUAMMOLAR", hFontSmall);
    hIssues = CreateWindowExW(0, L"EDIT", L"Parol kiriting...",
                              WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
                              RX, 36, 560, 90, wnd, nullptr, gInst, nullptr);
    SendMessageW(hIssues, WM_SETFONT, (WPARAM)hFontUi, TRUE);

    makeLabel(wnd, RX, 130, 480, 16, L"TAVSIYALAR", hFontSmall);
    hTips = CreateWindowExW(0, L"EDIT", L"",
                            WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
                            RX, 150, 560, 90, wnd, nullptr, gInst, nullptr);
    SendMessageW(hTips, WM_SETFONT, (WPARAM)hFontUi, TRUE);

    makeLabel(wnd, RX, 248, 480, 16, L"PAROLNI KUCHAYTIRISH", hFontSmall);
    makeLabel(wnd, RX, 400, 480, 16, L"YANGI KUCHLI PAROLLAR", hFontSmall);

    for (int i = 0; i < 3; i++) {
        int y = 268 + i * 42;
        hSugPass[i] = makeLabel(wnd, RX, y, 430, 18, L"—", hFontMono);
        hSugTag[i]  = makeLabel(wnd, RX, y + 18, 430, 16, L"", hFontSmall);
        hSugCopy[i] = makeBtn(wnd, ID_COPY_S0 + i, RX + 440, y, 90, 28, L"Nusxa");

        int gy = 420 + i * 42;
        hGenPass[i] = makeLabel(wnd, RX, gy, 430, 18, L"—", hFontMono);
        hGenTag[i]  = makeLabel(wnd, RX, gy + 18, 430, 16, L"", hFontSmall);
        hGenCopy[i] = makeBtn(wnd, ID_COPY_G0 + i, RX + 440, gy, 90, 28, L"Nusxa");
    }

    hStatus = makeLabel(wnd, 8, 590, 880, 20,
                        L"ParolTest v2.0 | Parol kiriting...", hFontSmall);
}

LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE:
        gWnd = wnd;
        createUi(wnd);
        return 0;

    case WM_ERASEBKGND: {
        RECT rc;
        GetClientRect(wnd, &rc);
        FillRect((HDC)wp, &rc, brBg);
        RECT side{0, 0, 300, rc.bottom - 28};
        FillRect((HDC)wp, &side, brSide);
        RECT st{0, rc.bottom - 28, rc.right, rc.bottom};
        FillRect((HDC)wp, &st, brBg);
        return 1;
    }

    case WM_CTLCOLORSTATIC: {
        HDC dc = (HDC)wp;
        SetBkMode(dc, TRANSPARENT);
        HWND ctrl = (HWND)lp;
        if (ctrl == hCrackTime) SetTextColor(dc, COL_ACCENT);
        else if (ctrl == hStrength) {
            if (!hasAnalysis) SetTextColor(dc, COL_MUTED);
            else SetTextColor(dc, scoreColor(lastScore));
        } else if (ctrl == hBadge) {
            if (!hasAnalysis) SetTextColor(dc, COL_MUTED);
            else SetTextColor(dc, lastCommon ? COL_BAD : COL_GOOD);
        } else if (ctrl == hStars) SetTextColor(dc, COL_WARN);
        else if (ctrl == hStatus || ctrl == hCrackType) SetTextColor(dc, COL_MUTED);
        else SetTextColor(dc, COL_TEXT);
        POINT pt{0, 0};
        MapWindowPoints(ctrl, wnd, &pt, 1);
        if (pt.x < 300) return (LRESULT)brSide;
        return (LRESULT)brBg;
    }

    case WM_CTLCOLOREDIT: {
        HDC dc = (HDC)wp;
        HWND ctrl = (HWND)lp;
        if (ctrl == hIssues || ctrl == hTips) {
            SetTextColor(dc, COL_TEXT);
            SetBkColor(dc, COL_BG);
            return (LRESULT)brBg;
        }
        SetTextColor(dc, COL_TEXT);
        SetBkColor(dc, COL_EDIT);
        return (LRESULT)brEdit;
    }

    case WM_COMMAND: {
        int id = LOWORD(wp);
        int code = HIWORD(wp);
        if (id == ID_EDIT && code == EN_CHANGE) {
            refreshAnalysis();
            return 0;
        }
        if (id == ID_TOGGLE) {
            passwordVisible = !passwordVisible;
            SendMessageW(hEdit, EM_SETPASSWORDCHAR, passwordVisible ? 0 : (WPARAM)L'*', 0);
            SetWindowTextW(hToggle, passwordVisible ? L"Yash" : L"Ko'r");
            InvalidateRect(hEdit, nullptr, TRUE);
            return 0;
        }
        if (id == ID_GENERATE) {
            std::wstring w = utf8ToWide(generateStrong(16));
            SetWindowTextW(hEdit, w.c_str());
            return 0;
        }
        if (id >= ID_COPY_S0 && id < ID_COPY_S0 + 3) {
            copyUtf8(sugStore[id - ID_COPY_S0]);
            return 0;
        }
        if (id >= ID_COPY_G0 && id < ID_COPY_G0 + 3) {
            copyUtf8(genStore[id - ID_COPY_G0]);
            return 0;
        }
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(wnd, msg, wp, lp);
}

} // namespace

int WINAPI WinMain(HINSTANCE inst, HINSTANCE, LPSTR, int show) {
    gInst = inst;

    INITCOMMONCONTROLSEX icc{sizeof(icc), ICC_PROGRESS_CLASS | ICC_STANDARD_CLASSES};
    InitCommonControlsEx(&icc);

    hFontTitle = CreateFontW(22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Consolas");
    hFontUi = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                          DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
    hFontMono = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                            DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Consolas");
    hFontSmall = CreateFontW(13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
    brBg   = CreateSolidBrush(COL_BG);
    brSide = CreateSolidBrush(COL_SIDE);
    brEdit = CreateSolidBrush(COL_EDIT);
    brLine = CreateSolidBrush(COL_LINE);

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = inst;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = brBg;
    wc.lpszClassName = L"ParolTestWindow";
    wc.hIcon = LoadIcon(nullptr, IDI_SHIELD);
    RegisterClassExW(&wc);

    HWND wnd = CreateWindowExW(
        0, L"ParolTestWindow",
        L"ParolTest — Parol Tahlilchisi",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 920, 660,
        nullptr, nullptr, inst, nullptr);

    ShowWindow(wnd, show);
    UpdateWindow(wnd);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    DeleteObject(hFontTitle);
    DeleteObject(hFontUi);
    DeleteObject(hFontMono);
    DeleteObject(hFontSmall);
    DeleteObject(brBg);
    DeleteObject(brSide);
    DeleteObject(brEdit);
    DeleteObject(brLine);
    return (int)msg.wParam;
}
