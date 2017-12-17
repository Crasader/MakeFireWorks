#include "GameScene.h"
#include "SimpleAudioEngine.h"

#include "Util.h"

USING_NS_CC;

Scene* Game::createScene()
{
    return Game::create();
}

// on "init" you need to initialize your instance
bool Game::init()
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

	this->addChild(simple::getSprite("res/star.png", winSize.width / 2, winSize.height / 2 + 210));
	this->addChild(simple::getSprite("res/bar.png", winSize.width / 2, winSize.height / 2));
	this->addChild(simple::getSprite("res/table.png", winSize.width / 2, winSize.height / 2 - 210));

	setBall(Ball::create());

	for (int i = 0; i < 4; ++i)
	{
		auto item = simple::getSprite(StringUtils::format("res/ball%d.png", i + 1), winSize.width / 2 + 120 * (i - 1) - 60, winSize.height / 2 - 300);
		this->addChild(item);

		auto lis = simple::setEventListener(item);
		lis->onTouchBegan = [this, i](Touch* t, Event* e) {
			if (util::isTouchInEvent(t, e))
			{
				auto mover = simple::getSprite(StringUtils::format("res/ball%d.png", i + 1), t->getLocation().x, t->getLocation().y);
				setMover(mover);
				addChild(mover);
				return true;
			}
			return false;
		};
		lis->onTouchMoved = [this] (Touch* t, Event* e){
			getMover()->setPosition(t->getLocation());			
		};
		lis->onTouchEnded = [this, i](Touch* t, Event* e) {
			getMover()->removeFromParent();
			getBall()->setEmpty(t->getLocation(), i == 0 ? Color3B::RED : i == 1 ? Color3B::BLUE : i == 2 ? Color3B::GREEN : Color3B::YELLOW);
			setMover(nullptr);
		};
		lis->onTouchCancelled = lis->onTouchEnded;
	}


    /////////////////////////////
    // 3. add your codes below...
	setBallWithInit();


    return true;
}

void Game::setBallWithInit()
{
	auto winSize = Director::getInstance()->getWinSize();

	if (this->getChildByTag(10))
		this->getChildByTag(10)->removeFromParent();
	
	auto ball = getBall()->set(0, 4);
	ball->setPosition(winSize.width / 2, winSize.height / 2 - 140);
	ball->setTag(10);
	this->addChild(ball);

	auto fire = getBall()->getFireWork(true);
	fire->setPosition(winSize.width / 3 - 30, winSize.height / 2);
	this->addChild(fire);

	auto blis = simple::setSingleListener(ball);
	blis->onFlick = [this, ball, winSize](Vec2 pos, Vec2 diff, float time)
	{
		if (diff.y > 0)
		{
			auto fire = getBall()->getFireWork(false);
			if (fire)
			{
				fire->setPosition(winSize.width / 3 - 30, winSize.height / 2);
				this->addChild(fire);
				ball->runAction(Sequence::create(
					Spawn::create(
						EaseSineOut::create(MoveBy::create(0.5f, Vec2(0, 300))),
						FadeOut::create(0.2f),
						NULL),
					DelayTime::create(1.0f),
					CallFunc::create([this] { setBallWithInit(); }),
					NULL));
			}
		}
	};
}


bool Ball::init()
{
	
	return true;
}

inline Sprite* getBallByType(Color3B color, int x, int y)
{
	return simple::getSprite(color == Color3B::RED ? "res/ball1.png" : color == Color3B::BLUE ? "res/ball2.png" : color == Color3B::GREEN ? "res/ball3.png" : "res/ball4.png", x, y);
}

cocos2d::Sprite* Ball::getBall(int pos, Color3B color)
{

	auto ball = getBallByType(color, 0, 0);

	switch(pos)
	{
	case 0: ball->setPosition(96, 101); break;
	case 1: ball->setPosition(96, 171); break;
	case 2: ball->setPosition(146, 151); break;
	case 3: ball->setPosition(170, 101); break;
	case 4: ball->setPosition(148, 51); break;
	case 5: ball->setPosition(96, 31); break;
	case 6: ball->setPosition(48, 51); break;
	case 7: ball->setPosition(26, 101); break;
	case 8: ball->setPosition(48, 151); break;
	};
	ball->setTag(pos);

	return ball;

}

Sprite* Ball::set(int start, int empty)
{
	std::vector<int> num_list;
	for (int i = 0; i < 9; ++i)
		num_list.push_back(i);
	for (int i = 0; i < empty; ++i)
	{ 
		auto pos = (int)(util::getRand(0.0, 1.0) * num_list.size());
		_empty.push_back(pos);
		num_list.erase(num_list.begin() + pos);
	}
	for (int i = 0; i < 9; ++i)
	{
		auto rand = util::getRand(0.0, 1.0);
		_colors.push_back(rand < 0.25 ? Color3B::RED : rand < 0.5 ? Color3B::BLUE : rand < 0.75 ? Color3B::GREEN : Color3B::YELLOW);
		_colors_set.push_back(rand < 0.25 ? Color3B::RED : rand < 0.5 ? Color3B::BLUE : rand < 0.75 ? Color3B::GREEN : Color3B::YELLOW);
	}

	_start_directon = start;

	auto ball = simple::getSprite("res/ball.png", 0, 0);
	ball->setCascadeOpacityEnabled(true);
	for (int i = 0; i < 9; ++i)
		if (!util::find(_empty, i))
			ball->addChild(getBall(i, _colors[i]));
		else
			_colors_set[i] = Color3B::BLACK;
	
	setSprite(ball);

	return ball;
}

cocos2d::Node * Ball::getFireWork(bool isTrue)
{
	if (!isTrue && util::find(_colors_set, Color3B::BLACK))
		return nullptr;

	auto fire = Node::create();
	fire->setCascadeOpacityEnabled(true);
	for (int i = 0; i < 9; ++i)
	{
		auto ball = !isTrue && util::find(_empty, i) ? getBall(i, _colors_set[i]) : getBall(i, _colors[i]);
		ball->runAction(Sequence::create(
			DelayTime::create(1.0f),
			!isTrue && isTrueBall() ? CallFunc::create([fire] {
				auto o = simple::getSprite("res/o.png", 100, 120);
				fire->addChild(o);
				o->runAction(Sequence::create(
					FadeIn::create(0.1f),
					DelayTime::create(0.5f),
					FadeOut::create(0.1f),
					CallFunc::create([o] { o->removeFromParent(); }),
					NULL));
			}) :
			!isTrue && !isTrueBall() ? CallFunc::create([fire] {
				auto x = simple::getSprite("res/x.png", 100, 120);
				fire->addChild(x);
				x->runAction(Sequence::create(
					FadeIn::create(0.1f),
					DelayTime::create(0.5f),
					FadeOut::create(0.1f),
					CallFunc::create([x] { x->removeFromParent(); }),
					NULL));
			}) : CallFunc::create([] {}),
			EaseSineOut::create(MoveBy::create(0.5f,
				i == 0 ? Vec2(0, 0) :
				i == 1 ? Vec2(0, 100) :
				i == 2 ? Vec2(70, 70) :
				i == 3 ? Vec2(100, 0) :
				i == 4 ? Vec2(70, -70) :
				i == 5 ? Vec2(0, -100) :
				i == 6 ? Vec2(-70, -70) :
				i == 7 ? Vec2(-100, 0) : Vec2(-70, 70))),
			Spawn::create(
				EaseSineOut::create(MoveBy::create(0.2f, Vec2(0, -100))),
				FadeOut::create(0.2f),
			NULL),
			CallFunc::create([fire]() {
				fire->removeFromParent();
			}),
		NULL));
		fire->addChild(ball);
	}

	fire->runAction(Sequence::create(
		DelayTime::create(0.5f),
		Spawn::create(
			EaseSineOut::create(MoveBy::create(0.5f, Vec2(0, 200))),
			FadeIn::create(0.2f),
			NULL),
		NULL));


	return fire;
}

bool Ball::setEmpty(Vec2 pos, Color3B color)
{
	int pos_num = 0;
	pos -= getSprite()->getPosition();

	if (pos.x > -90 && pos.x < -30)
	{
		if (pos.y > -90 && pos.y < -30)
			pos_num = 6;
		else if (pos.y < 30)
			pos_num = 7;
		else
			pos_num = 8;
	}
	else if (pos.x < 34)
	{
		if (pos.y > -90 && pos.y < -30)
			pos_num = 5;
		else if (pos.y < 30)
			pos_num = 0;
		else
			pos_num = 1;
	}
	else
	{
		if (pos.y > -90 && pos.y < -30)
			pos_num = 4;
		else if (pos.y < 30)
			pos_num = 3;
		else
			pos_num = 2;
	}

	auto in = util::find(_empty, pos_num);

	if (in)
	{
		auto pre = getSprite()->getChildByTag(pos_num);
		if (pre)
			pre->removeFromParent();
		getSprite()->addChild(getBall(pos_num, color));
		_colors_set[pos_num] = color;
	}

	return in;
}

bool Ball::isTrueBall()
{
	for (auto i = 0; i < 9; ++i)
		if (_colors[i] != _colors_set[i])
			return false;
	return true;
}
