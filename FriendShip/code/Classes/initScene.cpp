#include "initScene.h"
#include "FriendShip.h"
#pragma execution_character_set("utf-8")
#define database UserDefault::getInstance()

Scene* initScene::createScene() {
  // 'scene' is an autorelease object
  auto scene = Scene::create();

  // 'layer' is an autorelease object
  auto layer = initScene::create();

  // add layer as a child to scene
  scene->addChild(layer);

  // return the scene
  return scene;
}

// on "init" you need to initialize your instance
bool initScene::init() {
  //////////////////////////////
  // 1. super init first
  if (!Layer::init()) {
    return false;
  }
  visibleSize = Director::getInstance()->getVisibleSize();

  // add background
  auto bgSprite = Sprite::create("bg.png");
  bgSprite->setPosition(visibleSize / 2);
  bgSprite->setScale(visibleSize.width / bgSprite->getContentSize().width, visibleSize.height / bgSprite->getContentSize().height);
  this->addChild(bgSprite, 0);

  auto logo = Sprite::create("logo.png");
  logo->setPosition(visibleSize.width / 2, visibleSize.height - 150);
  logo->setScale(0.4);
  this->addChild(logo, 0);

  auto label1 = Label::createWithTTF("单身独木舟", "fonts/STXINWEI.TTF", 50);
  label1->setColor(Color3B(0, 0, 0));
  auto onePBtn = MenuItemLabel::create(label1, CC_CALLBACK_1(initScene::OneP, this));
  Menu* oneP = Menu::create(onePBtn, NULL);
  oneP->setPosition(visibleSize.width / 2, visibleSize.height / 2 - 50);
  this->addChild(oneP);

  auto label2 = Label::createWithTTF("友谊的小船", "fonts/STXINWEI.TTF", 50);
  label2->setColor(Color3B(0, 0, 0));
  auto twoPBtn = MenuItemLabel::create(label2, CC_CALLBACK_1(initScene::TwoP, this));
  Menu* twoP = Menu::create(twoPBtn, NULL);
  twoP->setPosition(visibleSize.width / 2, visibleSize.height / 2 - 150);
  this->addChild(twoP);

  return true;
}

void initScene::OneP(Ref * sender) {
  database->setBoolForKey("twoP", false);
  Director::getInstance()->replaceScene(TransitionCrossFade::create(0.1f, FriendShip::createScene()));
}

void initScene::TwoP(Ref * sender) {
  database->setBoolForKey("twoP", true);
  Director::getInstance()->replaceScene(TransitionCrossFade::create(0.1f, FriendShip::createScene()));
}
