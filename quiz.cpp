#include "quiz.h"

#include <iostream>
#include <cstdlib>
#include <limits>
#include <cctype>
#include <random>
#include <algorithm>

#include <iomanip>

using namespace std;


// =====================================================
// CLEAR TERMINAL
// =====================================================

void clearTerminal() {

    system("cls");
}


// =====================================================
// CLASS WORD
// =====================================================

Word::Word() {

    english = "";
    vietnamese = "";
}


Word::Word(string en, string vi) {

    english = en;
    vietnamese = vi;
}


string Word::getEnglish() const {

    return english;
}


string Word::getVietnamese() const {

    return vietnamese;
}


// =====================================================
// CLASS QUESTION
// =====================================================

Question::Question(
    Word w,
    vector<string> opts,
    int correct
) {

    word = w;

    options = opts;

    correctAnswer = correct;
}


// =====================================================
// HIỆN CÂU HỎI
// =====================================================

void Question::showQuestion(
    int number,
    int total
) const {

    cout << "========================================\n";

    cout << "             QUESTION "
         << number
         << "/"
         << total
         << "\n";

    cout << "========================================\n";


    cout << "\nWhat is the meaning of: "
         << word.getEnglish()
         << "?\n\n";


    char letter = 'A';


    for (
        int i = 0;
        i < static_cast<int>(options.size());
        i++
    ) {

        cout << letter
             << ". "
             << options[i]
             << "\n";

        letter++;
    }
}


// =====================================================
// KIỂM TRA ĐÁP ÁN
// =====================================================

bool Question::checkAnswer(
    char answer
) const {

    answer = toupper(answer);


    int selected =
        answer - 'A';


    return selected == correctAnswer;
}


// =====================================================
// LẤY ĐÁP ÁN ĐÚNG
// =====================================================

string Question::getCorrectAnswer() const {

    return options[correctAnswer];
}


// =====================================================
// LẤY TỪ TIẾNG ANH
// =====================================================

string Question::getWord() const {

    return word.getEnglish();
}


// =====================================================
// CLASS QUIZ SESSION
// =====================================================

QuizSession::QuizSession(
    vector<Word> words
) {

    wordBank = words;

    currentQuestion = 0;

    score = 0;
}


// =====================================================
// SHUFFLE WORDS
// =====================================================

void QuizSession::shuffleWords() {

    random_device rd;

    mt19937 g(rd());


    shuffle(
        wordBank.begin(),
        wordBank.end(),
        g
    );
}


// =====================================================
// CREATE QUESTIONS
// =====================================================

void QuizSession::createQuestions(
    int numberOfQuestions
) {

    questions.clear();


    shuffleWords();


    for (
        int i = 0;
        i < numberOfQuestions;
        i++
    ) {

        Word correctWord =
            wordBank[i];



        vector<string> options;


        // =========================
        // ĐÁP ÁN ĐÚNG
        // =========================

        options.push_back(
            correctWord.getVietnamese()
        );


        // =========================
        // CHỌN 3 ĐÁP ÁN SAI
        // =========================

        vector<int> usedIndexes;


        usedIndexes.push_back(i);


        while (options.size() < 4) {

            random_device rd;

            mt19937 g(rd());


            uniform_int_distribution<int> dist(
                0,
                static_cast<int>(
                    wordBank.size()
                ) - 1
            );


            int randomIndex =
                dist(g);


            bool alreadyUsed = false;


            for (
                int index : usedIndexes
            ) {

                if (
                    index == randomIndex
                ) {

                    alreadyUsed = true;

                    break;
                }
            }


            if (!alreadyUsed) {

                options.push_back(
                    wordBank[randomIndex]
                        .getVietnamese()
                );


                usedIndexes.push_back(
                    randomIndex
                );
            }
        }


        // =========================
        // XÁO TRỘN ĐÁP ÁN
        // =========================

        shuffle(
            options.begin(),
            options.end(),
            mt19937(random_device()())
        );


        // =========================
        // TÌM VỊ TRÍ ĐÁP ÁN ĐÚNG
        // =========================

        int correctIndex = 0;


        for (
            int j = 0;
            j < static_cast<int>(
                options.size()
            );
            j++
        ) {

            if (
                options[j]
                ==
                correctWord.getVietnamese()
            ) {

                correctIndex = j;

                break;
            }
        }


        // =========================
        // TẠO QUESTION
        // =========================

        questions.push_back(
            Question(
                correctWord,
                options,
                correctIndex
            )
         );


     }
}


// =====================================================
// START QUIZ
// =====================================================



void QuizSession::startQuiz() {

    int numberOfQuestions;
    // =========================
    // START QUIZ
    // =========================

    cout << "\n========================================\n";

    cout << "          START VOCABULARY QUIZ\n";

    cout << "========================================\n";


    cout << "\nHow many questions do you want? ";


    cin >> numberOfQuestions;


    // =========================
    // KIỂM TRA INPUT
    // =========================

    if (cin.fail()) {

        cin.clear();


        cin.ignore(
            numeric_limits<streamsize>::max(),
            '\n'
        );


        cout << "\nInvalid input!\n";


        return;
    }


    // =========================
    // KIỂM TRA SỐ CÂU
    // =========================

    if (
        numberOfQuestions < 1
        ||
        numberOfQuestions
        >
        static_cast<int>(
            wordBank.size()
        )
    ) {

        cout << "\nInvalid number of questions!\n";


        cout << "Please choose from 1 to "
             << wordBank.size()
             << ".\n";


        return;
    }


    // =================================================
    // CLEAR SAU KHI NHẬP SỐ CÂU
    // =================================================

    clearTerminal();


    // =================================================
    // RESET QUIZ
    // =================================================

    score = 0;

    currentQuestion = 0;

    wrongWords.clear();

    history.clear();


    // =================================================
    // TẠO CÂU HỎI
    // =================================================

    createQuestions(
        numberOfQuestions
    );


    // =================================================
    // BẮT ĐẦU LÀM QUIZ
    // =================================================

    for (
        int i = 0;
        i < numberOfQuestions;
        i++
    ) {

        currentQuestion = i;


        // =============================================
        // HIỆN CÂU HỎI
        // =============================================

        questions[i].showQuestion(
            i + 1,
            numberOfQuestions
        );


        // =============================================
        // NHẬP ĐÁP ÁN
        // =============================================

        char answer;


        cout << "\nYour answer: ";


        cin >> answer;


        answer = toupper(answer);


        // =============================================
        // KIỂM TRA ĐÁP ÁN
        // =============================================

        bool correct =
            questions[i].checkAnswer(
                answer
            );


        // =============================================
        // LƯU LỊCH SỬ
        // =============================================

        QuizHistory result;


        result.word =
            questions[i].getWord();


        result.userAnswer =
            answer;


        result.correctAnswer =
            questions[i].getCorrectAnswer();


        result.isCorrect =
            correct;


        history.push_back(result);


        // =============================================
        // CẬP NHẬT SCORE
        // =============================================

        if (correct) {

            score++;

        }

        else {

            wrongWords.push_back(
                questions[i].getWord()
            );
        }


        // =============================================
        // NẾU CHƯA PHẢI CÂU CUỐI
        // → CLEAR TERMINAL
        // → HIỆN CÂU TIẾP
        // =============================================

        if (
            i < numberOfQuestions - 1
        ) {

            clearTerminal();
        }
    }


    // =================================================
    // LÀM XONG TOÀN BỘ QUIZ
    // CLEAR TERMINAL
    // =================================================

    clearTerminal();


    // =================================================
    // HIỆN RESULT
    // =================================================

    showResult();
}


// =====================================================
// SHOW RESULT
// =====================================================

void QuizSession::showResult() {

    int total =
        static_cast<int>(
            history.size()
        );


    int wrong =
        total - score;


    double accuracy = 0;


    if (total > 0) {

        accuracy =
            static_cast<double>(score)
            /
            total
            *
            100;
    }


    // =================================================
    // QUIZ RESULT
    // =================================================

    cout << "========================================\n";

    cout << "              QUIZ RESULT\n";

    cout << "========================================\n";


    // =================================================
    // TẤT CẢ CÂU ĐÃ LÀM
    // =================================================

    cout << "\n";
    cout << "          QUESTIONS ANSWERED\n";
    cout << "----------------------------------------\n";


    for (
        int i = 0;
        i < static_cast<int>(
            history.size()
        );
        i++
    ) {

        cout << "\nQuestion "
             << i + 1
             << ": "
             << history[i].word
             << "\n";


        cout << "Your answer: "
             << history[i].userAnswer
             << "\n";


        if (
            history[i].isCorrect
        ) {

            cout << "Result: CORRECT\n";

        }
        else {

            cout << "Result: WRONG\n";


            cout << "Correct answer: "
                 << history[i].correctAnswer
                 << "\n";
        }
    }


    // =================================================
    // SCORE
    // =================================================

    cout << "\n========================================\n";

    cout << "                 SCORE\n";

    cout << "========================================\n";


    cout << "\nTotal questions : "
         << total
         << "\n";


    cout << "Correct         : "
         << score
         << "\n";


    cout << "Wrong           : "
         << wrong
         << "\n";


    cout << fixed
         << setprecision(1);


    cout << "Accuracy        : "
         << accuracy
         << "%\n";


    cout << "Score           : "
         << score
         << "/"
         << total
         << "\n";


    // =================================================
    // RATING
    // =================================================

    cout << "\nRating          : ";


    if (accuracy >= 90) {

        cout << "Excellent!";
     }
    else if (accuracy >= 70) {

        cout << "Good!";

     }
    else if (accuracy >= 50) {

        cout << "Keep practicing!";

     }
    else {

        cout << "Need more practice!";
    }


    // =================================================
    // WORDS LEARNED
    // =================================================

    cout << "\n\n========================================\n";

    cout << "             WORDS LEARNED\n";

    cout << "========================================\n";


    for (
        int i = 0;
        i < static_cast<int>(
            history.size()
        );
        i++
    ) {

        cout << i + 1
             << ". "
             << history[i].word
             << "\n";
    }


    // =================================================
    // WORDS TO REVIEW
    // =================================================

    cout << "\n========================================\n";

    cout << "             WORDS TO REVIEW\n";

    cout << "========================================\n";


    if (wrongWords.empty()) {

        cout << "\nNo words to review!\n";

    }
    else {

        for (
            int i = 0;
            i < static_cast<int>(
                wrongWords.size()
            );
            i++
        ) {

            cout << i + 1
                 << ". "
                 << wrongWords[i]
                 << "\n";
        }
    }



    // =================================================
    // QUAY VỀ MAIN MENU
    // =================================================

    cout << "\n========================================\n";

    cout << "\nPress ENTER to return to main menu...";


    // Xóa ký tự '\n' còn lại sau cin >> answer
    cin.ignore(
        numeric_limits<streamsize>::max(),
        '\n'
    );


    // Chờ ENTER
    cin.get();
}