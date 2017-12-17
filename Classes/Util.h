#ifndef __UTIL_H__
#define __UTIL_H__

#include "cocos2d.h"
#include "MultiListener.h"

USING_NS_CC;

/*********************************************************/
/*
 * Utility method
 */
namespace util
{
	// very small number
	constexpr float elim = 0.0000001f;

	// for creating skin cut right
	constexpr int CUT_MASK_RIGHT = 1;
	// for creating skin cut left
	constexpr int CUT_MASK_LEFT = 2;
	// for creating skin cut up
	constexpr int CUT_MASK_UP = 4;
	// for creating skin cut down
	constexpr int CUT_MASK_DOWN = 8;

	/*
	 * Load text file
	 * @param file Loded file name 
	 */
	std::string loadText(const std::string &file);
	/*
	 * Input is splited by delimiter
	 * @param input Input string splited by delimiter
	 * @param delim Delimiter for spliting
	 */
	std::vector<std::string> splitString(const std::string& input, char delim);
	/*
	 * File is splited by \n and \r
	 * It is compatible for BOM
	 * @param file Loaded and splited file name
	 */
	std::vector<std::string> splitFile(const std::string &file);

	/*
	 * Checking node range contains point
	 * @param point Judged coodinate
	 * @param t Node for range
	 * @param down_x Extension of left
	 * @param down_y Extension of lower
	 * @param append_x Extension of right
	 * @param append_y Extension of upper
	 */
	bool isTouchInEvent(Vec2 point, Node* t, int down_x, int down_y, int append_x, int append_y);
	/*
	 * Checking event range contains point
	 * @param point Judged coodinate
	 * @param event Event which contains node for range
	 * @param down_x Extension of left
	 * @param down_y Extension of lower
	 * @param append_x Extension of right
	 * @param append_y Extension of upper
	 */
	inline bool isTouchInEvent(Touch* touch, Event* event, int down_x, int down_y, int append_x, int append_y) { return isTouchInEvent(touch->getLocation(), event->getCurrentTarget(), down_x, down_y, append_x, append_y); };
	/*
	 * Checking node range contains point
	 * @param point Judged coodinate
	 * @param t Node for range
	 */
	inline bool isTouchInEvent(Vec2 point, Node* t) { return isTouchInEvent(point, t, 0, 0, 0, 0); };
	/*
	 * Checking event range contains point
	 * @param point Judged coodinate
	 * @param event Event which contains node for range
	 */
	inline bool isTouchInEvent(Vec2 point, Event* event) { return isTouchInEvent(point, event->getCurrentTarget()); };
	/*
	 * Checking event range contains touch point
	 * @param touch Touch which contains judged coodinate
	 * @param event Event which contains node for range
	 */
	inline bool isTouchInEvent(Touch* touch, Event* event) { return isTouchInEvent(touch->getLocation(), event->getCurrentTarget()); };

	/*
	 * Get coodinate in screen by target
	 * @param pos Coodinate for converting
	 * @param target Node for converting
	 */
	inline Vec2 getCoordinateInScreen(Vec2 pos, Node* target) { return target->convertToWorldSpace(pos); };

	/*
	 * Instanceof
	 * @param w Judge
	 */
	template <typename Of, typename What>
	inline bool instanceof(const What w) { return dynamic_cast<const Of*>(w) != NULL; };

	/*
	 * Get instance 
	 * @param w For getting instance
	 */
	template <typename Of, typename What>
	inline Of* instance(const What w) { return dynamic_cast<Of*>(w); };

	/*
	 * Create frame by skin data
	 * @param file Frame file name
	 * @param w Frame width
	 * @param h Frame height
	 * @param cut_mask Masking value for cut skin edge
	 * @param opacity Frame opacity
	 */
	Node* createCutSkin(const std::string &file, int w, int h, int cut_mask, int opacity = 255);
	/*
	 * Create frame by skin data with animation
	 * @param file Frame file name
	 * @param w Frame width
	 * @param h Frame height
	 * @param wnum Frame animation width number
	 * @param hnum Frame animation height number
	 * @param cut_mask Masking value for cut skin edge
	 * @param duration Frame animation duration
	 * @param opacity Frame opacity
	 */
	Node* createCutSkinAndAnimation(const std::string &file, int w, int h, int wnum, int hnum, int cut_mask, float duration, int opacity = 255);

	/*
	 * Find element in vector
	 * If vector has element, return true
	 * @param vec Vector to be searched
	 * @param element Searching element 
	 */
	template<typename V>
	bool find(const std::vector<V>& vec, V element)
	{
		for (V comp : vec)
			if (comp == element)
				return true;
		return false;
	};

	/*
 	 * Find element satisfying condition in vec
	 * If vec has element satisfying condition, return true
	 * @param vec Vector to be searched
	 * @param element Searching element
	 * @param cond Judgement condition. if cond(x, element)(x in vec) is true, return true
	 */
	template<typename V, typename T>
	bool find(const std::vector<V>& vec, T element, std::function<bool(V, T)> cond)
	{
		for (V comp : vec)
			if (cond(comp, element))
				return true;
		return false;
	};

	/*
 	 * Find element satisfying condition in vec and get its index number
	 * If vec has element satisfying condition, return its index number else, return -1
	 * @param vec Vector to be searched
	 * @param element Searching element
	 * @param cond Judgement condition. if cond(x, element)(x in vec) is true, return true
	 */
	template<typename V, typename T>
	int findIndex(const std::vector<V>& vec, T element, std::function<bool(V, T)> cond = [] (V a, T b) { return a == b; })
	{
		for (unsigned int i = 0; i < vec.size(); ++i) 
		{
			if (cond(vec[i], element))
				return i;
		}
		return -1;
	};

	/*
 	 * Find element satisfying condition in vec
	 * If vec has element satisfying condition, return it
	 * If not, return nullptr
	 * @param vec Vector to be searched
	 * @param cond Judgement condition. if cond(x)(x in vec) is true, return x
	 */
	template<typename V>
	V findElement(const std::vector<V>& vec, std::function<bool(V)> cond)
	{
		for (V comp : vec)
			if (cond(comp))
				return comp;
		return nullptr;
	};

	/*
	 * Collect element and return vector
	 * @param vec Vector to be searched
	 * @param cond Judgement condition. if cond(x)(x in vec) is true, vector to be returned contains x
	 */
	template<typename V>
	std::vector<V> collect(const std::vector<V>& vec, std::function<bool(V)> cond)
	{
		auto result = std::vector<V>();
		for (V comp : vec)
			if (cond(comp))
				result.push_back(comp);
		return result;
	};

	/*
	 * Sort map by value
	 * @param map Map to be sorted
	 * @param isGreater True, sort method is greater
	 */
	template<typename V, typename T>
	std::vector<std::pair<V, T>> sortMap(const std::map<V, T>& map, bool isGreater = true)
	{
		std::vector<std::pair<V, T>> arr;
		
		for (std::pair<V, T> pair : map)
			arr.push_back(pair);

		if (isGreater)
			std::sort(arr.begin(), arr.end(), [](const std::pair<V, T>& left, const std::pair<V, T> right) {return left.second > right.second; });
		else
			std::sort(arr.begin(), arr.end(), [](const std::pair<V, T>& left, const std::pair<V, T> right) {return left.second < right.second; });

		return arr;
	};

	template<typename V, typename S>
	S reduceLeft(const std::vector<V> & vec, S init, std::function<S(S, V)> func)
	{
		S res = init;
		for (auto item : vec)
			res = func(res, item);
		return res;
	}


	/*
	 * Initialize random seed
	 */
	void initRand();

	/*
	 * Get random integer n
	 * min < n < max
	 * @param min Minimum number of random
	 * @param max Max number of random
	 */
	int getRand(int min, int max);

	/*
	 * Get random double n
	 * min < n < max
	 * @param min Minimum number of random
	 * @param max Max number of random
	 */
	double getRand(double min, double max);

	/*
	 * Determine whether the specified string starts with a certain string
	 * @param text The specified string
	 * @param start A certain string
	 */
	inline bool startWith(const std::string &text, const std::string &start) { return text.substr(0, start.size()) == start; };

}

namespace simple {
	/*
	 * Get event listener with swallow and set node
	 * @param target Node where event listener set
	 */
	inline EventListenerTouchOneByOne* setEventListener(Node* target, bool swallow = true, bool isAlways = false)
	{
		auto lis = EventListenerTouchOneByOne::create();
		lis->setSwallowTouches(swallow);
		lis->onTouchBegan = [isAlways](Touch* touch, Event* event) { return isAlways || util::isTouchInEvent(touch, event); };
		target->getEventDispatcher()->addEventListenerWithSceneGraphPriority(lis, target);
		return lis;
	};

	/*
	 * Get single listener with swallow and set node
	 * @param target Node where single listener set
	 */
	inline SingleTouchListener* setSingleListener(Node* target, bool swallow = true, bool isAlways = false)
	{
		auto lis = SingleTouchListener::create();
		lis->setSwallowTouches(swallow);
		lis->onTouchBeganChecking = [isAlways](Vec2 pos, Node* t) { return isAlways || util::isTouchInEvent(pos, t); };
		target->getEventDispatcher()->addEventListenerWithSceneGraphPriority(lis, target);
		return lis;
	};

	/*
	 * Get mult listener with swallow and set node
	 * @param target Node where multi listener set
	 */
	inline MultiTouchListener* setMultiListener(Node* target)
	{
		auto lis = MultiTouchListener::create();
		target->getEventDispatcher()->addEventListenerWithSceneGraphPriority(lis, target);
		return lis;
	};
	
	/*
	 * Get sprite and set position
	 * @param fileName Sprite file name
	 * @param x Sprite x position
	 * @param y Sprite y position
	 * @param scale Sprite scale
	 * @param anchor Sprite anchor point
	 */
	inline Sprite* getSprite(std::string fileName, float x, float y, float scale = 1.0f, int opacity = 255, Vec2 anchor = Vec2::ANCHOR_MIDDLE) 
	{
		auto sp = Sprite::create(fileName);
		sp->setPosition(x, y);
		sp->setOpacity(opacity);
		if(scale != 1.0f)
			sp->setScale(scale);
		sp->setAnchorPoint(anchor);
		return sp;
	}

	/*
	 * Create frame by skin data simply
	 * @param file Frame file name
	 * @param x Frame x position
	 * @param y Frame y position
	 * @param w Frame width
	 * @param h Frame height
	 * @param ahchor Frame anchor point
	 * @param cut_mask Masking value for cut skin edge
	 * @param opacity Frame opacity
	 */
	inline Node* createCutSkinSimply(const std::string &file, int x, int y, int w, int h, int cut_mask, Vec2 anchor = Vec2::ANCHOR_MIDDLE, int opacity = 255) {
		auto skin = util::createCutSkin(file, w, h, cut_mask, opacity);
		skin->setAnchorPoint(anchor);
		skin->setPosition(x, y);
		return skin;
	}

	/*
	 * Create label simply
	 * @param text Label text
	 * @param x Label x position
	 * @param y Label y position
	 * @param size Label font size
	 * @param color Label font color
	 * @param anchor Label anchor position
	 * @param font Label font name
	 */
	inline Label* getLabel(const std::string text, int x, int y, float size, Color3B color = Color3B::WHITE, Vec2 anchor = Vec2::ANCHOR_BOTTOM_LEFT, std::string font = "MS ゴシック") {
		auto label = Label::createWithSystemFont(text, font, size);
		label->setPosition(x, y);
		label->setAnchorPoint(anchor);
		label->setColor(color);
		return label;
	}

}

#endif // __UTIL_H__