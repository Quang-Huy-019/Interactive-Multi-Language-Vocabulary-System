#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "Word.h"
#include <map>
#include <string>

using namespace std;

class Dictionary {
private:
    map<string, Word*> dictionaryMap;

public:
    Dictionary() {}   // Kh?i t?o r?ng
    ~Dictionary();    // CH? KHAI BÁO Ð?U HÀM ? ÐÂY (d?u ;)

    bool addWord(Word* word);
    Word* searchWord(string englishKey);
    bool deleteWord(string englishKey);
};

#endif
