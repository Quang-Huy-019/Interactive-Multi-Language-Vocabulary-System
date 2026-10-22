#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <conio.h>
#include <windows.h>

#include <algorithm>
#include <cctype>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include "Dictionary.h"
#include "StudyAnalytics.h"
#include "quiz.h"
#include "Flashcard.h"
#include "ui.h"
#include <fstream>
#include <random>
#include <ctime>

using namespace std;

static const string DATA_FILE = "data.txt";

// ============================================================
// TIEN ICH NHAP LIEU (chi dung getline de khong bi lech bo dem voi _getch)
// ============================================================
static string trimStr(const string& s)
{
    size_t b = 0, e = s.size();
    while (b < e && isspace((unsigned char)s[b])) b++;
    while (e > b && isspace((unsigned char)s[e - 1])) e--;
    return s.substr(b, e - b);
}

static string readLine(const string& prompt)
{
    cout << prompt;
    string s;
    getline(cin, s);
    return trimStr(s);
}

// Doc so nguyen trong khoang [lo, hi]
static int readInt(const string& prompt, int lo, int hi)
{
    while (true)
    {
        string s = readLine(prompt);
        try
        {
            size_t pos = 0;
            int v = stoi(s, &pos);
            if (pos == s.size() && v >= lo && v <= hi)
                return v;
        }
        catch (...) {}
        cout << "[!] Vui long nhap so tu " << lo << " den " << hi << ".\n";
    }
}

static vector<string> splitTrim(const string& s, char sep)
{
    vector<string> out;
    stringstream ss(s);
    string part;
    while (getline(ss, part, sep))
    {
        part = trimStr(part);
        if (!part.empty()) out.push_back(part);
    }
    return out;
}

// Khong de '|' lam hong dinh dang file
static string noPipe(string s)
{
    for (char& c : s) if (c == '|') c = '/';
    return s;
}

static void pauseAnyKey()
{
    cout << "\nNhan phim bat ky de quay lai menu...";
    _getch();
}

static void saveData(Dictionary& dictionary)
{
    if (!dictionary.saveToFile(DATA_FILE))
        cout << "\n[!] Khong ghi duoc file " << DATA_FILE << "!\n";
}

// Lay cac tu thuoc box cho truoc (box = 0 nghia la lay tat ca)
static vector<Word*> collectWords(const Dictionary& dictionary, int box)
{
    vector<Word*> result;
    for (Word* w : dictionary.getAllWords())
        if (box == 0 || w->getLeitnerBox() == box)
            result.push_back(w);
    return result;
}

static vector<QuizWord> toQuizWords(const vector<Word*>& words)
{
    vector<QuizWord> out;
    out.reserve(words.size());
    for (Word* w : words)
        out.emplace_back(w->getEnglish(), w->getVietnamese());
    return out;
}

// ============================================================
// SET CONSOLE FULLSCREEN (giu nguyen nhu ban cu)
// ============================================================
static void fullscreen()
{
    keybd_event(VK_MENU, 0x38, 0, 0);
    keybd_event(VK_RETURN, 0x1c, 0, 0);
    keybd_event(VK_RETURN, 0x1c, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_MENU, 0x38, KEYEVENTF_KEYUP, 0);
}

// ============================================================
// MAIN
// ============================================================
// Bo tron rieng cho on tap theo Box (doi mau ngau nhien moi lan)
static mt19937& reviewRng()
{
    static mt19937 gen(static_cast<unsigned>(time(nullptr)));
    return gen;
}

int main()
{
    fullscreen();

    Dictionary dictionary;
    int skipped = dictionary.loadFromFile(DATA_FILE);
    if (skipped > 0)
    {
        cout << "[!] Co " << skipped << " dong sai dinh dang trong " << DATA_FILE << " da bi bo qua.\n";
        pauseAnyKey();
    }

    while (true)
    {
        clearScreen();

        printTitle("INTERACTIVE MULTI-LANGUAGE VOCAB SYSTEM");
        cout << "  Tong so tu: " << dictionary.size() << "\n\n";
        printKey("1", "Tra cuu tu vung");
        printKey("2", "Them tu vung moi");
        printKey("3", "Lam bai trac nghiem (toan bo tu vung)");
        printKey("4", "Xem lich su thi");
        printKey("5", "Xoa tu vung");
        printKey("6", "Bang thong ke hoc tap");
        printKey("7", "On tap tu yeu theo Box");
        printKey("8", "The ghi nho (Flashcard)");
        printKey("9", "Nhap / Xuat file");
        printKey("0", "Thoat va luu du lieu");
        cout << "\n";

        cout << "Chon chuc nang (0-9): ";

        char choice = (char)_getch();
        cout << choice << "\n";

        switch (choice)
        {
        // --------------------------------------------------------
        // 1. TRA CUU
        // --------------------------------------------------------
        case '1': {
            while (true)
            {
                cout << "\n==================================\n";
                cout << "--- TRA CUU TU VUNG HAI CHIEU ---\n";
                cout << "1. Tra Anh -> Viet (Tim ca tu dong nghia)\n";
                cout << "2. Tra Viet -> Anh (Tim tu khoa trong nghia)\n";
                cout << "0. QUAY LAI MENU CHINH\n";
                cout << "Chon che do (0-2): ";

                char mode = (char)_getch();
                cout << mode << "\n";

                if (mode == '0') break;
                if (mode != '1' && mode != '2')
                {
                    cout << "\n[!] Lua chon khong hop le, vui long chon lai!\n";
                    continue;
                }

                string keyword = readLine("Nhap tu can tra (Hoac nhap '0' de quay lai): ");
                if (keyword == "0") break;
                if (keyword.empty())
                {
                    cout << "\n[!] Ban chua nhap tu can tra.\n";
                    continue;
                }

                vector<Word*> results = (mode == '1')
                    ? dictionary.searchEnglishExtended(keyword)
                    : dictionary.searchVietnameseKeyword(keyword);

                if (!results.empty())
                {
                    cout << "\n=== TIM THAY " << results.size() << " KET QUA ===\n";
                    for (Word* w : results)
                    {
                        w->displayDetails();
                        cout << "--------------------------------\n";
                    }
                }
                else if (mode == '1')
                {
                    cout << "\n[!] Khong tim thay tu chinh hay tu dong nghia nao chua: '" << keyword << "'\n";
                }
                else
                {
                    cout << "\n[!] Khong co tu vung nao chua tu khoa: '" << keyword << "'\n";
                }
            }
            break;
        }

        // --------------------------------------------------------
        // 2. THEM TU
        // --------------------------------------------------------
        case '2': {
            while (true)
            {
                cout << "\n\n==================================\n";
                cout << "--- THEM TU VUNG MOI ---\n";

                string eng = readLine("Nhap tu tieng Anh (Nhan '0' de thoat ve Menu chinh): ");
                if (eng == "0")
                {
                    cout << "\n[*] Da thoat che do them tu!\n";
                    break;
                }
                if (eng.empty())
                {
                    cout << "[!] Tu tieng Anh khong duoc de trong.\n";
                    continue;
                }
                if (dictionary.searchEnglish(eng))
                {
                    cout << "[!] Tu '" << eng << "' da ton tai!\n";
                    continue;
                }

                string vi = readLine("Nhap nghia tieng Viet: ");
                if (vi.empty())
                {
                    cout << "[!] Nghia tieng Viet khong duoc de trong.\n";
                    continue;
                }

                cout << "Loai tu: 1. Tu thong dung   2. Thuat ngu chuyen nganh   3. Thanh ngu\n";
                int typeChoice = readInt("Chon loai (1-3): ", 1, 3);
                string typeName = (typeChoice == 2) ? "TechnicalTerm"
                                : (typeChoice == 3) ? "Idiom"
                                                    : "GeneralWord";

                string syn = readLine("Nhap tu Dong nghia (cach nhau bang dau phay, Enter de bo qua): ");
                string ant = readLine("Nhap tu Trai nghia (cach nhau bang dau phay, Enter de bo qua): ");
                string ex = readLine("Nhap Cau vi du (nhieu cau cach nhau bang dau ';', Enter de bo qua): ");

                Word* newWord = createWordByType(typeName, noPipe(eng), noPipe(vi));
                for (const string& s : splitTrim(syn, ',')) newWord->addSynonym(noPipe(s));
                for (const string& a : splitTrim(ant, ',')) newWord->addAntonym(noPipe(a));
                for (const string& e : splitTrim(ex, ';')) newWord->addExample(noPipe(e));

                if (dictionary.addWord(newWord))
                {
                    saveData(dictionary);
                    cout << "\n[OK] Da them thanh cong tu '" << eng << "' vao Box 1!\n";
                }
                else
                {
                    cout << "\n[!] Khong them duoc tu '" << eng << "'.\n";
                    delete newWord;
                }
            }
            break;
        }

        // --------------------------------------------------------
        // 3. TRAC NGHIEM
        // --------------------------------------------------------
        case '3': {
            cout << "3\n\n--- LAM BAI TRAC NGHIEM ---\n";
            cout << "Bai thi gom ngau nhien cac tu trong toan bo tu vung.\n";
            cout << "(De on tap theo Box, dung Chuc nang 7 hoac 8.)\n\n";

            vector<Word*> pool = collectWords(dictionary, 0);
            if (pool.empty())
            {
                cout << "\n[!] Khong co tu nao de lam bai.\n";
                pauseAnyKey();
                break;
            }

            QuizSession quiz(toQuizWords(pool), &dictionary);
            quiz.startQuiz(); // se hoi so cau hoi
            saveData(dictionary);
            break;
        }

        // --------------------------------------------------------
        // 4. LICH SU THI        // --------------------------------------------------------
        // 4. LICH SU THI
        // --------------------------------------------------------
        case '4':
            cout << "4\n";
            QuizHistoryManager::showHistoryMenu();
            break;

        // --------------------------------------------------------
        // 5. XOA TU
        // --------------------------------------------------------
        case '5': {
            cout << "5\n\n--- XOA TU VUNG ---\n";
            string eng = readLine("Nhap tu tieng Anh can xoa: ");

            if (!eng.empty() && dictionary.deleteWord(eng))
            {
                saveData(dictionary);
                cout << "\n[OK] Da xoa thanh cong tu '" << eng << "' khoi he thong!\n";
            }
            else
            {
                cout << "\n[!] Khong tim thay tu '" << eng << "' trong tu dien.\n";
            }
            pauseAnyKey();
            break;
        }

        // --------------------------------------------------------
        // 6. THONG KE
        // --------------------------------------------------------
        case '6': {
            cout << "6\n\n";
            StudyAnalytics::showDashboard(dictionary.getAllWords());
            pauseAnyKey();
            break;
        }

        // --------------------------------------------------------
        // 7. ON TAP THEO LEITNER BOX        // --------------------------------------------------------
        // 7. ON TAP THEO LEITNER BOX
        // --------------------------------------------------------
        case '7': {
            printTitle("ON TAP TU YEU THEO LEITNER BOX");
            int targetBox = readInt("Nhap Leitner Box can on (1-5, 0 de huy): ", 0, 5);
            if (targetBox == 0) break;

            vector<Word*> pool = collectWords(dictionary, targetBox);
            if (pool.empty())
            {
                cout << "\n[+] Box " << targetBox << " hien dang trong. Khong co tu nao can on!\n";
                pauseAnyKey();
                break;
            }

            // Chi hien mot mau ngau nhien; nhap R de doi sang mau khac
            const size_t SHOW = 8;
            int numWords = 0;
            while (true)
            {
                vector<Word*> sample = pool;
                shuffle(sample.begin(), sample.end(), reviewRng());

                cout << "\nBox " << targetBox << " co " << pool.size() << " tu. Mau ngau nhien:\n";
                for (size_t i = 0; i < sample.size() && i < SHOW; i++)
                    cout << " - " << sample[i]->getEnglish() << " : " << sample[i]->getVietnamese() << "\n";
                if (pool.size() > SHOW)
                    cout << " ... va con " << pool.size() - SHOW << " tu khac\n";

                string in = readLine("\nNhap so cau on tap (1-" + to_string(pool.size())
                                     + "), R de xem mau khac, 0 de huy: ");
                if (in == "0") break;
                if (in == "r" || in == "R") continue;

                bool ok = false;
                try
                {
                    size_t pos = 0;
                    int v = stoi(in, &pos);
                    if (pos == in.size() && v >= 1 && v <= (int)pool.size()) { numWords = v; ok = true; }
                }
                catch (...) {}
                if (ok) break;
                cout << "[!] Nhap so tu 1 den " << pool.size() << ", hoac R, hoac 0.\n";
            }
            if (numWords == 0) break;

            cout << "\n------------------------------------------------------\n";
            printKey("Y", "Bat dau lam bai on tap ngay");
            printKey("N", "Quay ve menu");
            cout << "Lua chon: ";
            char doQuiz = (char)_getch();
            cout << doQuiz << "\n";

            if (toupper((unsigned char)doQuiz) == 'Y')
            {
                QuizSession quiz(toQuizWords(pool), &dictionary);
                quiz.startQuiz(numWords); // dung dung so cau da chon, khong hoi lai
                saveData(dictionary);
            }
            break;
        }

        // --------------------------------------------------------
        // 8. FLASHCARD
        // --------------------------------------------------------
        case '8': {
            cout << "8\n\n--- THE GHI NHO (FLASHCARD) ---\n";
            int box = readInt("Chon Box can hoc (0 = tat ca, 1-5): ", 0, 5);

            vector<Word*> pool = collectWords(dictionary, box);
            if (pool.empty())
            {
                cout << "\n[+] Khong co tu nao trong lua chon nay.\n";
                pauseAnyKey();
                break;
            }

            int n = readInt("Nhap so the muon hoc (1-" + to_string(pool.size()) + "): ",
                            1, (int)pool.size());
            runFlashcards(pool, n);
            saveData(dictionary);
            break;
        }

        // --------------------------------------------------------
        // 9. NHAP / XUAT FILE
        // --------------------------------------------------------
        case '9': {
            cout << "9\n\n--- NHAP / XUAT FILE ---\n";
            cout << "1. Nhap tu file (them cac tu chua co, giu nguyen Box)\n";
            cout << "2. Xuat bo tu ra file moi\n";
            cout << "0. Quay lai\n\n";
            int ioChoice = readInt("Chon (0-2): ", 0, 2);

            if (ioChoice == 1)
            {
                string path = readLine("Nhap duong dan file can nhap (vd: words.txt): ");
                ifstream check(path);
                if (path.empty() || !check.is_open())
                {
                    cout << "\n[!] Khong mo duoc file '" << path << "'.\n";
                }
                else
                {
                    check.close();
                    size_t before = dictionary.size();
                    int skipped = dictionary.loadFromFile(path);
                    size_t added = dictionary.size() - before;
                    saveData(dictionary);
                    cout << "\n[OK] Da nhap " << added << " tu moi.\n";
                    cout << "[i] Bo qua " << skipped << " dong (trung hoac sai dinh dang).\n";
                }
                pauseAnyKey();
            }
            else if (ioChoice == 2)
            {
                string path = readLine("Nhap ten file muon xuat (vd: backup, tu dong them .txt): ");
                if (!path.empty() && path.find('.') == string::npos)
                    path += ".txt"; // shortcut: chi kiem tra dau '.', ten co dau cham khac se giu nguyen
                if (path.empty())
                {
                    cout << "\n[!] Ten file khong duoc de trong.\n";
                }
                else if (dictionary.saveToFile(path))
                {
                    cout << "\n[OK] Da xuat " << dictionary.size() << " tu ra file '" << path << "'.\n";
                }
                else
                {
                    cout << "\n[!] Khong ghi duoc file '" << path << "'.\n";
                }
                pauseAnyKey();
            }
            break;
        }

        // --------------------------------------------------------
        // 0. THOAT
        // --------------------------------------------------------
        case '0':
        case 27:
            cout << "\n\nDang luu du lieu...\n";
            if (dictionary.saveToFile(DATA_FILE))
                cout << "[OK] Da luu thanh cong! Cam on ban da su dung he thong.\n";
            else
                cout << "[!] Khong luu duoc file " << DATA_FILE << "!\n";
            return 0;

        default:
            cout << "\n\n[!] Lua chon khong hop le!";
            cout << "\nNhan phim bat ky de tiep tuc...";
            _getch();
            break;
        }
    }

    return 0;
}
