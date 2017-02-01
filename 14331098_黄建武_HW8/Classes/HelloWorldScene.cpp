#include "HelloWorldScene.h"
#pragma execution_character_set("utf-8")
USING_NS_CC;
using namespace std;

int HelloWorld::bgnum = 1;

Scene* HelloWorld::createScene() {
  // 'scene' is an autorelease object
  auto scene = Scene::create();

  // 'layer' is an autorelease object
  auto layer = HelloWorld::create();

  // add layer as a child to scene
  scene->addChild(layer);

  // return the scene
  return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init() {
  //////////////////////////////
  // 1. super init first
  if (!Layer::init()) {
    return false;
  }

  Size visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  /////////////////////////////
  // 2. add a menu item with "X" image, which is clicked to quit the program
  //    you may modify it.

  // add a "close" icon to exit the progress. it's an autorelease object
  auto closeItem = MenuItemImage::create(
    "close.png",
    "closeSelected.png",
    CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

  closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width,
    origin.y + closeItem->getContentSize().height));

  // create menu, it's an autorelease object
  auto menu = Menu::create(closeItem, NULL);
  menu->setPosition(Vec2::ZERO);
  this->addChild(menu, 1);

  auto changeBgBtn = MenuItemImage::create(
    "change.png",
    "changeSelected.png",
    CC_CALLBACK_1(HelloWorld::menuChangeBg, this));
  changeBgBtn->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
  auto menu2 = Menu::create(changeBgBtn, NULL);
  menu2->setPosition(Vec2::ZERO);
  this->addChild(menu2, 1);

  /////////////////////////////
  // 3. add your codes below...

  // add a label
  // create and initialize a label
  auto name = Label::createWithSystemFont("ÄúºÃ,»Æ½¨Îä", "Microsoft Yahei UI", 20);
  // position the label on the center of the screen
  name->setPosition(Vec2(origin.x + visibleSize.width / 2,
    origin.y + visibleSize.height - name->getContentSize().height));

  // add the label as a child to this layer
  this->addChild(name, 1);

  auto stuNum = Label::createWithTTF("14331098", "fonts/Marker Felt.ttf", 20);
  stuNum->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - name->getContentSize().height - 30));
  this->addChild(stuNum, 2);
 
  bg = Sprite::create("1.jpg");
  // position the sprite on the center of the screen
  bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

  // add the sprite as a child to this layer
  this->addChild(bg, 0);

  return true;
}

void HelloWorld::menuCloseCallback(Ref* pSender) {
  Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
  exit(0);
#endif
}

void HelloWorld::menuChangeBg(Ref* pSender){
  if (bgnum == 7) bgnum = 1;
  else bgnum++;
  string bgname = "";
  char num[3];
  itoa(bgnum, num, 10);
  bgname += num;
  bgname += ".jpg";
  CCTexture2D * texture = CCTextureCache::sharedTextureCache()->addImage(bgname);
  bg->setTexture(texture);
}
