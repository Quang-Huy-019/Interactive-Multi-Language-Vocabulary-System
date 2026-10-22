#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "Word.h"
#include <cctype>
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

// ============================================================
// Dinh dang file data.txt (phan cach bang '|'):
//   english|vietnamese|type|leitnerBox|synonyms|antonyms|examples|correct|wrong
//   (cot correct, wrong khong bat buoc: file cu 7 cot van doc duoc)
//   - synonyms, antonyms: cach nhau bang dau phay ','
//   - examples: nhieu cau cach nhau bang dau cham phay ';'
//     (dung ';' de cau vi du co the chua dau phay)
// ============================================================
class Dictionary {
private:
    std::map<std::string, Word*> engToViMap; // key: tieng Anh viet thuong
    std::map<std::string, Word*> viToEngMap; // key: nghia tieng Viet viet thuong

    // ---------- Ham tien ich ----------
    static std::string trim(const std::string& s) {
        size_t b = 0, e = s.size();
        while (b < e && std::isspace(static_cast<unsigned char>(s[b]))) b++;
        while (e > b && std::isspace(static_cast<unsigned char>(s[e - 1]))) e--;
        return s.substr(b, e - b);
    }

    static std::string toLower(const std::string& s) {
        std::string r = s;
        for (char& c : r) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        return r;
    }

    // Tach chuoi theo ky tu phan cach, giu lai o rong, cat khoang trang 2 dau
    static std::vector<std::string> split(const std::string& s, char sep) {
        std::vector<std::string> parts;
        std::string cur;
        std::stringstream ss(s);
        while (std::getline(ss, cur, sep)) parts.push_back(trim(cur));
        return parts;
    }

    // Bo ky tu dung de phan cach trong du lieu (de khong vo file)
    static std::string sanitize(const std::string& s, char bad, char replacement) {
        std::string r = s;
        for (char& c : r) {
            if (c == bad) c = replacement;
            if (c == '\n' || c == '\r') c = ' ';
        }
        return trim(r);
    }

    // ---------- Khop theo DAU TU ----------
    // Chi khop khi tu khoa bat dau mot tu (dau chuoi hoac sau ky tu khong phai chu/so).
    // Khong khop o giua tu: "an" khong khop "khang" hay "cach tiep can".
    // Tra ve: -1 = khong khop, 0 = khop nguyen tu ("an" == "an"), 1 = khop tien to ("an" -> "ang")
    static bool isWordByte(char c) {
        unsigned char u = static_cast<unsigned char>(c);
        return std::isalnum(u) || u >= 0x80; // byte >= 0x80 la mot phan cua chu tieng Viet co dau
    }

    static int matchRank(const std::string& lowerText, const std::string& lowerKey) {
        if (lowerKey.empty()) return -1;
        int best = -1;
        size_t pos = lowerText.find(lowerKey);
        while (pos != std::string::npos) {
            bool startOk = (pos == 0) || !isWordByte(lowerText[pos - 1]);
            if (startOk) {
                size_t end = pos + lowerKey.size();
                bool wholeWord = (end == lowerText.size()) || !isWordByte(lowerText[end]);
                if (wholeWord) return 0;
                best = 1;
            }
            pos = lowerText.find(lowerKey, pos + 1);
        }
        return best;
    }

    static std::string toLowerStatic(const std::string& s) {
        std::string r = s;
        for (char& c : r) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        return r;
    }

    static std::string joinWith(const std::vector<std::string>& items, const std::string& sep) {
        std::string out;
        for (size_t i = 0; i < items.size(); i++) {
            if (i > 0) out += sep;
            out += items[i];
        }
        return out;
    }

public:
    Dictionary() {}

    ~Dictionary() {
        for (auto& entry : engToViMap) delete entry.second;
    }

    // Khong cho sao chep (tranh double delete)
    Dictionary(const Dictionary&) = delete;
    Dictionary& operator=(const Dictionary&) = delete;

    size_t size() const { return engToViMap.size(); }

    // Them tu. Dictionary chi nhan quyen so huu khi tra ve true;
    // neu false, nguoi goi phai tu delete word.
    bool addWord(Word* word) {
        if (word == nullptr) return false;
        std::string engKey = toLower(trim(word->getEnglish()));
        if (engKey.empty() || trim(word->getVietnamese()).empty()) return false;
        if (engToViMap.count(engKey)) return false;

        engToViMap[engKey] = word;
        std::string viKey = toLower(trim(word->getVietnamese()));
        if (!viToEngMap.count(viKey)) viToEngMap[viKey] = word;
        return true;
    }

    Word* searchEnglish(const std::string& englishKey) const {
        auto it = engToViMap.find(toLower(trim(englishKey)));
        return it == engToViMap.end() ? nullptr : it->second;
    }

    Word* searchVietnamese(const std::string& vietnameseKey) const {
        auto it = viToEngMap.find(toLower(trim(vietnameseKey)));
        return it == viToEngMap.end() ? nullptr : it->second;
    }

    bool deleteWord(const std::string& englishKey) {
        auto it = engToViMap.find(toLower(trim(englishKey)));
        if (it == engToViMap.end()) return false;

        Word* w = it->second;
        // Chi xoa key Viet neu no dang tro dung toi tu nay
        auto viIt = viToEngMap.find(toLower(trim(w->getVietnamese())));
        if (viIt != viToEngMap.end() && viIt->second == w) viToEngMap.erase(viIt);

        engToViMap.erase(it);
        delete w;
        return true;
    }

    std::vector<Word*> getAllWords() const {
        std::vector<Word*> words;
        words.reserve(engToViMap.size());
        for (const auto& entry : engToViMap) words.push_back(entry.second);
        return words;
    }

    // ==============================================================
    // TRA VIET -> ANH: tim tu co chua tu khoa trong nghia tieng Viet
    // ==============================================================
    std::vector<Word*> searchVietnameseKeyword(const std::string& keyword) const {
        std::string key = toLower(trim(keyword));
        std::vector<std::pair<int, Word*>> ranked;
        if (key.empty()) return {};

        for (const auto& pair : engToViMap) {
            int rank = matchRank(toLowerStatic(pair.second->getVietnamese()), key);
            if (rank >= 0) ranked.push_back({rank, pair.second});
        }
        std::stable_sort(ranked.begin(), ranked.end(),
                         [](const auto& a, const auto& b) { return a.first < b.first; });

        std::vector<Word*> results;
        for (const auto& r : ranked) results.push_back(r.second);
        return results;
    }

    // ==============================================================
    // TRA ANH -> VIET: tim theo tu chinh va tu dong nghia (chua tu khoa)
    // Ket qua khop chinh xac duoc dua len truoc.
    // ==============================================================
    std::vector<Word*> searchEnglishExtended(const std::string& keyword) const {
        std::string key = toLower(trim(keyword));
        std::vector<std::pair<int, Word*>> ranked;
        if (key.empty()) return {};

        for (const auto& pair : engToViMap) {
            Word* w = pair.second;
            int best = matchRank(toLowerStatic(w->getEnglish()), key);
            for (const std::string& syn : w->getSynonyms()) {
                int r = matchRank(toLowerStatic(syn), key);
                if (r >= 0 && (best < 0 || r < best)) best = r;
            }
            if (best >= 0) ranked.push_back({best, w});
        }
        std::stable_sort(ranked.begin(), ranked.end(),
                         [](const auto& a, const auto& b) { return a.first < b.first; });

        std::vector<Word*> results;
        for (const auto& r : ranked) results.push_back(r.second);
        return results;
    }

    // ==============================================================
    // DOC FILE. Tra ve so dong bi bo qua (sai dinh dang).
    // ==============================================================
    int loadFromFile(const std::string& filename) {
        std::ifstream inFile(filename);
        if (!inFile.is_open()) return 0;

        int skipped = 0;
        std::string line;
        while (std::getline(inFile, line)) {
            line = trim(line); // bo \r cua file Windows va khoang trang
            if (line.empty() || line[0] == '#') continue;

            std::vector<std::string> f;
            {
                std::string cur;
                std::stringstream ss(line);
                while (std::getline(ss, cur, '|')) f.push_back(trim(cur));
            }
            if (f.size() < 4 || f[0].empty() || f[1].empty()) {
                skipped++;
                continue;
            }

            int box = 1;
            try {
                box = std::stoi(f[3]);
            } catch (...) {
                box = 1;
            }

            Word* w = createWordByType(f[2], f[0], f[1]);
            w->setLeitnerBox(box); // setLeitnerBox tu kep trong [1, 5]

            if (f.size() > 4)
                for (const std::string& s : split(f[4], ',')) if (!s.empty()) w->addSynonym(s);
            if (f.size() > 5)
                for (const std::string& a : split(f[5], ',')) if (!a.empty()) w->addAntonym(a);
            if (f.size() > 6)
                for (const std::string& e : split(f[6], ';')) if (!e.empty()) w->addExample(e);

            int correct = 0, wrong = 0;
            try { if (f.size() > 7) correct = std::stoi(f[7]); } catch (...) {}
            try { if (f.size() > 8) wrong = std::stoi(f[8]); } catch (...) {}
            w->setStats(correct, wrong);

            if (!addWord(w)) {
                delete w; // trung tu
                skipped++;
            }
        }
        inFile.close();
        return skipped;
    }

    // ==============================================================
    // NHAP TU FILE KHAC (IMPORT)
    // Them cac tu chua co trong tu dien, bo qua tu da ton tai.
    // Tra ve so tu them moi, -1 neu khong mo duoc file.
    // ==============================================================
    int importFromFile(const std::string& filename, int& duplicates, int& skipped) {
        duplicates = 0;
        skipped = 0;
        {
            std::ifstream probe(filename);
            if (!probe.is_open()) return -1;
        }

        Dictionary temp;
        skipped = temp.loadFromFile(filename);

        int added = 0;
        for (Word* src : temp.getAllWords()) {
            if (searchEnglish(src->getEnglish())) {
                duplicates++;
                continue;
            }
            Word* copy = createWordByType(src->getTypeName(), src->getEnglish(), src->getVietnamese());
            copy->setLeitnerBox(src->getLeitnerBox());
            copy->setStats(src->getCorrect(), src->getWrong());
            for (const auto& s : src->getSynonyms()) copy->addSynonym(s);
            for (const auto& a : src->getAntonyms()) copy->addAntonym(a);
            for (const auto& e : src->getExamples()) copy->addExample(e);

            if (addWord(copy)) added++;
            else delete copy;
        }
        return added;
    }

    // ==============================================================
    // GHI FILE. Ghi vao file tam roi moi thay the file goc,
    // nen neu co loi giua chung thi data.txt cu van con nguyen.
    // ==============================================================
    bool saveToFile(const std::string& filename) const {
        std::string tmpName = filename + ".tmp";
        std::ofstream outFile(tmpName);
        if (!outFile.is_open()) return false;

        for (const auto& pair : engToViMap) {
            const Word* w = pair.second;
            outFile << sanitize(w->getEnglish(), '|', '/') << "|"
                    << sanitize(w->getVietnamese(), '|', '/') << "|"
                    << w->getTypeName() << "|"
                    << w->getLeitnerBox() << "|";

            // Tu dong nghia/trai nghia khong duoc chua ',' hoac '|'
            // Cau vi du khong duoc chua ';' hoac '|'
            std::vector<std::string> syns, ants, exs;
            for (const auto& s : w->getSynonyms()) syns.push_back(sanitize(sanitize(s, '|', '/'), ',', ' '));
            for (const auto& a : w->getAntonyms()) ants.push_back(sanitize(sanitize(a, '|', '/'), ',', ' '));
            for (const auto& e : w->getExamples()) exs.push_back(sanitize(sanitize(e, '|', '/'), ';', ','));

            outFile << joinWith(syns, ",") << "|"
                    << joinWith(ants, ",") << "|"
                    << joinWith(exs, ";") << "|"
                    << w->getCorrect() << "|" << w->getWrong() << "\n";
        }
        outFile.close();
        if (!outFile) return false;

        std::remove(filename.c_str());
        return std::rename(tmpName.c_str(), filename.c_str()) == 0;
    }
};

#endif
