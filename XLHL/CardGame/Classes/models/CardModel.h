#ifndef __CARD_MODEL_H__
#define __CARD_MODEL_H__

#include "cocos2d.h"
#include "models/CardTypes.h"

/**
 * @brief Runtime card data model. Pure data, no UI and no complex business logic.
 */
class CardModel
{
public:
    int cardId = -1;
    CardFaceType face = CardFaceType::CFT_NONE;
    CardSuitType suit = CardSuitType::CST_NONE;

    // UI-related runtime state that needs to be undoable.
    // In a more complex game you might store container/index instead of absolute position.
    cocos2d::Vec2 position;
    bool visible = true;

    int faceValue1To13() const
    {
        if (face == CardFaceType::CFT_NONE) return -1;
        return static_cast<int>(face) + 1;
    }
};

#endif

