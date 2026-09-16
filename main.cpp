#include <iostream>
#include "StudyAnalytics.h"

using namespace std;

int main() {

    StudyAnalytics app;


    // ==========================================
    // TC05: LEITNER - TRA LOI DUNG
    // Box 2 -> Box 3
    // ==========================================

    cout << "\n===== TC05: LEITNER - TRA LOI DUNG =====\n";

    app.addWord("algorithm", "thuat toan", 2);

    app.updateResult("algorithm", true);


    // ==========================================
    // TC06: LEITNER - TRA LOI SAI
    // Box 4 -> Box 1
    // ==========================================

    cout << "\n===== TC06: LEITNER - TRA LOI SAI =====\n";

    app.addWord("computer", "may tinh", 4);

    app.updateResult("computer", false);


    // ==========================================
    // TC08: PHAN TICH TU YEU
    // Ty le sai > 40%
    // ==========================================

    cout << "\n===== TC08: LOC TU YEU =====\n";

    app.addWord("idiom", "thanh ngu", 2);

    // 4 dung, 6 sai
    // Accuracy = 40%
    // Ty le sai = 60% > 40%

    app.updateResult("idiom", true);
    app.updateResult("idiom", true);
    app.updateResult("idiom", true);
    app.updateResult("idiom", true);

    app.updateResult("idiom", false);
    app.updateResult("idiom", false);
    app.updateResult("idiom", false);
    app.updateResult("idiom", false);
    app.updateResult("idiom", false);
    app.updateResult("idiom", false);


    // ==========================================
    // TC10: TU KHONG TON TAI
    // ==========================================

    cout << "\n===== TC10: TU KHONG TON TAI =====\n";

    app.updateResult("xyzunknown", true);


    // ==========================================
    // HIEN THI DASHBOARD
    // ==========================================

    cout << "\n===== DASHBOARD =====\n";

    app.showDashboard();


    return 0;
}
