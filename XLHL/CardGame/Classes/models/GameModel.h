#ifndef __GAME_MODEL_H__
#define __GAME_MODEL_H__

#include <vector>
#include "models/CardModel.h"

/**
 * @brief Runtime game model containing playfield cards and stack/hand cards.
 */
class GameModel
{
public:
    std::vector<CardModel> playfieldCards;

    // Cards moved from playfield to hand top (so they are no longer clickable on playfield)
    // We keep full CardModel to restore position/visibility on undo.
    std::vector<CardModel> movedPlayfieldCards;

    // Hand area split:
    // - handTopCardId: current top card used for matching
    // - handPileCardIds: remaining stack cards (draw from back as pile top)
    int handTopCardId = -1;
    std::vector<int> handPileCardIds;

    CardModel* findCard(int cardId)
    {
        for (auto &c : playfieldCards) if (c.cardId == cardId) return &c;
        for (auto &c : movedPlayfieldCards) if (c.cardId == cardId) return &c;
        // stack cards are not in these vectors in this simplified model.
        return nullptr;
    }

    const CardModel* findCardConst(int cardId) const
    {
        for (auto const& c : playfieldCards) if (c.cardId == cardId) return &c;
        for (auto const& c : movedPlayfieldCards) if (c.cardId == cardId) return &c;
        return nullptr;
    }
};

#endif

