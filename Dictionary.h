#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "Word.h"
#include <map>
#include <string>
#include <vector>

class Dictionary {
private:
    std::map<std::string, Word*> dictionaryMap;

public:
    // Temporary startup data until the vocabulary persistence layer is connected.
    Dictionary() {
        
        addWord(new GeneralWord("?????", "???", "adjective"));
        addWord(new GeneralWord("beautiful", "dep", "adjective"));
        addWord(new GeneralWord("difficult", "kho", "adjective"));
        addWord(new GeneralWord("important", "quan trong", "adjective"));
        addWord(new GeneralWord("successful", "thanh cong", "adjective"));
        addWord(new GeneralWord("interesting", "thu vi", "adjective"));
        addWord(new GeneralWord("expensive", "dat", "adjective"));
        addWord(new GeneralWord("cheap", "re", "adjective"));
        addWord(new GeneralWord("dangerous", "nguy hiem", "adjective"));
        addWord(new GeneralWord("comfortable", "thoai mai", "adjective"));
        addWord(new GeneralWord("intelligent", "thong minh", "adjective"));
        addWord(new GeneralWord("friendly", "than thien", "adjective"));
        addWord(new GeneralWord("necessary", "can thiet", "adjective"));
        addWord(new GeneralWord("possible", "co the", "adjective"));
        addWord(new GeneralWord("different", "khac nhau", "adjective"));
        addWord(new GeneralWord("problem", "van de", "noun"));
    }

    ~Dictionary() {
        for (auto& entry : dictionaryMap) delete entry.second;
    }

    bool addWord(Word* word) {
        if (!word || dictionaryMap.count(word->getEnglish())) return false;
        dictionaryMap[word->getEnglish()] = word;
        return true;
    }

    Word* searchWord(const std::string& englishKey) const {
        auto it = dictionaryMap.find(englishKey);
        return it == dictionaryMap.end() ? nullptr : it->second;
    }

    bool deleteWord(const std::string& englishKey) {
        auto it = dictionaryMap.find(englishKey);
        if (it == dictionaryMap.end()) return false;
        delete it->second;
        dictionaryMap.erase(it);
        return true;
    }

    std::vector<Word*> getAllWords() const {
        std::vector<Word*> words;
        for (const auto& entry : dictionaryMap) words.push_back(entry.second);
        return words;
    }
};

#endif
