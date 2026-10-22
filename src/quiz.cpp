
#include "ui.h"
#include "quiz.h"
#include "LeitnerSystem.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <conio.h>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <set>
#include <sstream>
#include <windows.h>

using namespace std;


// Bo sinh so ngau nhien dung chung cho ca phien chay
static mt19937& rng()
{
    static mt19937 gen(static_cast<unsigned>(time(nullptr)));
    return gen;
}

// ============================================================
// XOA MAN HINH TERMINAL
// ============================================================
void clearTerminal()
{
    // Flush output truoc khi xoa de tranh dong con nam trong buffer cout
    // bi in ra sau khi man hinh da duoc don.
    cout.flush();
    system("cls");
}

// ============================================================
// BO DEM THOI GIAN
// ============================================================
static ULONGLONG quizEndTime = 0;

static int getUnansweredCount(const vector<QuizHistory>& history)
{
    int count = 0;
    for (const QuizHistory& item : history)
        if (!item.answered) count++;
    return count;
}

// ============================================================
// HIEN THI THOI GIAN
// ============================================================
static void displayTimer(int unansweredCount)
{
    (void)unansweredCount;
    if (quizEndTime == 0)
        return;

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
        return;

    int width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int leftPosition = width - 35;
    if (leftPosition < 0)
        leftPosition = 0;

    COORD position;
    position.X = (SHORT)leftPosition;
    position.Y = 1;
    SetConsoleCursorPosition(hConsole, position);
    cout << "==============================";

    position.Y++;
    SetConsoleCursorPosition(hConsole, position);
    cout << "          THOI GIAN";

    position.Y++;
    SetConsoleCursorPosition(hConsole, position);

    DWORD now = GetTickCount();
    ULONGLONG remainingMs = (now < quizEndTime) ? quizEndTime - now : 0;
    unsigned long long totalSeconds = (remainingMs + 999) / 1000;
    unsigned long long minutes = totalSeconds / 60;
    unsigned long long seconds = totalSeconds % 60;

    cout << "          "
         << setfill('0') << setw(2) << minutes
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
// CAU HOI
// ============================================================
Question::Question(QuizWord w, vector<string> opts, int correct)
{
    word = w;
    options = opts;
    correctAnswer = correct;
}

void Question::showQuestion(int number, int total) const
{
    cout << "========================================\n";
    cout << "               CAU HOI " << number << "/" << total << "\n";
    cout << "========================================\n\n";
    cout << "Nghia cua tu:\n\n";
    cout << "  " << word.getEnglish() << "\n";
    cout << "\n";

    for (int i = 0; i < (int)options.size(); i++)
    {
        cout << "  " << char('A' + i) << ". " << options[i] << "\n";
    }
    cout << "\n";
}

bool Question::checkAnswer(char answer) const
{
    answer = (char)toupper((unsigned char)answer);
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
    if (index >= 0 && index < (int)options.size())
        return options[index];
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
// PHIEN THI
// ============================================================
QuizSession::QuizSession(vector<QuizWord> words, Dictionary* dictionary)
    : dict(dictionary),
      wordBank(words),
      currentQuestion(0),
      score(0),
      quizStartTime(0),
      quizDurationMs(0)
{
}

void QuizSession::shuffleWords()
{
    shuffle(wordBank.begin(), wordBank.end(), rng());
}

// Moi cau hoi: 1 dap an dung + 3 dap an sai lay tu cac tu KHAC nghia dung
void QuizSession::createQuestions(int numberOfQuestions)
{
    questions.clear();

    for (int i = 0; i < numberOfQuestions; i++)
    {
        QuizWord currentWord = wordBank[i];

        vector<int> candidates;
        for (int j = 0; j < (int)wordBank.size(); j++)
            if (wordBank[j].getVietnamese() != currentWord.getVietnamese())
                candidates.push_back(j);

        shuffle(candidates.begin(), candidates.end(), rng());

        vector<string> options;
        options.push_back(currentWord.getVietnamese());
        for (int j = 0; j < 3 && j < (int)candidates.size(); j++)
            options.push_back(wordBank[candidates[j]].getVietnamese());

        shuffle(options.begin(), options.end(), rng());

        int correctIndex = 0;
        for (int j = 0; j < (int)options.size(); j++)
        {
            if (options[j] == currentWord.getVietnamese())
            {
                correctIndex = j;
                break;
            }
        }

        questions.push_back(Question(currentWord, options, correctIndex));
    }
}

bool QuizSession::allQuestionsAnswered() const
{
    for (const QuizHistory& item : history)
        if (!item.answered) return false;
    return true;
}

void QuizSession::displayUnansweredQuestions() const
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
        return;

    int width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int leftPosition = width - 35;
    if (leftPosition < 0)
        leftPosition = 0;

    COORD position;
    position.X = (SHORT)leftPosition;
    position.Y = 7;
    SetConsoleCursorPosition(hConsole, position);
    cout << "==============================";

    position.Y++;
    SetConsoleCursorPosition(hConsole, position);
    cout << "          DAP AN";

    position.Y++;
    SetConsoleCursorPosition(hConsole, position);
    cout << "==============================";

    for (int i = 0; i < (int)history.size(); i++)
    {
        position.Y++;
        SetConsoleCursorPosition(hConsole, position);
        cout << "  Cau " << i + 1 << ":";
        if (history[i].answered)
            cout << " " << history[i].userAnswer;
        else
            cout << " ";
    }

    position.Y++;
    SetConsoleCursorPosition(hConsole, position);
    cout << "==============================";

    position.X = 0;
    position.Y += 2;
    SetConsoleCursorPosition(hConsole, position);
}

void QuizSession::displayCurrentQuestion()
{
    questions[currentQuestion].showQuestion(currentQuestion + 1, (int)questions.size());
    cout << "\n";

    if (history[currentQuestion].answered)
        cout << "Cau tra loi cua ban: " << history[currentQuestion].userAnswer << "\n";
    else
        cout << "Cau tra loi cua ban: ---\n";

    cout << "\n----------------------------------------\n";
    cout << "LEFT / RIGHT : Di chuyen cau hoi\n";
    cout << "A / B / C / D : Chon dap an\n";
    cout << "ENTER : Nop bai\n";
    cout << "ESC : Ve menu (khong tinh diem)\n";
    cout << "----------------------------------------\n";

    displayTimer(getUnansweredCount(history));
    displayUnansweredQuestions();

    if (currentQuestion == (int)questions.size() - 1 && history[currentQuestion].answered)
    {
        cout << "\n\n========================================\n";
        cout << "                NOP BAI\n";
        cout << "========================================\n";
        cout << "\nNhan ENTER de nop bai...";
    }
}

bool QuizSession::showSubmitConfirmation()
{
    clearTerminal();
    cout << "========================================\n";
    cout << "                NOP BAI\n";
    cout << "========================================\n\n";

    if (!allQuestionsAnswered())
    {
        cout << "CANH BAO!\n";
        cout << "Ban van con cau hoi chua lam.\n\n";
        displayUnansweredQuestions();
        cout << "\n";
    }

    cout << "Ban co chac muon nop bai?\n\n";
    cout << "[1] CO\n";
    cout << "[2] KHONG\n\n";
    cout << "Lua chon: ";

    int key = _getch();
    return key == '1';
}

// ============================================================
// CHAY BAI THI
// ============================================================
// Hoi xac nhan truoc khi bo bai lam; het gio thi tra ve false de runQuiz tu chot ket qua
static bool confirmLeaveQuiz()
{
    clearTerminal();
    printTitle("XAC NHAN VE MENU?");
    cout << "\nBai thi hien tai se KHONG duoc tinh diem.\n\n";
    printKey("Y", "Dong y, ve menu");
    printKey("N", "Khong, tiep tuc lam bai");
    cout << "\nLua chon: ";

    while (true)
    {
        if (GetTickCount() >= quizEndTime) return false;
        if (!_kbhit()) { Sleep(100); continue; }
        int k = _getch();
        if (k == 'y' || k == 'Y') { cout << "Y\n"; return true; }
        if (k == 'n' || k == 'N' || k == 27) return false;
    }
}

void QuizSession::runQuiz()
{
    while (true)
    {
        clearTerminal();
        displayCurrentQuestion();

        while (true)
        {
            ULONGLONG now = GetTickCount();

            if (now >= quizEndTime)
            {
                quizEndTime = 0;
                clearTerminal();
                showResult();
                cout << "\n\nNhan ENTER de quay lai menu...";
                while (_getch() != 13) {}
                return;
            }

            if (_kbhit())
            {
                int key = _getch();

                if (key == 27) // ESC: ve menu (co xac nhan)
                {
                    if (confirmLeaveQuiz())
                    {
                        quizEndTime = 0;
                        return;
                    }
                    break;
                }

                if (key == 0 || key == 224) // phim mui ten
                {
                    int arrow = _getch();
                    if (arrow == 75 && currentQuestion > 0) currentQuestion--;
                    if (arrow == 77 && currentQuestion < (int)questions.size() - 1) currentQuestion++;
                    break;
                }

                if (key >= 'a' && key <= 'd') key = toupper(key);

                if (key >= 'A' && key <= 'D')
                {
                    bool ok = questions[currentQuestion].checkAnswer((char)key);
                    history[currentQuestion].userAnswer = (char)key;
                    history[currentQuestion].answered = true;
                    history[currentQuestion].isCorrect = ok;

                    if (currentQuestion < (int)questions.size() - 1)
                        currentQuestion++;
                    break;
                }

                if (key == 13) // ENTER: nop bai o bat ky cau nao
                {
                    if (allQuestionsAnswered())
                    {
                        quizEndTime = 0;
                        clearTerminal();
                        showResult();
                        cout << "\n\nNhan ENTER de quay lai menu...";
                        while (_getch() != 13) {}
                        return;
                    }

                    clearTerminal();
                    cout << "========================================\n";
                    cout << "                NOP BAI\n";
                    cout << "========================================\n\n";
                    cout << "CANH BAO!\n";
                    cout << "Ban van con cau hoi chua lam.\n\n";

                    displayTimer(getUnansweredCount(history));
                    displayUnansweredQuestions();

                    cout << "\nBan co chac muon nop bai?\n\n";
                    cout << "[1] CO\n";
                    cout << "[2] KHONG\n\n";
                    cout << "Lua chon: ";

                    int confirm = 0;
                    while (true)
                    {
                        if (GetTickCount() >= quizEndTime)
                        {
                            quizEndTime = 0;
                            clearTerminal();
                            showResult();
                            cout << "\n\nNhan ENTER de quay lai menu...";
                            while (_getch() != 13) {}
                            return;
                        }

                        if (_kbhit())
                        {
                            confirm = _getch();
                            break;
                        }
                        displayTimer(getUnansweredCount(history));
                        Sleep(100);
                    }

                    if (confirm == '1')
                    {
                        quizEndTime = 0;
                        clearTerminal();
                        showResult();
                        cout << "\n\nNhan ENTER de quay lai menu...";
                        while (_getch() != 13) {}
                        return;
                    }
                    break;
                }
            }

            displayTimer(getUnansweredCount(history));
            Sleep(100);
        }
    }
}

// ============================================================
// BAT DAU BAI THI
// ============================================================
void QuizSession::startQuiz(int numberOfQuestions)
{
    clearTerminal();

    // Moi lan bat dau la mot luot quiz moi, khong dung lai trang thai luot truoc.
    questions.clear();
    history.clear();
    wrongWords.clear();
    currentQuestion = 0;
    score = 0;
    quizStartTime = 0;
    quizDurationMs = 0;
    quizEndTime = 0;

    // Kiem tra du lieu: can it nhat 4 nghia tieng Viet khac nhau de tao 4 phuong an
    set<string> distinctMeanings;
    for (const QuizWord& w : wordBank) distinctMeanings.insert(w.getVietnamese());
    if (distinctMeanings.size() < 4)
    {
        cout << "[!] Can it nhat 4 nghia tieng Viet khac nhau de tao cau hoi trac nghiem.\n";
        cout << "Hien co: " << distinctMeanings.size() << ". Hay them tu truoc!\n\n";
        cout << "Nhan phim bat ky de quay lai menu...";
        _getch();
        return;
    }

    if (numberOfQuestions <= 0)
    {
        cout << "========================================\n";
        cout << "             BAT DAU BAI THI\n";
        cout << "========================================\n\n";
        cout << "Nhap so cau (1-" << wordBank.size() << ", 0 de quay lai): ";

        while (!(cin >> numberOfQuestions) || numberOfQuestions < 0 || numberOfQuestions > (int)wordBank.size())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "[!] So khong hop le. Nhap lai: ";
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (numberOfQuestions == 0) return;
    }

    if (numberOfQuestions > (int)wordBank.size())
        numberOfQuestions = (int)wordBank.size();

    // Reset lan nua sau khi nhap so cau, ngay truoc khi tao de moi.
    questions.clear();
    history.clear();
    wrongWords.clear();
    currentQuestion = 0;
    score = 0;
    shuffleWords();
    createQuestions(numberOfQuestions);

    for (int i = 0; i < numberOfQuestions; i++)
    {
        QuizHistory item;
        item.word = questions[i].getWord();
        item.userAnswer = '-';
        item.correctAnswer = questions[i].getCorrectAnswer();
        item.answered = false;
        item.isCorrect = false;
        history.push_back(item);
    }

    quizDurationMs = (ULONGLONG)numberOfQuestions * 90ULL * 1000ULL;
    quizEndTime = GetTickCount() + quizDurationMs;
    quizStartTime = GetTickCount();

    clearTerminal();
    runQuiz();
}

// ============================================================
// HIEN THI KET QUA
// ============================================================
void QuizSession::showResult()
{
    ULONGLONG now = GetTickCount();
    ULONGLONG elapsedMs = (now > quizStartTime) ? (now - quizStartTime) : 0;
    if (quizDurationMs > 0 && elapsedMs > quizDurationMs)
        elapsedMs = quizDurationMs;
    unsigned long long elapsedSeconds = elapsedMs / 1000ULL;

    score = 0;
    wrongWords.clear();
    int wrongCount = 0;
    int unansweredCount = 0;

    // Cap nhat Leitner: dung -> len box, sai/bo trong -> xuong box
    LeitnerSystem leitner;
    for (int i = 0; i < (int)history.size(); i++)
    {
        Word* originalWord = dict->searchEnglish(history[i].word);
        bool remembered = history[i].answered && history[i].isCorrect;

        if (remembered)
            score++;
        else if (history[i].answered)
            wrongCount++;
        else
            unansweredCount++;

        if (!remembered)
            wrongWords.push_back(history[i].word);

        if (originalWord)
        {
            leitner.addCard(LeitnerCard(originalWord, originalWord->getLeitnerBox()));
            leitner.updateResult(originalWord, remembered);
        }
    }

    cout << "========================================\n";
    cout << "               KET QUA\n";
    cout << "========================================\n\n";
    cout << "Diem: " << score << "/" << history.size() << "\n\n";

    cout << "========================================\n";
    cout << "          CAC CAU TRA LOI DUNG\n";
    cout << "========================================\n\n";

    bool hasCorrect = false;
    for (int i = 0; i < (int)history.size(); i++)
    {
        if (history[i].answered && history[i].isCorrect)
        {
            hasCorrect = true;
            cout << "Cau " << i + 1 << ": " << history[i].word << "\n";
            cout << "  Dap an cua ban: " << history[i].userAnswer << " - " << history[i].correctAnswer << "\n\n";
        }
    }
    if (!hasCorrect)
        cout << "Khong co cau tra loi dung.\n\n";

    cout << "========================================\n";
    cout << "           CAC CAU TRA LOI SAI\n";
    cout << "========================================\n\n";

    bool hasWrong = false;
    for (int i = 0; i < (int)history.size(); i++)
    {
        if (!history[i].answered || !history[i].isCorrect)
        {
            hasWrong = true;
            cout << "Cau " << i + 1 << ": Nghia cua tu " << questions[i].getWord() << "\n\n";

            for (int j = 0; j < questions[i].getOptionsCount(); j++)
                cout << "  " << char('A' + j) << ". " << questions[i].getOption(j) << "\n";
            cout << "\n";

            if (history[i].answered)
            {
                cout << "  Dap an cua ban: " << history[i].userAnswer;
                int selectedIndex = history[i].userAnswer - 'A';
                cout << " - " << questions[i].getOption(selectedIndex) << "\n";
            }
            else
            {
                cout << "  Dap an cua ban: --- (Chua tra loi)\n";
            }

            cout << "  Dap an dung: " << char('A' + questions[i].getCorrectAnswerIndex())
                 << " - " << history[i].correctAnswer << "\n\n";
        }
    }
    if (!hasWrong)
        cout << "Khong co cau tra loi sai.\n\n";

    cout << "========================================\n";
    cout << "                TONG KET\n";
    cout << "========================================\n\n";

    double percentage = (double)score / history.size() * 100.0;
    cout << fixed << setprecision(1);
    cout << "So cau dung: " << score << "\n";
    cout << "So cau sai / chua tra loi: " << history.size() - score << "\n";
    cout << "Ti le dung: " << percentage << "%\n\n";

    if (percentage >= 90) cout << "Xuat sac!\n";
    else if (percentage >= 80) cout << "Rat tot!\n";
    else if (percentage >= 65) cout << "Tot!\n";
    else if (percentage >= 50) cout << "Hay tiep tuc luyen tap!\n";
    else cout << "Ban can luyen tap them!\n";

    cout << "\n========================================\n";
    cout << "           TU VUNG CAN ON TAP\n";
    cout << "========================================\n\n";

    if (wrongWords.empty())
    {
        cout << "Tuyet voi! Ban da tra loi dung tat ca!\n";
    }
    else
    {
        for (const string& w : wrongWords)
            cout << "- " << w << "\n";
    }

    string dateTime = QuizHistoryManager::getCurrentDateTime();
    string timeUsed = QuizHistoryManager::formatTimeUsed(elapsedSeconds);
    vector<QuizAttempt> existingAttempts = QuizHistoryManager::loadAllAttempts(QuizHistoryManager::HISTORY_FILE);
    int nextQuizNumber = (int)existingAttempts.size() + 1;

    string formattedQuiz = QuizHistoryManager::formatQuizAttempt(
        nextQuizNumber, dateTime, (int)history.size(), score, wrongCount, unansweredCount,
        timeUsed, history, questions
    );

    QuizHistoryManager::saveAttempt(formattedQuiz, QuizHistoryManager::HISTORY_FILE);

    cout << "\n========================================\n";
    cout << "Da luu ket qua vao: " << QuizHistoryManager::HISTORY_FILE << "\n";
    cout << "========================================\n";
}

// ============================================================
// QUAN LY LICH SU BAI THI
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
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%02llu:%02llu", minutes, seconds);
    return string(buffer);
}

string QuizHistoryManager::formatQuizAttempt(
    int quizNumber, const string& dateStr, int totalQuestions, int correct,
    int wrong, int unanswered, const string& timeUsed,
    const vector<QuizHistory>& history, const vector<Question>& questions)
{
    stringstream ss;
    ss << "============================================================\n";
    ss << "                    QUIZ #" << setfill('0') << setw(3) << quizNumber << setfill(' ') << "\n";
    ss << "============================================================\n\n";
    ss << "Date: " << dateStr << "\n";
    ss << "Total Questions: " << totalQuestions << "\n";
    ss << "Correct: " << correct << "\n";
    ss << "Wrong: " << wrong << "\n";
    ss << "Unanswered: " << unanswered << "\n";

    int percentage = 0;
    if (totalQuestions > 0)
        percentage = (int)round((double)correct / (double)totalQuestions * 100.0);
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
            ss << "Correct answer: " << char('A' + questions[i].getCorrectAnswerIndex()) << "\n\n";
        }
    }
    if (!hasCorrect) ss << "No correct answers.\n\n";

    ss << "--------------------- WRONG ANSWERS ----------------------\n\n";
    bool hasWrong = false;
    bool isFirstWrong = true;
    for (int i = 0; i < (int)history.size(); i++)
    {
        if (!history[i].answered || !history[i].isCorrect)
        {
            hasWrong = true;
            if (!isFirstWrong) ss << "----------------------------------------------------------\n\n";
            isFirstWrong = false;

            ss << "[" << (i + 1) << "] " << questions[i].getWord() << "\n\n";
            ss << "Question:\nWhat does \"" << questions[i].getWord() << "\" mean?\n\n";

            for (int j = 0; j < questions[i].getOptionsCount(); j++)
                ss << char('A' + j) << ". " << questions[i].getOption(j) << "\n";
            ss << "\n";

            if (history[i].answered) ss << "Your answer: " << history[i].userAnswer << "\n";
            else ss << "Your answer: (Not answered)\n";

            ss << "Correct answer: " << char('A' + questions[i].getCorrectAnswerIndex()) << "\n\n";
        }
    }
    if (!hasWrong) ss << "No wrong answers.\n\n";

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
    if (!inFile.is_open()) return attempts;

    string line;
    QuizAttempt current;
    stringstream currentText;
    bool insideQuiz = false;
    string prevBorder;

    while (getline(inFile, line))
    {
        if (!line.empty() && line.back() == '\r') line.pop_back();

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
                prevBorder.clear();
            }
            currentText << line << "\n";

            size_t pos = line.find("QUIZ #");
            int qNum = 0;
            try { qNum = stoi(line.substr(pos + 6)); }
            catch (...) { qNum = (int)attempts.size() + 1; }

            current.id = qNum;
            continue;
        }

        if (insideQuiz)
        {
            currentText << line << "\n";
            if (line.rfind("Date: ", 0) == 0) current.date = line.substr(6);
            else if (line.rfind("Total Questions: ", 0) == 0) try { current.totalQuestions = stoi(line.substr(17)); } catch (...) {}
            else if (line.rfind("Correct: ", 0) == 0) try { current.correct = stoi(line.substr(9)); } catch (...) {}
            else if (line.rfind("Wrong: ", 0) == 0) try { current.wrong = stoi(line.substr(7)); } catch (...) {}
            else if (line.rfind("Unanswered: ", 0) == 0) try { current.unanswered = stoi(line.substr(12)); } catch (...) {}
            else if (line.rfind("Score: ", 0) == 0) current.scoreStr = line.substr(7);
            else if (line.rfind("Time Used: ", 0) == 0) current.timeUsed = line.substr(11);
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
        cout << "================ LICH SU BAI THI ================\n\n";
        cout << "1. Xem tat ca bai thi\n";
        cout << "2. Xem bai thi gan nhat\n";
        cout << "3. Quay lai\n\n";
        cout << "==================================================\n\n";
        cout << "Chon 1, 2 hoac 3: ";

        char choice = (char)_getch();
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
        case 27:
            return;
        default:
            cout << "\n\nLua chon khong hop le!";
            cout << "\nNhan phim bat ky de tiep tuc...";
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
        cout << "================ DANH SACH BAI THI ================\n\n";

        if (attempts.empty())
        {
            cout << "Chua co lich su bai thi.\n\n";
            cout << "====================================================\n\n";
            cout << "Nhan phim bat ky de quay lai...";
            _getch();
            return;
        }

        cout << left << setw(5) << "#" << setw(21) << "Ngay va gio" << "Diem" << "\n";
        cout << "----------------------------------------------------\n";

        for (const QuizAttempt& item : attempts)
        {
            string scoreDisplay = to_string(item.correct) + "/" + to_string(item.totalQuestions);
            cout << left << setw(5) << item.id << setw(21) << item.date << scoreDisplay << "\n";
        }

        cout << "\nNhap so thu tu bai thi de xem chi tiet\n";
        cout << "Nhap back de quay lai\n\n";
        cout << "====================================================\n";
        cout << "Nhap lua chon: ";

        string input;
        getline(cin, input);

        while (!input.empty() && isspace((unsigned char)input.front())) input.erase(input.begin());
        while (!input.empty() && isspace((unsigned char)input.back())) input.pop_back();

        if (input == "exit" || input == "back") return;
        if (input.empty()) continue;

        bool isNumber = true;
        for (char c : input)
            if (!isdigit((unsigned char)c)) { isNumber = false; break; }

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
                cout << "\nNhan phim bat ky de quay lai danh sach...";
                _getch();
            }
            else
            {
                cout << "\nKhong tim thay bai thi #" << selectedId << "!";
                cout << "\nNhan phim bat ky de tiep tuc...";
                _getch();
            }
        }
        else
        {
            cout << "\nDu lieu khong hop le!";
            cout << "\nNhan phim bat ky de tiep tuc...";
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
        cout << "================ BAI THI GAN NHAT ================\n\n";
        cout << "Chua co lich su bai thi.\n\n";
        cout << "===================================================\n\n";
        cout << "Nhan phim bat ky de quay lai...";
        _getch();
        return;
    }

    const QuizAttempt& latest = attempts.back();
    cout << latest.fullText;
    cout << "\nNhan phim bat ky de quay lai menu...";
    _getch();
}
