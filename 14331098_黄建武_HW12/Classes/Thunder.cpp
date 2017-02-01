#include "Thunder.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include <algorithm>

USING_NS_CC;

using namespace CocosDenshion;

Scene* Thunder::createScene() {
  // 'scene' is an autorelease object
  auto scene = Scene::create();

  // 'layer' is an autorelease object
  auto layer = Thunder::create();

  // add layer as a child to scene
  scene->addChild(layer);

  // return the scene
  return scene;
}

bool Thunder::init() {
  if (!Layer::init()) {
    return false;
  }

  isMove = false;
  visibleSize = Director::getInstance()->getVisibleSize();

  // ��������
  auto bgsprite = Sprite::create("bg.jpg");
  bgsprite->setPosition(visibleSize / 2);
  bgsprite->setScale(visibleSize.width / bgsprite->getContentSize().width,
    visibleSize.height / bgsprite->getContentSize().height);
  this->addChild(bgsprite, 0);

  // ������ҷɻ�
  player = Sprite::create("player.png");
  player->setAnchorPoint(Vec2(0.5, 0.5));
  player->setPosition(visibleSize.width / 2, player->getContentSize().height);
  player->setName("player");
  this->addChild(player, 1);

  addEnemy(5);
  preloadMusic();
  playBgm();
  explosion();  // ������ը֡����
  addTouchListener();
  addKeyboardListener();
  addCustomListener();

  schedule(schedule_selector(Thunder::update), 0.05f, kRepeatForever, 0);

  return true;
}

//Ԥ���������ļ�
void Thunder::preloadMusic() {
  auto audio = SimpleAudioEngine::getInstance();
  audio->preloadBackgroundMusic("music/bgm.mp3");
  audio->preloadEffect("music/fire.wav");
  audio->preloadEffect("music/explore.wav");
}

//���ű�������
void Thunder::playBgm() {
  SimpleAudioEngine::getInstance()->playBackgroundMusic("music/bgm.mp3", true);
}

//��ʼ������
void Thunder::addEnemy(int n) {
  enemys.resize(n * 3);
  for (unsigned i = 0; i < 3; ++i) {
    char enemyPath[20];
    sprintf(enemyPath, "stone%d.png", 3 - i);
    double width = visibleSize.width / (n + 1.0),
      height = visibleSize.height - (50 * (i + 1));
    for (int j = 0; j < n; ++j) {
      auto enemy = Sprite::create(enemyPath);
      enemy->setAnchorPoint(Vec2(0.5, 0.5));
      enemy->setScale(0.5, 0.5);
      enemy->setPosition(width * (j + 1), height);
      enemys.push_back(enemy);
      addChild(enemy, 1);
    }
  }
}

//���������ƶ��������µ�һ��
void Thunder::newEnemy() {
  static int stone = 1;
  for (auto it = enemys.begin(); it != enemys.end(); ++it) {
    if (*it != NULL)
      (*it)->setPosition((*it)->getPosition() + Vec2(0, -50));
  }
  char enemyPath[20];
  sprintf(enemyPath, "stone%d.png", stone);
  double width = visibleSize.width / 6.0,
    height = visibleSize.height - 50;
  for (int j = 0; j < 5; ++j) {
    auto enemy = Sprite::create(enemyPath);
    enemy->setAnchorPoint(Vec2(0.5, 0.5));
    enemy->setScale(0.5, 0.5);
    enemy->setPosition(width * j + 65, height);
    enemys.push_back(enemy);
    addChild(enemy, 1);
  }
  stone = stone % 3 + 1;
}

//�ƶ��ɻ�
void Thunder::movePlane(char cid) {
  auto nowPos = player->getPosition();
  switch (cid) {
    case 'A':
      player->runAction(MoveBy::create(0.2f, Vec2(-min(player->getBoundingBox().getMinX(), 20.0f), 0)));
      break;
    case 'D':
      player->runAction(MoveBy::create(0.2f, Vec2(min(visibleSize.width - player->getBoundingBox().getMaxX(), 20.0f), 0)));
      break;
  }
}

//�����ӵ�
void Thunder::fire() {
  Sprite* bullet = Sprite::create("bullet.png");
  bullet->setAnchorPoint(Vec2(0.5, 0.5));
  bullets.push_back(bullet);
  bullet->setPosition(player->getPosition());
  addChild(bullet, 1);
  SimpleAudioEngine::getInstance()->playEffect("music/fire.wav", false);
  // ����ӵ�û�д���, ��ɳ���Ļ���Ƴ�; ����ӵ�������, �Ѿ�������˲���Ƴ���cleanup, ����ִ�д˻ص�����
  bullet->runAction(Sequence::create(MoveBy::create(1.0f, Vec2(0, visibleSize.height)), CallFunc::create([this, bullet]() {
    bullet->removeFromParentAndCleanup(true);
    this->bullets.remove(bullet);
  }), nullptr));
}

//��ը����֡
void Thunder::explosion() {
  auto texture = Director::getInstance()->getTextureCache()->addImage("explosion.png");
  explore.pushBack(SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(2, 0, 140, 140))));
  explore.pushBack(SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(210, 0, 140, 140))));
  explore.pushBack(SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(400, 0, 140, 140))));
  explore.pushBack(SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(590, 0, 140, 140))));
  explore.pushBack(SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(780, 0, 140, 140))));
  explore.pushBack(SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(2, 190, 140, 140))));
  explore.pushBack(SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(210, 190, 140, 140))));
  explore.pushBack(SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(400, 190, 140, 140))));
}

void Thunder::update(float f) {
  if (isMove)
    this->movePlane(movekey);
  static int ct = 0;
  static int dir = 4;
  ++ct;
  if (ct == 120)
    ct = 40, dir = -dir;
  else if (ct == 80) {
    dir = -dir;
    newEnemy();
  }
  else if (ct == 20)
    ct = 40, dir = -dir;

  //��ʯ�����ƶ�
  for (auto it = enemys.begin(); it != enemys.end(); ++it) {
    if (*it != NULL) {
      (*it)->setPosition((*it)->getPosition() + Vec2(dir, 0));
    }
  }
  EventCustom e("meet");
  _eventDispatcher->dispatchEvent(&e);
}

void Thunder::meet(EventCustom * event) {
  Sprite* temp;
  bool boom = false;
  for (auto it1 = bullets.begin(); it1 != bullets.end(); ) { //�ж��ӵ��Ƿ������ʯ
    boom = false;
    for (auto it2 = enemys.begin(); it2 != enemys.end(); ++it2) {
      if (*it1 && *it2 && (*it1)->getPosition().getDistance((*it2)->getPosition()) < 25) {
        temp = *it2;
        (*it1)->removeFromParentAndCleanup(true);
        (*it2)->runAction(Sequence::create(Animate::create(Animation::createWithSpriteFrames(explore, 0.05f, 1)), CallFunc::create([this, temp] {
          temp->removeFromParentAndCleanup(true);
        }), nullptr));
        SimpleAudioEngine::getInstance()->playEffect("music/explore.wav", false);
        boom = true;
        it1 = bullets.erase(it1);
        it2 = enemys.erase(it2);
        break;
      }
    }
    if (!boom) ++it1;
  }

  for (auto it = enemys.begin(); it != enemys.end(); ++it) { //�ж���Ϸ�Ƿ����
    if (*it && (*it)->getBoundingBox().getMinY() <= player->getBoundingBox().getMaxY()) {
      temp = player;
      player->runAction(Sequence::create(Animate::create(Animation::createWithSpriteFrames(explore, 0.05f, 1)), CallFunc::create([this, temp] {
        temp->removeFromParentAndCleanup(true);
        auto gameover = Sprite::create("gameOver.png");
        gameover->setAnchorPoint(Vec2(0.5, 0.5));
        gameover->setPosition(visibleSize / 2);
        this->addChild(gameover, 1);
      }), nullptr));

      SimpleAudioEngine::getInstance()->playEffect("music/explore.wav", false);
      unschedule(schedule_selector(Thunder::update));
      this->getEventDispatcher()->removeAllEventListeners();
      return; // ��Ϸ����Ҫ�˳�ѭ��, player�Ѿ��ǿ�ָ��, ����player->getBoundingBox()
    }
  }
}

void Thunder::addCustomListener() {
  auto meetListener = EventListenerCustom::create("meet", CC_CALLBACK_1(Thunder::meet, this));
  _eventDispatcher->addEventListenerWithFixedPriority(meetListener, 1);
}

//��Ӽ����¼�����
void Thunder::addKeyboardListener() {
  auto keyListener = EventListenerKeyboard::create();
  keyListener->onKeyPressed = CC_CALLBACK_2(Thunder::onKeyPressed, this);
  keyListener->onKeyReleased = CC_CALLBACK_2(Thunder::onKeyReleased, this);
  this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyListener, this);
}

void Thunder::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
  switch (code) {
    case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_A:
      movekey = 'A';
      isMove = true;
      break;
    case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_D:
      movekey = 'D';
      isMove = true;
      break;
    case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:
      fire();
      break;
    default:
      break;
  }
}

void Thunder::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
  switch (code) {
    case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_A:
      isMove = false;
      break;
    case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_D:
      isMove = false;
      break;
    default:
      break;
  }
}

//��Ӵ����¼�����
void Thunder::addTouchListener() {
  this->setTouchEnabled(true);
  auto touchListener = EventListenerTouchOneByOne::create();
  touchListener->onTouchBegan = CC_CALLBACK_2(Thunder::onTouchBegan, this);
  touchListener->onTouchMoved = CC_CALLBACK_2(Thunder::onTouchMoved, this);
  touchListener->onTouchEnded = CC_CALLBACK_2(Thunder::onTouchEnded, this);
  this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
}

bool Thunder::onTouchBegan(Touch *touch, Event *unused_event) {
  isClick = player->getBoundingBox().containsPoint(touch->getLocation());
  fire();
  return true;
}

void Thunder::onTouchMoved(Touch *touch, Event *unused_event) {
  if (isClick) {
    float x = touch->getDelta().x + player->getPositionX();
    if (x < 0) x = 0;
    if (x > visibleSize.width) x = visibleSize.width;
    player->setPosition(x, player->getPositionY());
  }
}

void Thunder::onTouchEnded(Touch *touch, Event *unused_event) {
  isClick = false;
}