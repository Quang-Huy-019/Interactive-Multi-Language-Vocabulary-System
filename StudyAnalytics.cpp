#include "StudyAnalytics.h"
#include <iostream>

using namespace std;

// ==============================
// WordStat
// ==============================

WordStat::WordStat(string w, string m, int b) {
    word = w;
    meaning = m;

    // Ðam bao Box nam trong khoang 1 - 5
    if (b < 1)
        box = 1;
    else if (b > 5)
        box = 5;
    else
        box = b;

    correct = 0;
    wrong = 0;
}

int WordStat::totalAttempts() const {
    return correct + wrong;
}

double WordStat::getAccuracy() const {
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


// C?p nh?t k?t qu? h?c
void StudyAnalytics::updateResult(string word, bool isCorrect) {

    for (size_t i = 0; i < wordList.size(); i++) {

        if (wordList[i].word == word) {

            if (isCorrect) {
                // Tr? l?i dúng
                wordList[i].correct++;

                // Ðúng -> tang Box
                if (wordList[i].box < 5)
                    wordList[i].box++;
            }
            else {
                // Tr? l?i sai
                wordList[i].wrong++;

                // Sai -> quay v? Box 1
                wordList[i].box = 1;
            }

            return;
        }
    }

    // Không tìm thay tu
    cout << "Tu khong ton tai: " << word << "\n";
}


// ==============================
// Tính ti le dung tong the
// ==============================

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


// ==============================
// Hien thi tu yeu
// ==============================

void StudyAnalytics::showWeakWords() const {

    cout << "\n--- DANH SACH TU YEU (TY LE SAI > 40%) ---\n";

    bool hasWeak = false;

    for (size_t i = 0; i < wordList.size(); i++) {

        if (wordList[i].totalAttempts() > 0 &&
            wordList[i].getAccuracy() < 60.0) {

            cout << "- " << wordList[i].word
                 << " (" << wordList[i].meaning << ")"
                 << ": Dung " << wordList[i].getAccuracy()
                 << "%\n";

            hasWeak = true;
        }
    }

    if (!hasWeak)
        cout << "Khong co tu yeu!\n";
}


// ==============================
// Hien thi phân bo Leitner Box
// ==============================

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


// ==============================
// Dashboard
// ==============================

void StudyAnalytics::showDashboard() const {

    cout << "\n========================================\n";
    cout << "       THONG KE KET QUA HOC TAP\n";
    cout << "========================================\n";

    cout << "Tong so tu: "
         << wordList.size() << "\n";

    cout << "Ty le tra loi dung: "
         << getOverallAccuracy()
         << "%\n";

    showLeitnerChart();
    showWeakWords();

    cout << "========================================\n";
}
