#ifndef __FRIEND_SHIP_H__
#define __FRIEND_SHIP_H__

#include "cocos2d.h"
using namespace std;
USING_NS_CC;
class FriendShip : public Layer {
public:
  static PhysicsWorld* world;
  static bool twoP;
  static cocos2d::Scene* createScene();
  void setPhysicsWorld(PhysicsWorld * world);
  virtual bool init();

  void addSprite();
  void addPlayer();
  void addListener();

  void update(float dt);
  void updateShip(float dt);
  void updateTime(float dt);
  void boxFall(float dt);

  void onKeyPressed(EventKeyboard::KeyCode code, Event * event);
  void onKeyReleased(EventKeyboard::KeyCode code, Event * event);
  bool onConcactBegin(PhysicsContact & contact);

  void loadAnimation(string filepath);
  void GameOver();
  void clickCallback(Ref * pSender);
  void exitCallback(Ref * pSender);

  // implement the "static create()" method manually
  CREATE_FUNC(FriendShip);

private:
  int dtime;
  Label* nowtime;
  PhysicsWorld* m_world;
  Size visibleSize;
  list<PhysicsBody*> boxes;
  Node* edgeNode;
  Node* holdbox1;
  Node* holdbox2;
  Sprite* ship;
  Sprite* player1;
  Sprite* player2;
  SpriteFrame* frame1;
  SpriteFrame* frame2;
  SpriteFrame* IdleWithBox1;
  SpriteFrame* IdleWithBox2;
  PhysicsJointDistance * joint1;
  PhysicsJointDistance * joint2;
  char LastPlayer1Press;
  char LastPlayer2Press;
  bool IsPlayer1Hold;
  bool IsPlayer2Hold;
  bool IsPlayer1Jump;
  bool IsPlayer2Jump;
  bool IsPlayer1Left;
  bool IsPlayer2Left;
  bool IsPlayer1Right;
  bool IsPlayer2Right;
  double detaH;
  double height;
};

#endif // __FRIEND_SHIP_H__
