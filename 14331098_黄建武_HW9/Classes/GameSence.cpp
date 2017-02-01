#include "GameSence.h"

USING_NS_CC;

Scene* GameSence::createScene() {
   // 'scene' is an autorelease object
   auto scene = Scene::create();

   // 'layer' is an autorelease object
   auto layer = GameSence::create();

   // add layer as a child to scene
   scene->addChild(layer);

   // return the scene
   return scene;
}

// on "init" you need to initialize your instance
bool GameSence::init() {

   if (!Layer::init()) {
      return false;
   }

   srand((unsigned)time(NULL));       // 设置随机种子

   // 设置背景
   Size visibleSize = Director::getInstance()->getVisibleSize();
   Vec2 origin = Director::getInstance()->getVisibleOrigin();
   float winw = visibleSize.width;  // 获取屏幕宽度
   float winh = visibleSize.height; // 获取屏幕高度
   auto bg = Sprite::create("level-background-0.jpg");
   bg->setPosition(Vec2(winw / 2 + origin.x, winh / 2 + origin.y));
   float spx = bg->getTextureRect().getMaxX();
   bg->setScaleX(winw / spx);      //  背景缩放
   bg->setScaleY(winw / spx);
   this->addChild(bg, 0);

   // 设置stoneLayer
   stoneLayer = Layer::create();
   stoneLayer->ignoreAnchorPointForPosition(false);
   stoneLayer->setAnchorPoint(Vec2::ZERO);
   stoneLayer->setPosition(Vec2::ZERO);

   stone = Sprite::create("stone.png");
   stone->setPosition(Vec2(560, 480));
   stoneLayer->addChild(stone);

   // 设置shoot，并且可点击
   auto label = Label::createWithTTF("Shoot", "fonts/Marker Felt.ttf", 65);
   auto shootBtn = MenuItemLabel::create(label, CC_CALLBACK_1(GameSence::shootCallback, this));
   Menu* shoot = Menu::create(shootBtn, NULL);
   shoot->setPosition(Vec2(740, 480));
   stoneLayer->addChild(shoot);

   // 设置mouseLayer
   mouseLayer = Layer::create();
   mouseLayer->ignoreAnchorPointForPosition(false);
   mouseLayer->setAnchorPoint(Vec2(0, 0));
   mouseLayer->setPosition(Vec2(0, winh / 2));

   // 设置老鼠的动作
   mouse = Sprite::createWithSpriteFrameName("gem-mouse-0.png");
   Animate* mouseAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("mouseAnimation"));
   mouse->runAction(RepeatForever::create(mouseAnimate));
   mouse->setPosition(winw / 2, 0);
   toPos = Vec2(winw / 2, 0);
   mouseLayer->addChild(mouse, 2);

   this->addChild(stoneLayer, 1);
   this->addChild(mouseLayer, 1);

   //add touch listener
   EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
   listener->setSwallowTouches(true);
   listener->onTouchBegan = CC_CALLBACK_2(GameSence::onTouchBegan, this);
   Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

   return true;
}

bool GameSence::onTouchBegan(Touch *touch, Event *unused_event) {
   auto clickPos = touch->getLocation();  // 获得点击的坐标, cocos2d-x的坐标
   CCLog("%f %f", clickPos.x, clickPos.y);  // 输出点击的坐标调试
   auto mousePos = mouse->getPosition();
   if (mousePos == toPos && clickPos.y < 420) { //老鼠停下且点击石头下方才能放置奶酪
      Size visibleSize = Director::getInstance()->getVisibleSize();
      float winh = visibleSize.height;
      toPos = Vec2((int)clickPos.x, (int)(clickPos.y - winh / 2));
      // 设置奶酪
      cheese = Sprite::create("cheese.png");
      cheese->setPosition(toPos);
      mouseLayer->addChild(cheese);

      // 老鼠跑到奶酪位置并且吃掉奶酪（奶酪淡出）
      auto moveto = MoveTo::create(2.0, toPos);
      mouse->runAction(moveto);
      cheese->runAction(Sequence::create(ScaleTo::create(2.0, 1.0), FadeOut::create(1.0), nullptr));
   }
   return true;
}

void GameSence::shootCallback(Ref * pSender) {
   Vec2 mousePos = mouse->getPosition();
   if (mousePos == toPos) {  //老鼠停下才能发射石头
      Size visibleSize = Director::getInstance()->getVisibleSize();
      float winw = visibleSize.width; // 获取屏幕宽度
      float winh = visibleSize.height; // 获取屏幕高度

      // 发射石头
      auto shootstone = Sprite::createWithSpriteFrameName("stone-0.png");
      Animate* stoneAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("stoneAnimation"));
      shootstone->runAction(RepeatForever::create(stoneAnimate));
      shootstone->setPosition(stone->getPosition());
      this->addChild(shootstone, 1);
      auto seq = Sequence::create(MoveTo::create(1.5, Vec2(mousePos.x, mousePos.y + winh / 2)), FadeOut::create(0.5), nullptr);
      shootstone->runAction(seq);

      // 老鼠跑开留下钻石
      auto diamond = Sprite::create("diamond.png");
      diamond->setPosition(mousePos);
      mouseLayer->addChild(diamond, 1);
      toPos.x = (int)(CCRANDOM_0_1() * 960);
      toPos.y = (int)(CCRANDOM_0_1() * 420 - winh / 2);
      auto mousemoveto = JumpTo::create(1.5, toPos, 50, 4);
      mouse->runAction(mousemoveto);
   }
}
