#include "BreakoutScene.h"
#include "GameOver.h"
#include "WellDone.h"
#include <string>
#include "SimpleAudioEngine.h"
#define database UserDefault::getInstance()

using namespace std;
using namespace CocosDenshion;

void Breakout::setPhysicsWorld(PhysicsWorld* world) { m_world = world; }

Scene* Breakout::createScene() {
  auto scene = Scene::createWithPhysics();
  //scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
  scene->getPhysicsWorld()->setGravity(Point(0, 0));

  auto layer = Breakout::create(scene->getPhysicsWorld());
  scene->addChild(layer);
  return scene;
}

bool Breakout::init(PhysicsWorld* world) {
  if (!Layer::init()) {
    return false;
  }
  this->setPhysicsWorld(world);

  visibleSize = Director::getInstance()->getVisibleSize();
  origin = Director::getInstance()->getVisibleOrigin();
  preloadMusic();
  playBgm();

  addBackground();
  addEdge();
  addHP();
  addPlayer();
  addTimer();

  addContactListener();
  addTouchListener();
  addKeyboardListener();

  this->schedule(schedule_selector(Breakout::update), 1);
  isMove = false;
  contactTime = 0;
  return true;
}

Breakout* Breakout::create(PhysicsWorld * world) {
  Breakout* pRet = new(std::nothrow) Breakout();
  if (pRet && pRet->init(world)) {
    pRet->autorelease();
    return pRet;
  }
  delete pRet;
  pRet = NULL;
  return NULL;
}

void Breakout::preloadMusic() {
  SimpleAudioEngine::getInstance()->preloadBackgroundMusic("music/bgm.mp3");
  SimpleAudioEngine::getInstance()->preloadBackgroundMusic("music/meet_stone.wav");
}

void Breakout::playBgm() {
  SimpleAudioEngine::getInstance()->playBackgroundMusic("music/bgm.mp3", true);
}

void Breakout::addTimer() {
  if (!database->getBoolForKey("isExist")) {
    database->setBoolForKey("isExist", true);
    database->setIntegerForKey("timeInt", 10);
  }

  time = database->getIntegerForKey("timeInt");
  char arr[10];
  _itoa(time, arr, 10);
  TTFConfig ttfConfig;
  ttfConfig.fontFilePath = "fonts/consola.ttf";
  ttfConfig.fontSize = 40;
  timeLabel = Label::createWithTTF(ttfConfig, arr);   //时间
  timeLabel->setPosition(Vec2(origin.x + visibleSize.width / 2,
    origin.y + visibleSize.height - timeLabel->getContentSize().height));
  timeLabel->setColor(Color3B(0, 255, 0));
  this->addChild(timeLabel, 1);
}

void Breakout::addBackground() {
  auto bgsprite = Sprite::create("black_hole_bg0.jpg");
  bgsprite->setPosition(visibleSize / 2);
  bgsprite->setScale(visibleSize.width / bgsprite->getContentSize().width, visibleSize.height / bgsprite->getContentSize().height);
  this->addChild(bgsprite, 0);

  auto ps = ParticleSystemQuad::create("black_hole.plist");
  ps->setPosition(visibleSize / 2);
  this->addChild(ps);
}

void Breakout::addEdge() {
  auto edgeSp = Sprite::create();
  auto boundBody = PhysicsBody::createEdgeBox(visibleSize, PhysicsMaterial(0.0f, 1.0f, 0.0f));
  boundBody->setDynamic(false);
  boundBody->setTag(0);
  edgeSp->setPosition(Point(visibleSize.width / 2, visibleSize.height / 2));
  edgeSp->setPhysicsBody(boundBody);
  this->addChild(edgeSp);
}

void Breakout::addPlayer() {
  player = Sprite::create("player.png");
  player->setAnchorPoint(Vec2(0.5, 0.5));
  player->setPhysicsBody(PhysicsBody::createCircle(33));

  player->getPhysicsBody()->setGroup(1);
  player->getPhysicsBody()->setCategoryBitmask(1);   // 001
  player->getPhysicsBody()->setCollisionBitmask(2);  // 010
  player->getPhysicsBody()->setContactTestBitmask(2);  // 010
  player->setPosition(visibleSize / 2);
  player->getPhysicsBody()->setAngularVelocityLimit(0);
  addChild(player);
}

void Breakout::addHP() {
  Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
  auto scale = visibleSize.width / 2.4 / sp0->getContentSize().width;
  sp0->setScaleX(scale);
  sp0->setScaleY(scale);
  Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

  //使用hp条设置progressBar
  pT = ProgressTimer::create(sp);
  pT->setScaleY(scale);
  pT->setScaleX(88.6 * scale);

  pT->setAnchorPoint(Vec2(0, 0));
  pT->setType(ProgressTimerType::BAR);
  pT->setBarChangeRate(Point(1, 0));
  pT->setMidpoint(Point(0, 1));
  pT->setPercentage(100);
  auto origin = Director::getInstance()->getVisibleOrigin();
  pT->setPosition(Vec2(origin.x + 61, origin.y + visibleSize.height - 42));
  this->addChild(pT, 3);

  sp0->setAnchorPoint(Vec2(0, 0));
  sp0->setPosition(Vec2(origin.x + 10, origin.y + visibleSize.height - 57));
  this->addChild(sp0, 1);
}

void Breakout::addBoom() {
  auto boom = Sprite::create("boom.png");
  boom->setPhysicsBody(PhysicsBody::createCircle(10));
  boom->setAnchorPoint(Vec2(0.5, 0.5));

  boom->getPhysicsBody()->setCategoryBitmask(2);  //010
  boom->getPhysicsBody()->setCollisionBitmask(3); //011
  boom->getPhysicsBody()->setContactTestBitmask(1); //001
  boom->getPhysicsBody()->setGroup(5);
  boom->setPosition(Point(rand() % (int)(visibleSize.width - 100) + 50, rand() % (int)(visibleSize.height - 100) + 50));
  this->addChild(boom);
}

void Breakout::addHeart() {
  auto heart = Sprite::create("heart.png");
  heart->setPhysicsBody(PhysicsBody::createCircle(10));
  heart->setAnchorPoint(Vec2(0.5, 0.5));

  heart->getPhysicsBody()->setCategoryBitmask(2);  //010
  heart->getPhysicsBody()->setCollisionBitmask(3); //011
  heart->getPhysicsBody()->setContactTestBitmask(1); //001
  heart->getPhysicsBody()->setGroup(6);
  heart->setPosition(Point(rand() % (int)(visibleSize.width - 100) + 50, rand() % (int)(visibleSize.height - 100) + 50));
  this->addChild(heart);
}

void Breakout::addContactListener() {
  auto contactListener = EventListenerPhysicsContact::create();
  contactListener->onContactBegin = CC_CALLBACK_1(Breakout::onConcactBegin, this);
  _eventDispatcher->addEventListenerWithFixedPriority(contactListener, 1);
}

void Breakout::addTouchListener() {
  auto touchListener = EventListenerTouchOneByOne::create();
  touchListener->onTouchBegan = CC_CALLBACK_2(Breakout::onTouchBegan, this);
  touchListener->onTouchMoved = CC_CALLBACK_2(Breakout::onTouchMoved, this);
  touchListener->onTouchEnded = CC_CALLBACK_2(Breakout::onTouchEnded, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

void Breakout::addKeyboardListener() {
  auto keboardListener = EventListenerKeyboard::create();
  keboardListener->onKeyPressed = CC_CALLBACK_2(Breakout::onKeyPressed, this);
  keboardListener->onKeyReleased = CC_CALLBACK_2(Breakout::onKeyReleased, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(keboardListener, this);
}

bool Breakout::onTouchBegan(Touch *touch, Event *unused_event) {
  Vec2 postion = touch->getLocation();
  if (player->getBoundingBox().containsPoint(postion)) {
    isMove = true;
    player->getPhysicsBody()->setVelocity(Vec2::ZERO);
  }
  return true;
}

void Breakout::onTouchMoved(Touch *touch, Event *unused_event) {
  if (isMove && player) {
    Vec2 delta = touch->getDelta();
    player->getPhysicsBody()->setVelocity(delta * 70);
  }
}

void Breakout::onTouchEnded(Touch *touch, Event *unused_event) {
  isMove = false;
}

bool Breakout::onConcactBegin(PhysicsContact& contact) {
  auto shapeA = contact.getShapeA();
  auto shapeB = contact.getShapeB();
  Node* sp;
  auto explosion = ParticleSystemQuad::create("explode.plist");
  if (shapeA->getGroup() == 5 || shapeB->getGroup() == 5) {
    cleanScreen(shapeA, shapeB);
    return true;
  }

  if (shapeA->getGroup() == 6 || shapeB->getGroup() == 6) {
    recoverHP(shapeA, shapeB);
    return true;
  }

  if (shapeA->getGroup() != 1) {
    sp = (shapeA->getBody())->getNode();
    enemys.remove(shapeA->getBody());
  }
  else {
    sp = (shapeB->getBody())->getNode();
    enemys.remove(shapeB->getBody());
  }

  explosion->setPosition(sp->getPosition());
  sp->removeFromParentAndCleanup(true);
  this->addChild(explosion);
  SimpleAudioEngine::getInstance()->playEffect("music/meet_stone.wav");
  ++contactTime;
  int tmp = contactTime;
  pT->runAction(Sequence::create(CCProgressTo::create(1.0, 10 * (10 - contactTime)), CCCallFunc::create([this, tmp] {
    if (tmp == 10)
      Director::getInstance()->replaceScene(TransitionCrossFade::create(0.8f, GameOver::createScene()));
  }), nullptr));
  if (contactTime == 10) {
    auto explosion = ParticleSystemQuad::create("explode.plist");
    explosion->setPosition(player->getPosition());
    this->addChild(explosion);
    player->removeFromParentAndCleanup(true);
    unschedule(schedule_selector(Breakout::update));
    _eventDispatcher->removeAllEventListeners();
  }
  return true;
}

void Breakout::update(float f) {
  if (time == 0) {
    unschedule(schedule_selector(Breakout::update));
    _eventDispatcher->removeAllEventListeners();
    Director::getInstance()->replaceScene(TransitionCrossFade::create(0.8f, WellDone::createScene()));
    return;
  }
  --time;
  char arr[10];
  _itoa(time, arr, 10);
  timeLabel->setString(arr);
  newEnemys();
}

void Breakout::cleanScreen(PhysicsShape* shapeA, PhysicsShape* shapeB) {
  if (shapeA->getGroup() == 5) {
    shapeA->getBody()->getNode()->removeFromParentAndCleanup(true);
    SimpleAudioEngine::getInstance()->playEffect("music/meet_stone.wav");
    for (auto it = enemys.begin(); it != enemys.end(); it++) {
      auto tmp = ParticleSystemQuad::create("explode.plist");
      tmp->setPosition((*it)->getPosition());
      this->addChild(tmp);
      (*it)->getNode()->removeFromParentAndCleanup(true);
    }
    enemys.clear();
  }
  if (shapeB->getGroup() == 5) {
    shapeB->getBody()->getNode()->removeFromParentAndCleanup(true);
    SimpleAudioEngine::getInstance()->playEffect("music/meet_stone.wav");
    for (auto it = enemys.begin(); it != enemys.end(); it++) {
      auto tmp = ParticleSystemQuad::create("explode.plist");
      tmp->setPosition((*it)->getPosition());
      this->addChild(tmp);
      (*it)->getNode()->removeFromParentAndCleanup(true);
    }
    enemys.clear();
  }
}

void Breakout::recoverHP(PhysicsShape * shapeA, PhysicsShape * shapeB) {
  if (shapeA->getGroup() == 6)
    shapeA->getBody()->getNode()->removeFromParentAndCleanup(true);
  else
    shapeB->getBody()->getNode()->removeFromParentAndCleanup(true);
  pT->runAction(CCProgressTo::create(1.0, min(pT->getPercentage() + 20, 100.0f)));
  contactTime = max(contactTime - 2, 0);
}

void Breakout::newEnemys() {
  if (enemys.size() >= 30 && CCRANDOM_0_1() > 0.85)
    addBoom();
  if (enemys.size() >= 30 && CCRANDOM_0_1() > 0.85)
    addHeart();
  if (enemys.size() > 50) return;
  int newNum = 2;
  while (newNum--) {
    int type = 0;
    if (CCRANDOM_0_1() > 0.85) { type = 2; }
    else if (CCRANDOM_0_1() > 0.6) { type = 1; }

    Point location = Vec2(0, 0);
    switch (rand() % 4) {
      case 0:
        location.y = visibleSize.height;
        location.x = rand() % (int)(visibleSize.width);
        break;
      case 1:
        location.x = visibleSize.width;
        location.y = rand() % (int)(visibleSize.height);
        break;
      case 2:
        location.y = 0;
        location.x = rand() % (int)(visibleSize.width);
        break;
      case 3:
        location.x = 0;
        location.y = rand() % (int)(visibleSize.height);
        break;
      default:
        break;
    }
    addEnemy(type, location);
  }
}

void Breakout::addEnemy(int type, Point p) {
  char path[20];
  int tag;
  switch (type) {
    case 0:
      sprintf(path, "stone1.png");
      tag = 2;
      break;
    case 1:
      sprintf(path, "stone2.png");
      tag = 3;
      break;
    case 2:
      sprintf(path, "stone3.png");
      tag = 4;
      break;
  }
  auto re = Sprite::create(path);
  re->setPhysicsBody(PhysicsBody::createCircle(re->getContentSize().height / 2 - 2, PhysicsMaterial(0.0f, 0.7f, 0.0f)));
  re->setAnchorPoint(Vec2(0.5, 0.5));
  re->setScale(0.5, 0.5);
  re->setPosition(p);

  re->getPhysicsBody()->setGroup(tag);
  re->getPhysicsBody()->setCategoryBitmask(2);  //010
  re->getPhysicsBody()->setCollisionBitmask(3); //011
  re->getPhysicsBody()->setContactTestBitmask(1); //001
  if (rand() % 100 < 50) {
    re->getPhysicsBody()->setVelocity((player->getPosition() - p) * (0.25));
  }
  else {
    re->getPhysicsBody()->setVelocity((Point(rand() % (int)(visibleSize.width - 100) + 50, rand() % (int)(visibleSize.height - 100) + 50) - p) * (0.25));
  }
  re->getPhysicsBody()->setAngularVelocity(CCRANDOM_0_1() * 10);
  enemys.push_back(re->getPhysicsBody());
  this->addChild(re);
}

void Breakout::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
  switch (code) {
    case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_A:
      player->getPhysicsBody()->setVelocity(Point(-200, player->getPhysicsBody()->getVelocity().y));
      break;
    case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_D:
      player->getPhysicsBody()->setVelocity(Point(200, player->getPhysicsBody()->getVelocity().y));
      break;
    case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_W:
      player->getPhysicsBody()->setVelocity(Point(player->getPhysicsBody()->getVelocity().x, 200));
      break;
    case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_S:
      player->getPhysicsBody()->setVelocity(Point(player->getPhysicsBody()->getVelocity().x, -200));
      break;
    default:
      break;
  }
}

void Breakout::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
  switch (code) {
    case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_A:
      player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(-200, 0));
      break;
    case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_D:
      player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(200, 0));
      break;
    case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_W:
      player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(0, 200));
      break;
    case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_S:
      player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(0, -200));
      break;
    default:
      break;
  }
}
