#ifndef __MULTI_LISTENER_H__
#define __MULTI_LISTENER_H__

#include "cocos2d.h"

USING_NS_CC;

// initial value of long tap threshold
constexpr float DefaultLongTapThreshold = 0.3f;
// initial value of double tap threshold
constexpr float DefaultDoubleTapThreshold = 0.15f;
// initial value of swipe length threshold
constexpr float DefaultSwipeThresholdDistance = 10.0f;
// initial value of flick length threshold
constexpr float DefaultFlickThreshold = 0.1f;

/** Scale of scroll by mouse */
constexpr float DefaultScrollMagnification = 1.2f;

/*
 * Detects multiple taps, execute pinch-in and pinch-out
 */
class MultiTouchListener : public cocos2d::EventListenerTouchAllAtOnce
{
public:
	/** Tap state */
	enum class State { check, tap, swipe, multi, longtap };

private:
	// Can not use the following variables
	using cocos2d::EventListenerTouchAllAtOnce::LISTENER_ID;
	using cocos2d::EventListenerTouchAllAtOnce::onTouchesBegan;
	using cocos2d::EventListenerTouchAllAtOnce::onTouchesMoved;
	using cocos2d::EventListenerTouchAllAtOnce::onTouchesEnded;
	using cocos2d::EventListenerTouchAllAtOnce::onTouchesCancelled;

	// number of touching fingers
	int _numberOfTouch;
	// start point of tap
	std::vector<cocos2d::Vec2> _startPos;
	// prior point of tap
	std::vector<cocos2d::Vec2> _movePos;
	// start time of tap
	std::vector<struct timeval> _startTime;
	// prior time of tap
	std::vector<struct timeval> _moveTime;
	// now state of tap
	std::vector<State> _state;

	// start point of multiple taps
	cocos2d::Vec2 _multiPos;
	// start time of multiple taps
	struct timeval _multiTime;
	// Squared distance of multiple taps
	float _multiDiff;
	// processing of tap
	cocos2d::Sequence* _tap_action;

public:
	CREATE_FUNC(MultiTouchListener);
	// value of long tap threshold
	CC_SYNTHESIZE(float, _longTapThreshold, LongTapThreshold);
	// value of double tap threshold
	CC_SYNTHESIZE(float, _doubleTapThreshold, DoubleTapThreshold);
	// value of swipe length threshold
	CC_SYNTHESIZE(float, _swipeThreshold, SwipeThreshold);
	// value of flick length threshold
	CC_SYNTHESIZE(float, _flickThreshold, FlickThreshold);

	// function for starting tap. argument is tap point
	std::function<void(cocos2d::Vec2)> onTapBegan;
	// function for tap. argument is tap point
	std::function<void(cocos2d::Vec2)> onTap;
	// function for starting long tap. argument is tap point
	std::function<void(cocos2d::Vec2)> onLongTapBegan;
	// function for finishing long tap. argument is tap point
	std::function<void(cocos2d::Vec2)> onLongTapEnd;
	// function for double tap. argument is tap point
	std::function<void(cocos2d::Vec2)> onDoubleTap;

	// function for swipe. arguments are tap point, swipe difference(current - prior), swipe time
	std::function<void(cocos2d::Vec2, cocos2d::Vec2, float)> onSwipe;
	// function for flick. arguments are tap point, flick difference(current - prior), flick time
	std::function<void(cocos2d::Vec2, cocos2d::Vec2, float)> onFlick;
	// function for pinch-in / pinch-out. arguments are tap point, ratio(current / prior)
	std::function<void(cocos2d::Vec2, float)> pinch;

	// function for checking before swipe.  arguments are tap point, swipe difference(current - prior), swipe time. if return true, move to swipe function
	std::function<bool(cocos2d::Vec2, cocos2d::Vec2, float)> onSwipeCheck;
	// function for checking before flick.  arguments are tap point, flick difference(current - prior), flicj time. if return true, move to flick function
	std::function<bool(cocos2d::Vec2, cocos2d::Vec2, float)> onFlickCheck;

private:
	/*
	 * Get difference between start time and current time
	 */
	inline float getStartDiffTime(const struct timeval &time, const int id) { return (float)(time.tv_sec - _startTime[id].tv_sec) + (float)(time.tv_usec - _startTime[id].tv_usec) / 1000000.0f; };
	/*
	 * Get difference between prior time and current time
	 */
	inline float getMoveDiffTime(const struct timeval &time, const int id) { return (float)(time.tv_sec - _moveTime[id].tv_sec) + (float)(time.tv_usec - _moveTime[id].tv_usec) / 1000000.0f; };
	/*
	 * Get difference between start time and current time for multiple taps
	 */
	inline float getMultiDiffTime(const struct timeval &time) { return (float)(time.tv_sec - _multiTime.tv_sec) + (float)(time.tv_usec - _multiTime.tv_usec) / 1000000.0f; };
protected:
	MultiTouchListener()
		: _numberOfTouch(0)
		, _longTapThreshold(DefaultLongTapThreshold), _doubleTapThreshold(DefaultDoubleTapThreshold)
		, _swipeThreshold(DefaultSwipeThresholdDistance), _flickThreshold(DefaultFlickThreshold)
		, onTapBegan(nullptr), onTap(nullptr), onLongTapBegan(nullptr), onLongTapEnd(nullptr), onDoubleTap(nullptr)
		, onSwipe(nullptr), pinch(nullptr)
		, onSwipeCheck(nullptr), onFlickCheck(nullptr)
		, _tap_action(nullptr)
	{
		_startPos.resize(5);
		_startTime.resize(5);
		_movePos.resize(5);
		_moveTime.resize(5);
		_state.resize(5);
	};

	~MultiTouchListener()
	{
		_numberOfTouch = 0;
		_longTapThreshold = _doubleTapThreshold = _swipeThreshold = _flickThreshold = 0;
		onTapBegan = onTap = onLongTapBegan = onLongTapEnd = onDoubleTap = nullptr;
		onSwipe = nullptr;
		pinch = nullptr;
		onSwipeCheck = onFlickCheck = nullptr;
		_tap_action = nullptr;
	};

	/*
	 * Initialization
	 * Setting listener process
	 */
	virtual bool init()
	{
		if (!EventListenerTouchAllAtOnce::init())
			return false;

		onTouchesBegan = [this](const std::vector<cocos2d::Touch*> &touches, cocos2d::Event* event)
		{
			// update current tap number
			_numberOfTouch += touches.size();
			for (auto t : touches)
			{
				// execute starting tap event
				if (onTapBegan)
					onTapBegan(t->getLocation());
				// Do not acquire after the third one
				if (t->getID() > 1)
					return;
				// initialize point and time, state
				_startPos[t->getID()] = _movePos[t->getID()] = t->getLocation();
				gettimeofday(&_startTime[t->getID()], NULL);
				gettimeofday(&_moveTime[t->getID()], NULL);
				_state[t->getID()] = MultiTouchListener::State::check;

				// run process for checking long tap
				if (t->getID() == 0)
					cocos2d::Director::getInstance()->getScheduler()->schedule(schedule_selector(MultiTouchListener::update), this, 0.05f, false);
			}
			// process for multiple taps
			if (_numberOfTouch > 1)
			{
				// all state change multi
				for (int i = 0; i < _numberOfTouch; i++)
					_state[i] = MultiTouchListener::State::multi;
				//
				// initialize point and time
				_multiPos = (_movePos[0] + _movePos[1]) / 2;
				_multiDiff = (_movePos[0] - _movePos[1]).lengthSquared();
				gettimeofday(&_multiTime, NULL);
			}
		};
		onTouchesMoved = [this](const std::vector<cocos2d::Touch*> &touches, cocos2d::Event* event)
		{
			for (cocos2d::Touch* t : touches)
			{
				// Do not acquire after the third one
				if (t->getID() > 1)
					return;
				// check move from prior point. when it exceeds the threshold, switch to swipe
				if (_state[t->getID()] == MultiTouchListener::State::check ||
					_state[t->getID()] != MultiTouchListener::State::tap)
				{
					auto diff = (t->getLocation() - _movePos[t->getID()]).lengthSquared();
					if (diff > _swipeThreshold)
						_state[t->getID()] = MultiTouchListener::State::swipe;
					else
						_state[t->getID()] = MultiTouchListener::State::tap;
				}

				// when swipe state, execute swipe process
				if (onSwipe && _state[t->getID()] == MultiTouchListener::State::swipe)
				{
					struct timeval time;
					gettimeofday(&time, NULL);
					auto diff = (t->getLocation() - _movePos[t->getID()]);
					auto diff_time = getMoveDiffTime(time, t->getID());
					if (!onSwipeCheck || onSwipeCheck(t->getLocation(), diff, diff_time))
						onSwipe(t->getLocation(), diff, diff_time);
				}

				// update point and time
				_movePos[t->getID()] = t->getLocation();
				gettimeofday(&_moveTime[t->getID()], NULL);
			}

			// multiple taps process
			if (_state[0] == MultiTouchListener::State::multi)
			{
				// update multiple taps information
				struct timeval time;
				gettimeofday(&time, NULL);
				auto diff = (_movePos[0] + _movePos[1]) / 2 - _multiPos;
				auto diff_time = getMultiDiffTime(time);
				_multiPos = (_movePos[0] + _movePos[1]) / 2;
				_multiTime = time;

				// swipe process
				if (onSwipe && (!onSwipeCheck || onSwipeCheck(_multiPos, diff, diff_time)))
					onSwipe(_multiPos, diff, diff_time);

				// pinch-in / pinch-out process
				auto diff_multi = (_movePos[0] - _movePos[1]).lengthSquared();
				if (pinch && diff_multi != _multiDiff)
					pinch(_multiPos, diff_multi / _multiDiff);

				// update difference
				_multiDiff = diff_multi;
			}
		};
		onTouchesCancelled = [this](const std::vector<cocos2d::Touch*> &touches, cocos2d::Event* event)
		{
			// decrease current tap number
			_numberOfTouch -= touches.size();
			if (_numberOfTouch < 0)
				_numberOfTouch = 0;
			// when tap number is zero, stop process for checking long tap
			if (_numberOfTouch == 0)
				cocos2d::Director::getInstance()->getScheduler()->unschedule(schedule_selector(MultiTouchListener::update), this);
		};
		onTouchesEnded = [this](const std::vector<cocos2d::Touch*> &touches, cocos2d::Event* event)
		{
			// decrease current tap number
			_numberOfTouch -= touches.size();
			if (_numberOfTouch < 0)
			{
				_numberOfTouch = 0;
				return;
			}

			for (cocos2d::Touch* t : touches)
			{
				// Do not acquire after the third one
				if (t->getID() > 1)
					return;

				// Set initial information
				struct timeval time;
				gettimeofday(&time, NULL);
				auto diff_time = getStartDiffTime(time, t->getID());
				// Set locale for capturing by lambda
				auto locale = t->getLocation();

				// Change processing according to the state
				switch (_state[t->getID()])
				{
				case MultiTouchListener::State::check:
					// Check proces is tap process
				case MultiTouchListener::State::tap:
					// If tap processing has already been done, stop it and execute double tap processing
					if (_tap_action)
					{
						Director::getInstance()->getRunningScene()->stopAction(_tap_action);
						if (onDoubleTap)
							onDoubleTap(locale);
						_tap_action = nullptr;
						break;
					}
					// Execute tap processing after delay
					_tap_action = Sequence::create(
						DelayTime::create(getDoubleTapThreshold()),
						CallFunc::create([this, locale] {
						if (onTap)
							onTap(locale);
						_tap_action = nullptr;
					}),
						NULL);
					Director::getInstance()->getRunningScene()->runAction(_tap_action);
					break;
				case MultiTouchListener::State::longtap:
					if (onLongTapEnd)
						onLongTapEnd(locale);
					break;
				case MultiTouchListener::State::swipe:
					if (diff_time < _flickThreshold)
						if (onFlick && (!onFlickCheck || onFlickCheck(locale, locale - _startPos[t->getID()], diff_time)))
							onFlick(t->getLocation(), locale - _startPos[t->getID()], diff_time);
					break;
				case MultiTouchListener::State::multi:
					break;
				}
			}
			// when tap number is zero, stop process for checking long tap
			if (_numberOfTouch == 0)
				cocos2d::Director::getInstance()->getScheduler()->unschedule(schedule_selector(MultiTouchListener::update), this);
			// when still tapping, switch to swipe state
			else if (_numberOfTouch == 1)
				_state[0] = _state[1] = MultiTouchListener::State::swipe;
		};
		return true;
	};

	/*
	 * Detect long tap
	 * @param t clock
	 */
	void update(float t)
	{
		struct timeval time;
		gettimeofday(&time, NULL);
		auto diff_time = getStartDiffTime(time, 0);
		switch (_state[0])
		{
		case MultiTouchListener::State::check:
		case MultiTouchListener::State::tap:
			// If tapped beyond the threshold, execute long tap processing and stop this function
			if (diff_time > _longTapThreshold)
			{
				if (onLongTapBegan)
					onLongTapBegan(_startPos[0]);
				_state[0] = MultiTouchListener::State::longtap;
			}
			else
				break;
		case MultiTouchListener::State::swipe:
		case MultiTouchListener::State::multi:
			// When state is swipe or multi, stop this process
			cocos2d::Director::getInstance()->getScheduler()->unschedule(schedule_selector(MultiTouchListener::update), this);
			break;
		}
	};
};

/*
 * シングルタップを検知し、種々の処理を実行できるリスナー
 */
class SingleTouchListener : public cocos2d::EventListenerTouchOneByOne
{
public:
	/** Tap state */
	enum class State { check, tap, swipe, longtap };
private:
	// Can not use the following variables
	using cocos2d::EventListenerTouchOneByOne::onTouchBegan;
	using cocos2d::EventListenerTouchOneByOne::onTouchMoved;
	using cocos2d::EventListenerTouchOneByOne::onTouchEnded;
	using cocos2d::EventListenerTouchOneByOne::onTouchCancelled;

	// start point of tap
	cocos2d::Vec2 _startPos;
	// prior point of tap
	cocos2d::Vec2 _movePos;
	// start time of tap
	struct timeval _startTime;
	// prior time of tap
	struct timeval _moveTime;
	// now state of tap
	State _state;
	// processing of tap
	cocos2d::Sequence* _tap_action;
	// reserved event pointer
	CC_SYNTHESIZE_RETAIN(cocos2d::Node*, _reserve_target, ReserveTarget);

public:
	CREATE_FUNC(SingleTouchListener);
	// value of long tap threshold
	CC_SYNTHESIZE(float, _longTapThreshold, LongTapThreshold);
	// value of double tap threshold
	CC_SYNTHESIZE(float, _doubleTapThreshold, DoubleTapThreshold);
	// value of swipe length threshold
	CC_SYNTHESIZE(float, _swipeThreshold, SwipeThreshold);
	// value of flick length threshold
	CC_SYNTHESIZE(float, _flickThreshold, FlickThreshold);

	// function for starting tap. arguments are tap point and node
	std::function<bool(cocos2d::Vec2, cocos2d::Node*)> onTouchBeganChecking;
	// function for finishing tap. arguments are tap point and node
	std::function<void(cocos2d::Vec2, cocos2d::Node*)> onTouchEndedChecking;
	// function for tap. arguments are tap point and node
	std::function<void(cocos2d::Vec2, cocos2d::Node*)> onTap;
	// function for starting long tap. arguments are tap point and node
	std::function<void(cocos2d::Vec2, cocos2d::Node*)> onLongTapBegan;
	// function for finishing long tap. arguments are tap point and node
	std::function<void(cocos2d::Vec2, cocos2d::Node*)> onLongTapEnd;
	// function for double tap. argument are tap point and node
	std::function<void(cocos2d::Vec2, cocos2d::Node*)> onDoubleTap;

	// function for swipe. arguments are tap point, swipe difference(current - prior), swipe time
	std::function<void(cocos2d::Vec2, cocos2d::Vec2, float)> onSwipe;
	// function for flick. arguments are tap point, flick difference(current - prior), flick time
	std::function<void(cocos2d::Vec2, cocos2d::Vec2, float)> onFlick;

	// function for checking before swipe.  arguments are tap point, swipe difference(current - prior), swipe time. if return true, move to swipe function
	std::function<bool(cocos2d::Vec2, cocos2d::Vec2, float)> onSwipeChecking;
	// function for checking before flick.  arguments are tap point, flick difference(current - prior), flicj time. if return true, move to flick function
	std::function<bool(cocos2d::Vec2, cocos2d::Vec2, float)> onFlickChecking;

private:
	/*
	 * Get difference between start time and current time
	 */
	inline float getStartDiffTime(const struct timeval &time) { return (float)(time.tv_sec - _startTime.tv_sec) + (float)(time.tv_usec - _startTime.tv_usec) / 1000000.0f; };
	/*
	 * Get difference between prior time and current time
	 */
	inline float getMoveDiffTime(const struct timeval &time) { return (float)(time.tv_sec - _moveTime.tv_sec) + (float)(time.tv_usec - _moveTime.tv_usec) / 1000000.0f; };
protected:
	SingleTouchListener()
		: _longTapThreshold(DefaultLongTapThreshold), _doubleTapThreshold(DefaultDoubleTapThreshold)
		, _swipeThreshold(DefaultSwipeThresholdDistance), _flickThreshold(DefaultFlickThreshold)
		, onTap(nullptr), onLongTapBegan(nullptr), onLongTapEnd(nullptr), onDoubleTap(nullptr)
		, onSwipe(nullptr), onSwipeChecking(nullptr)
		, onFlick(nullptr), onFlickChecking(nullptr)
		, onTouchBeganChecking(nullptr), onTouchEndedChecking(nullptr)
		, _tap_action(nullptr)
		, _reserve_target(nullptr)
	{
	};

	~SingleTouchListener()
	{
		_longTapThreshold = _doubleTapThreshold = _swipeThreshold = _flickThreshold = 0;
		onTap = onLongTapEnd = onDoubleTap = nullptr;
		onLongTapBegan = nullptr;
		onSwipe = nullptr;
		onSwipeChecking = nullptr;
		onFlick = nullptr;
		onFlickChecking = nullptr;
		onTouchBeganChecking = nullptr;
		onTouchEndedChecking = nullptr;
		_tap_action = nullptr;
		CC_SAFE_RELEASE_NULL(_reserve_target);
	};

	/*
	 * Initialization
	 * Setting listener process
	 */
	virtual bool init()
	{
		if (!EventListenerTouchOneByOne::init())
			return false;

		onTouchBegan = [this](cocos2d::Touch *touch, cocos2d::Event *event)
		{
			// Check tap process
			if (onTouchBeganChecking && !onTouchBeganChecking(touch->getLocation(), event->getCurrentTarget()))
				return false;

			// initialize point and time and state, node
			_startPos = touch->getLocation();
			gettimeofday(&_startTime, NULL);
			_movePos = touch->getLocation();
			gettimeofday(&_moveTime, NULL);
			_state = SingleTouchListener::State::check;
			setReserveTarget(event->getCurrentTarget());

			// run process for checking long tap
			cocos2d::Director::getInstance()->getScheduler()->schedule(schedule_selector(SingleTouchListener::update), this, 0.05f, false);
			return true;
		};
		onTouchMoved = [this](cocos2d::Touch *touch, cocos2d::Event *event)
		{
			// Set base data
			struct timeval time;
			gettimeofday(&time, NULL);
			auto diff = (touch->getLocation() - _movePos);
			auto diff_time = getMoveDiffTime(time);

			// check move from prior point. when it exceeds the threshold, switch to swipe
			if (_state == SingleTouchListener::State::check ||
				_state == SingleTouchListener::State::tap)
			{
				if (diff.lengthSquared() > _swipeThreshold)
				{
					if (!onSwipeChecking || onSwipeChecking(touch->getLocation(), diff, 0))
						_state = SingleTouchListener::State::swipe;
				}
				else
					_state = SingleTouchListener::State::tap;
			}

			// when swipe state, execute swipe process
			if (onSwipe && _state == SingleTouchListener::State::swipe)
				onSwipe(touch->getLocation(), diff, diff_time);

			// update point and time
			_movePos = touch->getLocation();
			gettimeofday(&_moveTime, NULL);
		};
		onTouchCancelled = [this](cocos2d::Touch *touch, cocos2d::Event *event)
		{
			// stop process for checking long tap
			cocos2d::Director::getInstance()->getScheduler()->unschedule(schedule_selector(SingleTouchListener::update), this);
			CC_SAFE_RELEASE_NULL(_reserve_target);
		};
		onTouchEnded = [this](cocos2d::Touch *touch, cocos2d::Event *event)
		{
			// Set initial information
			struct timeval time;
			gettimeofday(&time, NULL);
			auto diff_time = getStartDiffTime(time);
			auto locale = touch->getLocation();

			// Change processing according to the state
			switch (_state)
			{
			case SingleTouchListener::State::check:
				// Check proces is tap process
			case SingleTouchListener::State::tap:
				// If tap processing has already been done, stop it and execute double tap processing
				if (_tap_action)
				{
					Director::getInstance()->getRunningScene()->stopAction(_tap_action);
					if (onDoubleTap)
						onDoubleTap(touch->getLocation(), _reserve_target);
					_tap_action = nullptr;
					break;
				}
				// Execute tap processing after delay
				_tap_action = Sequence::create(
					DelayTime::create(getDoubleTapThreshold()),
					CallFunc::create([this, locale, event] {
					if (onTap)
						onTap(locale, _reserve_target);
					_tap_action = nullptr;
					}),
				NULL);
				Director::getInstance()->getRunningScene()->runAction(_tap_action);
				break;
			case SingleTouchListener::State::longtap:
				if (onLongTapEnd)
					onLongTapEnd(touch->getLocation(), _reserve_target);
				break;
			case SingleTouchListener::State::swipe:
				if (diff_time < _flickThreshold)
					if (onFlick && (!onFlickChecking || onFlickChecking(touch->getLocation(), touch->getLocation() - _startPos, diff_time)))
						onFlick(touch->getLocation(), touch->getLocation() - _startPos, diff_time);
				break;
			}

			cocos2d::Director::getInstance()->getScheduler()->unschedule(schedule_selector(SingleTouchListener::update), this);
			if (onTouchEndedChecking) 
				onTouchEndedChecking(touch->getLocation(), _reserve_target);

			CC_SAFE_RELEASE_NULL(_reserve_target);
		};
		return true;
	};

	/*
	 * Detect long tap
	 * @param t clock
	 */
	void update(float t)
	{
		struct timeval time;
		gettimeofday(&time, NULL);
		auto diff_time = (float)(time.tv_sec - _startTime.tv_sec) + (float)(time.tv_usec - _startTime.tv_usec) / 1000000;

		switch (_state)
		{
		case SingleTouchListener::State::check:
		case SingleTouchListener::State::tap:
			// If tapped beyond the threshold, execute long tap processing and stop this function
			if (diff_time > _longTapThreshold)
			{
				if (onLongTapBegan) 
					onLongTapBegan(_startPos, _reserve_target);
				_state = SingleTouchListener::State::longtap;
			}
			else
				break;
		case SingleTouchListener::State::swipe:
			// When state is swipe or multi, stop this process
			cocos2d::Director::getInstance()->getScheduler()->unschedule(schedule_selector(SingleTouchListener::update), this);
			CC_SAFE_RELEASE_NULL(_reserve_target);
			break;
		}
	};
};

/*
 * mouse listener for pinch-in / pinch-out process
 */
class MultiMouseListener : public cocos2d::EventListenerMouse
{
protected:
	MultiMouseListener()
		: _touchListener(nullptr)
		, _scrollMagnification(DefaultScrollMagnification)
	{
	};
	~MultiMouseListener()
	{
		CC_SAFE_RELEASE_NULL(_touchListener);
		_scrollMagnification = 0;
	};
public:
	CC_SYNTHESIZE_RETAIN(MultiTouchListener*, _touchListener, TouchListener);
	CC_SYNTHESIZE(float, _scrollMagnification, ScrollMagnification);
	/*
	 * Create instance by multiple tap listener
	 * @param lis multiple tap listener
	 */
	static MultiMouseListener* create(MultiTouchListener *lis)
	{
		MultiMouseListener *pRet = new(std::nothrow) MultiMouseListener();
		if (pRet && pRet->init(lis))
		{
			pRet->autorelease();
			return pRet;
		}
		else
		{
			delete pRet;
			pRet = nullptr;
			return nullptr;
		}
	}
	/*
	 * Initialize
	 */
	virtual bool init(MultiTouchListener *lis)
	{
		if (!EventListenerMouse::init())
			return false;

		setTouchListener(lis);

		// Convert scroll events into pinch-in / pinch-out
		onMouseScroll = [this](cocos2d::EventMouse *event)
		{
			auto scroll = event->getScrollY() * getScrollMagnification();
			scroll *= (scroll < 0) ? -1 : 1;
			if (scroll < 1.0)
				return;

			if (getTouchListener()->pinch)
				getTouchListener()->pinch(cocos2d::Vec2(event->getCursorX(), event->getCursorY()), event->getScrollY() > 0 ? scroll : (1.0f / scroll));
		};
		return true;
	};
};

#endif // __MULTI_LISTENER_H__