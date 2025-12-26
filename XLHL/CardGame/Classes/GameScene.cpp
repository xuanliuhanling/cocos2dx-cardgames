#include "GameScene.h"

#include <memory>

#include "controllers/GameController.h"
#include "views/GameView.h"

USING_NS_CC;

GameScene* GameScene::create()
{
    auto ret = new (std::nothrow) GameScene();
    if (ret && ret->init())
    {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool GameScene::init()
{
    if (!Scene::init()) return false;

    _root = Node::create();
    addChild(_root);

    auto gameView = GameView::create();
    _root->addChild(gameView);

    // Controller is intentionally NOT a Node/Ref; keep it alive for scene lifetime.
    auto controller = std::make_shared<GameController>();
    controller->init(gameView);
    controller->startDemo();

    // NOTE: simplest approach: store shared_ptr in static for demo; replace with member variable in production.
    static std::shared_ptr<GameController> s_controllerKeeper;
    s_controllerKeeper = controller;

    return true;
}
