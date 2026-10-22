#ifndef LEITNER_SYSTEM_H
#define LEITNER_SYSTEM_H

#include "Word.h"
#include <map>
#include <string>
#include <vector>

// Luu thong ke hoc cua mot tu (dung trong 1 lan lam bai)
struct LeitnerCard {
    Word* word;
    int level;
    int correctCount;
    int wrongCount;

    LeitnerCard(Word* vocabularyWord = nullptr, int initialLevel = 1);
};

class LeitnerSystem {
private:
    std::map<std::string, LeitnerCard> cards;

public:
    static const int MIN_LEVEL = 1;
    static const int MAX_LEVEL = 5;

    void addCard(const LeitnerCard& card);
    bool updateResult(Word* word, bool remembered);
    const LeitnerCard* getCard(const Word* word) const;
    std::vector<LeitnerCard> getCardsAtLevel(int level) const;
    std::vector<LeitnerCard> getWeakCards(int maximumLevel = 2) const;
};

#endif
