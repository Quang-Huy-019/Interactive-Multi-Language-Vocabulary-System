#ifndef QUIZ_H
#define QUIZ_H

#include <string>
#include <vector>


// ============================================================
// CLASS WORD
// ============================================================

class Word
{
private:

    std::string english;
    std::string vietnamese;


public:

    Word();

    Word(
        std::string en,
        std::string vi
    );


    std::string getEnglish() const;

    std::string getVietnamese() const;
};


// ============================================================
// CLASS QUESTION
// ============================================================

class Question
{
private:

    Word word;

    std::vector<std::string> options;

    int correctAnswer;


public:

    Question(
        Word w,
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
};


// ============================================================
// QUIZ HISTORY
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
// CLASS QUIZ SESSION
// ============================================================

class QuizSession
{
private:

    std::vector<Word> wordBank;

    std::vector<Question> questions;

    std::vector<std::string> wrongWords;

    std::vector<QuizHistory> history;

    int currentQuestion;

    int score;


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
        std::vector<Word> words
    );

    void startQuiz();
};

#endif