#include "Global.h"
#include "LoginScene.h"
#include "BreakoutScene.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include <string>
#define database UserDefault::getInstance()
#pragma execution_character_set("utf-8")

using namespace std;
using namespace rapidjson;
USING_NS_CC;

Scene* LoginScene::createScene() {
  // 'scene' is an autorelease object
  auto scene = Scene::create();

  // 'layer' is an autorelease object
  auto layer = LoginScene::create();

  // add layer as a child to scene
  scene->addChild(layer);

  // return the scene
  return scene;
}

// on "init" you need to initialize your instance
bool LoginScene::init() {
  // 1. super init first
  if (!Layer::init()) {
    return false;
  }

  Size size = Director::getInstance()->getVisibleSize();
  visibleHeight = size.height;
  visibleWidth = size.width;

  // 用户头像
  auto user = Sprite::create("user.png");
  user->setPosition(visibleWidth / 2 - 160, visibleHeight / 2 + 100);
  this->addChild(user);
  auto box = Sprite::create("box.png");
  box->setPosition(visibleWidth / 2 + 40, visibleHeight / 2 + 100);
  this->addChild(box);

  // 用户名输入框
  userName = TextField::create("Player Name", "fonts/STXINWEI.TTF", 40);
  userName->setPosition(Vec2(visibleWidth / 2 + 40, visibleHeight / 2 + 100));
  userName->setMaxLengthEnabled(true);
  userName->setMaxLength(8);
  userName->setTextColor(Color4B(0, 0, 0, 255));
  this->addChild(userName, 1);


  // 登录按钮
  auto loginLabel = Label::createWithTTF("L o g i n", "fonts/Marker Felt.ttf", 48);
  loginLabel->setColor(Color3B(247, 147, 0));
  auto loginBtn = MenuItemLabel::create(loginLabel, CC_CALLBACK_1(LoginScene::loginClick, this));
  Menu* login = Menu::create(loginBtn, NULL);
  login->setPosition(visibleWidth / 2 + 30, visibleHeight / 2);
  this->addChild(login);

  // 记住登录,如果已登录直接登录
  if (database->getStringForKey("username") != "") {
    userName->setString(database->getStringForKey("username"));
    loginClick(nullptr);
  }
  return true;
}

// 点击登录按钮
void LoginScene::loginClick(Ref * pSender) {
  if (userName->getString() != "") {
    HttpRequest* req = new HttpRequest();
    req->setRequestType(HttpRequest::Type::POST);
    req->setUrl("localhost:8080/login");
    string data = "username=" + userName->getString();
    req->setRequestData(data.c_str(), data.size());
    req->setResponseCallback(CC_CALLBACK_2(LoginScene::onLoginCompleted, this));
    HttpClient::getInstance()->send(req);
    req->release();
  }
}

// 登录响应
void LoginScene::onLoginCompleted(HttpClient * sender, HttpResponse * res) {
  if (res == nullptr) return;

  // 登录成功, 获取ResponseHeader,提取gameSessionId,并保存用户名,下次可直接登录
  // 跳转到游戏界面
  if (res->isSucceed()) {
    string resHeader(res->getResponseHeader()->begin(), res->getResponseHeader()->end());
    Global::gameSessionId = Global::getSessionIdFromHeader(resHeader);
    database->setStringForKey("username", userName->getString());
    Director::getInstance()->replaceScene(TransitionCrossFade::create(0.3f, Breakout::createScene()));
  }
  else {
    log("Login failed: %s", res->getErrorBuffer());
  }
}

