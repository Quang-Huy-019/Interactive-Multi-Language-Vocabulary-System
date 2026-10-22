#ifndef FLASHCARD_H
#define FLASHCARD_H

#include <vector>
#include "Dictionary.h"

// Che do the ghi nho (flashcard):
//   Enter / Space : lat the xem nghia
//   1 : nho      2 : chua nho      Q / ESC : thoat
// Ket qua cap nhat Leitner Box va duoc tinh vao thong ke.
void runFlashcards(std::vector<Word*> pool, int numCards);

#endif
