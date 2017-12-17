#include "TutorialScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* Tutorial::createScene()
{
    return Tutorial::create();
}

// on "init" you need to initialize your instance
bool Tutorial::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.


    /////////////////////////////
    // 3. add your codes below...

    
    return true;
}
