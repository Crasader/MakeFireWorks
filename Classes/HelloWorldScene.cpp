#include "HelloWorldScene.h"

#include "GameScene.h"
#include "TutorialScene.h"
#include "Util.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }
    
	auto winSize = Director::getInstance()->getWinSize();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.
	this->addChild(simple::getLabel("Make Fireworks !!", winSize.width / 2, winSize.height / 4 * 3, 60, Color3B::WHITE, Vec2::ANCHOR_MIDDLE));

	auto start = simple::getLabel("Start to game !", winSize.width / 2, winSize.height / 4 + 100, 30, Color3B::WHITE, Vec2::ANCHOR_MIDDLE);
	this->addChild(start);

	auto tutorial = simple::getLabel("Start to tutorial !", winSize.width / 2, winSize.height / 4 - 50, 30, Color3B::WHITE, Vec2::ANCHOR_MIDDLE);
	this->addChild(tutorial);

    /////////////////////////////
    // 3. add your codes below...
	auto slis = simple::setSingleListener(start);
	slis->onTap = [this](Vec2 pos, Node* t) {
		Director::getInstance()->replaceScene(Game::createScene());
	};

	auto tlis = simple::setSingleListener(tutorial);
	tlis->onTap = [this](Vec2 pos, Node* t) {
		Director::getInstance()->replaceScene(Tutorial::createScene());
	};

	util::initRand();
    
    return true;
}
