#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "ui/CocosGUI.h"
#include "cocos2d.h"
#include "network/HttpClient.h"

using namespace cocos2d::ui;
using namespace cocos2d::network;

class LoginScene : public cocos2d::Layer {
public:
  // there's no 'id' in cpp, so we recommend returning the class instance pointer
  static cocos2d::Scene* createScene();

  // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
  virtual bool init();

  // implement the "static create()" method manually
  CREATE_FUNC(LoginScene);
  void loginClick(Ref* pSender);
  void onLoginCompleted(HttpClient* sender, HttpResponse* res);
private:
  float visibleHeight;
  float visibleWidth;
  TextField * userName;
};

#endif // __HELLOWORLD_SCENE_H__
