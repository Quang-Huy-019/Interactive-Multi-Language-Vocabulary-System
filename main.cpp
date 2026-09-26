#include <conio.h>
#include <windows.h>
#include <iostream>
#include <vector>

#include "quiz.h"
#include "Dictionary.h"

using namespace std;


// ============================================================
// SET CONSOLE DISPLAY MODE
// ============================================================

BOOL NT_SetConsoleDisplayMode(
    HANDLE hOutputHandle,
    DWORD dwNewMode
)
{
    typedef BOOL(WINAPI *SCDMProc_t)(
        HANDLE,
        DWORD,
        LPDWORD
    );

    SCDMProc_t SetConsoleDisplayMode;

    HMODULE hKernel32;

    BOOL bFreeLib = FALSE;
    BOOL ret;

    const char KERNEL32_NAME[] = "kernel32.dll";


    hKernel32 =
        GetModuleHandleA(KERNEL32_NAME);


    if (hKernel32 == NULL)
    {
        hKernel32 =
            LoadLibraryA(KERNEL32_NAME);

        if (hKernel32 == NULL)
        {
            return FALSE;
        }

        bFreeLib = TRUE;
    }


    SetConsoleDisplayMode =
        (SCDMProc_t)GetProcAddress(
            hKernel32,
            "SetConsoleDisplayMode"
        );


    if (SetConsoleDisplayMode == NULL)
    {
        SetLastError(
            ERROR_CALL_NOT_IMPLEMENTED
        );

        ret = FALSE;
    }
    else
    {
        DWORD tmp;

        ret =
            SetConsoleDisplayMode(
                hOutputHandle,
                dwNewMode,
                &tmp
            );
    }


    if (bFreeLib)
    {
        FreeLibrary(hKernel32);
    }


    return ret;
}


// ============================================================
// FULL SCREEN
// ============================================================

void fullscreen()
{
    keybd_event(
        VK_MENU,
        0x38,
        0,
        0
    );

    keybd_event(
        VK_RETURN,
        0x1c,
        0,
        0
    );

    keybd_event(
        VK_RETURN,
        0x1c,
        KEYEVENTF_KEYUP,
        0
    );

    keybd_event(
        VK_MENU,
        0x38,
        KEYEVENTF_KEYUP,
        0
    );
}


// ============================================================
// MAIN
// ============================================================

int main()
{
    fullscreen();


    // Simulate loading the existing vocabulary successfully at startup.
    Dictionary dictionary;
    vector<QuizWord> words;
    for (Word* word : dictionary.getAllWords())
        words.emplace_back(word->getEnglish(), word->getVietnamese());

    QuizSession quiz(words);


    while (true)
    {
        system("cls");


        cout
            << "========================================\n";

        cout
            << "       VOCABULARY QUIZ SYSTEM\n";

        cout
            << "========================================\n\n";


        cout
            << "1. Start Quiz\n";

        cout
            << "2. Quiz History\n";

        cout
            << "3. Exit\n\n";


        cout
            << "Press 1, 2 or 3: ";


        char choice = _getch();


        switch (choice)
        {
        case '1':

            cout << "1\n";

            quiz.startQuiz();

            break;


        case '2':

            cout << "2\n";

            QuizHistoryManager::showHistoryMenu();

            break;


        case '3':

            cout << "3\n";

            cout
                << "\nThank you for using the system!\n";

            return 0;


        case 27:

            cout
                << "\n\nESC pressed. Exiting...\n";

            return 0;


        default:

            cout
                << "\n\nInvalid choice!";

            cout
                << "\nPress any key to continue...";

            _getch();

            break;
        }
    }


    return 0;
}

