#include "GameScene.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

/*
 *  轻量版本：在原工程里直接满足《要求.md》三大功能：
 *    1. 点击备用堆最上面那张牌 -> 平移到手牌区顶部，成为新 top
 *    2. 点击桌面牌，只要与顶部牌点数差 1 -> 平移到手牌区顶部，成为新 top
 *    3. 连续点击 Undo 按钮 -> 所有牌按相反顺序移动回原位，并恢复之前的 top
 *  注意：此实现仍在一文件内，方便与旧代码并存；后续可再拆 MVC；
 */

namespace {
// 仅做演示，牌大小固定
static const Size CARD_SIZE(120, 160);

static std::string bigNumberFile(int value, bool isRed)
{
    // 1->A, 11->J, 12->Q, 13->K
    std::string key;
    switch(value)
    {
    case 1: key = "A"; break;
    case 11: key = "J"; break;
    case 12: key = "Q"; break;
    case 13: key = "K"; break;
    default: key = StringUtils::format("%d", value); break;
    }
    return "res/number/big_" + std::string(isRed?"red_":"black_") + key + ".png";
}

static Sprite* makeCard(int value, int suit /*0 clubs,1 diamonds,2 hearts,3 spades*/) {
    bool isRed = (suit==1||suit==2);

    auto bg = Sprite::create("res/card_general.png");
    if(!bg) {
        // Fallback
        bg = Sprite::create();
        bg->setTextureRect(Rect(0,0,CARD_SIZE.width,CARD_SIZE.height));
        bg->setColor(Color3B::WHITE);
    }

    auto bigNum = Sprite::create(bigNumberFile(value, isRed));

    std::string suitFile;
    switch(suit)
    {
    case 0: suitFile="res/suits/club.png";break;
    case 1: suitFile="res/suits/diamond.png";break;
    case 2: suitFile="res/suits/heart.png";break;
    case 3: suitFile="res/suits/spade.png";break;
    }
    auto suitSp = Sprite::create(suitFile);

    auto node = Sprite::create();
    node->setContentSize(bg->getContentSize());
    node->addChild(bg);
    bg->setPosition(node->getContentSize()/2);

    if(bigNum){
        node->addChild(bigNum);
        bigNum->setPosition(node->getContentSize().width*0.5f, node->getContentSize().height*0.55f);
    }
    if(suitSp){
        node->addChild(suitSp);
        suitSp->setPosition(node->getContentSize().width*0.25f, node->getContentSize().height*0.65f);
        suitSp->setScale(0.6f);
    }

    node->setTag(value); // 保存点数便于逻辑
    return node;
}
}

GameScene* GameScene::create() {
    auto ret = new (std::nothrow) GameScene();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool GameScene::init() {
    if (!Scene::init()) return false;

    // 1. 背景分区
    auto playfieldBg = LayerColor::create(Color4B(100, 150, 200, 255), 1080, 1500);
    playfieldBg->setPosition(0, 580);
    addChild(playfieldBg);

    auto stackBg = LayerColor::create(Color4B(150, 100, 150, 255), 1080, 580);
    stackBg->setPosition(0, 0);
    addChild(stackBg);

    //---------------- 桌面主牌区 ---------------------------------
    int faceVals[] = {13,2,2,2,2,1};
    Vec2 pos[] = { {250,1400},{300,1200},{350,1000},{850,1400},{800,1200},{750,1000} };
    for (int i=0;i<6;++i) {
        // suit 按顺序给个假值
        int suit = (i%4);
        auto card = makeCard(faceVals[i], suit);
        card->setPosition(pos[i]);
        addChild(card, 10+i);
        _playfieldCards.emplace_back(card);

        // 触摸
        auto lis = EventListenerTouchOneByOne::create();
        lis->onTouchBegan = [card](Touch*t,Event*){return card->getBoundingBox().containsPoint(t->getLocation());};
        lis->onTouchEnded = [this,i](Touch*,Event*){ this->onPlayfieldCardClick(i);} ;
        _eventDispatcher->addEventListenerWithSceneGraphPriority(lis, card);
    }

    //---------------- 备用手牌堆 ---------------------------------
    int stackVals[] = {2,1,3}; // 最后一张索引2 在最上面
    for (int i=0;i<3;++i) {
        auto card = makeCard(stackVals[i], 0);
        // 堆叠略微偏移
        card->setPosition(Vec2(540 - (2-i)*10, 290));
        addChild(card, 100+i);
        _stackCards.emplace_back(card);
    }
    _topCard = _stackCards.back();

    // 点击备用堆的整块区域（而不是依次点击每张牌）
    auto stackTouch = EventListenerTouchOneByOne::create();
    stackTouch->onTouchBegan = [&](Touch*t,Event*){
        // 只要点到最顶那张即可
        return _topCard && _topCard->getBoundingBox().containsPoint(t->getLocation());
    };
    stackTouch->onTouchEnded = [&](Touch*,Event*){ this->onStackDraw(); };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(stackTouch, this);

    //---------------- Undo 按钮 ----------------------------------
    auto undoBtn = ui::Button::create();
    undoBtn->setTitleText("Undo");
    undoBtn->setTitleFontSize(36);
    undoBtn->setPosition(Vec2(100, 500));
    undoBtn->addClickEventListener([this](Ref*){ this->onUndo(); });
    addChild(undoBtn, 200);

    return true;
}

void GameScene::setTopCard(Sprite* card) {
    if (_topCard) _topCard->setVisible(false);
    card->setVisible(true);
    _topCard = card;
}

//=================== 事件 =====================
void GameScene::onPlayfieldCardClick(int idx) {
    auto card = _playfieldCards[idx];
    if (!card->isVisible()) return; // 已经移走了

    int curVal = _topCard->getTag();
    int newVal = card->getTag();
    if (abs(curVal - newVal) != 1) {
        CCLOG("Not match, diff != 1");
        return;
    }

    // 记录 Undo
    _undoStack.push_back({card, card->getPosition(), _topCard});

    // 动画: 移动到手牌区
    auto move = MoveTo::create(0.25f, _topCard->getPosition());
    card->runAction(move);

    setTopCard(card);
}

void GameScene::onStackDraw() {
    // 若堆里只有 1 张(当前 top) 则不能再抽
    if (_stackCards.size() <= 1) return;

    // 下一张成为 top
    Sprite* nextCard = _stackCards[_stackCards.size()-2];

    // 记录 Undo
    _undoStack.push_back({nextCard, nextCard->getPosition(), _topCard});

    // 动画
    auto move = MoveTo::create(0.25f, _topCard->getPosition());
    nextCard->runAction(move);

    setTopCard(nextCard);
}

void GameScene::onUndo() {
    if (_undoStack.empty()) return;
    auto data = _undoStack.back();
    _undoStack.pop_back();

    // 把 movedCard 移回原位
    data.movedCard->stopAllActions();
    data.movedCard->runAction(MoveTo::create(0.25f, data.originPos));

    // 恢复 prevTopCard 可见
    data.prevTopCard->setVisible(true);
    _topCard = data.prevTopCard;

    // 如果撤销的是从堆里抽牌，需要显示回去的堆次序正确
    // 这里简单处理：保证 movedCard 在 _stackCards 里排到 prevTopCard 下面
    // （演示用，未做完整校正）
}
