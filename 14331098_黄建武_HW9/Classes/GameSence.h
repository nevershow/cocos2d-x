#ifndef __GAME_SENCE_H__
#define __GAME_SENCE_H__

#include "cocos2d.h"
USING_NS_CC;

class GameSence : public cocos2d::Layer {
public:
  static cocos2d::Scene* createScene();

  virtual bool init();

  virtual bool onTouchBegan(Touch *touch, Event *unused_event);

  virtual void shootCallback(Ref* pSender);

  CREATE_FUNC(GameSence);

private:
  Sprite* mouse;
  Sprite* stone;
  Sprite* cheese;
  Layer* mouseLayer;
  Layer* stoneLayer;
  Vec2 toPos;
};

#endif // __GAME_SENCE_H__
