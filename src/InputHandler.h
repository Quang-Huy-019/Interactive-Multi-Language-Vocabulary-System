#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <iostream>
#include <string>
#include <limits> // Thu viện cần thiết để dọn dẹp bộ nhớ đệm

using namespace std;

class InputHandler {
public:
    // 1. Hàm ép buộc người dùng phải nhập đúng số nguyên
    static int getValidInt(string prompt) {
        int value;
        while (true) {
            cout << prompt;
            if (cin >> value) {
                // Nhập đúng số -> Dọn sạch phím Enter thừa trong bộ nhớ đệm
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return value;
            } else {
                // Bị lỗi (do nhập chữ) -> Bắt lỗi và yêu cầu nhập lại
                cout << "[!] Loi: Vui long chi nhap so nguyen!\n";
                cin.clear(); // Xóa trạng thái lỗi của cin
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Vứt bỏ chuỗi ký tự sai
            }
        }
    }

    // 2. Hàm nhập chuỗi văn bản an toàn (chấp nhận cả dấu cách)
    static string getValidString(string prompt) {
        string value;
        cout << prompt;
        getline(cin, value); // Dùng getline thay vì cin >> để nhận được cả câu dài
        return value;
    }
};

#endif
