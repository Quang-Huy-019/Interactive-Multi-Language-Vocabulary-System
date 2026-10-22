#include "StudyAnalytics.h"
#include "ui.h"

#include <iomanip>
#include <iostream>

using namespace std;

double StudyAnalytics::overallAccuracy(const vector<Word*>& words) {
    int correct = 0, attempts = 0;
    for (const Word* w : words) {
        correct += w->getCorrect();
        attempts += w->getCorrect() + w->getWrong();
    }
    return attempts == 0 ? 0.0 : (double)correct / attempts * 100.0;
}

void StudyAnalytics::showLeitnerChart(const vector<Word*>& words) {
    setColor(C_TITLE);
    cout << "\n--- PHAN BO LEITNER BOX ---\n";
    resetColor();

    const int total = (int)words.size();
    for (int b = 1; b <= 5; b++) {
        int count = 0;
        for (const Word* w : words)
            if (w->getLeitnerBox() == b) count++;

        double pct = total > 0 ? (double)count / total * 100.0 : 0.0;
        int bar = total > 0 ? (int)((double)count * 50.0 / total + 0.5) : 0;
        if (count > 0 && bar == 0) bar = 1;

        setColor(b <= 2 ? C_BAD : (b >= 4 ? C_OK : C_WARN));
        cout << "Box " << b << ": " << string(bar, '*')
             << " (" << count << " tu, " << fixed << setprecision(1) << pct << "%)\n";
        resetColor();
    }
}

void StudyAnalytics::showWeakWords(const vector<Word*>& words) {
    setColor(C_TITLE);
    cout << "\n--- TU YEU CAN ON TAP (BOX 1-2) ---\n";
    resetColor();

    const int MAX_SHOW = 30; // gioi han de khong tran man hinh
    int count = 0;
    for (const Word* w : words) {
        if (w->getLeitnerBox() > 2) continue;
        count++;
        if (count > MAX_SHOW) continue;

        int attempts = w->getCorrect() + w->getWrong();
        cout << " - " << w->getEnglish() << " : " << w->getVietnamese()
             << " (Box " << w->getLeitnerBox() << ", dung "
             << w->getCorrect() << "/" << attempts << ")\n";
    }

    if (count == 0)
        cout << "Khong co tu yeu. Tuyet voi!\n";
    else {
        if (count > MAX_SHOW)
            cout << " ... va " << (count - MAX_SHOW) << " tu khac\n";
        cout << "Tong: " << count << " tu. Dung Chuc nang 8 (The ghi nho) hoac 7 de on tap.\n";
    }
}

void StudyAnalytics::showDashboard(const vector<Word*>& words) {
    int practiced = 0;
    for (const Word* w : words)
        if (w->getCorrect() + w->getWrong() > 0) practiced++;

    setColor(C_TITLE);
    cout << "\n========================================\n";
    cout << "        DASHBOARD THONG KE HOC TAP\n";
    cout << "========================================\n";
    resetColor();

    cout << "Tong so tu: " << words.size() << "\n";
    cout << "Tu da tung luyen tap: " << practiced << "\n";
    cout << "Ty le tra loi dung (tich luy): " << fixed << setprecision(1)
         << overallAccuracy(words) << "%\n";

    showLeitnerChart(words);
    showWeakWords(words);

    setColor(C_TITLE);
    cout << "========================================\n";
    resetColor();
}
