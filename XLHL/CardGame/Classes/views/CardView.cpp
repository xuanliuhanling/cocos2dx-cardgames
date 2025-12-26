#include "views/CardView.h"

USING_NS_CC;

static const Size CARD_SIZE(120, 160);

CardView* CardView::create(int cardId, int face1To13, int suit0To3)
{
    auto ret = new (std::nothrow) CardView();
    if (ret && ret->init(cardId, face1To13, suit0To3))
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool CardView::init(int cardId, int face1To13, int suit0To3)
{
    if (!Node::init()) return false;

    _cardId = cardId;
    _root = makeCardSprite(face1To13, suit0To3);
    addChild(_root);

    setContentSize(_root->getContentSize());
    _root->setPosition(getContentSize() / 2);

    auto lis = EventListenerTouchOneByOne::create();
    lis->setSwallowTouches(true);
    lis->onTouchBegan = [this](Touch* t, Event*) {
        return this->getBoundingBox().containsPoint(t->getLocation());
    };
    lis->onTouchEnded = [this](Touch*, Event*) {
        if (_onClick) _onClick(_cardId);
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(lis, this);

    return true;
}

std::string CardView::bigNumberFile(int value, bool isRed)
{
    std::string key;
    switch (value)
    {
    case 1: key = "A"; break;
    case 11: key = "J"; break;
    case 12: key = "Q"; break;
    case 13: key = "K"; break;
    default: key = StringUtils::format("%d", value); break;
    }
    return "res/number/big_" + std::string(isRed ? "red_" : "black_") + key + ".png";
}

Sprite* CardView::makeCardSprite(int value, int suit)
{
    bool isRed = (suit == 1 || suit == 2);

    auto bg = Sprite::create("res/card_general.png");
    if (!bg)
    {
        bg = Sprite::create();
        bg->setTextureRect(Rect(0, 0, CARD_SIZE.width, CARD_SIZE.height));
        bg->setColor(Color3B::WHITE);
    }

    auto bigNum = Sprite::create(bigNumberFile(value, isRed));

    std::string suitFile;
    switch (suit)
    {
    case 0: suitFile = "res/suits/club.png"; break;
    case 1: suitFile = "res/suits/diamond.png"; break;
    case 2: suitFile = "res/suits/heart.png"; break;
    case 3: suitFile = "res/suits/spade.png"; break;
    default: break;
    }
    auto suitSp = suitFile.empty() ? nullptr : Sprite::create(suitFile);

    auto node = Sprite::create();
    node->setContentSize(bg->getContentSize());
    node->addChild(bg);
    bg->setPosition(node->getContentSize() / 2);

    if (bigNum)
    {
        node->addChild(bigNum);
        bigNum->setPosition(node->getContentSize().width * 0.5f, node->getContentSize().height * 0.55f);
    }
    if (suitSp)
    {
        node->addChild(suitSp);
        suitSp->setPosition(node->getContentSize().width * 0.25f, node->getContentSize().height * 0.65f);
        suitSp->setScale(0.6f);
    }

    return node;
}


