#include"Monster.h"
USING_NS_CC;

Factory* Factory::factory = NULL;
Factory::Factory() {
  initSpriteFrame();
}
Factory* Factory::getInstance() {
  if (factory == NULL) {
    factory = new Factory();
  }
  return factory;
}
void Factory::initSpriteFrame() {
  auto texture = Director::getInstance()->getTextureCache()->addImage("Monster.png");
  monsterDead.reserve(4);
  for (int i = 0; i < 4; i++) {
    auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(258 - 48 * i, 0, 42, 42)));
    monsterDead.pushBack(frame);
  }
}

Sprite* Factory::createMonster() {
  Sprite* mons = Sprite::create("Monster.png", CC_RECT_PIXELS_TO_POINTS(Rect(364, 0, 42, 42)));
  monster.pushBack(mons);
  return mons;
}

void Factory::removeMonster(Sprite* sp) {
  sp->runAction(Sequence::create(Animate::create(Animation::createWithSpriteFrames(monsterDead, 0.1f)), CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, sp)), nullptr));
  monster.eraseObject(sp);
}
void Factory::moveMonster(Vec2 playerPos, float time) {
  for each (Sprite* sp in monster) {
    Vec2 spPos = sp->getPosition();
    Vec2 direction = playerPos - spPos;
    direction.normalize();
    sp->runAction(MoveBy::create(time, direction * 30));
  }
}

Sprite* Factory::collider(Rect rect) {
  for each (Sprite* sp in monster) {
    if (rect.containsPoint(sp->getPosition()))
      return sp;
  }
}

Sprite* Factory::hitMonster(Rect rect) {
  Rect area = Rect(rect.getMinX() - 30, rect.getMinY() - 20, rect.getMaxX() - rect.getMinX() + 60, rect.getMaxY() - rect.getMinY() + 40);
  for each (Sprite* sp in monster) {
    if (area.containsPoint(sp->getPosition()))
      return sp;
  }
}