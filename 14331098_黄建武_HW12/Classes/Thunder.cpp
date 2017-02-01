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

  // 创建背景
  auto bgsprite = Sprite::create("bg.jpg");
  bgsprite->setPosition(visibleSize / 2);
  bgsprite->setScale(visibleSize.width / bgsprite->getContentSize().width,
    visibleSize.height / bgsprite->getContentSize().height);
  this->addChild(bgsprite, 0);

  // 创建玩家飞机
  player = Sprite::create("player.png");
  player->setAnchorPoint(Vec2(0.5, 0.5));
  player->setPosition(visibleSize.width / 2, player->getContentSize().height);
  player->setName("player");
  this->addChild(player, 1);

  addEnemy(5);
  preloadMusic();
  playBgm();
  explosion();  // 创建爆炸帧动画
  addTouchListener();
  addKeyboardListener();
  addCustomListener();

  schedule(schedule_selector(Thunder::update), 0.05f, kRepeatForever, 0);

  return true;
}

//预加载音乐文件
void Thunder::preloadMusic() {
  auto audio = SimpleAudioEngine::getInstance();
  audio->preloadBackgroundMusic("music/bgm.mp3");
  audio->preloadEffect("music/fire.wav");
  audio->preloadEffect("music/explore.wav");
}

//播放背景音乐
void Thunder::playBgm() {
  SimpleAudioEngine::getInstance()->playBackgroundMusic("music/bgm.mp3", true);
}

//初始化敌人
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

//敌人向下移动并生成新的一行
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

//移动飞机
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

//发射子弹
void Thunder::fire() {
  Sprite* bullet = Sprite::create("bullet.png");
  bullet->setAnchorPoint(Vec2(0.5, 0.5));
  bullets.push_back(bullet);
  bullet->setPosition(player->getPosition());
  addChild(bullet, 1);
  SimpleAudioEngine::getInstance()->playEffect("music/fire.wav", false);
  // 如果子弹没有打中, 则飞出屏幕后移除; 如果子弹打中了, 已经被打中瞬间移除并cleanup, 不会执行此回调函数
  bullet->runAction(Sequence::create(MoveBy::create(1.0f, Vec2(0, visibleSize.height)), CallFunc::create([this, bullet]() {
    bullet->removeFromParentAndCleanup(true);
    this->bullets.remove(bullet);
  }), nullptr));
}

//爆炸动画帧
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

  //陨石左右移动
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
  for (auto it1 = bullets.begin(); it1 != bullets.end(); ) { //判断子弹是否打中陨石
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

  for (auto it = enemys.begin(); it != enemys.end(); ++it) { //判断游戏是否结束
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
      return; // 游戏结束要退出循环, player已经是空指针, 不能player->getBoundingBox()
    }
  }
}

void Thunder::addCustomListener() {
  auto meetListener = EventListenerCustom::create("meet", CC_CALLBACK_1(Thunder::meet, this));
  _eventDispatcher->addEventListenerWithFixedPriority(meetListener, 1);
}

//添加键盘事件监听
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

//添加触摸事件监听
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