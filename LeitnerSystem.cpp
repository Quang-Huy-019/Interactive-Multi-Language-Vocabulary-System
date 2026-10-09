#include "LeitnerSystem.h"

#include <algorithm>

LeitnerCard::LeitnerCard(Word* vocabularyWord, int initialLevel)
    : word(vocabularyWord),
      level(std::max(LeitnerSystem::MIN_LEVEL,
                     std::min(initialLevel, LeitnerSystem::MAX_LEVEL))),
      correctCount(0),
      wrongCount(0)
{
}

void LeitnerSystem::addCard(const LeitnerCard& card)
{
    if (card.word == nullptr)
        return;

    cards[card.word->getEnglish()] = card;
    cards[card.word->getEnglish()].level = std::max(
        MIN_LEVEL,
        std::min(cards[card.word->getEnglish()].level, MAX_LEVEL)
    );

    card.word->setLeitnerBox(cards[card.word->getEnglish()].level);
}

bool LeitnerSystem::updateResult(Word* word, bool remembered)
{
    if (word == nullptr)
        return false;

    auto it = cards.find(word->getEnglish());

    if (it == cards.end())
        return false;

    LeitnerCard& card = it->second;

    if (remembered)
    {
        ++card.correctCount;
        card.level = std::min(card.level + 1, MAX_LEVEL);
    }
    else
    {
        ++card.wrongCount;
        card.level = std::max(card.level - 1, MIN_LEVEL);
    }

    word->setLeitnerBox(card.level);

    return true;
}

const LeitnerCard* LeitnerSystem::getCard(const Word* word) const
{
    if (word == nullptr)
        return nullptr;

    auto it = cards.find(word->getEnglish());
    return it == cards.end() ? nullptr : &it->second;
}

std::vector<LeitnerCard> LeitnerSystem::getCardsAtLevel(int level) const
{
    std::vector<LeitnerCard> result;

    for (const auto& entry : cards)
    {
        if (entry.second.level == level)
            result.push_back(entry.second);
    }

    return result;
}

std::vector<LeitnerCard> LeitnerSystem::getWeakCards(int maximumLevel) const
{
    std::vector<LeitnerCard> result;

    for (const auto& entry : cards)
    {
        if (entry.second.level <= maximumLevel)
            result.push_back(entry.second);
    }

    return result;
}
