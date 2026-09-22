#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "Word.h"
#include <map>

class Dictionary {
private:
    map<string, Word*> dictionaryMap;

public:
    Dictionary() {}
    ~Dictionary() {
        
    }

    bool addWord(Word* word);
    Word* searchWord(string englishKey);
    bool deleteWord(string englishKey);
};

#endif
