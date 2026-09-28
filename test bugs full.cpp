#include <iostream>
#include <string>
#include <vector>

using namespace std;

// ==========================================
// 1. KHAI BAO CAU TRUC (WordStat & StudyAnalytics)
// ==========================================

struct WordStat {
    std::string word;
    std::string meaning;
    int box;     // Box Leitner (1 - 5)
    int correct; // So lan dung
    int wrong;   // So lan sai

    WordStat(std::string w = "", std::string m = "", int b = 1);

    int totalAttempts() const;
    double getAccuracy() const;
};

class StudyAnalytics {
private:
    std::vector<WordStat> wordList;

public:
    void addWord(std::string word, std::string meaning, int box = 1);
    void updateResult(std::string word, bool isCorrect);

    double getOverallAccuracy() const;
    void showWeakWords() const;
    void showLeitnerChart() const;
    void showDashboard() const;
};

// ==========================================
// 2. TRIEN KHAI PHUONG THUC
// ==========================================

WordStat::WordStat(string w, string m, int b) {
    word = w;
    meaning = m;
    // Fix Bug: Dam bao Box luon nam trong khoang [1, 5]
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
    // Fix Bug 1: Chong loi chia cho 0 khi tu chua co luot lam nao
    if (totalAttempts() == 0)
        return 0.0;

    return (double)correct / totalAttempts() * 100.0;
}

void StudyAnalytics::addWord(string word, string meaning, int box) {
    wordList.push_back(WordStat(word, meaning, box));
}

void StudyAnalytics::updateResult(string word, bool isCorrect) {
    for (size_t i = 0; i < wordList.size(); i++) {
        if (wordList[i].word == word) {
            if (isCorrect) {
                wordList[i].correct++;
                // Fix Bug 2: Chan box khong vuot qua 5
                if (wordList[i].box < 5)
                    wordList[i].box++;
            }
            else {
                wordList[i].wrong++;
                // Sai quay ve Box 1 theo chuan Leitner
                wordList[i].box = 1;
            }
            return;
        }
    }
    // Fix Bug 3: Xu ly an toan khi tu khong ton tai trong he thong
    cout << "[BUG TEST WARNING] Tu khong ton tai trong he thong: " << word << "\n";
}

double StudyAnalytics::getOverallAccuracy() const {
    int totalCorrect = 0;
    int totalAttempts = 0;

    for (size_t i = 0; i < wordList.size(); i++) {
        totalCorrect += wordList[i].correct;
        totalAttempts += wordList[i].totalAttempts();
    }

    if (totalAttempts == 0)
        return 0.0;

    return (double)totalCorrect / totalAttempts * 100.0;
}

void StudyAnalytics::showWeakWords() const {
    cout << "\n--- DANH SACH TU YEU (TY LE SAI > 40% / ACCURACY < 60%) ---\n";
    bool hasWeak = false;

    for (size_t i = 0; i < wordList.size(); i++) {
        if (wordList[i].totalAttempts() > 0 &&
            wordList[i].getAccuracy() < 60.0) {

            cout << "- " << wordList[i].word
                 << " (" << wordList[i].meaning << ")"
                 << ": Ty le dung " << wordList[i].getAccuracy()
                 << "%\n";

            hasWeak = true;
        }
    }

    if (!hasWeak)
        cout << "Khong co tu yeu!\n";
}

void StudyAnalytics::showLeitnerChart() const {
    cout << "\n--- PHAN BO LEITNER BOX ---\n";

    for (int b = 1; b <= 5; b++) {
        int count = 0;
        for (size_t i = 0; i < wordList.size(); i++) {
            if (wordList[i].box == b)
                count++;
        }

        cout << "Box " << b << ": ";
        for (int j = 0; j < count; j++)
            cout << "*";
        cout << " (" << count << " tu)\n";
    }
}

void StudyAnalytics::showDashboard() const {
    cout << "\n========================================\n";
    cout << "        DASHBOARD THONG KE HOC TAP\n";
    cout << "========================================\n";

    cout << "Tong so tu: " << wordList.size() << "\n";
    cout << "Ty le tra loi dung tong the: " << getOverallAccuracy() << "%\n";

    showLeitnerChart();
    showWeakWords();

    cout << "========================================\n";
}

// ==========================================
// 3. HAM MAIN: KHOI TAO SEED DATA & CHAY TEST BUGS
// ==========================================

int main() {
    StudyAnalytics app;

    cout << "========================================\n";
    cout << "   KHOI TAO DU LIEU MAU (SEED DATA)\n";
    cout << "========================================\n";

    // 1. Them Seed Data vao he thong
    app.addWord("apple", "qua tao", 1);         // Tu moi (Chua hoc)
    app.addWord("algorithm", "thuat toan", 2);  // Dang o Box 2
    app.addWord("computer", "may tinh", 4);     // Dang o Box 4
    app.addWord("database", "co so du lieu", 3); // Dang o Box 3
    app.addWord("idiom", "thanh ngu", 2);       // Tu se chuyen thanh tu yeu
    app.addWord("framework", "khung lam viec", 5); // Dang o Box 5 (Gioi han toi da)

    cout << "Da nap thanh cong 6 tu vung mau.\n";

    cout << "\n========================================\n";
    cout << "   CHAY THU NGHIEM PHAT HIEN LOI (TEST BUGS)\n";
    cout << "========================================\n";

    // Test Bug 1: Tu moi chua co luot hoc (Kiem tra tranh loi chia cho 0)
    cout << "\n[Test Bug 1] Kiem tra tu chua hoc ('apple'):\n";
    // Khong goi updateResult cho "apple", goi truc tiep Dashboard de test accuracy = 0.0

    // Test Bug 2: Cap nhat tu hoat dong dung logic Leitner & gioi han Box
    cout << "\n[Test Bug 2] Kiem tra cap nhat ket qua 'algorithm' (Dung):\n";
    app.updateResult("algorithm", true); 
    app.updateResult("algorithm", true); // Len Box 4

    cout << "\n[Test Bug 2.1] Kiem tra cap nhat 'computer' (Sai -> Ve Box 1):\n";
    app.updateResult("computer", false); // Sai -> Ve Box 1

    // Test Bug 3: Tao tap du lieu tu yeu (Ty le sai > 40%) cho tu 'idiom'
    cout << "\n[Test Bug 3] Mo phong tu yeu ('idiom' - 4 dung, 6 sai):\n";
    for(int i=0; i<4; i++) app.updateResult("idiom", true);
    for(int i=0; i<6; i++) app.updateResult("idiom", false);

    // Test Bug 4: Truy van tu khong ton tai trong he thong (Kiem tra bat loi ngoai le)
    cout << "\n[Test Bug 4] Truy van tu khong ton tai ('unknown_word'):\n";
    app.updateResult("unknown_word", true);

    // ==========================================
    // HIEN THI KET QUA TONG QUAN (DASHBOARD)
    // ==========================================
    app.showDashboard();

    return 0;
}
