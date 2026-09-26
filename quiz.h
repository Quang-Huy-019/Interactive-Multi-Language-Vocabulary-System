#ifndef QUIZ_H
#define QUIZ_H

#include <string>
#include <vector>
#include <windows.h>
#include "Word.h"

struct QuizWord {
    std::string english;
    std::string vietnamese;
    QuizWord() = default;
    QuizWord(std::string en, std::string vi) : english(en), vietnamese(vi) {}
    std::string getEnglish() const { return english; }
    std::string getVietnamese() const { return vietnamese; }
};


// ============================================================
// QUIZ VIEW OF A WORD (adapted from the dictionary's Word hierarchy)
// ============================================================

// ============================================================
// CLASS QUESTION
// ============================================================

class Question
{
private:

    QuizWord word;

    std::vector<std::string> options;

    int correctAnswer;


public:

    Question(
        QuizWord w,
        std::vector<std::string> opts,
        int correct
    );


    void showQuestion(
        int number,
        int total
    ) const;


    bool checkAnswer(
        char answer
    ) const;


    std::string getCorrectAnswer() const;

    std::string getWord() const;

    std::string getOption(
        int index
    ) const;

    int getOptionsCount() const;

    int getCorrectAnswerIndex() const;
};


// ============================================================
// QUIZ HISTORY (ITEM LEVEL)
// ============================================================

struct QuizHistory
{
    std::string word;

    char userAnswer;

    std::string correctAnswer;

    bool answered;

    bool isCorrect;
};


// ============================================================
// QUIZ ATTEMPT (FILE RECORD LEVEL)
// ============================================================

struct QuizAttempt
{
    int id;

    std::string date;

    int totalQuestions;

    int correct;

    int wrong;

    int unanswered;

    std::string scoreStr;

    std::string timeUsed;

    std::string fullText;
};


// ============================================================
// QUIZ HISTORY MANAGER
// ============================================================

class QuizHistoryManager
{
public:

    static const std::string HISTORY_FILE;

    static std::string getCurrentDateTime();

    static std::string formatTimeUsed(
        unsigned long long totalSeconds
    );

    static std::vector<QuizAttempt> loadAllAttempts(
        const std::string& filename = HISTORY_FILE
    );

    static void saveAttempt(
        const std::string& formattedQuiz,
        const std::string& filename = HISTORY_FILE
    );

    static std::string formatQuizAttempt(
        int quizNumber,
        const std::string& dateStr,
        int totalQuestions,
        int correct,
        int wrong,
        int unanswered,
        const std::string& timeUsed,
        const std::vector<QuizHistory>& history,
        const std::vector<Question>& questions
    );

    static void showHistoryMenu();

    static void viewAllAttempts();

    static void viewLatestAttempt();
};


// ============================================================
// CLASS QUIZ SESSION
// ============================================================

class QuizSession
{
private:

    std::vector<QuizWord> wordBank;

    std::vector<Question> questions;

    std::vector<std::string> wrongWords;

    std::vector<QuizHistory> history;

    int currentQuestion;

    int score;

    ULONGLONG quizStartTime;

    ULONGLONG quizDurationMs;


    void shuffleWords();

    void createQuestions(
        int numberOfQuestions
    );

    void showResult();

    bool allQuestionsAnswered() const;

    void displayCurrentQuestion();

    void displayUnansweredQuestions() const;

    bool showSubmitConfirmation();

    void runQuiz();


public:

    QuizSession(
        std::vector<QuizWord> words
    );

    void startQuiz();
};

#endif
