#ifndef WORD_H
#define WORD_H

#include <iostream>
#include <string>
#include <vector>
#include "ui.h"

// ============================================================
// Lop co so truu tuong cho moi tu vung
// ============================================================
class Word {
protected:
    std::string english;
    std::string vietnamese;
    std::vector<std::string> synonyms;
    std::vector<std::string> antonyms;
    std::vector<std::string> examples;
    int leitnerBox;
    int correctCount = 0; // so lan tra loi dung (ben vung)
    int wrongCount = 0;   // so lan tra loi sai (ben vung)

public:
    Word(std::string en, std::string vi) : english(en), vietnamese(vi), leitnerBox(1) {}
    virtual ~Word() {}

    std::string getEnglish() const { return english; }
    std::string getVietnamese() const { return vietnamese; }

    int getLeitnerBox() const { return leitnerBox; }
    int getCorrect() const { return correctCount; }
    int getWrong() const { return wrongCount; }
    void setStats(int correct, int wrong) { correctCount = correct; wrongCount = wrong; }
    void recordAnswer(bool ok) { if (ok) correctCount++; else wrongCount++; }
    void setLeitnerBox(int box) {
        if (box < 1) box = 1;
        if (box > 5) box = 5;
        leitnerBox = box;
    }

    void addSynonym(const std::string& syn) { synonyms.push_back(syn); }
    std::vector<std::string> getSynonyms() const { return synonyms; }

    void addAntonym(const std::string& ant) { antonyms.push_back(ant); }
    std::vector<std::string> getAntonyms() const { return antonyms; }

    void addExample(const std::string& ex) { examples.push_back(ex); }
    std::vector<std::string> getExamples() const { return examples; }

    // Ten loai tu dung de ghi vao file (GeneralWord / TechnicalTerm / Idiom)
    virtual std::string getTypeName() const = 0;

    // In thong tin mo rong (dung chung cho cac lop con)
    // Dong tieu de: [loai] TU TIENG ANH  (phu)  roi nghia tieng Viet ben duoi
    void printHeadline(const std::string& tag, const std::string& extra) const {
        setColor(C_DIM);
        std::cout << "[" << tag << "]";
        setColor(C_BRIGHT);
        std::cout << "  " << english;
        if (!extra.empty()) {
            setColor(C_DIM);
            std::cout << "  " << extra;
        }
        std::cout << "\n";
        setColor(C_OK);
        std::cout << "     " << vietnamese << "\n";
        resetColor();
    }

    // Nhan phu (mau xanh nhat) + noi dung (mau thuong)
    static void printLabel(const char* label) {
        setColor(C_TITLE);
        std::cout << "   " << label << ": ";
        resetColor();
    }

    // Thong tin phu: hop, dong nghia, trai nghia, vi du
    void displayExtendedInfo() const {
        std::cout << "\n";
        printLabel("Leitner");
        int box = leitnerBox;
        setColor(box <= 2 ? C_BAD : (box >= 4 ? C_OK : C_WARN));
        std::cout << "Box " << box;
        resetColor();
        std::cout << "  (dung " << correctCount << ", sai " << wrongCount << ")\n";

        if (!synonyms.empty()) {
            printLabel("Dong nghia");
            for (size_t i = 0; i < synonyms.size(); i++)
                std::cout << synonyms[i] << (i + 1 < synonyms.size() ? ", " : "");
            std::cout << "\n";
        }
        if (!antonyms.empty()) {
            printLabel("Trai nghia");
            for (size_t i = 0; i < antonyms.size(); i++)
                std::cout << antonyms[i] << (i + 1 < antonyms.size() ? ", " : "");
            std::cout << "\n";
        }
        if (!examples.empty()) {
            printLabel("Vi du");
            std::cout << "\n";
            for (const std::string& ex : examples) {
                setColor(C_DIM);
                std::cout << "     - ";
                resetColor();
                std::cout << ex << "\n";
            }
        }
    }

    virtual void displayDetails() const = 0;
};

// ============================================================
// Tu thong dung
// ============================================================
class GeneralWord : public Word {
private:
    std::string partOfSpeech;

public:
    GeneralWord(std::string en, std::string vi, std::string pos = "")
        : Word(en, vi), partOfSpeech(pos) {}

    std::string getTypeName() const override { return "GeneralWord"; }

    void displayDetails() const override {
        printHeadline("Tu thong dung", partOfSpeech.empty() ? "" : "(" + partOfSpeech + ")");
        displayExtendedInfo();
    }
};

// ============================================================
// Thuat ngu chuyen nganh
// ============================================================
class TechnicalTerm : public Word {
private:
    std::string field;

public:
    TechnicalTerm(std::string en, std::string vi, std::string f = "")
        : Word(en, vi), field(f) {}

    std::string getTypeName() const override { return "TechnicalTerm"; }

    void displayDetails() const override {
        printHeadline("Thuat ngu", field.empty() ? "" : "[" + field + "]");
        displayExtendedInfo();
    }
};

// ============================================================
// Thanh ngu
// ============================================================
class Idiom : public Word {
private:
    std::string origin;

public:
    Idiom(std::string en, std::string vi, std::string orig = "")
        : Word(en, vi), origin(orig) {}

    std::string getTypeName() const override { return "Idiom"; }

    void displayDetails() const override {
        printHeadline("Thanh ngu", origin.empty() ? "" : "(Origin: " + origin + ")");
        displayExtendedInfo();
    }
};

// Tao doi tuong Word dung lop con theo ten loai (dung khi doc file)
// Loai khong nhan ra se duoc coi la GeneralWord
inline Word* createWordByType(const std::string& type, const std::string& en, const std::string& vi) {
    if (type == "TechnicalTerm") return new TechnicalTerm(en, vi);
    if (type == "Idiom") return new Idiom(en, vi);
    return new GeneralWord(en, vi);
}

#endif
