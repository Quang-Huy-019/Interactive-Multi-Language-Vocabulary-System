#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <iostream>
#include <string>
#include <limits> // Thu vi?n c?n thi?t d? d?n d?p b? nh? d?m

using namespace std;

class InputHandler {
public:
    // 1. Hàm ép bu?c ngu?i dùng ph?i nh?p dúng s? nguyên
    static int getValidInt(string prompt) {
        int value;
        while (true) {
            cout << prompt;
            if (cin >> value) {
                // Nh?p dúng s? -> D?n s?ch phím Enter th?a trong b? nh? d?m
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return value;
            } else {
                // B? l?i (do nh?p ch?) -> B?t l?i và yêu c?u nh?p l?i
                cout << "[!] Loi: Vui long chi nhap so nguyen!\n";
                cin.clear(); // Xóa tr?ng thái l?i c?a cin
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // V?t b? chu?i ký t? sai
            }
        }
    }

    // 2. Hàm nh?p chu?i van b?n an toàn (ch?p nh?n c? d?u cách)
    static string getValidString(string prompt) {
        string value;
        cout << prompt;
        getline(cin, value); // Dùng getline thay vì cin >> d? nh?n du?c c? câu dài
        return value;
    }
};

#endif
