#include <iostream>
#include <string>
#include <vector>
#include <iomanip> // Thu vien dung de can le bang thong ke

using namespace std;

// ==========================================
// 1. KHAI BAO CAU TRUC (WordStat & StudyAnalytics)
// ==========================================

// Thong tin thong ke tung tu
struct WordStat {
    std::string word;
    std::string meaning;
    int box;     
    int correct; 
    int wrong;   

    WordStat(std::string w = "", std::string m = "", int b = 1);

    int totalAttempts() const;
    double getAccuracy() const;
};

// Class quan ly phan tich hoc tap
class StudyAnalytics {
private:
    std::vector<WordStat> wordList;

public:
    // Cap nhat kieu tra ve cua addWord de bat loi trung lap (Bug-01)
    bool addWord(std::string word, std::string meaning, int box = 1);
    void updateResult(std::string word, bool isCorrect);

    double getOverallAccuracy() const;
    
    // Module Bang thong ke va Loc tu
    void showStatisticsTable() const; 
    void filterWordsByAccuracy(double threshold) const; 
};


// ==========================================
// 2. TRIEN KHAI PHUONG THUC
// ==========================================

WordStat::WordStat(string w, string m, int b) {
    word = w;
    meaning = m;
    
    if (b < 1) box = 1;
    else if (b > 5) box = 5;
    else box = b;

    correct = 0;
    wrong = 0;
}

int WordStat::totalAttempts() const {
    return correct + wrong;
}

double WordStat::getAccuracy() const {
    if (totalAttempts() == 0) return 0.0;
    return (double)correct / totalAttempts() * 100.0;
}

bool StudyAnalytics::addWord(string word, string meaning, int box) {
    // FIX BUG-01: Kiem tra tu vung trung lap
    for (size_t i = 0; i < wordList.size(); i++) {
        if (wordList[i].word == word) {
            cout << "[Loi] Tu '" << word << "' da ton tai trong he thong!\n";
            return false; // Khong them vao danh sach
        }
    }
    
    wordList.push_back(WordStat(word, meaning, box));
    return true;
}

void StudyAnalytics::updateResult(string word, bool isCorrect) {
    for (size_t i = 0; i < wordList.size(); i++) {
        if (wordList[i].word == word) {
            if (isCorrect) {
                wordList[i].correct++;
                if (wordList[i].box < 5) wordList[i].box++;
            }
            else {
                wordList[i].wrong++;
                wordList[i].box = 1;
            }
            return;
        }
    }
    cout << "[Canh bao] Tu khong ton tai: " << word << "\n";
}

double StudyAnalytics::getOverallAccuracy() const {
    int totalCorrect = 0;
    int totalAttempts = 0;

    for (size_t i = 0; i < wordList.size(); i++) {
        totalCorrect += wordList[i].correct;
        totalAttempts += wordList[i].totalAttempts();
    }

    if (totalAttempts == 0) return 0.0;
    return (double)totalCorrect / totalAttempts * 100.0;
}

// MODULE BANG THONG KE
void StudyAnalytics::showStatisticsTable() const {
    // FIX BUG-03: Kiem tra danh sach trong
    if (wordList.empty()) {
        cout << "\n[Thong bao] Danh sach tu vung hien dang trong!\n";
        return;
    }

    cout << "\n========================================================================\n";
    cout << "                       BANG THONG KE CHI TIET                           \n";
    cout << "========================================================================\n";
    
    // In tieu de cot voi do rong co dinh (setw)
    cout << left << setw(15) << "Tu vung" 
         << setw(20) << "Nghia" 
         << setw(10) << "Box" 
         << setw(15) << "Dung/Tong" 
         << setw(10) << "Accuracy" << "\n";
    cout << "------------------------------------------------------------------------\n";

    // FIX BUG-02: Dinh dang so thap phan 2 chu so
    cout << fixed << setprecision(2);

    for (size_t i = 0; i < wordList.size(); i++) {
        string attemptsInfo = to_string(wordList[i].correct) + "/" + to_string(wordList[i].totalAttempts());
        
        cout << left << setw(15) << wordList[i].word 
             << setw(20) << wordList[i].meaning 
             << setw(10) << wordList[i].box 
             << setw(15) << attemptsInfo 
             << wordList[i].getAccuracy() << "%" << "\n";
    }
    
    cout << "========================================================================\n";
    cout << ">> TONG DO CHINH XAC CUA HE THONG (OVERALL): " << getOverallAccuracy() << "%\n";
    cout << "========================================================================\n";
}

// MODULE LOC TU (Theo Accuracy)
void StudyAnalytics::filterWordsByAccuracy(double threshold) const {
    if (wordList.empty()) return;

    cout << "\n--- KET QUA LOC: CAC TU CO ACCURACY DUOI " << threshold << "% ---\n";
    bool found = false;
    
    cout << fixed << setprecision(2);

    for (size_t i = 0; i < wordList.size(); i++) {
        // Chi xet cac tu da hoc (totalAttempts > 0)
        if (wordList[i].totalAttempts() > 0 && wordList[i].getAccuracy() < threshold) {
            cout << "[Can On Tap] " << left << setw(12) << wordList[i].word 
                 << " - Accuracy: " << wordList[i].getAccuracy() << "%\n";
            found = true;
        }
    }

    if (!found) {
        cout << "Tot qua! Khong co tu nao duoi nguong nay.\n";
    }
}


// ==========================================
// 3. HAM MAIN: KHOI TAO DU LIEU & TEST BUGS
// ==========================================

int main() {
    StudyAnalytics app;

    cout << "========================================\n";
    cout << "   KHOI TAO DU LIEU & TEST BUGS\n";
    cout << "========================================\n";

    // Kiem thu Them tu binh thuong
    app.addWord("apple", "qua tao", 1);
    app.addWord("algorithm", "thuat toan", 2);
    app.addWord("database", "co so du lieu", 3);
    app.addWord("idiom", "thanh ngu", 2);

    // Kiem thu BUG-01: Them tu trung lap (Se in ra thong bao loi va khong them)
    cout << "\n[Test BUG-01] Thu them tu da ton tai:\n";
    app.addWord("apple", "trai tao", 1); 

    // Cap nhat ket qua de co du lieu phan tich
    // algorithm: 1 dung, 2 sai (Accuracy 33.33%) -> Test Bug-02 chia so thap phan
    app.updateResult("algorithm", true);
    app.updateResult("algorithm", false);
    app.updateResult("algorithm", false);

    // database: 3 dung (Accuracy 100%)
    app.updateResult("database", true);
    app.updateResult("database", true);
    app.updateResult("database", true);

    // idiom: 1 dung, 4 sai (Accuracy 20%) -> Tu cuc yeu
    app.updateResult("idiom", true);
    for(int i = 0; i < 4; i++) app.updateResult("idiom", false);

    // ==========================================
    // MODULE MOI: HIEN THI BANG THONG KE
    // ==========================================
    app.showStatisticsTable();

    // ==========================================
    // MODULE MOI: LOC TU THEO NGUONG (VD: Nguong 50%)
    // ==========================================
    app.filterWordsByAccuracy(50.0);

    return 0;
}
