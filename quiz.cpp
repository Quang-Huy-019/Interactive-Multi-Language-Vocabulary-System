#include "quiz.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <limits>
#include <random>
#include <algorithm>
#include <iomanip>
#include <cctype>
#include <ctime>
#include <cmath>
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
// TIMER
// ============================================================

static ULONGLONG quizEndTime = 0;

static int getUnansweredCount(const vector<QuizHistory>& history)
{
    int count = 0;

    for (const QuizHistory& item : history)
    {
        if (!item.answered)
            count++;
    }

    return count;
}


// ============================================================
// DISPLAY TIMER
// ============================================================

static void displayTimer(int unansweredCount)
{
    if (quizEndTime == 0)
        return;

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
        return;

    int width =
        csbi.srWindow.Right -
        csbi.srWindow.Left + 1;

    int leftPosition = width - 35;

    if (leftPosition < 0)
        leftPosition = 0;

    // Timer nằm phía trên bảng đáp án
    int y = 1;

    COORD position;

    position.X = (SHORT)leftPosition;
    position.Y = (SHORT)y;

    SetConsoleCursorPosition(hConsole, position);

    cout << "==============================";

    position.Y++;

    SetConsoleCursorPosition(hConsole, position);

    cout << "            TIME";

    position.Y++;

    SetConsoleCursorPosition(hConsole, position);

    ULONGLONG now = GetTickCount64();

    ULONGLONG remainingMs =
        (now < quizEndTime)
        ? quizEndTime - now
        : 0;

    unsigned long long totalSeconds =
        (remainingMs + 999) / 1000;

    unsigned long long minutes =
        totalSeconds / 60;

    unsigned long long seconds =
        totalSeconds % 60;

    cout << "          "
         << setfill('0')
         << setw(2) << minutes
         << ":"
         << setw(2) << seconds
         << setfill(' ');

    position.Y++;

    SetConsoleCursorPosition(hConsole, position);

    cout << "==============================";

    position.X = 0;
    position.Y = 0;

    SetConsoleCursorPosition(hConsole, position);
}


// ============================================================
// WORD
// ============================================================

// ============================================================
// QUESTION
// ============================================================

Question::Question(
    QuizWord w,
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

    cout << "  "
         << word.getEnglish()
         << "\n\n";

    for (int i = 0;
         i < (int)options.size();
         i++)
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


int Question::getOptionsCount() const
{
    return (int)options.size();
}


int Question::getCorrectAnswerIndex() const
{
    return correctAnswer;
}



// ============================================================
// QUIZ SESSION CONSTRUCTOR
// ============================================================

QuizSession::QuizSession(vector<QuizWord> words)
{
    wordBank = words;

    currentQuestion = 0;

    score = 0;

    quizStartTime = 0;

    quizDurationMs = 0;
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
        QuizWord currentWord = wordBank[i];

        vector<string> options;

        // Đáp án đúng
        options.push_back(
            currentWord.getVietnamese()
        );


        // Tạo danh sách đáp án sai
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


        // Lấy 3 đáp án sai
        for (int j = 0; j < 3; j++)
        {
            options.push_back(
                wordBank[indexes[j]]
                    .getVietnamese()
            );
        }


        // Random vị trí đáp án
        shuffle(
            options.begin(),
            options.end(),
            g
        );


        // Tìm vị trí đáp án đúng
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
            return false;
    }

    return true;
}


// ============================================================
// DISPLAY ANSWER STATUS
// ============================================================

void QuizSession::displayUnansweredQuestions() const
{
    HANDLE hConsole =
        GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if (!GetConsoleScreenBufferInfo(
            hConsole,
            &csbi))
    {
        return;
    }

    int width =
        csbi.srWindow.Right -
        csbi.srWindow.Left + 1;

    int leftPosition = width - 35;

    if (leftPosition < 0)
        leftPosition = 0;


    COORD position;

    // Bảng bắt đầu từ dòng 7
    position.X = (SHORT)leftPosition;
    position.Y = 7;

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

    cout << "          DAP AN";

    position.Y++;

    SetConsoleCursorPosition(
        hConsole,
        position
    );

    cout << "==============================";


    // Hiện tất cả câu
    for (int i = 0;
         i < (int)history.size();
         i++)
    {
        position.Y++;

        SetConsoleCursorPosition(
            hConsole,
            position
        );

        cout << "  Cau "
             << i + 1
             << ":";

        if (history[i].answered)
        {
            cout << " "
                 << history[i].userAnswer;
        }
        else
        {
            cout << " ";
        }
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


    // Câu trả lời hiện tại
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


    // Navigation
    cout << "----------------------------------------\n";

    cout << "LEFT / RIGHT : Navigate\n";

    cout << "A / B / C / D : Answer\n";

    cout << "ENTER : Submit\n";

    cout << "ESC : Exit Quiz\n";

    cout << "----------------------------------------\n";


    // Timer
    displayTimer(
        getUnansweredCount(history)
    );


    // Bảng đáp án
    displayUnansweredQuestions();


    // Submit ở câu cuối
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


    cout << "Ban co chac muon nop bai?\n\n";

    cout << "[1] YES\n";

    cout << "[2] NO\n\n";

    cout << "Lua chon: ";


    int key = _getch();

    if (key == '1')
        return true;

    return false;
}


// ============================================================
// RUN QUIZ
// ============================================================

void QuizSession::runQuiz()
{
    while (true)
    {
        clearTerminal();

        displayCurrentQuestion();


        while (true)
        {
            // =================================================
            // TIME UP
            // =================================================

            ULONGLONG now =
                GetTickCount64();

            if (now >= quizEndTime)
            {
                quizEndTime = 0;

                clearTerminal();

                showResult();

                cout << "\n\nNhan ENTER de quay lai menu...";

                while (_getch() != 13)
                {
                }

                return;
            }


            // =================================================
            // KEYBOARD
            // =================================================

            if (_kbhit())
            {
                int key = _getch();


                // =================================================
                // ESC - EXIT QUIZ
                // =================================================

                if (key == 27)
                {
                    quizEndTime = 0;

                    clearTerminal();

                    cout << "========================================\n";

                    cout << "              EXIT QUIZ\n";

                    cout << "========================================\n\n";

                    cout << "Ban da thoat khoi bai quiz.\n";

                    cout << "Ket qua bai nay se khong duoc tinh.\n\n";

                    cout << "Nhan ENTER de quay lai menu...";

                    while (_getch() != 13)
                    {
                    }

                    return;
                }


                // =================================================
                // ARROW KEYS
                // =================================================

                if (key == 0 || key == 224)
                {
                    int arrow = _getch();


                    // LEFT
                    if (arrow == 75)
                    {
                        if (currentQuestion > 0)
                        {
                            currentQuestion--;
                        }

                        break;
                    }


                    // RIGHT
                    if (arrow == 77)
                    {
                        if (currentQuestion <
                            (int)questions.size() - 1)
                        {
                            currentQuestion++;
                        }

                        break;
                    }


                    continue;
                }


                // =================================================
                // ANSWER A-D
                // =================================================

                if (key >= 'a' &&
                    key <= 'd')
                {
                    key = toupper(key);
                }


                if (key >= 'A' &&
                    key <= 'D')
                {
                    history[currentQuestion].userAnswer =
                        key;

                    history[currentQuestion].answered =
                        true;

                    history[currentQuestion].isCorrect =
                        questions[currentQuestion]
                            .checkAnswer(key);


                    // Tự sang câu tiếp theo
                    if (currentQuestion <
                        (int)questions.size() - 1)
                    {
                        currentQuestion++;

                        break;
                    }


                    // Nếu là câu cuối
                    break;
                }


                // =================================================
                // ENTER
                // =================================================

                if (key == 13)
                {
                    // Chỉ submit ở câu cuối
                    if (currentQuestion ==
                        (int)questions.size() - 1)
                    {
                        // -----------------------------------------
                        // TẤT CẢ ĐÃ LÀM
                        // -----------------------------------------

                        if (allQuestionsAnswered())
                        {
                            quizEndTime = 0;

                            clearTerminal();

                            showResult();

                            cout << "\n\nNhan ENTER de quay lai menu...";

                            while (_getch() != 13)
                            {
                            }

                            return;
                        }


                        // -----------------------------------------
                        // CÒN CÂU CHƯA LÀM
                        // -----------------------------------------

                        clearTerminal();

                        cout << "========================================\n";

                        cout << "              NOP BAI\n";

                        cout << "========================================\n\n";

                        cout << "CANH BAO!\n";

                        cout << "Ban van con cau hoi chua lam.\n\n";


                        // Hiện timer
                        displayTimer(
                            getUnansweredCount(history)
                        );


                        // Hiện bảng đáp án
                        displayUnansweredQuestions();


                        cout << "\n";

                        cout << "Ban co chac muon nop bai?\n\n";

                        cout << "[1] YES\n";

                        cout << "[2] NO\n\n";

                        cout << "Lua chon: ";


                        int confirm = 0;


                        // Chờ xác nhận nhưng timer vẫn chạy
                        while (true)
                        {
                            if (GetTickCount64() >=
                                quizEndTime)
                            {
                                quizEndTime = 0;

                                clearTerminal();

                                showResult();

                                cout << "\n\nNhan ENTER de quay lai menu...";

                                while (_getch() != 13)
                                {
                                }

                                return;
                            }


                            if (_kbhit())
                            {
                                confirm = _getch();

                                break;
                            }


                            displayTimer(
                                getUnansweredCount(history)
                            );

                            Sleep(100);
                        }


                        // YES
                        if (confirm == '1')
                        {
                            quizEndTime = 0;

                            clearTerminal();

                            showResult();

                            cout << "\n\nNhan ENTER de quay lai menu...";

                            while (_getch() != 13)
                            {
                            }

                            return;
                        }


                        // NO
                        if (confirm == '2')
                        {
                            break;
                        }
                    }
                }
            }


            // Update timer
            displayTimer(
                getUnansweredCount(history)
            );

            Sleep(100);
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
           numberOfQuestions >
               (int)wordBank.size())
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


    // =========================================================
    // RESET
    // =========================================================

    currentQuestion = 0;

    score = 0;

    wrongWords.clear();


    // =========================================================
    // SHUFFLE
    // =========================================================

    shuffleWords();


    // =========================================================
    // CREATE QUESTIONS
    // =========================================================

    createQuestions(
        numberOfQuestions
    );


    // =========================================================
    // CREATE HISTORY
    // =========================================================

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


    // =========================================================
    // START TIMER
    // =========================================================

    // 1 câu = 90 giây
    quizDurationMs =
        (ULONGLONG)numberOfQuestions *
        90ULL *
        1000ULL;

    quizEndTime =
        GetTickCount64() +
        quizDurationMs;

    quizStartTime =
        GetTickCount64();


    clearTerminal();

    runQuiz();
}


// ============================================================
// SHOW RESULT
// ============================================================

void QuizSession::showResult()
{
    ULONGLONG now = GetTickCount64();
    ULONGLONG elapsedMs = (now > quizStartTime) ? (now - quizStartTime) : 0;
    if (quizDurationMs > 0 && elapsedMs > quizDurationMs)
    {
        elapsedMs = quizDurationMs;
    }
    unsigned long long elapsedSeconds = elapsedMs / 1000ULL;

    score = 0;

    wrongWords.clear();

    int wrongCount = 0;
    int unansweredCount = 0;


    // =========================================================
    // CALCULATE SCORE
    // =========================================================

    for (int i = 0;
         i < (int)history.size();
         i++)
    {
        if (history[i].answered &&
            history[i].isCorrect)
        {
            score++;
        }
        else if (history[i].answered &&
                 !history[i].isCorrect)
        {
            wrongCount++;
            wrongWords.push_back(
                history[i].word
            );
        }
        else
        {
            unansweredCount++;
            wrongWords.push_back(
                history[i].word
            );
        }
    }


    // =========================================================
    // RESULT HEADER
    // =========================================================

    cout << "========================================\n";

    cout << "               RESULT\n";

    cout << "========================================\n\n";


    cout << "Score: "
         << score
         << "/"
         << history.size()
         << "\n\n";


    // =========================================================
    // CORRECT ANSWERS
    // =========================================================

    cout << "========================================\n";

    cout << "          CORRECT ANSWERS\n";

    cout << "========================================\n\n";


    bool hasCorrect = false;


    for (int i = 0;
         i < (int)history.size();
         i++)
    {
        if (history[i].answered &&
            history[i].isCorrect)
        {
            hasCorrect = true;


            cout << "Question "
                 << i + 1
                 << ": "
                 << history[i].word
                 << "\n";


            cout << "  Your answer: "
                 << history[i].userAnswer
                 << " - "
                 << history[i].correctAnswer
                 << "\n\n";
        }
    }


    if (!hasCorrect)
    {
        cout << "No correct answers.\n\n";
    }


    // =========================================================
    // WRONG ANSWERS
    // =========================================================

    cout << "========================================\n";

    cout << "           WRONG ANSWERS\n";

    cout << "========================================\n\n";


    bool hasWrong = false;


    for (int i = 0;
         i < (int)history.size();
         i++)
    {
        if (!history[i].answered ||
            !history[i].isCorrect)
        {
            hasWrong = true;


            // -------------------------------------------------
            // QUESTION
            // -------------------------------------------------

            cout << "Question "
                 << i + 1
                 << ": What is the meaning of: "
                 << questions[i].getWord()
                 << "\n\n";

            for (int j = 0;
                 j < questions[i].getOptionsCount();
                 j++)
            {
                cout << "  "
                     << char('A' + j)
                     << ". "
                     << questions[i].getOption(j)
                     << "\n";
            }

            cout << "\n";


            // -------------------------------------------------
            // USER ANSWER
            // -------------------------------------------------

            if (history[i].answered)
            {
                cout << "  Your answer: "
                     << history[i].userAnswer;


                int selectedIndex =
                    history[i].userAnswer - 'A';


                cout << " - "
                     << questions[i]
                            .getOption(selectedIndex)
                     << "\n";
            }
            else
            {
                cout << "  Your answer: --- "
                     << "(Not answered)\n";
            }


            // -------------------------------------------------
            // CORRECT ANSWER
            // -------------------------------------------------

            int correctIndex =
                questions[i].getCorrectAnswerIndex();


            cout << "  Correct answer: ";


            if (correctIndex >= 0)
            {
                cout << char('A' + correctIndex)
                     << " - ";
            }


            cout << history[i].correctAnswer
                 << "\n\n";
        }
    }


    if (!hasWrong)
    {
        cout << "No wrong answers.\n\n";
    }


    // =========================================================
    // SUMMARY
    // =========================================================

    cout << "========================================\n";

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


    // =========================================================
    // RATING
    // =========================================================

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


    // =========================================================
    // WORDS TO REVIEW
    // =========================================================

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


    // =========================================================
    // SAVE QUIZ HISTORY
    // =========================================================

    string dateTime =
        QuizHistoryManager::getCurrentDateTime();

    string timeUsed =
        QuizHistoryManager::formatTimeUsed(elapsedSeconds);

    vector<QuizAttempt> existingAttempts =
        QuizHistoryManager::loadAllAttempts(QuizHistoryManager::HISTORY_FILE);

    int nextQuizNumber =
        (int)existingAttempts.size() + 1;

    string formattedQuiz =
        QuizHistoryManager::formatQuizAttempt(
            nextQuizNumber,
            dateTime,
            (int)history.size(),
            score,
            wrongCount,
            unansweredCount,
            timeUsed,
            history,
            questions
        );

    QuizHistoryManager::saveAttempt(
        formattedQuiz,
        QuizHistoryManager::HISTORY_FILE
    );

    cout << "\n========================================\n";
    cout << "Da luu ket qua vao: " << QuizHistoryManager::HISTORY_FILE << "\n";
    cout << "========================================\n";
}


// ============================================================
// QUIZ HISTORY MANAGER IMPLEMENTATION
// ============================================================

const string QuizHistoryManager::HISTORY_FILE = "quiz_history.txt";

string QuizHistoryManager::getCurrentDateTime()
{
    time_t now = time(nullptr);
    tm* ltm = localtime(&now);
    char buffer[32];
    if (ltm)
    {
        strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M", ltm);
        return string(buffer);
    }
    return "01/01/2026 00:00";
}

string QuizHistoryManager::formatTimeUsed(unsigned long long totalSeconds)
{
    unsigned long long minutes = totalSeconds / 60ULL;
    unsigned long long seconds = totalSeconds % 60ULL;
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%02llu:%02llu", minutes, seconds);
    return string(buffer);
}

string QuizHistoryManager::formatQuizAttempt(
    int quizNumber,
    const string& dateStr,
    int totalQuestions,
    int correct,
    int wrong,
    int unanswered,
    const string& timeUsed,
    const vector<QuizHistory>& history,
    const vector<Question>& questions
)
{
    stringstream ss;

    ss << "============================================================\n";
    ss << "                    QUIZ #" << setfill('0') << setw(3) << quizNumber << "\n";
    ss << "============================================================\n\n";

    ss << "Date: " << dateStr << "\n";
    ss << "Total Questions: " << totalQuestions << "\n";
    ss << "Correct: " << correct << "\n";
    ss << "Wrong: " << wrong << "\n";
    ss << "Unanswered: " << unanswered << "\n";

    int percentage = 0;
    if (totalQuestions > 0)
    {
        percentage = (int)round((double)correct / (double)totalQuestions * 100.0);
    }
    ss << "Score: " << percentage << "%\n";
    ss << "Time Used: " << timeUsed << "\n\n";

    ss << "-------------------- CORRECT ANSWERS --------------------\n\n";

    bool hasCorrect = false;
    for (int i = 0; i < (int)history.size(); i++)
    {
        if (history[i].answered && history[i].isCorrect)
        {
            hasCorrect = true;
            ss << "[" << (i + 1) << "] " << questions[i].getWord() << "\n";
            ss << "Your answer: " << history[i].userAnswer << "\n";
            char correctChar = 'A' + questions[i].getCorrectAnswerIndex();
            ss << "Correct answer: " << correctChar << "\n\n";
        }
    }

    if (!hasCorrect)
    {
        ss << "No correct answers.\n\n";
    }

    ss << "--------------------- WRONG ANSWERS ----------------------\n\n";

    bool hasWrong = false;
    bool isFirstWrong = true;
    for (int i = 0; i < (int)history.size(); i++)
    {
        if (!history[i].answered || !history[i].isCorrect)
        {
            hasWrong = true;
            if (!isFirstWrong)
            {
                ss << "----------------------------------------------------------\n\n";
            }
            isFirstWrong = false;

            ss << "[" << (i + 1) << "] " << questions[i].getWord() << "\n\n";
            ss << "Question:\n";
            ss << "What does \"" << questions[i].getWord() << "\" mean?\n\n";

            for (int j = 0; j < questions[i].getOptionsCount(); j++)
            {
                ss << char('A' + j) << ". " << questions[i].getOption(j) << "\n";
            }
            ss << "\n";

            if (history[i].answered)
            {
                ss << "Your answer: " << history[i].userAnswer << "\n";
            }
            else
            {
                ss << "Your answer: (Not answered)\n";
            }

            char correctChar = 'A' + questions[i].getCorrectAnswerIndex();
            ss << "Correct answer: " << correctChar << "\n\n";
        }
    }

    if (!hasWrong)
    {
        ss << "No wrong answers.\n\n";
    }

    ss << "---------------------- END OF QUIZ -----------------------\n";

    return ss.str();
}

void QuizHistoryManager::saveAttempt(const string& formattedQuiz, const string& filename)
{
    ofstream outFile(filename, ios::app);
    if (outFile.is_open())
    {
        outFile << formattedQuiz << "\n\n";
        outFile.close();
    }
}

vector<QuizAttempt> QuizHistoryManager::loadAllAttempts(const string& filename)
{
    vector<QuizAttempt> attempts;
    ifstream inFile(filename);
    if (!inFile.is_open())
    {
        return attempts;
    }

    string line;
    QuizAttempt current;
    stringstream currentText;
    bool insideQuiz = false;
    string prevBorder = "";

    while (getline(inFile, line))
    {
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }

        if (line.find("============================================================") != string::npos)
        {
            if (!insideQuiz)
            {
                prevBorder = line;
                continue;
            }
        }

        if (!insideQuiz && line.find("QUIZ #") != string::npos)
        {
            insideQuiz = true;
            current = QuizAttempt();
            currentText.str("");
            currentText.clear();

            if (!prevBorder.empty())
            {
                currentText << prevBorder << "\n";
                prevBorder = "";
            }
            currentText << line << "\n";

            size_t pos = line.find("QUIZ #");
            int qNum = 0;
            try
            {
                qNum = stoi(line.substr(pos + 6));
            }
            catch (...)
            {
                qNum = (int)attempts.size() + 1;
            }
            current.id = qNum;
            continue;
        }

        if (insideQuiz)
        {
            currentText << line << "\n";

            if (line.rfind("Date: ", 0) == 0)
            {
                current.date = line.substr(6);
            }
            else if (line.rfind("Total Questions: ", 0) == 0)
            {
                try { current.totalQuestions = stoi(line.substr(17)); } catch (...) {}
            }
            else if (line.rfind("Correct: ", 0) == 0)
            {
                try { current.correct = stoi(line.substr(9)); } catch (...) {}
            }
            else if (line.rfind("Wrong: ", 0) == 0)
            {
                try { current.wrong = stoi(line.substr(7)); } catch (...) {}
            }
            else if (line.rfind("Unanswered: ", 0) == 0)
            {
                try { current.unanswered = stoi(line.substr(12)); } catch (...) {}
            }
            else if (line.rfind("Score: ", 0) == 0)
            {
                current.scoreStr = line.substr(7);
            }
            else if (line.rfind("Time Used: ", 0) == 0)
            {
                current.timeUsed = line.substr(11);
            }
            else if (line.find("END OF QUIZ") != string::npos)
            {
                current.fullText = currentText.str();
                attempts.push_back(current);
                insideQuiz = false;
                current = QuizAttempt();
                currentText.str("");
                currentText.clear();
            }
        }
    }

    inFile.close();
    return attempts;
}

void QuizHistoryManager::showHistoryMenu()
{
    while (true)
    {
        clearTerminal();

        cout << "================ QUIZ HISTORY ================\n\n";
        cout << "1. View all quiz attempts\n";
        cout << "2. View latest quiz\n";
        cout << "3. Back\n\n";
        cout << "===============================================\n\n";
        cout << "Press 1, 2 or 3: ";

        char choice = _getch();

        switch (choice)
        {
        case '1':
            cout << "1\n";
            viewAllAttempts();
            break;

        case '2':
            cout << "2\n";
            viewLatestAttempt();
            break;

        case '3':
        case 'b':
        case 'B':
        case 27:
            return;

        default:
            cout << "\n\nInvalid choice!";
            cout << "\nPress any key to continue...";
            _getch();
            break;
        }
    }
}

void QuizHistoryManager::viewAllAttempts()
{
    while (true)
    {
        clearTerminal();

        vector<QuizAttempt> attempts = loadAllAttempts(HISTORY_FILE);

        cout << "================ QUIZ ATTEMPTS ================\n\n";

        if (attempts.empty())
        {
            cout << "No quiz attempts found.\n\n";
            cout << "================================================\n\n";
            cout << "Press any key to return...";
            _getch();
            return;
        }

        cout << left << setw(5) << "#"
             << setw(21) << "Date & Time"
             << "Score" << "\n";
        cout << "------------------------------------------------\n";

        for (const QuizAttempt& item : attempts)
        {
            string scoreDisplay = to_string(item.correct) + "/" + to_string(item.totalQuestions);
            cout << left << setw(5) << item.id
                 << setw(21) << item.date
                 << scoreDisplay << "\n";
        }

        cout << "\nEnter quiz number to view details\n";
        cout << "B. Back\n\n";
        cout << "================================================\n";
        cout << "Enter choice: ";

        string input;
        getline(cin, input);

        while (!input.empty() && isspace((unsigned char)input.front())) input.erase(input.begin());
        while (!input.empty() && isspace((unsigned char)input.back())) input.pop_back();

        if (input == "b" || input == "B" || input == "exit" || input == "back")
        {
            return;
        }

        if (input.empty())
        {
            continue;
        }

        bool isNumber = true;
        for (char c : input)
        {
            if (!isdigit((unsigned char)c))
            {
                isNumber = false;
                break;
            }
        }

        if (isNumber)
        {
            int selectedId = stoi(input);
            const QuizAttempt* found = nullptr;
            for (const QuizAttempt& item : attempts)
            {
                if (item.id == selectedId)
                {
                    found = &item;
                    break;
                }
            }

            if (found != nullptr)
            {
                clearTerminal();
                cout << found->fullText;
                cout << "\nPress any key to return to attempts list...";
                _getch();
            }
            else
            {
                cout << "\nQuiz #" << selectedId << " not found!";
                cout << "\nPress any key to continue...";
                _getch();
            }
        }
        else
        {
            cout << "\nInvalid input!";
            cout << "\nPress any key to continue...";
            _getch();
        }
    }
}

void QuizHistoryManager::viewLatestAttempt()
{
    clearTerminal();

    vector<QuizAttempt> attempts = loadAllAttempts(HISTORY_FILE);

    if (attempts.empty())
    {
        cout << "================ LATEST QUIZ ================\n\n";
        cout << "No quiz attempts found.\n\n";
        cout << "==============================================\n\n";
        cout << "Press any key to return...";
        _getch();
        return;
    }

    const QuizAttempt& latest = attempts.back();
    cout << latest.fullText;
    cout << "\nPress any key to return to menu...";
    _getch();
}
