#ifndef WORD_H
#define WORD_H

#include <iostream>
#include <string>
#include <vector>

using namespace std;
	
// Abstract Base Class
class Word {
protected:
    string english;
    string vietnamese;
    vector<string> synonyms;
    vector<string> antonyms;
    int leitnerBox;

public:
    Word(string en, string vi) : english(en), vietnamese(vi), leitnerBox(1) {}
    virtual ~Word() {}

    string getEnglish() const { return english; }
    int getLeitnerBox() const { return leitnerBox; }
    void setLeitnerBox(int box) { leitnerBox = box; }

    virtual void displayDetails() const = 0; // Pure virtual function
};

// Derived Class Example
class GeneralWord : public Word {
private:
    string partOfSpeech;
public:
    GeneralWord(string en, string vi, string pos) : Word(en, vi), partOfSpeech(pos) {}
    void displayDetails() const override {
        cout << "Word: " << english << " (" << partOfSpeech << ") - " << vietnamese << endl;
    }
};

#endif
