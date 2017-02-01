#include "GameOver.h"
#include "Global.h"
#include "LoginScene.h"
#include "network/HttpClient.h"
#include "json/rapidjson.h"
#include "json/document.h"

#define database UserDefault::getInstance()

USING_NS_CC;
using namespace cocos2d::network;

Scene* GameOver::createScene() {
  // 'scene' is an autorelease object
  auto scene = Scene::create();

  // 'layer' is an autorelease object
  auto layer = GameOver::create();

  // add layer as a child to scene
  scene->addChild(layer);

  // return the scene
  return scene;
}

bool GameOver::init() {
  if (!Layer::init()) {
    return false;
  }

  auto visibleSize = Director::getInstance()->getVisibleSize();
  auto visibleHeight = visibleSize.height;
  auto visibleWidth = visibleSize.width;

  auto user = Sprite::create("user.png");
  user->setPosition(45, visibleHeight - 40);
  this->addChild(user);
  Label* username = Label::createWithTTF(database->getStringForKey("username"), "fonts/STXINWEI.TTF", 40);
  username->setPosition(110, visibleHeight - 40);
  username->setAnchorPoint(Vec2(0.2f, 0.5f));
  this->addChild(username);

  auto label = Label::createWithTTF("Logout", "fonts/Marker Felt.ttf", 40);
  label->setColor(Color3B(247, 147, 0));
  auto logoutBtn = MenuItemLabel::create(label, CC_CALLBACK_1(GameOver::logoutCallback, this));
  Menu* logout = Menu::create(logoutBtn, NULL);
  logout->setPosition(visibleSize.width - 80, 50);
  this->addChild(logout);

  string str = "Your score: ";
  char s[10];
  _itoa(database->getIntegerForKey("scoreInt"), s, 10);
  str += s;
  score = Label::createWithTTF(str, "fonts/STXINWEI.TTF", 40);
  score->setPosition(visibleWidth / 2 - 10, visibleHeight / 2);
  this->addChild(score);

  auto over = Sprite::create("gameOver.png");
  over->setPosition(visibleSize.width / 2, visibleSize.height / 3 * 2);
  this->addChild(over);
  
  // 提交按钮
  label = Label::createWithTTF("Submit", "fonts/Marker Felt.ttf", 40);
  label->setColor(Color3B(247, 147, 0));
  auto submitBtn = MenuItemLabel::create(label, CC_CALLBACK_1(GameOver::submitCallback, this));
  Menu* submit = Menu::create(submitBtn, NULL);
  submit->setPosition(visibleSize.width / 2 - 80, visibleSize.height / 2 - 100);
  this->addChild(submit);

  // 排名按钮
  label = Label::createWithTTF("Rank", "fonts/Marker Felt.ttf", 40);
  label->setColor(Color3B(247, 147, 0));
  auto rankBtn = MenuItemLabel::create(label, CC_CALLBACK_1(GameOver::rankCallback, this));
  Menu* rank = Menu::create(rankBtn, NULL);
  rank->setPosition(visibleSize.width / 2 + 45, visibleSize.height / 2 - 100);
  this->addChild(rank);

  topx = TextField::create("10", "fonts/STXINWEI.TTF", 40);
  topx->setPosition(Size(visibleWidth / 2 + 115, visibleHeight / 2 - 100));
  topx->setMaxLengthEnabled(true);
  topx->setMaxLength(1);
  this->addChild(topx);

  rank_field = Label::createWithTTF("", "fonts/STXINWEI.TTF", 40);
  rank_field->setPosition(visibleWidth / 5 * 4, visibleHeight - 50);
  rank_field->setContentSize(Size(visibleWidth / 4, visibleHeight / 5 * 3));
  rank_field->setAnchorPoint(Vec2(0.5, 1));
  this->addChild(rank_field);

  database->setIntegerForKey("scoreInt", 0);
  database->setIntegerForKey("timeInt", 10);

  return true;
}

// 点击提交按钮
void GameOver::submitCallback(Ref* sender) {
  HttpRequest* req = new HttpRequest();
  req->setRequestType(HttpRequest::Type::POST);
  req->setUrl("localhost:8080/submit");
  req->setResponseCallback(CC_CALLBACK_2(GameOver::onsubmitCompleted, this));
  vector<string> header;
  header.push_back("Cookie: " + Global::gameSessionId);
  req->setHeaders(header);
  string data = "score=" + score->getString().substr(12);
  req->setRequestData(data.c_str(), data.size());
  HttpClient::getInstance()->send(req);
  req->release();
}

// 点击排名按钮,最多查询前10名,默认查前十名,
void GameOver::rankCallback(Ref * sender) {
  if (topx->getString() == "0")
    topx->setString("");
  string top = (topx->getString() != "" ? topx->getString() : "10");
  HttpRequest* req = new HttpRequest();
  req->setRequestType(HttpRequest::Type::GET);
  req->setUrl(("localhost:8080/rank?top=" + top).c_str());
  req->setResponseCallback(CC_CALLBACK_2(GameOver::onrankCompleted, this));
  vector<string> header;
  header.push_back("Cookie: " + Global::gameSessionId);
  req->setHeaders(header);
  HttpClient::getInstance()->send(req);
  req->release();
}

void GameOver::logoutCallback(Ref * sender) {
  database->setStringForKey("username", "");
  Director::getInstance()->replaceScene(TransitionCrossFade::create(0.3f, LoginScene::createScene()));
}

// 提交响应函数,如果成功提交则更新排名
void GameOver::onsubmitCompleted(HttpClient* sender, HttpResponse* res) {
  if (res == nullptr) return;
  if (res->isSucceed())
    rankCallback(nullptr); 
  else {
    log("Login failed: %s", res->getErrorBuffer());
  }
}

// 排名响应函数,成功则显示前n名排名
void GameOver::onrankCompleted(HttpClient* sender, HttpResponse* res) {
  if (res == nullptr) return;
  if (res->isSucceed()) {
    string resData(res->getResponseData()->begin(), res->getResponseData()->end());
    rapidjson::Document json;
    json.Parse(resData.c_str());
    if (!json.HasParseError() && json.HasMember("result") && json["result"].GetBool()) {
      string rankRes = json["info"].GetString();
      rankRes.erase(0, 1);
      for (size_t i = 0; i < rankRes.length(); ++i)
        if (i && rankRes[i] == '|')
          rankRes[i] = '\n';
      rank_field->setString(rankRes);
    }
  }
  else {
    log("Login failed: %s", res->getErrorBuffer());
  }
}