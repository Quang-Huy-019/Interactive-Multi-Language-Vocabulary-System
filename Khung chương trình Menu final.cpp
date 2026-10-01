#include <iostream>
#include <vector>
#include <string>

using namespace std;

// =========================
// CLASS WORD
// =========================
class Word {
public:
    string english;
    string vietnamese;

    Word(string e, string v) {
        english = e;
        vietnamese = v;
    }
};

// =========================
// HIEN THI MENU CHINH
// =========================
void showMenu() {
    cout << "\n";
    cout << "=================================\n";
    cout << "       UNG DUNG HOC TU VUNG      \n";
    cout << "=================================\n";
    cout << "1. Them tu\n";
    cout << "2. Sua tu\n";
    cout << "3. Xoa tu\n";
    cout << "4. Tim tu\n";
    cout << "5. Hien thi tat ca tu\n";
    cout << "0. Thoat\n";
    cout << "=================================\n";
    cout << "Nhap lua chon: ";
}

// =========================
// 1. THEM TU
// =========================
void addWord(vector<Word>& words) {
    string english;
    string vietnamese;

    cout << "\n========== THEM TU ==========\n";

    cout << "Nhap tu tieng Anh: ";
    cin >> english;

    cout << "Nhap nghia tieng Viet: ";
    cin.ignore();
    getline(cin, vietnamese);

    words.push_back(Word(english, vietnamese));

    cout << "=> Them tu thanh cong!\n";
}

// =========================
// 2. SUA TU
// =========================
void editWord(vector<Word>& words) {
    string english;

    cout << "\n========== SUA TU ==========\n";

    cout << "Nhap tu tieng Anh can sua: ";
    cin >> english;

    for (Word& word : words) {

        if (word.english == english) {

            cout << "Tu hien tai: "
                 << word.english
                 << " - "
                 << word.vietnamese << endl;

            cout << "Nhap nghia moi: ";
            cin.ignore();
            getline(cin, word.vietnamese);

            cout << "=> Sua tu thanh cong!\n";
            return;
        }
    }

    cout << "=> Khong tim thay tu!\n";
}

// =========================
// 3. XOA TU
// =========================
void deleteWord(vector<Word>& words) {
    string english;

    cout << "\n========== XOA TU ==========\n";

    cout << "Nhap tu tieng Anh can xoa: ";
    cin >> english;

    for (int i = 0; i < words.size(); i++) {

        if (words[i].english == english) {

            words.erase(words.begin() + i);

            cout << "=> Xoa tu thanh cong!\n";
            return;
        }
    }

    cout << "=> Khong tim thay tu!\n";
}

// =========================
// 4. TIM TU
// =========================
void findWord(const vector<Word>& words) {
    string english;

    cout << "\n========== TIM TU ==========\n";

    cout << "Nhap tu can tim: ";
    cin >> english;

    for (const Word& word : words) {

        if (word.english == english) {

            cout << "\nTim thay tu:\n";
            cout << "Tu: " << word.english << endl;
            cout << "Nghia: " << word.vietnamese << endl;

            return;
        }
    }

    cout << "=> Khong tim thay tu!\n";
}

// =========================
// 5. HIEN THI TAT CA TU
// =========================
void showWords(const vector<Word>& words) {

    cout << "\n========== DANH SACH TU ==========\n";

    if (words.empty()) {
        cout << "Danh sach dang rong!\n";
        return;
    }

    for (int i = 0; i < words.size(); i++) {

        cout << i + 1 << ". "
             << words[i].english
             << " - "
             << words[i].vietnamese
             << endl;
    }
}

// =========================
// MAIN
// =========================
int main() {

    vector<Word> words;

    int choice;

    do {

        showMenu();

        cin >> choice;

        switch (choice) {

            case 1:
                addWord(words);
                break;

            case 2:
                editWord(words);
                break;

            case 3:
                deleteWord(words);
                break;

            case 4:
                findWord(words);
                break;

            case 5:
                showWords(words);
                break;

            case 0:
                cout << "\nCam on ban da su dung ung dung!\n";
                break;

            default:
                cout << "\n=> Lua chon khong hop le!\n";
        }

    } while (choice != 0);

    return 0;
}