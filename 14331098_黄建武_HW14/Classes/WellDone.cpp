#include "WellDone.h"
#include "BreakoutScene.h"
#include "Global.h"
#include "ui/CocosGUI.h"
#define database UserDefault::getInstance()

USING_NS_CC;

Scene* WellDone::createScene() {
  // 'scene' is an autorelease object
  auto scene = Scene::create();

  // 'layer' is an autorelease object
  auto layer = WellDone::create();

  // add layer as a child to scene
  scene->addChild(layer);

  // return the scene
  return scene;
}

bool WellDone::init() {
  if (!Layer::init()) {
    return false;
  }

  auto visibleSize = Director::getInstance()->getVisibleSize();

  database->setIntegerForKey("scoreInt", database->getIntegerForKey("scoreInt") +
    database->getIntegerForKey("timeInt"));
  database->setIntegerForKey("timeInt", database->getIntegerForKey("timeInt") + 10);

  auto label = Label::createWithTTF("WELL DONE", "fonts/Marker Felt.ttf", 60);
  label->setColor(Color3B(186, 4, 0));
  label->setPosition(visibleSize.width / 2, visibleSize.height / 3 * 2);
  this->addChild(label);

  string str = "Your score: ";
  char s[10];
  itoa(database->getIntegerForKey("scoreInt"), s, 10);
  str += s;
  Label* score = Label::create(str, "fonts/STXINWEI.TTF", 40);
  score->setPosition(visibleSize.width / 2 - 10, visibleSize.height / 2);
  this->addChild(score);

  label = Label::createWithTTF("Continue", "fonts/Marker Felt.ttf", 40);
  label->setColor(Color3B(247, 147, 0));
  auto continueBtn = MenuItemLabel::create(label, CC_CALLBACK_1(WellDone::continueCallback, this));
  Menu* next = Menu::create(continueBtn, NULL);
  next->setPosition(visibleSize.width / 2 - 80, visibleSize.height / 2 - 100);
  this->addChild(next);

  label = Label::createWithTTF("Exit", "fonts/Marker Felt.ttf", 40);
  label->setColor(Color3B(247, 147, 0));
  auto exitBtn = MenuItemLabel::create(label, CC_CALLBACK_1(WellDone::exitCallback, this));
  Menu* exit = Menu::create(exitBtn, NULL);
  exit->setPosition(visibleSize.width / 2 + 90, visibleSize.height / 2 - 100);
  this->addChild(exit);

  return true;
}

void WellDone::continueCallback(Ref * pSender) {
  Director::getInstance()->replaceScene(TransitionCrossFade::create(0.4f, Breakout::createScene()));
}

void WellDone::exitCallback(Ref * pSender) {
  Director::getInstance()->end();
}