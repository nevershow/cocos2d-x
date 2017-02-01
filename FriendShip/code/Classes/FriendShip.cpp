#pragma execution_character_set("utf-8")
#include "FriendShip.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#define database UserDefault::getInstance()

using namespace CocosDenshion;
USING_NS_CC;

bool FriendShip::twoP = false;

void FriendShip::setPhysicsWorld(PhysicsWorld* world) { m_world = world; }

Scene* FriendShip::createScene() {
  srand((unsigned)time(NULL));
  auto scene = Scene::createWithPhysics();
  //scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
  scene->getPhysicsWorld()->setAutoStep(false);
  scene->getPhysicsWorld()->setGravity(Vec2(0, -300.0f));
  auto layer = FriendShip::create();
  layer->setPhysicsWorld(scene->getPhysicsWorld());
  scene->addChild(layer);
  return scene;
}

void FriendShip::update(float dt) {
  this->getScene()->getPhysicsWorld()->step(1 / 100.0f);

  // 是否在跳起来情况下 左右移动
  if (IsPlayer1Left || IsPlayer1Right) {
    if (player1->getDisplayFrame() == frame1) {
      auto animation = RepeatForever::create(Animate::create(AnimationCache::getInstance()->getAnimation("player1RunWithoutBoxAnimation")));
      animation->setTag(11);
      player1->runAction(animation);
    }
    else if (player1->getDisplayFrame() == IdleWithBox1) {
      auto animation = RepeatForever::create(Animate::create(AnimationCache::getInstance()->getAnimation("player1RunWithBoxAnimation")));
      animation->setTag(11);
      player1->runAction(animation);
    }
  }

  // 是否在跳起来情况下 左右移动
  if (IsPlayer2Left || IsPlayer2Right) {
    if (player2->getDisplayFrame() == frame2) {
      auto animation = RepeatForever::create(Animate::create(AnimationCache::getInstance()->getAnimation("player2RunWithoutBoxAnimation")));
      animation->setTag(12);
      player2->runAction(animation);
    }
    else if (player2->getDisplayFrame() == IdleWithBox2) {
      auto animation = RepeatForever::create(Animate::create(AnimationCache::getInstance()->getAnimation("player2RunWithBoxAnimation")));
      animation->setTag(12);
      player2->runAction(animation);
    }
  }
}

// 添加背景和各种精灵
void FriendShip::addSprite() {
  // add background
  auto bgSprite = Sprite::create("bg.png");
  bgSprite->setPosition(visibleSize / 2);
  bgSprite->setScale(visibleSize.width / bgSprite->getContentSize().width, visibleSize.height / bgSprite->getContentSize().height);
  this->addChild(bgSprite, 0);

  // add ship
  ship = Sprite::create("ship.png");
  ship->setScale(visibleSize.width / ship->getContentSize().width * 0.97, 1.2f);
  ship->setPosition(visibleSize.width / 2, 110);
  auto shipbody = PhysicsBody::createBox(ship->getContentSize(), PhysicsMaterial(100.0f, 0.0f, 1.0f));
  shipbody->setCategoryBitmask(0xFFFFFFFF);
  shipbody->setCollisionBitmask(0xFFFFFFFF);
  shipbody->setContactTestBitmask(0xFFFFFFFF);
  ship->setPhysicsBody(shipbody);
  this->addChild(ship, 1);

  // add see
  auto seeSprite = Sprite::create("see.png");
  seeSprite->setPosition(visibleSize.width / 2, 60);
  seeSprite->setScale(visibleSize.width / seeSprite->getContentSize().width, 120 / seeSprite->getContentSize().height);
  seeSprite->setOpacity(230);
  this->addChild(seeSprite, 3);

  // add sun and cloud
  auto sunSprite = Sprite::create("sun.png");
  sunSprite->setPosition(rand() % (int)(visibleSize.width - 200) + 100, 550);
  this->addChild(sunSprite);
  auto cloudSprite1 = Sprite::create("cloud.png");
  cloudSprite1->setPosition(rand() % (int)(visibleSize.width - 200) + 100, rand() % 100 + 450);
  this->addChild(cloudSprite1);
  auto cloudSprite2 = Sprite::create("cloud.png");
  cloudSprite2->setPosition(rand() % (int)(visibleSize.width - 200) + 100, rand() % 100 + 450);
  this->addChild(cloudSprite2);

  // 添加计时和最高分
  if (database->getStringForKey("higheststr") != "") {
    auto label1 = Label::createWithTTF("最高分: " + database->getStringForKey("higheststr") , "fonts/STXINWEI.TTF", 30);
    label1->setColor(Color3B(0, 0, 0));
    label1->setPosition(visibleSize.width - 100, visibleSize.height - 50);
    this->addChild(label1, 3);
  }
  nowtime = Label::createWithTTF("00:00", "fonts/STXINWEI.TTF", 30);
  nowtime->setColor(Color3B(0, 0, 0));
  nowtime->setPosition(visibleSize.width / 2, visibleSize.height - nowtime->getContentSize().height);
  this->addChild(nowtime, 3);

  // 定义世界的边界
  Vec2* points = new Vec2[4];
  points[0] = Vec2(-visibleSize.width / 2, visibleSize.height * 0.6);
  points[1] = Vec2(visibleSize.width / 2, visibleSize.height * 0.6);
  points[2] = Vec2(visibleSize.width / 2, -visibleSize.height / 2 + 70);
  points[3] = Vec2(-visibleSize.width / 2, -visibleSize.height / 2 + 70);
  auto body = PhysicsBody::createEdgePolygon(points, 4, PhysicsMaterial(1.0f, 0.0f, 1.0f));
  edgeNode = Node::create();
  edgeNode->setPosition(visibleSize / 2);
  edgeNode->setPhysicsBody(body);
  this->addChild(edgeNode);
}

// 创建角色
void FriendShip::addPlayer() {
  IsPlayer1Jump = false;
  IsPlayer1Left = false;
  IsPlayer1Right = false;
  IsPlayer1Hold = false;
  LastPlayer1Press = 'D';

  //create player1
  auto texture1 = Director::getInstance()->getTextureCache()->addImage("player1.png");
  frame1 = SpriteFrame::createWithTexture(texture1, CC_RECT_PIXELS_TO_POINTS(Rect(0, 4, 32, 36)));
  player1 = Sprite::createWithSpriteFrame(frame1);
  player1->setPhysicsBody(PhysicsBody::createBox(Size(32, 33), PhysicsMaterial(1.0f, 0.0f, 0.001f)));
  int xpos = twoP ? 100 : visibleSize.width / 2;
  player1->setPosition(Vec2(xpos, 185));
  player1->getPhysicsBody()->setCategoryBitmask(2);
  player1->getPhysicsBody()->setCategoryBitmask(2);
  player1->getPhysicsBody()->setCollisionBitmask(2);
  player1->getPhysicsBody()->setContactTestBitmask(2);
  player1->getPhysicsBody()->setAngularVelocityLimit(0);
  player1->getPhysicsBody()->setRotationEnable(false);
  player1->setName("player1");
  this->addChild(player1, 2);
  loadAnimation("player1");

  IsPlayer2Jump = false;
  IsPlayer2Left = false;
  IsPlayer2Right = false;
  IsPlayer2Hold = false;
  LastPlayer2Press = 'A';

  //create player2
  auto texture2 = Director::getInstance()->getTextureCache()->addImage("player2.png");
  frame2 = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(0, 4, 32, 36)));
  player2 = Sprite::createWithSpriteFrame(frame2);
  player2->setPhysicsBody(PhysicsBody::createBox(Size(32, 33), PhysicsMaterial(1.0f, 0.0f, 0.001f)));
  xpos = twoP ? visibleSize.width - 100 : visibleSize.width / 2;
  player2->setPosition(Vec2(xpos, 185));
  player2->getPhysicsBody()->setCategoryBitmask(2);
  player2->getPhysicsBody()->setCategoryBitmask(2);
  player2->getPhysicsBody()->setCollisionBitmask(2);
  player2->getPhysicsBody()->setContactTestBitmask(2);
  player2->getPhysicsBody()->setAngularVelocityLimit(0);
  player2->getPhysicsBody()->setRotationEnable(false);
  player2->setName("player2");
  player2->setFlippedX(true);
  this->addChild(player2, 2);
  if (!twoP) {
    player2->setVisible(false);
  }
  loadAnimation("player2");
}

// 添加监听器
void FriendShip::addListener() {
  auto keyboardListener = EventListenerKeyboard::create();
  keyboardListener->onKeyPressed = CC_CALLBACK_2(FriendShip::onKeyPressed, this);
  keyboardListener->onKeyReleased = CC_CALLBACK_2(FriendShip::onKeyReleased, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

  auto contactListener = EventListenerPhysicsContact::create();
  contactListener->onContactBegin = CC_CALLBACK_1(FriendShip::onConcactBegin, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
}

// on "init" you need to initialize your instance
bool FriendShip::init() {
  //////////////////////////////
  // 1. super init first
  if (!Layer::init()) {
    return false;
  }
  visibleSize = Director::getInstance()->getVisibleSize();
  twoP = database->getBoolForKey("twoP");
  dtime = 0;

  addSprite();
  addListener();
  addPlayer();

  schedule(schedule_selector(FriendShip::update), 0.01, kRepeatForever, 0.1);
  schedule(schedule_selector(FriendShip::boxFall), twoP ? 3.0 : 4.0, kRepeatForever, 0);
  schedule(schedule_selector(FriendShip::updateTime), 1.0, kRepeatForever, 0);

  detaH = height = 0;
  SimpleAudioEngine::getInstance()->preloadEffect("boxfall.mp3");
  SimpleAudioEngine::getInstance()->preloadEffect("gameover.mp3");
  SimpleAudioEngine::getInstance()->preloadEffect("jump.mp3");
  SimpleAudioEngine::getInstance()->preloadBackgroundMusic("bgm.mp3");

  SimpleAudioEngine::getInstance()->playBackgroundMusic("bgm.mp3", true);

  return true;
}

// 更新船的平衡和倾斜
// 通过改变边界的形状使船倾斜
void FriendShip::updateShip(float dt) {
  if ((int)detaH == abs((int)height)) {
    unschedule(schedule_selector(FriendShip::updateShip));
    return;
  }
  if (detaH > fabs(height))
    detaH -= 1;
  else
    detaH += 1;

  Vec2* points = new Vec2[4];
  points[0] = Vec2(-visibleSize.width / 2, visibleSize.height * 0.6);
  points[1] = Vec2(visibleSize.width / 2, visibleSize.height * 0.6);
  if (height > 0) {
    points[2] = Vec2(visibleSize.width / 2, -visibleSize.height / 2 + 70 + detaH);
    points[3] = Vec2(-visibleSize.width / 2, -visibleSize.height / 2 + 70 - 0.15 * detaH);
  }
  else {
    points[2] = Vec2(visibleSize.width / 2, -visibleSize.height / 2 + 70 - 0.15 * detaH);
    points[3] = Vec2(-visibleSize.width / 2, -visibleSize.height / 2 + 70 + detaH);
  }

  auto body = PhysicsBody::createEdgePolygon(points, 4, PhysicsMaterial(1.0f, 0.0f, 1.0f));
  edgeNode->removeAllComponents();
  edgeNode->setPhysicsBody(body);
}

// 更新时间
void FriendShip::updateTime(float dt) {
  string t = "0";
  char minute[5], second[5];
  _itoa(dtime / 60, minute, 10);
  _itoa(dtime % 60, second, 10);
  t += minute;
  t += ':';
  if (dtime % 60 < 10)
    t += '0';
  t += second;
  nowtime->setString(t);
  ++dtime;
}

// 掉落箱子 设置掩码使箱子掉落过程不会砸到玩家
// 为了增加难度，箱子将掉落在船重的一端
void FriendShip::boxFall(float dt) {
  auto box = Sprite::create("box.png");
  auto boxbody = PhysicsBody::createBox(box->getContentSize(), PhysicsMaterial(100.0f, 0.0f, 1.0f));
  boxbody->setCategoryBitmask(1);
  boxbody->setCollisionBitmask(1);
  boxbody->setContactTestBitmask(1);
  boxbody->setAngularVelocityLimit(0);
  boxbody->setTag(5);
  boxbody->setRotationEnable(false);
  box->setPhysicsBody(boxbody);
  if (height > 0)
    box->setPosition(rand() % (int)(visibleSize.width / 2 - 50) + 50, visibleSize.height);
  else
    box->setPosition(rand() % (int)(visibleSize.width / 2 - 50) + visibleSize.width / 2, visibleSize.height);
  boxes.push_back(boxbody);
  this->addChild(box);
}

// 按下按键
// 上下左右 举起箱子 扔掉箱子
void FriendShip::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
  Action* animation;
  switch (code) {
    case cocos2d::EventKeyboard::KeyCode::KEY_A:
    case cocos2d::EventKeyboard::KeyCode::KEY_D:
      if (code == EventKeyboard::KeyCode::KEY_A) {
        IsPlayer1Left = true;
        IsPlayer1Right = false;
        if (LastPlayer1Press == 'D')
          player1->setFlipX(true);
        LastPlayer1Press = 'A';
        player1->getPhysicsBody()->setVelocity(Vec2(IsPlayer1Hold ? -200 : -100, player1->getPhysicsBody()->getVelocity().y));
      }
      else {
        IsPlayer1Right = true;
        IsPlayer1Left = false;
        if (LastPlayer1Press == 'A')
          player1->setFlipX(false);
        LastPlayer1Press = 'D';
        player1->getPhysicsBody()->setVelocity(Vec2(IsPlayer1Hold ? 200 : 100, player1->getPhysicsBody()->getVelocity().y));
      }

      if (IsPlayer1Jump == false) {
        player1->stopAllActions();
        if (IsPlayer1Hold) {
          animation = RepeatForever::create(Animate::create(AnimationCache::getInstance()->getAnimation("player1RunWithBoxAnimation")));
          animation->setTag(11);
          player1->runAction(animation);
        }
        else {
          animation = RepeatForever::create(Animate::create(AnimationCache::getInstance()->getAnimation("player1RunWithoutBoxAnimation")));
          animation->setTag(11);
          player1->runAction(animation);
        }
      }
      break;

    case cocos2d::EventKeyboard::KeyCode::KEY_W:
      if (IsPlayer1Jump) break;
      player1->stopAllActions();
      player1->getPhysicsBody()->setVelocity(Vec2(0, 250));
      if (holdbox1) holdbox1->getPhysicsBody()->setVelocity(player1->getPhysicsBody()->getVelocity());
      IsPlayer1Jump = true;
      SimpleAudioEngine::getInstance()->playEffect("jump.mp3", false);

      if (IsPlayer1Hold) {
        auto animation = Animate::create(AnimationCache::getInstance()->getAnimation("player1JumpWithBoxAnimation"));
        animation->setTag(11);
        player1->runAction(animation);
      }
      else {
        auto animation = Animate::create(AnimationCache::getInstance()->getAnimation("player1JumpWithoutBoxAnimation"));
        animation->setTag(11);
        player1->runAction(animation);
      }
      break;

    // 使用固定距离关节
    case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:
      if (IsPlayer1Hold) {
        player1->stopActionByTag(1);
        player1->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("player1PutDownAnimation")));
        IsPlayer1Hold = false;
        m_world->removeJoint(joint1);
        auto box = Sprite::create("box.png");
        auto boxbody = PhysicsBody::createBox(box->getContentSize(), PhysicsMaterial(100.0f, 0.0f, 1.0f));
        boxbody->setCategoryBitmask(1);
        boxbody->setCollisionBitmask(1);
        boxbody->setContactTestBitmask(1);
        boxbody->setAngularVelocityLimit(0);
        boxbody->setRotationEnable(false);
        boxbody->setTag(5);
        boxbody->setVelocity(Vec2(LastPlayer1Press == 'A' ? -120 : 120, 180));
        box->setPhysicsBody(boxbody);
        box->setPosition(holdbox1->getPosition());
        boxes.push_back(boxbody);
        this->addChild(box);
        holdbox1->removeFromParentAndCleanup(true);
        holdbox1 = nullptr;
      }
      else {
        auto playerPos = player1->getPosition();
        for (auto box : boxes) {
          auto boxPos = box->getPosition();
          if (player1->getBoundingBox().intersectsRect(box->getNode()->getBoundingBox())) {
            IsPlayer1Hold = true;
            player1->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("player1PutUpAnimation")));
            holdbox1 = box->getNode();
            boxes.remove(box);
            holdbox1->getPhysicsBody()->setCategoryBitmask(1);
            holdbox1->getPhysicsBody()->setCollisionBitmask(1);
            holdbox1->getPhysicsBody()->setContactTestBitmask(1);
            holdbox1->runAction(Sequence::create(MoveTo::create(0.1, Vec2(playerPos.x, playerPos.y + 28)), CallFunc::create([this] {
              holdbox1->removeAllComponents();
              holdbox1->setPhysicsBody(PhysicsBody::createBox(holdbox1->getContentSize(), PhysicsMaterial(1.0f, 0.0f, 100.0f)));
              holdbox1->getPhysicsBody()->setCategoryBitmask(2);
              holdbox1->getPhysicsBody()->setCollisionBitmask(2);
              holdbox1->getPhysicsBody()->setContactTestBitmask(2);
              joint1 = PhysicsJointDistance::construct(player1->getPhysicsBody(), holdbox1->getPhysicsBody(), player1->getAnchorPoint(), holdbox1->getAnchorPoint());
              m_world->addJoint(joint1);
            }), nullptr));
            break;
          }
        }
      }
      break;
  }

  // 如果两个玩家则相应一下按键
  if (twoP) {
    switch (code) {
      case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
      case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
        if (code == EventKeyboard::KeyCode::KEY_LEFT_ARROW) {
          IsPlayer2Left = true;
          IsPlayer2Right = false;
          if (LastPlayer2Press == 'D')
            player2->setFlipX(true);
          LastPlayer2Press = 'A';
          player2->getPhysicsBody()->setVelocity(Vec2(IsPlayer2Hold ? -200 : -100, player2->getPhysicsBody()->getVelocity().y));
        }
        else {
          IsPlayer2Right = true;
          IsPlayer2Left = false;
          if (LastPlayer2Press == 'A')
            player2->setFlipX(false);
          LastPlayer2Press = 'D';
          player2->getPhysicsBody()->setVelocity(Vec2(IsPlayer2Hold ? 200 : 100, player2->getPhysicsBody()->getVelocity().y));
        }

        if (IsPlayer2Jump == false) {
          player2->stopAllActions();
          if (IsPlayer2Hold) {
            animation = RepeatForever::create(Animate::create(AnimationCache::getInstance()->getAnimation("player2RunWithBoxAnimation")));
            animation->setTag(12);
            player2->runAction(animation);
          }
          else {
            animation = RepeatForever::create(Animate::create(AnimationCache::getInstance()->getAnimation("player2RunWithoutBoxAnimation")));
            animation->setTag(12);
            player2->runAction(animation);
          }
        }
        break;

      case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
        if (IsPlayer2Jump) break;
        player2->stopAllActions();
        player2->getPhysicsBody()->setVelocity(Vec2(0, 250));
        if (holdbox2) holdbox2->getPhysicsBody()->setVelocity(player2->getPhysicsBody()->getVelocity());
        IsPlayer2Jump = true;
        SimpleAudioEngine::getInstance()->playEffect("jump.mp3", false);

        if (IsPlayer2Hold) {
          auto animation = Animate::create(AnimationCache::getInstance()->getAnimation("player2JumpWithBoxAnimation"));
          animation->setTag(12);
          player2->runAction(animation);
        }
        else {
          auto animation = Animate::create(AnimationCache::getInstance()->getAnimation("player2JumpWithoutBoxAnimation"));
          animation->setTag(12);
          player2->runAction(animation);
        }
        break;

      case cocos2d::EventKeyboard::KeyCode::KEY_ENTER:
        if (IsPlayer2Hold) {
          player2->stopActionByTag(2);
          player2->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("player2PutDownAnimation")));
          IsPlayer2Hold = false;
          m_world->removeJoint(joint2);
          auto box = Sprite::create("box.png");
          auto boxbody = PhysicsBody::createBox(box->getContentSize(), PhysicsMaterial(100.0f, 0.0f, 1.0f));
          boxbody->setCategoryBitmask(1);
          boxbody->setCollisionBitmask(1);
          boxbody->setContactTestBitmask(1);
          boxbody->setAngularVelocityLimit(0);
          boxbody->setRotationEnable(false);
          boxbody->setTag(5);
          boxbody->setVelocity(Vec2(LastPlayer2Press == 'A' ? -120 : 120, 180));
          box->setPhysicsBody(boxbody);
          box->setPosition(holdbox2->getPosition());
          boxes.push_back(boxbody);
          this->addChild(box);
          holdbox2->removeFromParentAndCleanup(true);
          holdbox2 = nullptr;
        }
        else {
          auto playerPos = player2->getPosition();
          for (auto box : boxes) {
            auto boxPos = box->getPosition();
            if (player2->getBoundingBox().intersectsRect(box->getNode()->getBoundingBox())) {
              IsPlayer2Hold = true;
              player2->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("player2PutUpAnimation")));
              holdbox2 = box->getNode();
              boxes.remove(box);
              holdbox2->getPhysicsBody()->setCategoryBitmask(1);
              holdbox2->getPhysicsBody()->setCollisionBitmask(1);
              holdbox2->getPhysicsBody()->setContactTestBitmask(1);
              holdbox2->runAction(Sequence::create(MoveTo::create(0.1, Vec2(playerPos.x, playerPos.y + 28)), CallFunc::create([this] {
                holdbox2->removeAllComponents();
                holdbox2->setPhysicsBody(PhysicsBody::createBox(holdbox2->getContentSize(), PhysicsMaterial(1.0f, 0.0f, 100.0f)));
                holdbox2->getPhysicsBody()->setCategoryBitmask(2);
                holdbox2->getPhysicsBody()->setCollisionBitmask(2);
                holdbox2->getPhysicsBody()->setContactTestBitmask(2);
                joint2 = PhysicsJointDistance::construct(player2->getPhysicsBody(), holdbox2->getPhysicsBody(), player2->getAnchorPoint(), holdbox2->getAnchorPoint());
                m_world->addJoint(joint2);
              }), nullptr));
              break;
            }
          }
        }
        break;
    }
  }
}

// 释放按键
void FriendShip::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
  switch (code) {
    case cocos2d::EventKeyboard::KeyCode::KEY_A:
    case cocos2d::EventKeyboard::KeyCode::KEY_D:
      if (code == EventKeyboard::KeyCode::KEY_A)
        IsPlayer1Left = false;
      else IsPlayer1Right = false;
      player1->stopAllActionsByTag(11);
      if (IsPlayer1Hold)
        player1->setDisplayFrame(IdleWithBox1);
      else player1->setDisplayFrame(frame1);
      player1->getPhysicsBody()->setVelocity(Vec2(0, player1->getPhysicsBody()->getVelocity().y));
      if (holdbox1) holdbox1->getPhysicsBody()->setVelocity(player1->getPhysicsBody()->getVelocity());
      break;
    case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
      if (code == EventKeyboard::KeyCode::KEY_LEFT_ARROW)
        IsPlayer2Left = false;
      else IsPlayer2Right = false;
      player2->stopAllActionsByTag(12);
      if (IsPlayer2Hold)
        player2->setDisplayFrame(IdleWithBox2);
      else player2->setDisplayFrame(frame2);
      player2->getPhysicsBody()->setVelocity(Vec2(0, player2->getPhysicsBody()->getVelocity().y));
      if (holdbox2) holdbox2->getPhysicsBody()->setVelocity(player2->getPhysicsBody()->getVelocity());
      break;
  }
}

// 箱子碰地后改变掩码，与玩家可发生碰撞
bool FriendShip::onConcactBegin(PhysicsContact & contact) {
  auto shapeA = contact.getShapeA();
  auto shapeB = contact.getShapeB();

  // 箱子碰地
  if (shapeA->getCollisionBitmask() == 1 || shapeB->getCollisionBitmask() == 1) {
    if (shapeA->getCollisionBitmask() == 1) {
      shapeA->setCategoryBitmask(3);
      shapeA->setCollisionBitmask(3);
      shapeA->setContactTestBitmask(3);
    }
    else {
      shapeB->setCategoryBitmask(3);
      shapeB->setCollisionBitmask(3);
      shapeB->setContactTestBitmask(3);
    }

    SimpleAudioEngine::getInstance()->playEffect("boxfall.mp3", false);

    // 计算船当前重量，更改倾斜程度
    double leftmass = 0, rightmass = 0;
    for (auto box : boxes) {
      double a = box->getPosition().x - 600;
      if (a < 0)
        leftmass += box->getTag() * (-a);
      else
        rightmass += box->getTag() * a;
    }
    height = (leftmass - rightmass) / 70;
    if (fabs(height) > 190) {
      GameOver();
      return true;
    }
    schedule(schedule_selector(FriendShip::updateShip), 0.01, kRepeatForever, 0);
  }

  // 玩家跳起来后下落过程碰到箱子
  else if (shapeA->getCollisionBitmask() == 2 || shapeB->getCollisionBitmask() == 2) {
    if (shapeA->getBody()->getNode()->getName() == "player1" || shapeB->getBody()->getNode()->getName() == "player1") {
      player1->stopActionByTag(1);
      IsPlayer1Jump = false;
    }

    else if (shapeA->getBody()->getNode()->getName() == "player2" || shapeB->getBody()->getNode()->getName() == "player2") {
      player2->stopActionByTag(2);
      IsPlayer2Jump = false;
    }
  }

  return true;
}

//加载玩家动画帧
void FriendShip::loadAnimation(string filepath) {
  Vector<SpriteFrame*> PlayerIdleWithoutBox;
  Vector<SpriteFrame*> PlayerIdleWithBox;
  Vector<SpriteFrame*> PlayerPutUp;
  Vector<SpriteFrame*> PlayerPutDown;
  Vector<SpriteFrame*> PlayerRunWithBox;
  Vector<SpriteFrame*> PlayerRunWithoutBox;
  Vector<SpriteFrame*> PlayerJumpWithoutBox;
  Vector<SpriteFrame*> PlayerJumpWithBox;

  auto PlayerImage = Director::getInstance()->getTextureCache()->addImage(filepath + ".png");

  PlayerIdleWithoutBox.reserve(3);
  for (int i = 0; i < 3; i++) {
    auto frame = SpriteFrame::createWithTexture(PlayerImage, CC_RECT_PIXELS_TO_POINTS(Rect(32 * i, 5, 32, 36)));
    PlayerIdleWithoutBox.pushBack(frame);
  }
  Animation* PlayerIdelWithoutBoxAnimation = Animation::createWithSpriteFrames(PlayerIdleWithoutBox, 0.1);
  AnimationCache::getInstance()->addAnimation(PlayerIdelWithoutBoxAnimation, filepath + "IdleWithoutBoxAnimation");

  PlayerIdleWithBox.reserve(3);
  auto IdleWithBox = SpriteFrame::createWithTexture(PlayerImage, CC_RECT_PIXELS_TO_POINTS(Rect(96, 7, 32, 36)));
  PlayerIdleWithBox.pushBack(IdleWithBox);
  if (filepath == "player1")
    IdleWithBox1 = IdleWithBox;
  else IdleWithBox2 = IdleWithBox;
  for (int i = 1; i < 3; i++) {
    auto frame = SpriteFrame::createWithTexture(PlayerImage, CC_RECT_PIXELS_TO_POINTS(Rect(96 + 32 * i, 7, 32, 33)));
    PlayerIdleWithBox.pushBack(frame);
  }
  Animation* PlayerIdleWithBoxAnimation = Animation::createWithSpriteFrames(PlayerIdleWithBox, 0.1);
  AnimationCache::getInstance()->addAnimation(PlayerIdleWithBoxAnimation, filepath + "IdleWithBoxAnimation");

  for (int i = 0; i < 3; i++) {
    auto frame = SpriteFrame::createWithTexture(PlayerImage, CC_RECT_PIXELS_TO_POINTS(Rect(96 + 32 * i, 4, 32, 36)));
    PlayerPutUp.pushBack(frame);
  }
  Animation* PlayerPutUpAnimation = Animation::createWithSpriteFrames(PlayerPutUp, 0.1);
  AnimationCache::getInstance()->addAnimation(PlayerPutUpAnimation, filepath + "PutUpAnimation");

  for (int i = 0; i < 2; i++) {
    auto frame = SpriteFrame::createWithTexture(PlayerImage, CC_RECT_PIXELS_TO_POINTS(Rect(193 + 32 * i, 4, 32, 36)));
    PlayerPutDown.pushBack(frame);
  }
  if (filepath == "player1")
    PlayerPutDown.pushBack(frame1);
  else
    PlayerPutDown.pushBack(frame2);
  Animation* PlayerPutDownAnimation = Animation::createWithSpriteFrames(PlayerPutDown, 0.1);
  AnimationCache::getInstance()->addAnimation(PlayerPutDownAnimation, filepath + "PutDownAnimation");

  PlayerRunWithoutBox.reserve(8);
  for (int i = 0; i < 8; i++) {
    auto frame = SpriteFrame::createWithTexture(PlayerImage, CC_RECT_PIXELS_TO_POINTS(Rect(32 * i, 41, 32, 36)));
    PlayerRunWithoutBox.pushBack(frame);
  }

  Animation* PlayerRunWithoutBoxAnimation = Animation::createWithSpriteFrames(PlayerRunWithoutBox, 0.1);
  AnimationCache::getInstance()->addAnimation(PlayerRunWithoutBoxAnimation, filepath + "RunWithoutBoxAnimation");

  PlayerRunWithBox.reserve(8);
  for (int i = 0; i < 8; i++) {
    auto frame = SpriteFrame::createWithTexture(PlayerImage, CC_RECT_PIXELS_TO_POINTS(Rect(32 * i, 81, 32, 36)));
    PlayerRunWithBox.pushBack(frame);
  }
  Animation* PlayerRunWithBoxAnimation = Animation::createWithSpriteFrames(PlayerRunWithBox, 0.1);
  AnimationCache::getInstance()->addAnimation(PlayerRunWithBoxAnimation, filepath + "RunWithBoxAnimation");

  PlayerJumpWithoutBox.pushBack(SpriteFrame::createWithTexture(PlayerImage, CC_RECT_PIXELS_TO_POINTS(Rect(64, 41, 32, 36))));
  if (filepath == "player1")
    PlayerJumpWithoutBox.pushBack(frame1);
  else
    PlayerJumpWithoutBox.pushBack(frame2);
  Animation* PlayerJumpWithoutBoxAnimation = Animation::createWithSpriteFrames(PlayerJumpWithoutBox, 1.0f);
  AnimationCache::getInstance()->addAnimation(PlayerJumpWithoutBoxAnimation, filepath + "JumpWithoutBoxAnimation");

  PlayerJumpWithBox.pushBack(SpriteFrame::createWithTexture(PlayerImage, CC_RECT_PIXELS_TO_POINTS(Rect(64, 81, 32, 36))));
  PlayerJumpWithBox.pushBack(IdleWithBox);
  Animation* PlayerJumpWithBoxAnimation = Animation::createWithSpriteFrames(PlayerJumpWithBox, 1.0);
  AnimationCache::getInstance()->addAnimation(PlayerJumpWithBoxAnimation, filepath + "JumpWithBoxAnimation");
}

// 游戏结束
void FriendShip::GameOver() {
  unschedule(schedule_selector(FriendShip::updateShip));
  unschedule(schedule_selector(FriendShip::boxFall));
  unschedule(schedule_selector(FriendShip::updateTime));
  m_world->removeBody(edgeNode->getPhysicsBody());
  SimpleAudioEngine::getInstance()->stopBackgroundMusic("bgm.mp3");
  SimpleAudioEngine::getInstance()->playEffect("gameover.mp3", false);

  if (database->getIntegerForKey("highestint") < dtime - 1) {
    --dtime;
    database->setIntegerForKey("highestint", dtime - 1);
    string t = "0";
    char minute[5], second[5];
    _itoa(dtime / 60, minute, 10);
    _itoa(dtime % 60, second, 10);
    t += minute;
    t += ':';
    if (dtime % 60 < 10)
      t += '0';
    t += second;
    database->setStringForKey("higheststr", t);
  }

  auto label = Label::createWithTTF(twoP ? "友谊的小船说翻就翻" : "单身狗独木舟说沉就沉", "fonts/STXINWEI.TTF", 60);
  label->setColor(Color3B(0, 0, 0));
  label->setPosition(visibleSize.width / 2, visibleSize.height / 2);
  this->addChild(label);

  label = Label::createWithTTF("重玩", "fonts/STXINWEI.TTF", 40);
  label->setColor(Color3B(0, 0, 0));
  auto replayBtn = MenuItemLabel::create(label, CC_CALLBACK_1(FriendShip::clickCallback, this));
  Menu* replay = Menu::create(replayBtn, NULL);
  replay->setPosition(visibleSize.width / 2 - 80, visibleSize.height / 2 - 100);
  this->addChild(replay);

  label = Label::createWithTTF("退出", "fonts/STXINWEI.TTF", 40);
  label->setColor(Color3B(0, 0, 0));
  auto exitBtn = MenuItemLabel::create(label, CC_CALLBACK_1(FriendShip::exitCallback, this));
  Menu* exit = Menu::create(exitBtn, NULL);
  exit->setPosition(visibleSize.width / 2 + 90, visibleSize.height / 2 - 100);
  this->addChild(exit);
}

// 继续或重玩按钮响应函数
void FriendShip::clickCallback(Ref * pSender) {
  Director::getInstance()->replaceScene(FriendShip::createScene());
}

// 退出
void FriendShip::exitCallback(Ref * pSender) {
  Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
  exit(0);
#endif
}
