#include "quiz.h"

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <cctype>
#include <iomanip>

using namespace std;


// ============================================================
// CLASS WORD
// ============================================================

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


// ============================================================
// CLASS QUESTION
// ============================================================

Question::Question(
    Word w,
    vector<string> opts,
    int correct
) {
    word = w;
    options = opts;
    correctAnswer = correct;
}


void Question::showQuestion(
    int number,
    int total
) const {

    cout << "\n========================================\n";

    cout << "Question "
         << number
         << "/"
         << total
         << endl;

    cout << "========================================\n";

    cout << "What does \""
         << word.getEnglish()
         << "\" mean?\n\n";


    for (int i = 0; i < options.size(); i++) {

        cout << char('A' + i)
             << ". "
             << options[i]
             << endl;
    }

    cout << "\nYour answer: ";
}


bool Question::checkAnswer(char answer) const {

    answer = toupper(answer);

    int choice = answer - 'A';

    return choice == correctAnswer;
}


string Question::getCorrectAnswer() const {

    return options[correctAnswer];
}


string Question::getWord() const {

    return word.getEnglish();
}


// ============================================================
// CLASS QUIZ SESSION
// ============================================================

QuizSession::QuizSession(
    vector<Word> words
) {

    wordBank = words;

    currentQuestion = 0;

    score = 0;
}


// ============================================================
// RANDOMIZE WORDS
// ============================================================

void QuizSession::shuffleWords() {

    random_device rd;

    mt19937 g(rd());

    shuffle(
        wordBank.begin(),
        wordBank.end(),
        g
    );
}


// ============================================================
// CREATE QUESTIONS
// ============================================================

void QuizSession::createQuestions(
    int numberOfQuestions
) {

    questions.clear();

    // Random danh sách từ
    shuffleWords();


    // Nếu số câu hỏi lớn hơn số từ
    if (numberOfQuestions > wordBank.size()) {

        numberOfQuestions = wordBank.size();
    }


    // Tạo từng câu hỏi
    for (int i = 0;
         i < numberOfQuestions;
         i++) {

        // Từ hiện tại
        Word currentWord = wordBank[i];


        // Danh sách đáp án
        vector<string> options;


        // ----------------------------------------------------
        // ĐÁP ÁN ĐÚNG
        // ----------------------------------------------------

        options.push_back(
            currentWord.getVietnamese()
        );


        // ----------------------------------------------------
        // TÌM CÁC ĐÁP ÁN SAI
        // ----------------------------------------------------

        vector<int> indexes;


        for (int j = 0;
             j < wordBank.size();
             j++) {

            // Không lấy chính từ đang hỏi
            if (j != i) {

                indexes.push_back(j);
            }
        }


        // Random
        random_device rd;

        mt19937 g(rd());

        shuffle(
            indexes.begin(),
            indexes.end(),
            g
        );


        // Lấy tối đa 3 đáp án sai
        int wrongCount =
            min(
                3,
                (int)indexes.size()
            );


        for (int j = 0;
             j < wrongCount;
             j++) {

            options.push_back(
                wordBank[indexes[j]]
                    .getVietnamese()
            );
        }


        // ----------------------------------------------------
        // RANDOM VỊ TRÍ ĐÁP ÁN
        // ----------------------------------------------------

        shuffle(
            options.begin(),
            options.end(),
            g
        );


        // ----------------------------------------------------
        // TÌM VỊ TRÍ ĐÁP ÁN ĐÚNG
        // ----------------------------------------------------

        int correctIndex = 0;


        for (int j = 0;
             j < options.size();
             j++) {

            if (
                options[j]
                == currentWord.getVietnamese()
            ) {

                correctIndex = j;

                break;
            }
        }


        // ----------------------------------------------------
        // TẠO QUESTION
        // ----------------------------------------------------

        Question q(
            currentWord,
            options,
            correctIndex
        );


        questions.push_back(q);
    }
}


// ============================================================
// START QUIZ
// ============================================================

void QuizSession::startQuiz() {

    // --------------------------------------------------------
    // KIỂM TRA SỐ LƯỢNG TỪ
    // --------------------------------------------------------

    if (wordBank.size() < 4) {

        cout << "\n";
        cout << "Khong du tu vung de tao Quiz!\n";

        return;
    }


    int numberOfQuestions;


    // --------------------------------------------------------
    // HEADER
    // --------------------------------------------------------

    cout << "\n========================================\n";

    cout << "           START QUIZ\n";

    cout << "========================================\n";


    cout << "So cau hoi (1 - "
         << wordBank.size()
         << "): ";


    cin >> numberOfQuestions;


    // --------------------------------------------------------
    // KIỂM TRA SỐ CÂU HỎI
    // --------------------------------------------------------

    if (
        numberOfQuestions <= 0
        ||
        numberOfQuestions > wordBank.size()
    ) {

        cout << "\n";
        cout << "So luong cau hoi khong hop le!\n";

        return;
    }


    // --------------------------------------------------------
    // RESET DỮ LIỆU
    // --------------------------------------------------------

    score = 0;

    currentQuestion = 0;

    wrongWords.clear();


    // --------------------------------------------------------
    // TẠO CÂU HỎI
    // --------------------------------------------------------

    createQuestions(
        numberOfQuestions
    );


    // --------------------------------------------------------
    // LÀM QUIZ
    // --------------------------------------------------------

    for (
        int i = 0;
        i < questions.size();
        i++
    ) {

        currentQuestion = i;


        // Hiển thị câu hỏi
        questions[i].showQuestion(
            i + 1,
            questions.size()
        );


        // Nhập đáp án
        char answer;

        cin >> answer;


        // ----------------------------------------------------
        // KIỂM TRA ĐÁP ÁN
        // ----------------------------------------------------

        if (
            questions[i]
                .checkAnswer(answer)
        ) {

            cout << "\n";
            cout << "CORRECT!\n";


            score++;
        }

        else {

            cout << "\n";
            cout << "WRONG!\n";


            cout << "Correct answer: "
                 << questions[i]
                        .getCorrectAnswer()
                 << endl;


            // Lưu từ trả lời sai
            wrongWords.push_back(
                questions[i].getWord()
            );
        }
    }


    // --------------------------------------------------------
    // HIỂN THỊ KẾT QUẢ
    // --------------------------------------------------------

    showResult();
}


// ============================================================
// SHOW RESULT
// ============================================================

void QuizSession::showResult() {

    int total = questions.size();

    int wrong = total - score;


    // --------------------------------------------------------
    // TÍNH ĐỘ CHÍNH XÁC
    // --------------------------------------------------------

    double accuracy =
        (double)score
        / total
        * 100;


    // --------------------------------------------------------
    // HEADER
    // --------------------------------------------------------

    cout << "\n\n";

    cout << "========================================\n";

    cout << "              QUIZ RESULT\n";

    cout << "========================================\n";


    // --------------------------------------------------------
    // THỐNG KÊ
    // --------------------------------------------------------

    cout << "Total questions : "
         << total
         << endl;


    cout << "Correct         : "
         << score
         << endl;


    cout << "Wrong           : "
         << wrong
         << endl;


    cout << fixed
         << setprecision(2);


    cout << "Accuracy        : "
         << accuracy
         << "%\n";


    cout << "Score           : "
         << score
         << "/"
         << total
         << endl;


    // --------------------------------------------------------
    // XẾP LOẠI
    // --------------------------------------------------------

    cout << "\nRating: ";


    if (accuracy >= 90) {

        cout << "Excellent!";
    }

    else if (accuracy >= 80) {

        cout << "Very Good!";
    }

    else if (accuracy >= 65) {

        cout << "Good!";
    }

    else if (accuracy >= 50) {

        cout << "Need More Practice!";
    }

    else {

        cout << "Keep Practicing!";
    }


    cout << endl;


    // --------------------------------------------------------
    // DANH SÁCH TỪ SAI
    // --------------------------------------------------------

    if (!wrongWords.empty()) {

        cout << "\n----------------------------------------\n";

        cout << "           WORDS TO REVIEW\n";

        cout << "----------------------------------------\n";


        for (
            int i = 0;
            i < wrongWords.size();
            i++
        ) {

            cout << i + 1
                 << ". "
                 << wrongWords[i]
                 << endl;
        }
    }

    else {

        cout << "\n";
        cout << "Perfect! No weak words.\n";
    }


    cout << "========================================\n";
}