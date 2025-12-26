#ifndef __GAME_VIEW_H__
#define __GAME_VIEW_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include <functional>
#include <unordered_map>

class CardView;

/**
 * @brief Root view for the game. Responsible for layout, animations and forwarding UI events.
 */
class GameView : public cocos2d::Node
{
public:
    static GameView* create();
    bool init() override;

    void setOnPlayfieldCardClick(const std::function<void(int)>& cb) { _onPlayfieldCardClick = cb; }
    void setOnHandPileClick(const std::function<void()>& cb) { _onHandPileClick = cb; }
    void setOnUndoClick(const std::function<void()>& cb) { _onUndoClick = cb; }

    // View building
    void addPlayfieldCard(CardView* cardView);
    void addHandPileCard(CardView* cardView);
    void setHandTopCard(CardView* cardView);

    // Sync hand area views with model-owned id lists.
    // This makes undo deterministic (pile order restored exactly by model state).
    void syncHandFromModel(int topCardId, const std::vector<int>& pileCardIds);

    // Layout/animation helpers
    cocos2d::Vec2 getHandTopPos() const { return _handTopPos; }
    cocos2d::Vec2 getHandPilePos() const { return _handPilePos; }

    void layoutHandPile();

    CardView* getCardView(int cardId) const;

private:
    std::function<void(int)> _onPlayfieldCardClick;
    std::function<void()> _onHandPileClick;
    std::function<void()> _onUndoClick;

    cocos2d::Vec2 _handTopPos;
    cocos2d::Vec2 _handPilePos;

    CardView* _handTopCard = nullptr;
    std::vector<CardView*> _handPileCards;

    std::unordered_map<int, CardView*> _cardMap;
};

#endif

