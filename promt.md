loyiha nomi ParolTest

Maqsad{
    Foydalanuvchi parolini:

kuchini baholash
buzilish vaqtini hisoblash
mashhur bazalar bilan tekshirish
avtomatik kuchli parol tavsiya qilish
}

3. ASOSIY FUNKSIYALAR
🔹 1. Password Analyzer
uzunlik
charset (a-z, A-Z, 0-9, symbol)
pattern detection (salom, qwerty)


2. Crack Time Calculator:

T= N**L / R (N — belgilar soni (charset)
L — parol uzunligi
R — sekundiga urinishlar)
entropy modeli ham ishlat
Dictionary attack (real hayotga yaqin)

Bu yerda parol:

oddiy so‘zmi (salom)
mashhur parolmi (123456)
pattern bormi (qwerty)
Algoritm:
agar parol lug‘atda bo‘lsa:
    vaqt = juda kichik (sekundlar)
aks holda:
    brute-force ga o‘tadi

Entropy (zamonaviy va aniqroq model)

Bu usul parolning tasodifiyligini (randomness) o‘lchaydi:

H=L*log2(N)

H — entropy (bitlarda)
Qanchalik katta bo‘lsa → parol shunchalik kuchli
Keyin vaqt:
T = N**L/R
✔ Afzallik:
Ilmiyroq
Zamonaviy security tizimlarda ishlatiladi
Hybrid model:
1. Agar parol mashhur bo‘lsa → darrov "juda zaif"
2. Pattern bo‘lsa → vaqtni kamaytir
3. Entropy hisobla
4. Yakuniy vaqtni chiqar.


3. Common Password Detection
lokal baza (100–10k)
katta baza (API orqali: Have I Been Pwned)

{123456
12345678
123456789
password
qwerty
qwerty123
111111
000000
123123
abc123

admin
admin123
root
user
test123
welcome
welcome123
login
guest
pass123

iloveyou
love123
azerty
qwertyuiop
asdfgh
zxcvbnm
1234qwer
qazwsx
1q2w3e

salom
salom123
salomjon
salom1234
salomjon123
salom2024
salom2000
salom01
salom777
salom7777

uzbek
uzbek123
toshkent
tashkent
uzb123
uzbekiston
uzb777
uzb2024
uzb2000
uzb01

diyor
diyor123
ali123
vali123
aziz123
umar123
islom123
muslim123
bek123
bekzod123

telefon
nomer123
raqam123
parol123
kod123
pin123
pin2024
pin0000
0000
1111

987654
654321
159753
258456
741852
369258
147258
112233
223344
556677

qwerty1
qwerty12
qwerty1234
qwerty2024
qwerty777
admin2024
admin777
password1
password123
pass2024} shunday bazayat bazada o'rta osiyodagi mashhur parollar bo'lsin.


4. Smart Suggestion Engine
parolni kuchaytiradi
bir nechta variant beradi

5. Password Generator
random emas
aqlli (pattern asosida)
Tavsiya berish (ENG MUHIM QISM)

Masalan:
parol: salom1234

❌ muammo:
oddiy so‘z ("salom")
faqat kichik harf
predict qilish oson
✅ dastur quyidagilarni taklif qiladi:

👉 Variantlar:

S@lom1234!
s@L0m!234
S4l0m#X9!
saLOM_93$
🤖 5. Parolni avtomatik kuchaytirish algoritmi

Oddiy qoidalar:

a → @
o → 0
i → 1
s → $
l → !
random qo‘sh:
katta harf
symbol
random son


6. Qo‘shimcha “realistik faktorlar”

Agar loyihani kuchaytirmoqchi bo‘lsang:

⚙️ hisobga ol:
GPU tezligi (10^9 vs 10^12)
Hash turi (bcrypt sekin, MD5 tez)
Online vs offline attack

7. Natijani odamga tushunarli qilish

Oddiy sekund bermagin, balki:

0.5 sekund → juda zaif
10 minut → zaif
2 yil → yaxshi
1000 yil → juda kuchli


va hazilomus natija ham bersin masalan vaqt 10 yildan ko'p bolsa hacker zerikib uxlab qoldi desin 😁



Xulosa

Ha, algoritmlar bor:

Brute force → oddiy
Entropy → ilmiy
Dictionary → real
Hybrid → ENG ZO‘R (sen uchun tavsiya)


GUI qanday bo‘lishi kerak
🖥 Layout:


[ Parol kiriting: ******** ]

[ Strength: █████░░░░ ] (rangli)

⏱ Crack time: 2 seconds

⚠ Muammolar:
- common password
- weak pattern

💡 Tavsiyalar:
- katta harf qo‘sh
- symbol qo‘sh

🔐 Tavsiya parollar:
[ S@l9X!2pQ ]
[ K#4Lm!92X ]
1. Input password
2. Check common DB
3. Pattern analysis
4. Entropy calculation
5. Crack time (3 model)
6. Take minimum
7. Show UI
8. Generate strong alternatives


