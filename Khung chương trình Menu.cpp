#include <iostream>
using namespace std;

void showMenu() {
    cout << "\n========== TU DIEN ==========\n";
    cout << "1. Them tu\n";
    cout << "2. Sua tu\n";
    cout << "3. Xoa tu\n";
    cout << "4. Tim tu\n";
    cout << "5. Hien thi danh sach\n";
    cout << "6. Hoc Leitner\n";
    cout << "7. Lam Quiz\n";
    cout << "0. Thoat\n";
    cout << "==============================\n";
    cout << "Nhap lua chon: ";
}

int main() {
    int choice;

    do {
        showMenu();
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "Chuc nang Them tu\n";
                break;

            case 2:
                cout << "Chuc nang Sua tu\n";
                break;

            case 3:
                cout << "Chuc nang Xoa tu\n";
                break;

            case 4:
                cout << "Chuc nang Tim tu\n";
                break;

            case 5:
                cout << "Chuc nang Hien thi danh sach\n";
                break;

            case 6:
                cout << "Chuc nang Hoc Leitner\n";
                break;

            case 7:
                cout << "Chuc nang Lam Quiz\n";
                break;

            case 0:
                cout << "Thoat chuong trinh!\n";
                break;

            default:
                cout << "Lua chon khong hop le!\n";
        }

    } while (choice != 0);

    return 0;
}