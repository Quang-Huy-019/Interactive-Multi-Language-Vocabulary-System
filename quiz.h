#ifndef QUIZ_H
#define QUIZ_H

#include <iostream>
#include <vector>
#include <string>

using namespace std;

// =========================
// CLASS WORD
// =========================
class Word {
private:
    string english;
    string vietnamese;

public:
    Word();
    Word(string en, string vi);

    string getEnglish() const;
    string getVietnamese() const;
};


// =========================
// CLASS QUESTION
// =========================
class Question {
private:
    Word word;
    vector<string> options;
    int correctAnswer;

public:
    Question(Word w, vector<string> opts, int correct);

    void showQuestion(int number, int total) const;
    bool checkAnswer(char answer) const;

    string getCorrectAnswer() const;
    string getWord() const;
};


// =========================
// CLASS QUIZ SESSION
// =========================
class QuizSession {
private:
    vector<Word> wordBank;
    vector<Question> questions;
    vector<string> wrongWords;

    int currentQuestion;
    int score;

    void shuffleWords();
    void createQuestions(int numberOfQuestions);
    void showResult();

public:
    QuizSession(vector<Word> words);

    void startQuiz();
};

#endif