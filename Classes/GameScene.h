#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"

class Ball : public cocos2d::Ref
{
private:
	std::vector<cocos2d::Color3B> _colors;
	std::vector<cocos2d::Color3B> _colors_set;
	std::vector<int> _empty;
	int _start_directon;
protected:
	Ball() : _sprite(nullptr) 
	{};
	bool init();
public:
	CREATE_FUNC(Ball);
	CC_SYNTHESIZE_RETAIN(cocos2d::Sprite*, _sprite, Sprite);
	cocos2d::Sprite* getBall(int pos, cocos2d::Color3B color);
	cocos2d::Sprite* set(int start, int empty);
	cocos2d::Node* getFireWork(bool isTrue);
	bool setEmpty(cocos2d::Vec2 pos, cocos2d::Color3B color);
	bool isTrueBall();
	
};

class Game : public cocos2d::Scene
{
private:
	CC_SYNTHESIZE_RETAIN(Ball*, _ball, Ball);
	CC_SYNTHESIZE_RETAIN(cocos2d::Sprite*, _mover, Mover);
	void setBallWithInit();
protected:
	Game() : _ball(nullptr), _mover(nullptr)
	{};
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // implement the "static create()" method manually
    CREATE_FUNC(Game);
};

#endif // __GAME_SCENE_H__
