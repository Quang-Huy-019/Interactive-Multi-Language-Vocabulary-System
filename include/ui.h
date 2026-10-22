#ifndef UI_H
#define UI_H

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <cstdlib>
#include <iostream>
#include <string>

// Mau chu cua console Windows
const int C_DEFAULT = 7;
const int C_TITLE = 11; // xanh nhat
const int C_OK = 10;    // xanh la
const int C_BAD = 12;   // do
const int C_WARN = 14;  // vang
const int C_DIM = 8;    // xam (thong tin phu)
const int C_BRIGHT = 15; // trang sang (noi dung chinh)

inline void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)color);
}

inline void resetColor() { setColor(C_DEFAULT); }

// Xoa man hinh bang API (it nhap nhay hon system("cls")); fallback neu loi
inline void clearScreen() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(h, &csbi)) { system("cls"); return; }
    DWORD cells = (DWORD)csbi.dwSize.X * (DWORD)csbi.dwSize.Y;
    DWORD written = 0;
    COORD home = {0, 0};
    FillConsoleOutputCharacterA(h, ' ', cells, home, &written);
    FillConsoleOutputAttribute(h, csbi.wAttributes, cells, home, &written);
    SetConsoleCursorPosition(h, home);
}

// Tieu de khung: dong '=' va ten can giua
inline void printTitle(const std::string& title, int width = 50) {
    std::string line(width, '=');
    int pad = (width - (int)title.size()) / 2;
    if (pad < 0) pad = 0;
    setColor(C_TITLE);
    std::cout << line << "\n" << std::string(pad, ' ') << title << "\n" << line << "\n";
    resetColor();
}

// Dong phim tat: [phim] mo ta
inline void printKey(const std::string& key, const std::string& desc) {
    setColor(C_WARN);
    std::cout << "  [" << key << "]";
    resetColor();
    std::cout << " " << desc << "\n";
}

// Thanh tien do: done/total
inline void printBar(int done, int total, int width = 30) {
    if (total <= 0) total = 1;
    if (done < 0) done = 0;
    if (done > total) done = total;
    int filled = done * width / total;
    setColor(C_OK);
    std::cout << std::string(filled, '#');
    setColor(C_DIM);
    std::cout << std::string(width - filled, '-');
    resetColor();
    std::cout << " " << done << "/" << total;
}

#endif
