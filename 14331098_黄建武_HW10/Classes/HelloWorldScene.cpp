#include "HelloWorldScene.h"
#pragma execution_character_set("utf-8")
USING_NS_CC;
using namespace std;

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
   this->
   visibleSize = Director::getInstance()->getVisibleSize();
   origin = Director::getInstance()->getVisibleOrigin();

   auto bg = Sprite::create("bg.jpg");
   bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
   float spy = bg->getTextureRect().getMaxY();
   bg->setScaleX(visibleSize.height / spy); //设置宽度缩放比例
   bg->setScaleY(visibleSize.height / spy); //设置高度缩放比例
   this->addChild(bg, 0);

   TTFConfig ttfConfig;
   ttfConfig.fontFilePath = "fonts/consola.ttf";
   ttfConfig.fontSize = 36;

   time = Label::createWithTTF(ttfConfig, "01:30");   //倒计时
   schedule(schedule_selector(HelloWorld::updateTime), 1.0f, kRepeatForever, 0);  //倒计时周期性调用调度器
   dtime = 90;    //剩余时间
   time->setPosition(Vec2(origin.x + visibleSize.width / 2,
                          origin.y + visibleSize.height - time->getContentSize().height));
   time->setColor(Color3B(0, 153, 255));
   this->addChild(time, 1);

   //hp条
   Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
   auto scale = visibleSize.width / 2.4 / sp0->getContentSize().width;
   sp0->setScaleX(scale);
   sp0->setScaleY(scale);
   Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

   //使用hp条设置progressBar
   pT = ProgressTimer::create(sp);
   pT->setScaleY(scale);
   pT->setScaleX(88.8 * scale);

   pT->setAnchorPoint(Vec2(0, 0));
   pT->setType(ProgressTimerType::BAR);
   pT->setBarChangeRate(Point(1, 0));
   pT->setMidpoint(Point(0, 1));
   pT->setPercentage(100);
   pT->setPosition(Vec2(origin.x + 61, origin.y + visibleSize.height - 42));
   this->addChild(pT, 1);

   sp0->setAnchorPoint(Vec2(0, 0));
   sp0->setPosition(Vec2(origin.x + 10, origin.y + visibleSize.height - 57));
   this->addChild(sp0, 0);

   //创建一张贴图
   auto texture = Director::getInstance()->getTextureCache()->addImage("$lucia_forward.png");
   //从贴图中以像素单位切割，创建关键帧
   frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 68, 101)));
   //使用第一帧创建精灵
   player = Sprite::createWithSpriteFrame(frame0);
   player->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
   this->addChild(player, 2);

   //运动动画
   for (int i = 0; i < 8; i++) {
      auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(68 * i, 0, 68, 101)));
      run.pushBack(frame);
   }
   run.pushBack(frame0);

   auto texture2 = Director::getInstance()->getTextureCache()->addImage("$lucia_2.png");
   //攻击动画
   attack.reserve(17);
   for (int i = 0; i < 17; i++) {
      auto frame = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(113 * i, 0, 113, 113)));
      attack.pushBack(frame);
   }
   attack.pushBack(frame0);

   //死亡动画
   auto texture3 = Director::getInstance()->getTextureCache()->addImage("$lucia_dead.png");
   dead.reserve(22);
   for (int i = 0; i < 22; i++) {
      auto frame = SpriteFrame::createWithTexture(texture3, CC_RECT_PIXELS_TO_POINTS(Rect(79 * i, 0, 79, 90)));
      dead.pushBack(frame);
   }
   dead.pushBack(frame0);

   //Label
   auto menuLabel1 = Label::createWithTTF(ttfConfig, "W");
   auto menuLabel2 = Label::createWithTTF(ttfConfig, "S");
   auto menuLabel3 = Label::createWithTTF(ttfConfig, "A");
   auto menuLabel4 = Label::createWithTTF(ttfConfig, "D");
   auto menuLabel5 = Label::createWithTTF(ttfConfig, "X");
   auto menuLabel6 = Label::createWithTTF(ttfConfig, "Y");
   //menuItem
   auto item1 = MenuItemLabel::create(menuLabel1, CC_CALLBACK_1(HelloWorld::moveEvent, this, 'W'));
   auto item2 = MenuItemLabel::create(menuLabel2, CC_CALLBACK_1(HelloWorld::moveEvent, this, 'S'));
   auto item3 = MenuItemLabel::create(menuLabel3, CC_CALLBACK_1(HelloWorld::moveEvent, this, 'A'));
   auto item4 = MenuItemLabel::create(menuLabel4, CC_CALLBACK_1(HelloWorld::moveEvent, this, 'D'));
   auto item5 = MenuItemLabel::create(menuLabel5, CC_CALLBACK_1(HelloWorld::actionEvent, this, 'X'));
   auto item6 = MenuItemLabel::create(menuLabel6, CC_CALLBACK_1(HelloWorld::actionEvent, this, 'Y'));
   item3->setPosition(Vec2(origin.x + item3->getContentSize().width * 2, origin.y + item3->getContentSize().height * 2));
   item4->setPosition(Vec2(item3->getPosition().x + 3 * item4->getContentSize().width, item3->getPosition().y));
   item2->setPosition(Vec2(item3->getPosition().x + 1.5 * item2->getContentSize().width, item3->getPosition().y));
   item1->setPosition(Vec2(item2->getPosition().x, item2->getPosition().y + item1->getContentSize().height));
   item5->setPosition(Vec2(origin.x + visibleSize.width - item5->getContentSize().width * 3, item1->getPosition().y));
   item6->setPosition(Vec2(item5->getPosition().x - item6->getContentSize().width, item3->getPosition().y));

   auto menu = Menu::create(item1, item2, item3, item4, item5, item6, NULL);
   menu->setPosition(Vec2(0, 0));
   this->addChild(menu, 1);

   // 键盘响应事件
   auto keyListener = EventListenerKeyboard::create();
   keyListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event * event) {
      //W:146 ↑:28
      //A:124 ←:26
      //S:142 ↓:29
      //D:127 →:27
      //X:147 Y:148
      switch ((int)keyCode) {
      case 146: case 28:
         this->moveEvent(this, 'W');
         break;
      case 124: case 26:
         this->moveEvent(this, 'A');
         break;
      case 142: case 29:
         this->moveEvent(this, 'S');
         break;
      case 127: case 27:
         this->moveEvent(this, 'D');
         break;
      case 147:
         this->actionEvent(this, 'X');
         break;
      case 148:
         this->actionEvent(this, 'Y');
         break;
      }
   };
   this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyListener, this);

   return true;
}


void HelloWorld::moveEvent(Ref*, char cid) {
   if (player->getSpriteFrame() == frame0 && pT->getPercentage()) {
      auto nowPos = player->getPosition();
      switch (cid) {
      case 'W':
         runkAnimate = Animate::create(Animation::createWithSpriteFrames(run, 0.05f, 1));
         player->runAction(Spawn::create(runkAnimate, MoveBy::create(0.45f, Vec2(0, min(visibleSize.height - nowPos.y - player->getContentSize().height / 2, 50.0f))), nullptr));
         break;
      case 'A':
         runkAnimate = Animate::create(Animation::createWithSpriteFrames(run, 0.05f, 1));
         player->runAction(Spawn::create(runkAnimate, MoveBy::create(0.45f, Vec2(-min(nowPos.x - player->getContentSize().width / 2, 50.0f), 0)), nullptr));
         break;
      case 'S':
         runkAnimate = Animate::create(Animation::createWithSpriteFrames(run, 0.05f, 1));
         player->runAction(Spawn::create(runkAnimate, MoveBy::create(0.45f, Vec2(0, -min(nowPos.y - player->getContentSize().height / 2, 50.0f))), nullptr));
         break;
      case 'D':
         runkAnimate = Animate::create(Animation::createWithSpriteFrames(run, 0.05f, 1));
         player->runAction(Spawn::create(runkAnimate, MoveBy::create(0.45f, Vec2(min(visibleSize.width - nowPos.x - player->getContentSize().width / 2, 50.0f), 0)), nullptr));
         break;
      }
   }
}

void HelloWorld::actionEvent(Ref*, char cid) {
   if (player->getSpriteFrame() == frame0 && pT->getPercentage()) {
      switch (cid) {
      case 'X':
         if (pT->getPercentage() <= 20)
            dead.popBack();
         deadAnimate = Animate::create(Animation::createWithSpriteFrames(dead, 0.1f, 1));
         player->runAction(deadAnimate);
         pT->runAction(CCProgressTo::create(2, pT->getPercentage() - 20));
         break;
      case 'Y':
         attackAnimate = Animate::create(Animation::createWithSpriteFrames(attack, 0.1f, 1));
         player->runAction(attackAnimate);
         if (pT->getPercentage() != 100)
            pT->runAction(CCProgressTo::create(1.8f, pT->getPercentage() + 20));
         break;
      }
   }
}

void HelloWorld::updateTime(float dt) {
   if (pT->getPercentage() == 0) {
      unschedule(schedule_selector(HelloWorld::updateTime));
      auto over = Sprite::create("over.png");
      over->setPosition(visibleSize.width / 2, visibleSize.height / 2);
      this->addChild(over, 2);
      return;
   }
   --dtime;
   if (dtime < 0) {
      unschedule(schedule_selector(HelloWorld::updateTime));
      dead.popBack();
      deadAnimate = Animate::create(Animation::createWithSpriteFrames(dead, 0.1f, 1));
      player->runAction(deadAnimate);
      pT->runAction(Sequence::create(CCProgressTo::create(2, 0), CallFunc::create([this]() {
         auto over = Sprite::create("over.png");
         over->setPosition(visibleSize.width / 2, visibleSize.height / 2);
         this->addChild(over, 2);
      }), nullptr));
      return;
   }
   string t = "0";
   char minute[5], second[5];
   _itoa(dtime / 60, minute, 10);
   _itoa(dtime % 60, second, 10);
   t += minute;
   t += ':';
   if (dtime % 60 < 10)
      t += '0';
   t += second;
   time->setString(t);
}
