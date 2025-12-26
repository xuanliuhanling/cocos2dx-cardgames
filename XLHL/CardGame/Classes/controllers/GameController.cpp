#include "controllers/GameController.h"
#include "views/GameView.h"
#include "views/CardView.h"
#include "services/CardMatchService.h"

USING_NS_CC;

GameController::GameController() = default;

void GameController::init(GameView* gameView)
{
    _gameView = gameView;

    if (_gameView)
    {
        _gameView->setOnPlayfieldCardClick([this](int cardId) { this->onPlayfieldCardClick(cardId); });
        _gameView->setOnHandPileClick([this]() { this->onHandPileClick(); });
        _gameView->setOnUndoClick([this]() { this->onUndoClick(); });
    }
}

void GameController::startDemo()
{
    // Demo random generation (each launch random) - meets your expectation.
    // Note: positions are fixed for layout, but face/suit are randomized.
    cocos2d::RandomHelper::random_real(0.0f, 1.0f); // touch RNG to ensure init

    Vec2 pos[] = { {250,1400},{300,1200},{350,1000},{850,1400},{800,1200},{750,1000} };

    // Generate 6 consecutive faces to avoid "no move" dead-start as much as possible.
    // Example: start=5 => 5,6,7,8,9,10
    // (No wrap-around; we clamp start so start+5 <= 13)
    const int kPlayfieldCount = 6;
    int startFace = cocos2d::random(1, 13 - (kPlayfieldCount - 1));

    for (int i = 0; i < kPlayfieldCount; ++i)
    {
        int id = _nextCardId++;
        int face1To13 = startFace + i;
        int suit0To3 = cocos2d::random(0, 3);

        _gameModel.playfieldCards.push_back(CardModel());
        auto& m = _gameModel.playfieldCards.back();
        m.cardId = id;
        m.face = static_cast<CardFaceType>(face1To13 - 1);
        m.suit = static_cast<CardSuitType>(suit0To3);
        m.position = pos[i];
        m.visible = true;

        auto v = CardView::create(id, face1To13, suit0To3);
        v->setPosition(pos[i]);
        _gameView->addPlayfieldCard(v);
    }

    // Hand cards demo (random each launch): stack size 3, top is last
    // To reduce dead-end, we pick initial top around the playfield range, so there is likely a +/-1 match.
    const int kHandStackSize = 3;
    std::vector<int> handIds;
    for (int i = 0; i < kHandStackSize; ++i)
    {
        int id = _nextCardId++;
        int face1To13 = (i == kHandStackSize - 1)
            ? cocos2d::random(startFace, startFace + (kPlayfieldCount - 1))
            : cocos2d::random(1, 13);
        int suit0To3 = cocos2d::random(0, 3);

        _handAllCards.push_back({ id, face1To13, suit0To3 });
        handIds.push_back(id);

        auto v = CardView::create(id, face1To13, suit0To3);
        _gameView->addHandPileCard(v);
    }

    // Set initial top/pile from generated order
    // Pile is everything except last; top is last
    _gameModel.handTopCardId = handIds.back();
    handIds.pop_back();
    _gameModel.handPileCardIds = handIds;

    applyHandStateToView();

    // Ensure all cards have correct initial positions
    // pile cards are already in view list; layoutHandPile will place them.
}

int GameController::getHandTopFace() const
{
    return getCardFaceById(_gameModel.handTopCardId);
}

void GameController::applyHandStateToView()
{
    if (!_gameView) return;
    _gameView->syncHandFromModel(_gameModel.handTopCardId, _gameModel.handPileCardIds);
}

int GameController::getCardFaceById(int cardId) const
{
    if (auto c = _gameModel.findCardConst(cardId))
        return c->faceValue1To13();

    for (auto const& h : _handAllCards)
        if (h.cardId == cardId) return h.face1To13;

    return -1;
}


void GameController::onPlayfieldCardClick(int cardId)
{
    if (!_gameView) return;

    int topFace = getHandTopFace();
    int face = getCardFaceById(cardId);
    if (topFace < 0 || face < 0) return;

    if (!CardMatchService::canMatchByFaceDiff1(topFace, face))
        return;

    auto movedView = _gameView->getCardView(cardId);
    if (!movedView || !movedView->isVisible()) return;

    moveCardToHandTop(cardId, UndoModel::Type::PlayfieldToTop, movedView->getPosition());
}

void GameController::onHandPileClick()
{
    if (!_gameView) return;
    if (_gameModel.handPileCardIds.empty()) return;

    int movedId = _gameModel.handPileCardIds.back();
    auto movedView = _gameView->getCardView(movedId);
    if (!movedView) return;

    _gameModel.handPileCardIds.pop_back();

    moveCardToHandTop(movedId, UndoModel::Type::StackDrawToTop, movedView->getPosition());
}

void GameController::moveCardToHandTop(int movedCardId, UndoModel::Type type, const Vec2& movedFromPos)
{
    int prevTopId = _gameModel.handTopCardId;
    auto prevTopView = _gameView->getCardView(prevTopId);
    auto movedView = _gameView->getCardView(movedCardId);
    if (!movedView) return;

    Vec2 handTopPos = _gameView->getHandTopPos();

    // record undo (snapshot hand state)
    UndoModel record;
    record.type = type;
    record.movedCardId = movedCardId;
    record.movedFromPos = movedFromPos;
    record.movedToPos = handTopPos;
    record.prevHandTopCardId = _gameModel.handTopCardId;
    record.prevHandPileCardIds = _gameModel.handPileCardIds;

    // For playfield card, remember its model state so undo can fully restore
    if (type == UndoModel::Type::PlayfieldToTop)
    {
        if (auto m = _gameModel.findCard(movedCardId))
        {
            record.hadPlayfieldModel = true;
            record.playfieldPos = m->position;
            record.playfieldVisible = m->visible;
        }
    }

    // update model
    // - new top becomes movedCardId
    // - for playfield-to-top: old top goes into pile (typical TriPeaks-like behavior)
    // - for stack-draw-to-top: we use "ROTATE" behavior (user choice A): old top goes into pile so handTop/handPile can swap
    _gameModel.handTopCardId = movedCardId;
    if (record.prevHandTopCardId >= 0)
    {
        _gameModel.handPileCardIds.push_back(record.prevHandTopCardId);
    }

    // If moved card was from pile, remove it from pile list already done by caller.
    // If moved card was from playfield, mark it as no longer on playfield.
    if (type == UndoModel::Type::PlayfieldToTop)
    {
        // move card model from playfieldCards -> movedPlayfieldCards
        for (auto it = _gameModel.playfieldCards.begin(); it != _gameModel.playfieldCards.end(); ++it)
        {
            if (it->cardId == movedCardId)
            {
                _gameModel.movedPlayfieldCards.push_back(*it);
                _gameModel.playfieldCards.erase(it);
                break;
            }
        }
    }

    record.nextHandTopCardId = _gameModel.handTopCardId;
    record.nextHandPileCardIds = _gameModel.handPileCardIds;
    _undoManager.push(record);

    // update view
    if (prevTopView) prevTopView->stopAllActions();

    movedView->stopAllActions();

    // IMPORTANT:
    // Do NOT call applyHandStateToView() immediately here, because syncHandFromModel/layoutHandPile
    // will snap the moving card to handTopPos and fight with MoveTo, causing "messy" movement.
    // We let the MoveTo finish, then sync layout.
    movedView->runAction(Sequence::create(
        MoveTo::create(0.25f, handTopPos),
        CallFunc::create([this]() {
            this->applyHandStateToView();
        }),
        nullptr));

    // When a playfield card becomes the new hand-top card, we should keep it visible
    // and show it at the hand-top position (requirement 2: move to top and replace).
    // Its clickability is handled by model ownership (it is removed from playfieldCards).
}

void GameController::onUndoClick()
{
    if (!_gameView) return;

    UndoModel record;
    if (!_undoManager.pop(record)) return;

    // restore model hand state exactly
    _gameModel.handTopCardId = record.prevHandTopCardId;
    _gameModel.handPileCardIds = record.prevHandPileCardIds;

    // restore playfield ownership/state if needed
    if (record.type == UndoModel::Type::PlayfieldToTop)
    {
        // move card model back from movedPlayfieldCards -> playfieldCards
        for (auto it = _gameModel.movedPlayfieldCards.begin(); it != _gameModel.movedPlayfieldCards.end(); ++it)
        {
            if (it->cardId == record.movedCardId)
            {
                if (record.hadPlayfieldModel)
                {
                    it->position = record.playfieldPos;
                    it->visible = record.playfieldVisible;
                }
                _gameModel.playfieldCards.push_back(*it);
                _gameModel.movedPlayfieldCards.erase(it);
                break;
            }
        }
    }

    // restore view + animate reverse
    auto movedView = _gameView->getCardView(record.movedCardId);
    if (movedView)
    {
        movedView->stopAllActions();
        movedView->setVisible(true);

        // Same as forward move: do not sync hand layout immediately, otherwise layout may fight with the undo MoveTo.
        movedView->runAction(Sequence::create(
            MoveTo::create(0.25f, record.movedFromPos),
            CallFunc::create([this, record, movedView]() {
                this->applyHandStateToView();

                // For PlayfieldToTop undo, ensure the moved card ends at its original playfield position.
                if (record.type == UndoModel::Type::PlayfieldToTop && movedView)
                {
                    movedView->stopAllActions();
                    movedView->setVisible(true);
                    movedView->setPosition(record.movedFromPos);
                }
            }),
            nullptr));
    }
    else
    {
        // If the view is missing (shouldn't happen), still restore layout.
        applyHandStateToView();
    }
}

