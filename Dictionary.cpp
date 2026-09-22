#include "Dictionary.h"
#include <iostream>

using namespace std;

// 1. HÀM HỦY: Giải phóng toàn bộ bộ nhớ con trỏ khi tắt chương trình (Tối quan trọng)
Dictionary::~Dictionary() {
    for (auto const& pair : dictionaryMap) {
        delete pair.second; // Giải phóng đối tượng Word* khỏi RAM
    }
    dictionaryMap.clear(); // Làm sạch map
}

// 2. HÀM THÊM TỪ VỰNG
bool Dictionary::addWord(Word* word) {
    if (word == nullptr) return false;
    
    string key = word->getEnglish();
    
    // Kiểm tra xem từ này đã có trong map chưa
    if (dictionaryMap.find(key) != dictionaryMap.end()) {
        cout << "[!] Tu '" << key << "' da ton tai trong tu dien.\n";
        return false; 
    }
    
    // Thêm vào map
    dictionaryMap[key] = word;
    return true;
}

// 3. HÀM TRA CỨU TỪ VỰNG
Word* Dictionary::searchWord(string englishKey) {
    // Sử dụng thuật toán tìm kiếm nhị phân của map (Tốc độ O(logN))
    auto it = dictionaryMap.find(englishKey);
    
    if (it != dictionaryMap.end()) {
        return it->second; // Trả về con trỏ Word* nếu tìm thấy
    }
    return nullptr; // Không tìm thấy
}

// 4. HÀM XÓA TỪ VỰNG
bool Dictionary::deleteWord(string englishKey) {
    auto it = dictionaryMap.find(englishKey);
    
    if (it != dictionaryMap.end()) {
        delete it->second;       // 1. Xóa vùng nhớ của từ vựng (Chống memory leak)
        dictionaryMap.erase(it); // 2. Xóa key đó khỏi map
        return true;
    }
    return false;
}