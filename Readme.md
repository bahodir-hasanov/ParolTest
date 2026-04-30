# ParolTest — Parol kuchini tahlil qiluvchi dastur

C++17 da yozilgan terminal dastur. Qt kutubxonasi ishlatilmagan.

## Kompilyatsiya

```bash
# Linux / macOS
g++ -std=c++17 -O2 -o paroltest main.cpp

# Yoki Makefile orqali
make

# Windows (MinGW)
g++ -std=c++17 -O2 -o paroltest.exe main.cpp
```

## Ishlatish

```bash
# Interaktiv menyu
./paroltest

# To'g'ridan-to'g'ri parol tekshirish
./paroltest myPassword123
```

## Funksiyalar

| Funksiya | Tavsif |
|---|---|
| Password Analyzer | Uzunlik, charset, entropy tahlili |
| Crack Time | GPU brute-force + hybrid model |
| Dictionary Check | O'rta Osiyo + global baza (~150 parol) |
| Pattern Detection | Keyboard row, sequential, repeat |
| Smart Suggestions | Substitution + kuchaytirish variantlari |
| Password Generator | Kriptografik kuchli tasodifiy parol |

## Algoritmlar

- **Entropy**: `H = L * log2(N)` (Shannon)
- **Brute-force**: `T = N^L / R` (R = 10^10 GPU/s)
- **Hybrid**: Dictionary → Pattern → Entropy → Min
- **Strength score**: 0–100 ball tizimi