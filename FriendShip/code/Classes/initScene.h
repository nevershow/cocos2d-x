#pragma once
#include "cocos2d.h"
using namespace std;
USING_NS_CC;
class initScene : public Layer {
public:
  static cocos2d::Scene* createScene();

  virtual bool init();

  // implement the "static create()" method manually
  CREATE_FUNC(initScene);

  void OneP(Ref* sender);
  void TwoP(Ref* sender);

private:
  Size visibleSize;
};
