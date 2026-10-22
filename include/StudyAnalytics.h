#ifndef STUDYANALYTICS_H
#define STUDYANALYTICS_H

#include <vector>
#include "Word.h"

// Thong ke doc truc tiep tu so lan dung/sai ben vung luu trong tung Word
// (duoc luu vao data.txt nen tich luy qua cac phien)
class StudyAnalytics {
public:
    static double overallAccuracy(const std::vector<Word*>& words);
    static void showLeitnerChart(const std::vector<Word*>& words);
    static void showWeakWords(const std::vector<Word*>& words); // tu Box 1-2
    static void showDashboard(const std::vector<Word*>& words);
};

#endif
