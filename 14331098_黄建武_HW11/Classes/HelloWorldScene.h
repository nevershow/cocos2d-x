#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
using namespace cocos2d;
class HelloWorld : public cocos2d::Layer {
public:
  static cocos2d::Scene* createScene();

  virtual bool init();
  void createBg();
  void createItem();
  void createPlayer();
  void hitMonster();
  void moveEvent(Ref*, char);
  void actionEvent(Ref*, char);
  void createMonster(float dt);
  void hitByMonster(float dt);
  void isKeyDown();

  // implement the "static create()" method manually
  CREATE_FUNC(HelloWorld);
private:
  cocos2d::Sprite* player;
  cocos2d::Vector<SpriteFrame*> dead;
  cocos2d::Animate* deadAnimate;
  cocos2d::Vector<SpriteFrame*> attack;
  cocos2d::Animate* attackAnimate;
  cocos2d::Vector<SpriteFrame*> run;
  cocos2d::Animate* runkAnimate;
  cocos2d::Size visibleSize;
  cocos2d::Vec2 origin;
  cocos2d::Label* killcountLabel;
  cocos2d::ProgressTimer* pT;
  cocos2d::SpriteFrame* frame0;
  int killcount;
  bool faceright;
  bool keydown;
  char key;
};

#endif // __HELLOWORLD_SCENE_H__
