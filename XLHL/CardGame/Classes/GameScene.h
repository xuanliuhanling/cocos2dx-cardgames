#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"

/**
 * @brief Scene entry. Only responsible for composing controller + view and starting the game.
 */
class GameScene : public cocos2d::Scene
{
public:
    static GameScene* create();
    bool init() override;

private:
    cocos2d::Node* _root = nullptr;
};

#endif
