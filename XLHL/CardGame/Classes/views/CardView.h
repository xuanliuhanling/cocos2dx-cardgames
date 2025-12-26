#ifndef __CARD_VIEW_H__
#define __CARD_VIEW_H__

#include "cocos2d.h"
#include <functional>

/**
 * @brief Visual component for a card. Only displays and forwards input events.
 */
class CardView : public cocos2d::Node
{
public:
    static CardView* create(int cardId, int face1To13, int suit0To3);
    bool init(int cardId, int face1To13, int suit0To3);

    int getCardId() const { return _cardId; }

    void setOnClickCallback(const std::function<void(int)>& cb) { _onClick = cb; }

private:
    int _cardId = -1;
    std::function<void(int)> _onClick;

    cocos2d::Sprite* _root = nullptr;

    static std::string bigNumberFile(int value, bool isRed);
    static cocos2d::Sprite* makeCardSprite(int value, int suit);
};

#endif


