#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <algorithm>
#include <cctype>

// 1. Cấu trúc lưu thông tin chi tiết của 1 từ
struct WordEntry {
    std::string word;
    std::string partOfSpeech;              // noun, verb, adj,...
    std::vector<std::string> definitions;  // Định nghĩa / Nghĩa tiếng Việt
    std::vector<std::string> synonyms;     // Từ đồng nghĩa
    std::vector<std::string> antonyms;     // Từ trái nghĩa
    std::vector<std::string> examples;     // Câu ví dụ
};

// Kết quả trả về của tìm kiếm 2 chiều
enum class SearchDirection { FORWARD, REVERSE, NOT_FOUND };

struct SearchResult {
    SearchDirection direction;
    std::vector<WordEntry> matches;
};

// 2. Class xử lý tra cứu hai chiều
class DualDirectionDictionary {
private:
    // Tra xuôi: word -> WordEntry (O(1))
    std::unordered_map<std::string, WordEntry> forwardIndex;

    // Tra ngược: token trong nghĩa -> danh sách các word (Inverted Index)
    std::unordered_map<std::string, std::unordered_set<std::string>> reverseIndex;

    // Hàm tiện ích: chuyển chuỗi về chữ thường
    static std::string toLower(const std::string& str) {
        std::string lower = str;
        std::transform(lower.begin(), lower.end(), lower.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        return lower;
    }

    // Tách từ khóa từ định nghĩa để làm chỉ mục tra ngược
    std::vector<std::string> tokenize(const std::string& text) const {
        std::vector<std::string> tokens;
        std::string cleanText;

        for (char ch : text) {
            if (std::isalnum(static_cast<unsigned char>(ch)) || std::isspace(static_cast<unsigned char>(ch))) {
                cleanText += ch;
            } else {
                cleanText += ' ';
            }
        }

        std::stringstream ss(cleanText);
        std::string token;
        while (ss >> token) {
            token = toLower(token);
            if (token.length() > 2) { // Bỏ qua từ quá ngắn (a, an, of...)
                tokens.push_back(token);
            }
        }
        return tokens;
    }

public:
    // Thêm từ vào từ điển và tự động xây dựng chỉ mục 2 chiều
    void addEntry(const WordEntry& entry) {
        std::string key = toLower(entry.word);
        forwardIndex[key] = entry;

        // Xây dựng chỉ mục ngược từ tất cả các định nghĩa
        for (const auto& def : entry.definitions) {
            auto tokens = tokenize(def);
            for (const auto& token : tokens) {
                reverseIndex[token].insert(key);
            }
        }
    }

    // [Tính năng 1] Tra xuôi: Nhập từ -> Lấy chi tiết (O(1))
    const WordEntry* lookupForward(const std::string& word) const {
        std::string key = toLower(word);
        auto it = forwardIndex.find(key);
        if (it != forwardIndex.end()) {
            return &(it->second);
        }
        return nullptr;
    }

    // [Tính năng 2] Tra ngược: Nhập nghĩa/mô tả -> Lấy danh sách từ phù hợp
    std::vector<WordEntry> lookupReverse(const std::string& conceptQuery) const {
        auto tokens = tokenize(conceptQuery);
        std::unordered_map<std::string, int> matchScores;

        for (const auto& token : tokens) {
            auto it = reverseIndex.find(token);
            if (it != reverseIndex.end()) {
                for (const auto& matchedWord : it->second) {
                    matchScores[matchedWord]++;
                }
            }
        }

        // Sắp xếp kết quả theo độ khớp giảm dần
        std::vector<std::pair<std::string, int>> sortedMatches(matchScores.begin(), matchScores.end());
        std::sort(sortedMatches.begin(), sortedMatches.end(),
                  [](const auto& a, const auto& b) { return a.second > b.second; });

        std::vector<WordEntry> results;
        for (const auto& match : sortedMatches) {
            results.push_back(forwardIndex.at(match.first));
        }
        return results;
    }

    // [Tính năng thông minh] Tự nhận diện tra xuôi hay tra ngược
    SearchResult smartSearch(const std::string& query) const {
        // 1. Thử tra xuôi trước
        const WordEntry* forwardResult = lookupForward(query);
        if (forwardResult) {
            return { SearchDirection::FORWARD, { *forwardResult } };
        }

        // 2. Không thấy từ -> Tra ngược theo nghĩa / từ khóa
        auto reverseResults = lookupReverse(query);
        if (!reverseResults.empty()) {
            return { SearchDirection::REVERSE, reverseResults };
        }

        return { SearchDirection::NOT_FOUND, {} };
    }
};