#ifndef __UNDO_MODEL_H__
#define __UNDO_MODEL_H__

#include "cocos2d.h"
#include <vector>

/**
 * @brief Data model for undo records.
 * Stores enough information to restore both model state and play a reverse MoveTo animation.
 */
struct UndoModel
{
    enum class Type
    {
        StackDrawToTop,
        PlayfieldToTop
    };

    Type type = Type::StackDrawToTop;

    // Hand state snapshots (ids) BEFORE and AFTER the action.
    // Using model-owned ids makes undo deterministic and future-proof.
    int prevHandTopCardId = -1;
    std::vector<int> prevHandPileCardIds;

    int nextHandTopCardId = -1;
    std::vector<int> nextHandPileCardIds;

    // Card moved to top by this action
    int movedCardId = -1;

    // For reverse animation (simple MoveTo)
    cocos2d::Vec2 movedFromPos;
    cocos2d::Vec2 movedToPos;

    // Only for PlayfieldToTop: restore playfield card state (position/visibility)
    bool hadPlayfieldModel = false;
    cocos2d::Vec2 playfieldPos;
    bool playfieldVisible = true;
};

#endif

