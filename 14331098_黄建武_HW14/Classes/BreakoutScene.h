#pragma once

#include "cocos2d.h"
USING_NS_CC;
using namespace std;

class Breakout :public Layer {
public:
  void setPhysicsWorld(PhysicsWorld * world);
  // there's no 'id' in cpp, so we recommend returning the class instance pointer
  static cocos2d::Scene* createScene();

  // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
  virtual bool init(PhysicsWorld* world);

  // implement the "static create()" method manually
  static Breakout* create(PhysicsWorld* world);
  //CREATE_FUNC(Breakout);
private:
  bool isMove;
  Sprite* player;
  Sprite* boom;
  Sprite* heart;
  PhysicsWorld* m_world;
  Size visibleSize;
  Vec2 origin;
  Label* timeLabel;
  list<PhysicsBody*> enemys;
  Vector<SpriteFrame*> explore;
  ProgressTimer* pT;
  int contactTime;
  int time;

  void preloadMusic();
  void playBgm();
  void addTimer();
  void addBackground();
  void addEdge();
  void addPlayer();
  void addHP();
  void addBoom();
  void addHeart();

  void addContactListener();
  void addTouchListener();
  void addKeyboardListener();

  void update(float f);
  void cleanScreen(PhysicsShape* shapeA, PhysicsShape* shapeB);
  void recoverHP(PhysicsShape* shapeA, PhysicsShape* shapeB);
  bool onConcactBegin(PhysicsContact& contact);
  bool onTouchBegan(Touch *touch, Event *unused_event);
  void onTouchMoved(Touch *touch, Event *unused_event);
  void onTouchEnded(Touch *touch, Event *unused_event);
  void onKeyPressed(EventKeyboard::KeyCode code, Event* event);
  void onKeyReleased(EventKeyboard::KeyCode code, Event* event);

  void newEnemys();
  void addEnemy(int type, Point p);
};