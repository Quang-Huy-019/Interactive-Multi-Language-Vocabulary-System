#include <iostream>
#include "StudyAnalytics.h"

using namespace std;

int main() {
    StudyAnalytics app;

    cout << "========================================\n";
    cout << "   KHOI TAO DU LIEU MAU (SEED DATA)\n";
    cout << "========================================\n";

    // 1. Them Seed Data vao he thong
    app.addWord("apple", "qua tao", 1);         // Tu moi (Chua hoc)
    app.addWord("algorithm", "thuat toan", 2);  // Dang o Box 2
    app.addWord("computer", "may tinh", 4);     // Dang o Box 4
    app.addWord("database", "co so du lieu", 3); // Dang o Box 3
    app.addWord("idiom", "thanh ngu", 2);       // Tu se chuyen thanh tu yeu
    app.addWord("framework", "khung lam viec", 5); // Dang o Box 5 (Gioi han toi da)

    cout << "Da nap thanh cong 6 tu vung mau.\n";

    cout << "\n========================================\n";
    cout << "   CHAY THU NGHIEM PHAT HIEN LOI (TEST BUGS)\n";
    cout << "========================================\n";

    // Test Bug 1: Tu moi chua co luot hoc (Kiem tra tranh loi chia cho 0)
    cout << "\n[Test Bug 1] Kiem tra tu chua hoc ('apple'):\n";
    // Khong goi updateResult cho "apple", goi truc tiep Dashboard de test accuracy = 0.0

    // Test Bug 2: Cap nhat tu hoat dong dung logic Leitner & gioi han Box
    cout << "\n[Test Bug 2] Kiem tra cap nhat ket qua 'algorithm' (Dung):\n";
    app.updateResult("algorithm", true); 
    app.updateResult("algorithm", true); // Len Box 4

    cout << "\n[Test Bug 2.1] Kiem tra cap nhat 'computer' (Sai -> Ve Box 1):\n";
    app.updateResult("computer", false); // Sai -> Ve Box 1

    // Test Bug 3: Tao tap du lieu tu yeu (Ty le sai > 40%) cho tu 'idiom'
    cout << "\n[Test Bug 3] Mo phong tu yeu ('idiom' - 4 dung, 6 sai):\n";
    for(int i=0; i<4; i++) app.updateResult("idiom", true);
    for(int i=0; i<6; i++) app.updateResult("idiom", false);

    // Test Bug 4: Truy van tu khong ton tai trong he thong (Kiem tra bat loi ngoai le)
    cout << "\n[Test Bug 4] Truy van tu khong ton tai ('unknown_word'):\n";
    app.updateResult("unknown_word", true);

    // ==========================================
    // HIEN THI KET QUA TONG QUAN (DASHBOARD)
    // ==========================================
    app.showDashboard();

    return 0;
}
