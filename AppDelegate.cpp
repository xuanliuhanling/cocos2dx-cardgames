#include "AppDelegate.h"
#include "GameScene.h"

USING_NS_CC;

AppDelegate::AppDelegate() {}
AppDelegate::~AppDelegate() {}

void AppDelegate::initGLContextAttrs() {
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};
    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() {
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLViewImpl::createWithRect("CardGame", Rect(0, 0, 1080, 2080), 0.5);
        director->setOpenGLView(glview);
    }
    director->setDisplayStats(true);
    director->setAnimationInterval(1.0f / 60);
    glview->setDesignResolutionSize(1080, 2080, ResolutionPolicy::FIXED_WIDTH);
    
    auto scene = GameScene::create();
    // 添加资源搜索路径
    auto fu = FileUtils::getInstance();
    // 1) 优先查找 exe 同级 res 目录
    fu->addSearchPath("res");
    // 2) 若开发阶段未被拷贝，尝试相对路径跳到仓库根 res 目录
    fu->addSearchPath("..\\..\\..\\..\\res");
    director->runWithScene(scene);
    return true;
}

void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
}

void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();
}
