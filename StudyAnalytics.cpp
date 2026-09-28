#include "StudyAnalytics.h"
#include <iostream>

using namespace std;

// ==============================
// WordStat
// ==============================

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

// ==============================
// StudyAnalytics
// ==============================

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
