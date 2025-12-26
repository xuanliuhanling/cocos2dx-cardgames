#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"

class GameScene : public cocos2d::Scene {
public:
    static GameScene* create();
    virtual bool init();

private:
    // 事件处理
    void onPlayfieldCardClick(int idx);
    void onStackDraw();
    void onUndo();

    // 工具
    void setTopCard(cocos2d::Sprite* card);

    // 数据
    std::vector<cocos2d::Sprite*> _playfieldCards; // 桌面纸牌
    std::vector<cocos2d::Sprite*> _stackCards;     // 备用牌堆（翻面朝上）
    cocos2d::Sprite* _topCard = nullptr;           // 当前手牌区顶部牌

    struct UndoData {
        cocos2d::Sprite* movedCard = nullptr;  // 被移动的牌
        cocos2d::Vec2     originPos;           // 移动前的位置
        cocos2d::Sprite* prevTopCard = nullptr; // 被替换掉的顶部牌（回退时恢复）
    };
    std::vector<UndoData> _undoStack;
};

#endif
