
#include <iostream>
#include <vector>
#include "quiz.h"

using namespace std;

int main() {

    vector<Word> words = {

        Word("beautiful", "dep"),
        Word("difficult", "kho"),
        Word("important", "quan trong"),
        Word("successful", "thanh cong"),
        Word("interesting", "thu vi"),
        Word("expensive", "dat"),
        Word("cheap", "re"),
        Word("dangerous", "nguy hiem"),
        Word("comfortable", "thoai mai"),
        Word("intelligent", "thong minh"),
        Word("friendly", "than thien"),
        Word("necessary", "can thiet"),
        Word("possible", "co the"),
        Word("different", "khac nhau"),
        Word("problem", "van de")
    };

    QuizSession quiz(words);

    int choice;

    do {

        cout << "\n\n";
        cout << "========================================\n";
        cout << "       VOCABULARY QUIZ SYSTEM\n";
        cout << "========================================\n";

        cout << "1. Start Quiz\n";
        cout << "2. Exit\n";

        cout << "\nYour choice: ";

        cin >> choice;

        switch (choice) {

            case 1:
                quiz.startQuiz();
                break;

            case 2:
                cout << "\nThank you for using the system!\n";
                break;

            default:
                cout << "\nInvalid choice!\n";
        }

    } while (choice != 2);

    return 0;
}

