#ifndef __GAME_CONTROLLER_H__
#define __GAME_CONTROLLER_H__

#include <memory>
#include "models/GameModel.h"
#include "managers/UndoManager.h"

class GameView;

/**
 * @brief Main controller coordinating model and view.
 */
class GameController
{
public:
    GameController();

    void init(GameView* gameView);

    // Demo start (in real project startGame(levelId) + config loading)
    void startDemo();

    void onPlayfieldCardClick(int cardId);
    void onHandPileClick();
    void onUndoClick();

private:
    GameModel _gameModel;
    UndoManager _undoManager;

    GameView* _gameView = nullptr; // view is owned by scene graph

    // card registry for hand cards (since simplified GameModel only stores playfieldCards)
    struct HandCardRuntime
    {
        int cardId;
        int face1To13;
        int suit0To3;
    };
    std::vector<HandCardRuntime> _handAllCards;

    int _nextCardId = 1;

    int getHandTopFace() const;
    int getCardFaceById(int cardId) const;

    void applyHandStateToView();

    void moveCardToHandTop(int movedCardId, UndoModel::Type type, const cocos2d::Vec2& movedFromPos);
};

#endif

