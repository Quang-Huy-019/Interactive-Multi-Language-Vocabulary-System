# Interactive-Multi-Language-Vocabulary-System
Build an intelligent vocabulary learning and dictionary system. Supports word management across multiple word types, synonym mapping, flashcard quiz modes with Leitner spaced repetition, and study analytics.

## Build

Compile the program with:

```bash
g++ main.cpp quiz.cpp LeitnerSystem.cpp -o main.exe
```

## Leitner spaced repetition

`LeitnerSystem` manages flashcard mastery levels from 1 to 5. A remembered
card moves up one level, while a forgotten card moves down one level. The
level is clamped so it never goes below 1 or above 5. The class also tracks
correct/wrong answers and can return cards due at a level or weak cards.

Run the focused tests with:

```bash
g++ -std=c++11 test_leitner.cpp LeitnerSystem.cpp -o test_leitner
./test_leitner
```
