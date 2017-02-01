#ifndef __WELLDONE_H__
#define __WELLDONE_H__

#include "cocos2d.h"

USING_NS_CC;
using namespace std;
class WellDone : public cocos2d::Layer {
public:
  // there's no 'id' in cpp, so we recommend returning the class instance pointer
  static cocos2d::Scene* createScene();

  void continueCallback(Ref * pSender);

  void exitCallback(Ref * pSender);

  // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
  virtual bool init();

  // implement the "static create()" method manually
  CREATE_FUNC(WellDone);
};

#endif // __THUNDER_H__
