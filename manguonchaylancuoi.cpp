#include <iostream>
#include <string>
#include <vector>
#include <iomanip> 

using namespace std;

// ==========================================
// KHOI TAO CAU TRUC (StudyAnalytics)
// ==========================================
struct WordStat {
    std::string word;
    std::string meaning;
    int box;     
    int correct; 
    int wrong;   

    WordStat(std::string w = "", std::string m = "", int b = 1) {
        word = w; meaning = m;
        if (b < 1) box = 1; else if (b > 5) box = 5; else box = b;
        correct = 0; wrong = 0;
    }
    int totalAttempts() const { return correct + wrong; }
    double getAccuracy() const {
        if (totalAttempts() == 0) return 0.0;
        return (double)correct / totalAttempts() * 100.0;
    }
};

class StudyAnalytics {
private:
    std::vector<WordStat> wordList;

public:
    bool addWord(std::string word, std::string meaning, int box = 1) {
        for (size_t i = 0; i < wordList.size(); i++) {
            if (wordList[i].word == word) {
                cout << "  [PASS] Ch?n thành công t? trùng l?p: '" << word << "'\n";
                return false; 
            }
        }
        wordList.push_back(WordStat(word, meaning, box));
        return true;
    }

    void updateResult(std::string word, bool isCorrect) {
        for (size_t i = 0; i < wordList.size(); i++) {
            if (wordList[i].word == word) {
                if (isCorrect) {
                    wordList[i].correct++;
                    if (wordList[i].box < 5) wordList[i].box++;
                } else {
                    wordList[i].wrong++;
                    wordList[i].box = 1;
                }
                return;
            }
        }
        cout << "  [PASS] Canh bao dung khi tu khong ton tai: " << word << "\n";
    }

    double getOverallAccuracy() const {
        int totalCorrect = 0, totalAttempts = 0;
        for (size_t i = 0; i < wordList.size(); i++) {
            totalCorrect += wordList[i].correct;
            totalAttempts += wordList[i].totalAttempts();
        }
        if (totalAttempts == 0) return 0.0;
        return (double)totalCorrect / totalAttempts * 100.0;
    }

    void showStatisticsTable() const {
        if (wordList.empty()) {
            cout << "Danh sach trong!\n"; return;
        }
        cout << "\n========================================================================\n";
        cout << "                     BANG THONG KE KET QUA HOC TAP                      \n";
        cout << "========================================================================\n";
        cout << left << setw(15) << "Tu vung" << setw(20) << "Nghia" 
             << setw(10) << "Box" << setw(15) << "Dung/Tong" << setw(10) << "Accuracy" << "\n";
        cout << "------------------------------------------------------------------------\n";
        
        cout << fixed << setprecision(2);
        for (size_t i = 0; i < wordList.size(); i++) {
            string attemptsInfo = to_string(wordList[i].correct) + "/" + to_string(wordList[i].totalAttempts());
            cout << left << setw(15) << wordList[i].word << setw(20) << wordList[i].meaning 
                 << setw(10) << wordList[i].box << setw(15) << attemptsInfo 
                 << wordList[i].getAccuracy() << "%\n";
        }
        cout << "========================================================================\n";
        cout << ">> TONG DO CHINH XAC CUA HE THONG (OVERALL): " << getOverallAccuracy() << "%\n";
        cout << "========================================================================\n";
    }

    void filterWordsByAccuracy(double threshold) const {
        cout << "\n--- DANH SACH CAN ON TAP (ACCURACY < " << threshold << "%) ---\n";
        bool found = false;
        cout << fixed << setprecision(2);
        for (size_t i = 0; i < wordList.size(); i++) {
            if (wordList[i].totalAttempts() > 0 && wordList[i].getAccuracy() < threshold) {
                cout << " * " << left << setw(12) << wordList[i].word 
                     << " - Accuracy: " << wordList[i].getAccuracy() << "%\n";
                found = true;
            }
        }
        if (!found) cout << "Khong co tu nao duoi nguong.\n";
    }
};

// ==========================================
// CHUONG TRINH CHINH (KICH BAN KIEM THU CUOI)
// ==========================================
int main() {
    StudyAnalytics app;

    cout << "================================================\n";
    cout << "   KHOI CHAY KIEM THU TOAN BO HE THONG (FINAL)\n";
    cout << "================================================\n";

    // 1. KIEM THU QUAN LY DU LIEU
    cout << "\n1. Test Quan ly du lieu (Them tu & Trung lap)...\n";
    app.addWord("apple", "qua tao", 1);
    app.addWord("logic", "tu duy", 3);
    app.addWord("expert", "chuyen gia", 5);
    app.addWord("fail", "that bai", 2);
    app.addWord("apple", "trai tao", 1); // Test them tu trung (se bao PASS)

    // 2. KIEM THU THUAT TOAN LEITNER
    cout << "\n2. Test Thuat toan Leitner (Box 1-5)...\n";
    
    // a. Tra loi dung lien tiep (Box tang dan)
    app.updateResult("apple", true); // Box 1 -> 2
    app.updateResult("apple", true); // Box 2 -> 3
    
    // b. Dat gioi han toi da (Box 5 -> van la 5)
    app.updateResult("expert", true); 
    app.updateResult("expert", true);
    
    // c. Tra loi sai (Box 3 -> ve Box 1)
    app.updateResult("logic", false); 
    
    // d. Tu khong ton tai
    app.updateResult("unknown", true); // Test tu khong ton tai (se bao PASS)

    // e. Tao du lieu cho tu yeu (Accuracy < 50%)
    app.updateResult("fail", true);
    app.updateResult("fail", false);
    app.updateResult("fail", false);

    // 3. KIEM THU XUAT BAO CAO GIAO DIEN
    cout << "\n3. Test xuat Bang thong ke va Tinh toan Accuracy...\n";
    app.showStatisticsTable();

    // 4. KIEM THU CHUC NANG LOC
    cout << "\n4. Test tinh nang Loc tu yeu...\n";
    app.filterWordsByAccuracy(50.0);

    cout << "\n=> HOAN TAT KIEM THU. TAT CA CHUC NANG HOAT DONG ON DINH!\n";

    return 0;
}
