# ParolTest — Parol kuchini tahlil qiluvchi dastur

C++17. Tahlil yadrosi (`analyzer.hpp`, `passwords_db.hpp`) Linux va Windowsda **bir xil**.
GUI: Linuxda GTK3, Windowsda Win32 (GTK o‘rnatish shart emas).

Linuxda GTK3 kerak: `sudo apt install libgtk-3-dev cmake g++`

## Kompilyatsiya

Loyiha ildizidan yoki `src/` dan:

```bash
# Linux
cmake -S . -B build
cmake --build build -j
./build/paroltest      # ildizdan cmake
# yoki src ichida:
# ./build/paroltest
```

```bat
REM Windows (Visual Studio)
cmake -S . -B build -A x64
cmake --build build --config Release
build\Release\paroltest.exe
```

```bat
REM Windows (MinGW)
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
build\paroltest.exe
```

`src/` papkadan (oldingi oqim):

```bash
cd src && rm -rf build && mkdir build && cd build && cmake .. && cmake --build . -j
./paroltest
```

## Ishlatish

```bash
./paroltest          # Linux
paroltest.exe        # Windows
```

## Funksiyalar

| Funksiya | Tavsif |
|---|---|
| Password Analyzer | Uzunlik, charset, entropy tahlili |
| Crack Time | GPU brute-force + hybrid model |
| Dictionary Check | O'rta Osiyo + global baza (1000+ parol) |
| Pattern Detection | Keyboard row, sequential, repeat |
| Smart Suggestions | Substitution + kuchaytirish variantlari |
| Password Generator | Kriptografik kuchli tasodifiy parol |

## Algoritmlar

- **Entropy**: `H = L * log2(N)` (Shannon o‘rtacha)
- **Brute-force**: `T = N^L / R` (R = 10^10 GPU/s)
- **Hybrid**: Dictionary → Pattern → Entropy → Min
- **Strength score**: 0–100 ball tizimi
