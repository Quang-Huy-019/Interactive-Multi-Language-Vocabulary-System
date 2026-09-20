#include "quiz.h"

#include <iostream>
#include <cstdlib>
#include <limits>
#include <random>
#include <algorithm>
#include <iomanip>
#include <cctype>
#include <conio.h>
#include <windows.h>

using namespace std;


// ============================================================
// CLEAR TERMINAL
// ============================================================

void clearTerminal()
{
    system("cls");
}


// ============================================================
// WORD
// ============================================================

Word::Word()
{
    english = "";
    vietnamese = "";
}


Word::Word(string en, string vi)
{
    english = en;
    vietnamese = vi;
}


string Word::getEnglish() const
{
    return english;
}


string Word::getVietnamese() const
{
    return vietnamese;
}


// ============================================================
// QUESTION
// ============================================================

Question::Question(
    Word w,
    vector<string> opts,
    int correct
)
{
    word = w;
    options = opts;
    correctAnswer = correct;
}


void Question::showQuestion(
    int number,
    int total
) const
{
    cout << "========================================\n";

    cout << "              QUESTION "
         << number
         << "/"
         << total
         << "\n";

    cout << "========================================\n\n";


    cout << "What is the meaning of:\n\n";

    cout << "  " << word.getEnglish() << "\n\n";


    for (int i = 0; i < (int)options.size(); i++)
    {
        cout << "  "
             << char('A' + i)
             << ". "
             << options[i]
             << "\n";
    }

    cout << "\n";
}


bool Question::checkAnswer(char answer) const
{
    answer = toupper(answer);

    int index = answer - 'A';

    return index == correctAnswer;
}


string Question::getCorrectAnswer() const
{
    return options[correctAnswer];
}


string Question::getWord() const
{
    return word.getEnglish();
}


string Question::getOption(int index) const
{
    if (index >= 0 &&
        index < (int)options.size())
    {
        return options[index];
    }

    return "";
}


// ============================================================
// QUIZ SESSION CONSTRUCTOR
// ============================================================

QuizSession::QuizSession(vector<Word> words)
{
    wordBank = words;

    currentQuestion = 0;

    score = 0;
}


// ============================================================
// SHUFFLE WORDS
// ============================================================

void QuizSession::shuffleWords()
{
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

void QuizSession::createQuestions(int numberOfQuestions)
{
    questions.clear();


    for (int i = 0;
         i < numberOfQuestions;
         i++)
    {
        Word currentWord = wordBank[i];


        vector<string> options;


        // Correct answer
        options.push_back(
            currentWord.getVietnamese()
        );


        // ================================================
        // CREATE WRONG ANSWERS
        // ================================================

        vector<int> indexes;


        for (int j = 0;
             j < (int)wordBank.size();
             j++)
        {
            if (j != i)
            {
                indexes.push_back(j);
            }
        }


        random_device rd;

        mt19937 g(rd());


        shuffle(
            indexes.begin(),
            indexes.end(),
            g
        );


        for (int j = 0; j < 3; j++)
        {
            options.push_back(
                wordBank[indexes[j]].getVietnamese()
            );
        }


        // ================================================
        // SHUFFLE OPTIONS
        // ================================================

        shuffle(
            options.begin(),
            options.end(),
            g
        );


        int correctIndex = 0;


        for (int j = 0;
             j < (int)options.size();
             j++)
        {
            if (options[j] ==
                currentWord.getVietnamese())
            {
                correctIndex = j;

                break;
            }
        }


        questions.push_back(
            Question(
                currentWord,
                options,
                correctIndex
            )
        );
    }
}


// ============================================================
// CHECK ALL QUESTIONS ANSWERED
// ============================================================

bool QuizSession::allQuestionsAnswered() const
{
    for (const QuizHistory& item : history)
    {
        if (!item.answered)
        {
            return false;
        }
    }

    return true;
}


// ============================================================
// DISPLAY UNANSWERED QUESTIONS
// ============================================================

void QuizSession::displayUnansweredQuestions() const
{
    HANDLE hConsole =
        GetStdHandle(STD_OUTPUT_HANDLE);


    CONSOLE_SCREEN_BUFFER_INFO csbi;


    GetConsoleScreenBufferInfo(
        hConsole,
        &csbi
    );


    int width =
        csbi.srWindow.Right -
        csbi.srWindow.Left +
        1;


    int leftPosition = width - 35;


    if (leftPosition < 0)
    {
        leftPosition = 0;
    }


    COORD position;


    position.X = leftPosition;

    position.Y = 5;


    SetConsoleCursorPosition(
        hConsole,
        position
    );


    cout << "==============================";


    position.Y++;


    SetConsoleCursorPosition(
        hConsole,
        position
    );


    cout << "        CHUA LAM";


    position.Y++;


    SetConsoleCursorPosition(
        hConsole,
        position
    );


    cout << "==============================";


    int count = 0;


    for (int i = 0;
         i < (int)history.size();
         i++)
    {
        if (!history[i].answered)
        {
            position.Y++;


            SetConsoleCursorPosition(
                hConsole,
                position
            );


            cout << "  Cau "
                 << i + 1;


            count++;
        }
    }


    if (count == 0)
    {
        position.Y++;


        SetConsoleCursorPosition(
            hConsole,
            position
        );


        cout << "  Khong co";
    }


    position.Y++;


    SetConsoleCursorPosition(
        hConsole,
        position
    );


    cout << "==============================";


    position.X = 0;

    position.Y += 2;


    SetConsoleCursorPosition(
        hConsole,
        position
    );
}


// ============================================================
// DISPLAY CURRENT QUESTION
// ============================================================

void QuizSession::displayCurrentQuestion()
{
    questions[currentQuestion].showQuestion(
        currentQuestion + 1,
        questions.size()
    );


    cout << "\n";


    // ================================================
    // CURRENT ANSWER
    // ================================================

    if (history[currentQuestion].answered)
    {
        cout << "Your answer: "
             << history[currentQuestion].userAnswer
             << "\n";
    }
    else
    {
        cout << "Your answer: ---\n";
    }


    cout << "\n";


    // ================================================
    // NAVIGATION
    // ================================================

    cout << "----------------------------------------\n";

    cout << "LEFT / RIGHT : Navigate";

    cout << "\nA / B / C / D : Answer";

    cout << "\nENTER : Submit";

    cout << "\n----------------------------------------\n";


    // ================================================
    // UNANSWERED PANEL
    // ================================================

    displayUnansweredQuestions();


    // ================================================
    // SUBMIT BUTTON
    // ONLY SHOW ON LAST QUESTION AFTER ANSWERING
    // ================================================

    if (currentQuestion ==
        (int)questions.size() - 1 &&
        history[currentQuestion].answered)
    {
        cout << "\n\n";

        cout << "========================================\n";

        cout << "              NOP BAI\n";

        cout << "========================================\n";

        cout << "\nNhan ENTER de nop bai...";
    }
}


// ============================================================
// SUBMIT CONFIRMATION
// ============================================================

bool QuizSession::showSubmitConfirmation()
{
    clearTerminal();


    cout << "========================================\n";

    cout << "              NOP BAI\n";

    cout << "========================================\n\n";


    if (!allQuestionsAnswered())
    {
        cout << "CANH BAO!\n";

        cout << "Ban van con cau hoi chua lam.\n\n";


        displayUnansweredQuestions();


        cout << "\n";
    }


    cout << "Ban co chac muon nop bai khong?\n\n";

    cout << "[1] YES\n";

    cout << "[2] NO\n\n";

    cout << "Lua chon: ";


    int key = _getch();


    if (key == '1')
    {
        return true;
    }


    return false;
}


// ============================================================
// RUN QUIZ
// ============================================================

void QuizSession::runQuiz()
{
    while (true)
    {
        // ====================================================
        // DISPLAY QUESTION
        // ====================================================

        displayCurrentQuestion();


        // ====================================================
        // GET KEY
        // ====================================================

        int key = _getch();


        // ====================================================
        // ARROW KEYS
        // ====================================================

        if (key == 0 || key == 224)
        {
            int arrow = _getch();


            // LEFT
            if (arrow == 75)
            {
                if (currentQuestion > 0)
                {
                    currentQuestion--;

                    clearTerminal();
                }

                continue;
            }


            // RIGHT
            if (arrow == 77)
            {
                if (currentQuestion <
                    (int)questions.size() - 1)
                {
                    currentQuestion++;

                    clearTerminal();
                }

                continue;
            }
        }


        // ====================================================
        // ANSWER A - D
        // ====================================================

        if (key >= 'a' && key <= 'd')
        {
            key = toupper(key);
        }


        if (key >= 'A' && key <= 'D')
        {
            // Save answer
            history[currentQuestion].userAnswer =
                key;


            history[currentQuestion].answered =
                true;


            history[currentQuestion].isCorrect =
                questions[currentQuestion].checkAnswer(
                    key
                );


            // ================================================
            // NOT LAST QUESTION
            // ================================================

            if (currentQuestion <
                (int)questions.size() - 1)
            {
                currentQuestion++;

                clearTerminal();

                continue;
            }


            // ================================================
            // LAST QUESTION
            // ================================================

            // Chỉ xóa màn hình.
            // Không tự in câu hỏi ở đây.
            // Vòng while phía trên sẽ gọi
            // displayCurrentQuestion() đúng 1 lần.

            clearTerminal();

            continue;
        }


        // ====================================================
        // ENTER
        // ====================================================

        if (key == 13)
        {
            // ENTER ONLY AT LAST QUESTION
            if (currentQuestion ==
                (int)questions.size() - 1)
            {
                // ============================================
                // CASE 1: ALL ANSWERED
                // ============================================

                if (allQuestionsAnswered())
                {
                    clearTerminal();


                    showResult();


                    cout << "\n\nNhan ENTER de quay lai menu...";


                    while (_getch() != 13)
                    {
                    }


                    return;
                }


                // ============================================
                // CASE 2: UNANSWERED QUESTIONS
                // ============================================

                clearTerminal();


                cout << "========================================\n";

                cout << "              NOP BAI\n";

                cout << "========================================\n\n";


                cout << "CANH BAO!\n";

                cout << "Ban van con cau hoi chua lam.\n\n";


                displayUnansweredQuestions();


                cout << "\n";


                cout << "Ban co chac muon nop bai khong?\n\n";


                cout << "[1] YES\n";

                cout << "[2] NO\n\n";


                cout << "Lua chon: ";


                int confirm = _getch();


                // =========================================
                // YES
                // =========================================

                if (confirm == '1')
                {
                    clearTerminal();


                    showResult();


                    cout << "\n\nNhan ENTER de quay lai menu...";


                    while (_getch() != 13)
                    {
                    }


                    return;
                }


                // =========================================
                // NO
                // =========================================

                if (confirm == '2')
                {
                    clearTerminal();

                    continue;
                }
            }
        }
    }
}


// ============================================================
// START QUIZ
// ============================================================

void QuizSession::startQuiz()
{
    clearTerminal();


    int numberOfQuestions;


    cout << "========================================\n";

    cout << "              START QUIZ\n";

    cout << "========================================\n\n";


    cout << "Enter number of questions: ";


    while (!(cin >> numberOfQuestions) ||
           numberOfQuestions < 1 ||
           numberOfQuestions > (int)wordBank.size())
    {
        cin.clear();


        cin.ignore(
            numeric_limits<streamsize>::max(),
            '\n'
        );


        cout << "\nInvalid number!\n";

        cout << "Enter again: ";
    }


    cin.ignore(
        numeric_limits<streamsize>::max(),
        '\n'
    );


    // ================================================
    // RESET
    // ================================================

    currentQuestion = 0;

    score = 0;

    wrongWords.clear();


    // ================================================
    // SHUFFLE
    // ================================================

    shuffleWords();


    // ================================================
    // CREATE QUESTIONS
    // ================================================

    createQuestions(
        numberOfQuestions
    );


    // ================================================
    // CREATE HISTORY
    // ================================================

    history.clear();


    for (int i = 0;
         i < numberOfQuestions;
         i++)
    {
        QuizHistory item;


        item.word =
            questions[i].getWord();


        item.userAnswer =
            '-';


        item.correctAnswer =
            questions[i].getCorrectAnswer();


        item.answered =
            false;


        item.isCorrect =
            false;


        history.push_back(item);
    }


    // ================================================
    // CLEAR TERMINAL
    // ================================================

    clearTerminal();


    // ================================================
    // START QUIZ
    // ================================================

    runQuiz();
}


// ============================================================
// SHOW RESULT
// ============================================================

void QuizSession::showResult()
{
    score = 0;

    wrongWords.clear();


    // ================================================
    // CALCULATE SCORE
    // ================================================

    for (int i = 0;
         i < (int)history.size();
         i++)
    {
        if (history[i].answered &&
            history[i].isCorrect)
        {
            score++;
        }
        else
        {
            wrongWords.push_back(
                history[i].word
            );
        }
    }


    // ================================================
    // RESULT HEADER
    // ================================================

    cout << "========================================\n";

    cout << "               RESULT\n";

    cout << "========================================\n\n";


    cout << "Score: "
         << score
         << "/"
         << history.size()
         << "\n\n";


    // ================================================
    // DETAILS
    // ================================================

    cout << "----------------------------------------\n";

    cout << "              DETAILS\n";

    cout << "----------------------------------------\n\n";


    for (int i = 0;
         i < (int)history.size();
         i++)
    {
        cout << "Question "
             << i + 1
             << ": ";


        if (history[i].answered)
        {
            cout << history[i].userAnswer;


            if (history[i].isCorrect)
            {
                cout << " - CORRECT";
            }
            else
            {
                cout << " - WRONG";

                cout << " | Correct: "
                     << history[i].correctAnswer;
            }
        }
        else
        {
            cout << "NOT ANSWERED";

            cout << " | Correct: "
                 << history[i].correctAnswer;
        }


        cout << "\n";
    }


    // ================================================
    // SUMMARY
    // ================================================

    cout << "\n========================================\n";

    cout << "              SUMMARY\n";

    cout << "========================================\n\n";


    double percentage =
        (double)score /
        history.size() *
        100.0;


    cout << fixed
         << setprecision(1);


    cout << "Correct: "
         << score
         << "\n";


    cout << "Wrong / Unanswered: "
         << history.size() - score
         << "\n";


    cout << "Percentage: "
         << percentage
         << "%\n";


    // ================================================
    // RATING
    // ================================================

    cout << "\n";


    if (percentage >= 90)
    {
        cout << "Excellent!\n";
    }
    else if (percentage >= 80)
    {
        cout << "Very Good!\n";
    }
    else if (percentage >= 65)
    {
        cout << "Good!\n";
    }
    else if (percentage >= 50)
    {
        cout << "Keep Practicing!\n";
    }
    else
    {
        cout << "You Need More Practice!\n";
    }


    // ================================================
    // WORDS TO REVIEW
    // ================================================

    cout << "\n========================================\n";

    cout << "           WORDS TO REVIEW\n";

    cout << "========================================\n\n";


    if (wrongWords.empty())
    {
        cout << "Great! You got everything correct!\n";
    }
    else
    {
        for (int i = 0;
             i < (int)wrongWords.size();
             i++)
        {
            cout << "- "
                 << wrongWords[i]
                 << "\n";
        }
    }
}