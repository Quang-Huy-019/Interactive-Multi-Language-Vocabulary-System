#ifndef STUDYANALYTICS_H
#define STUDYANALYTICS_H

#include <string>
#include <vector>

// Thông tin thong kê tung tu
struct WordStat {
    std::string word;
    std::string meaning;
    int box;     // Box Leitner (1 - 5)
    int correct; // So lan dung
    int wrong;   // So lan sai

    WordStat(std::string w = "", std::string m = "", int b = 1);

    int totalAttempts() const;
    double getAccuracy() const;
};

// Class quan lý phân tích hoc tap
class StudyAnalytics {
private:
    std::vector<WordStat> wordList;

public:
    void addWord(std::string word, std::string meaning, int box = 1);
    void updateResult(std::string word, bool isCorrect);

    double getOverallAccuracy() const;
    void showWeakWords() const;
    void showLeitnerChart() const;
    void showDashboard() const;
};

#endif
