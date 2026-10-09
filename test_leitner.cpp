#include "LeitnerSystem.h"

#include <cassert>

int main()
{
    GeneralWord easyWord("easy", "de", "adjective");
    GeneralWord hardWord("hard", "kho", "adjective");
    GeneralWord newWord("new", "moi", "adjective");
    GeneralWord missingWord("missing", "thieu", "adjective");

    LeitnerSystem system;
    system.addCard(LeitnerCard(&easyWord, 1));

    assert(system.updateResult(&easyWord, true));
    assert(system.getCard(&easyWord)->level == 2);
    assert(easyWord.getLeitnerBox() == 2);

    assert(system.updateResult(&easyWord, false));
    assert(system.getCard(&easyWord)->level == 1);

    system.addCard(LeitnerCard(&hardWord, 5));
    assert(system.updateResult(&hardWord, true));
    assert(system.getCard(&hardWord)->level == 5);

    system.addCard(LeitnerCard(&newWord, 1));
    assert(system.updateResult(&newWord, false));
    assert(system.getCard(&newWord)->level == 1);

    assert(!system.updateResult(&missingWord, true));
    return 0;
}
