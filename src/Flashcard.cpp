#include "Flashcard.h"
#include "LeitnerSystem.h"
#include "ui.h"

#include <algorithm>
#include <conio.h>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <random>
#include <ctime>

using namespace std;

static void clearScreenFC()
{
    clearScreen();
}

void runFlashcards(vector<Word*> pool, int numCards)
{
    if (pool.empty()) return;

    static mt19937 gen(static_cast<unsigned>(time(nullptr)));
    shuffle(pool.begin(), pool.end(), gen);

    int total = min((int)pool.size(), numCards);
    int remembered = 0, forgot = 0;
    LeitnerSystem leitner;

    for (int i = 0; i < total; i++)
    {
        Word* w = pool[i];

        // ---- Mat truoc: chi hien tu tieng Anh ----
        clearScreenFC();
        printTitle("THE GHI NHO  " + to_string(i + 1) + " / " + to_string(total));
        cout << "\n";
        setColor(C_DIM);
        cout << "  Box " << w->getLeitnerBox() << "   Tien do: ";
        printBar(i, total, 24);
        cout << "\n\n\n\n";
        setColor(C_BRIGHT);
        cout << "                " << w->getEnglish() << "\n";
        resetColor();
        cout << "\n\n\n\n";
        printKey("ENTER / SPACE", "Lat the xem nghia");
        printKey("Q / ESC", "Thoat");
        cout << "\n";

        int key = _getch();
        if (key == 'q' || key == 'Q' || key == 27) break;

        // ---- Mat sau: hien nghia va vi du ----
        clearScreenFC();
        printTitle("THE GHI NHO  " + to_string(i + 1) + " / " + to_string(total));
        cout << "\n";
        w->displayDetails();
        cout << "\n";
        printKey("1", "Nho");
        printKey("2", "Chua nho");
        printKey("Q", "Thoat");
        cout << "\n";

        bool stop = false;
        bool knew = false;
        while (true)
        {
            int ans = _getch();
            if (ans == 'q' || ans == 'Q' || ans == 27) { stop = true; break; }
            if (ans == '1') { knew = true; break; }
            if (ans == '2') { knew = false; break; }
        }
        if (stop) break;

        // ---- Cap nhat Leitner va thong ke ----
        leitner.addCard(LeitnerCard(w, w->getLeitnerBox()));
        leitner.updateResult(w, knew);

        if (knew) remembered++;
        else forgot++;
    }

    int done = remembered + forgot;
    clearScreenFC();
    printTitle("KET QUA THE GHI NHO");
    cout << "\n";
    cout << "So the da lat: " << done << "/" << total << "\n";
    cout << "Nho: " << remembered << "\n";
    cout << "Chua nho: " << forgot << "\n";
    if (done > 0)
        cout << "Ty le nho: " << fixed << setprecision(1)
             << (double)remembered / done * 100.0 << "%\n";
    cout << "\nNhan phim bat ky de quay lai menu...";
    _getch();
}
