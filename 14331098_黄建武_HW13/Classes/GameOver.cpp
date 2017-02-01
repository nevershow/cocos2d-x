#include "GameOver.h"
#include "BreakoutScene.h"
#include <algorithm>
#define database UserDefault::getInstance()

USING_NS_CC;

Scene* GameOver::createScene() {
  // 'scene' is an autorelease object
  auto scene = Scene::create();

  // 'layer' is an autorelease object
  auto layer = GameOver::create();

  // add layer as a child to scene
  scene->addChild(layer);

  // return the scene
  return scene;
}

bool GameOver::init() {
  if (!Layer::init()) {
    return false;
  }

  auto visibleSize = Director::getInstance()->getVisibleSize();

  auto over = Sprite::create("gameOver.png");
  over->setPosition(visibleSize.width / 2, visibleSize.height / 2);
  this->addChild(over, 1);
  
  auto label = Label::createWithTTF("Replay", "fonts/Marker Felt.ttf", 40);
  label->setColor(Color3B(247, 147, 0));
  auto replayBtn = MenuItemLabel::create(label, CC_CALLBACK_1(GameOver::replayCallback, this));
  Menu* replay = Menu::create(replayBtn, NULL);
  replay->setPosition(visibleSize.width / 2 - 80, visibleSize.height / 2 - 100);
  this->addChild(replay);

  label = Label::createWithTTF("Exit", "fonts/Marker Felt.ttf", 40);
  label->setColor(Color3B(247, 147, 0));
  auto exitBtn = MenuItemLabel::create(label, CC_CALLBACK_1(GameOver::exitCallback, this));
  Menu* exit = Menu::create(exitBtn, NULL);
  exit->setPosition(visibleSize.width / 2 + 80, visibleSize.height / 2 - 100);
  this->addChild(exit);

  return true;
}

void GameOver::replayCallback(Ref * pSender) {
  Director::getInstance()->replaceScene(TransitionCrossFade::create(0.4f, Breakout::createScene()));
}

void GameOver::exitCallback(Ref * pSender) {
  database->setBoolForKey("isExist", false);
  Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
  exit(0);
#endif
}