#include "views/GameView.h"
#include "views/CardView.h"

USING_NS_CC;

GameView* GameView::create()
{
    auto ret = new (std::nothrow) GameView();
    if (ret && ret->init())
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool GameView::init()
{
    if (!Node::init()) return false;

    // Hand area layout: pile on the left, top card on the right (as in reference screenshot)
    _handPilePos = Vec2(360, 240);
    _handTopPos = Vec2(720, 240);

    auto playfieldBg = LayerColor::create(Color4B(100, 150, 200, 255), 1080, 1500);
    playfieldBg->setPosition(0, 580);
    addChild(playfieldBg);

    auto stackBg = LayerColor::create(Color4B(150, 100, 150, 255), 1080, 580);
    stackBg->setPosition(0, 0);
    addChild(stackBg);

    // Hand pile click catcher (transparent node)
    auto pileCatcher = Node::create();
    pileCatcher->setContentSize(Size(360, 360));
    pileCatcher->setPosition(_handPilePos - Vec2(pileCatcher->getContentSize().width / 2, pileCatcher->getContentSize().height / 2));
    addChild(pileCatcher, 5000);

    auto pileTouch = EventListenerTouchOneByOne::create();
    pileTouch->setSwallowTouches(true);
    pileTouch->onTouchBegan = [this, pileCatcher](Touch* t, Event*) {
        // Only treat touches on the hand-pile area as "draw" clicks.
        // IMPORTANT: This listener swallows touches, so it must NOT eat touches on playfield cards.
        return pileCatcher->getBoundingBox().containsPoint(t->getLocation());
    };
    pileTouch->onTouchEnded = [this](Touch*, Event*) {
        if (_onHandPileClick) _onHandPileClick();
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(pileTouch, pileCatcher);

    auto undoBtn = ui::Button::create();
    undoBtn->setTitleText("Undo");
    undoBtn->setScale(0.8f);
    undoBtn->setTitleFontSize(36);
    // Place undo button to the far right, centered vertically in stack area, so it won't overlap cards
    undoBtn->setPosition(Vec2(980, 290));
    undoBtn->addClickEventListener([this](Ref*) {
        if (_onUndoClick) _onUndoClick();
    });
    addChild(undoBtn, 10);

    return true;
}

void GameView::addPlayfieldCard(CardView* cardView)
{
    if (!cardView) return;
    addChild(cardView, 10);
    _cardMap[cardView->getCardId()] = cardView;

    cardView->setOnClickCallback([this](int cardId) {
        if (_onPlayfieldCardClick) _onPlayfieldCardClick(cardId);
    });
}

void GameView::addHandPileCard(CardView* cardView)
{
    if (!cardView) return;
    addChild(cardView, 100);
    _cardMap[cardView->getCardId()] = cardView;
    _handPileCards.push_back(cardView);
}

void GameView::setHandTopCard(CardView* cardView)
{
    _handTopCard = cardView;
    if (cardView)
    {
        _cardMap[cardView->getCardId()] = cardView;
        cardView->setLocalZOrder(2000);
    }
    layoutHandPile();
}

CardView* GameView::getCardView(int cardId) const
{
    auto it = _cardMap.find(cardId);
    return it == _cardMap.end() ? nullptr : it->second;
}

void GameView::syncHandFromModel(int topCardId, const std::vector<int>& pileCardIds)
{
    _handPileCards.clear();
    for (int id : pileCardIds)
    {
        auto v = getCardView(id);
        if (v) _handPileCards.push_back(v);
    }

    _handTopCard = getCardView(topCardId);
    if (_handTopCard)
    {
        _handTopCard->setLocalZOrder(2000);
    }

    layoutHandPile();
}

void GameView::layoutHandPile()
{
    // Stack fanning offsets (slight overlap)
    const float dx = -18.0f;
    const float dy = 10.0f;

    for (int i = 0; i < (int)_handPileCards.size(); ++i)
    {
        auto c = _handPileCards[i];
        if (!c) continue;
        c->setVisible(true);
        c->setPosition(_handPilePos + Vec2(dx * ((int)_handPileCards.size() - 1 - i), dy * ((int)_handPileCards.size() - 1 - i)));
        c->setLocalZOrder(100 + i);
    }

    if (_handTopCard)
    {
        _handTopCard->setVisible(true);
        _handTopCard->setPosition(_handTopPos);
        _handTopCard->setLocalZOrder(2000);
    }
}

