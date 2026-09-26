#ifndef WORD_H
#define WORD_H

#include <iostream>
#include <string>
#include <vector>

using namespace std;

// 1. L?p co s? tr?u tu?ng
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
    string getVietnamese() const { return vietnamese; }
    int getLeitnerBox() const { return leitnerBox; }
    void setLeitnerBox(int box) { leitnerBox = box; }

    virtual void displayDetails() const = 0; // Hàm ?o thu?n túy
};

// 2. L?p T? thông d?ng
class GeneralWord : public Word {
private:
    string partOfSpeech;
public:
    GeneralWord(string en, string vi, string pos) : Word(en, vi), partOfSpeech(pos) {}
    void displayDetails() const override {
        cout << "Word: " << english << " (" << partOfSpeech << ") - " << vietnamese << "\n";
    }
};

// 3. L?p Thu?t ng? chuyên ngành (B? sung thêm)
class TechnicalTerm : public Word {
private:
    string field;
public:
    TechnicalTerm(string en, string vi, string f) : Word(en, vi), field(f) {}
    void displayDetails() const override {
        cout << "Technical Term: " << english << " [" << field << "] - " << vietnamese << "\n";
    }
};

// 4. L?p Thành ng? (B? sung thêm)
class Idiom : public Word {
private:
    string origin;
public:
    Idiom(string en, string vi, string orig) : Word(en, vi), origin(orig) {}
    void displayDetails() const override {
        cout << "Idiom: " << english << " (Origin: " << origin << ") - " << vietnamese << "\n";
    }
};

#endif
