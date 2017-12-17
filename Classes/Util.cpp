#include "Util.h"

#include <random>

static std::mt19937 _mt;

std::string util::loadText(const std::string &file)
{
	return FileUtils::getInstance()->getStringFromFile(file);
}


std::vector<std::string> util::splitString(const std::string& input, char delim)
{
	std::string item;
	std::vector<std::string> result;
	for (char ch : input)
	{
		if (ch == delim)
		{
			result.push_back(item);
			item.clear();
		}
		else
		{
			item += ch;
		}
	}
	result.push_back(item);
	return result;
}

std::vector<std::string> util::splitFile(const std::string &file)
{
	// Result
	std::string item;
	std::vector<std::string> result;

	// for BOM
	bool first = true;
	bool bom = false;
	int bom_count = 0;

	for (char ch : loadText(file))
	{
		// BOM Check
		if (first)
		{
			if ((unsigned char)ch == 0xEF)
				bom = true;
			first = false;
		}
		if (bom && bom_count++ < 3)
			continue;

		// '\r' is skip
		if (ch == '\r')
			continue;

		if (ch == '\n')
		{
			result.push_back(item);
			item.clear();
		}
		else
		{
			item += ch;
		}
	}

	result.push_back(item);
	return result;
}

bool util::isTouchInEvent(Vec2 point, Node* t, int down_x, int down_y, int append_x, int append_y)
{
	//NULL check
	CCASSERT(t, "[ERROR] isInTocuh node is NULL");

	Rect targetBox;
	//if ignore anchorpoint, not multiple anchorpoint.
	if (t->isIgnoreAnchorPointForPosition())
		targetBox = Rect(t->getParent()->convertToWorldSpace(t->getPosition()).x - t->getContentSize().width - down_x,
			t->getParent()->convertToWorldSpace(t->getPosition()).y - t->getContentSize().height - down_y,
			t->getContentSize().width + append_x, t->getContentSize().height + append_y);
	else
		targetBox = Rect(t->getParent()->convertToWorldSpace(t->getPosition()).x - t->getContentSize().width * t->getAnchorPoint().x - down_x,
			t->getParent()->convertToWorldSpace(t->getPosition()).y - t->getContentSize().height * t->getAnchorPoint().y - down_y,
			t->getContentSize().width + append_x, t->getContentSize().height + append_y);
	return targetBox.containsPoint(point);
}


Node* util::createCutSkin(const std::string &file, int w, int h, int cut_mask, int opacity)
{
	auto origine = Sprite::create(file);

	auto ori_w = origine->getContentSize().width;
	auto ori_h = origine->getContentSize().height;

	int top = ori_h / 3;
	int under = ori_h / 3;
	int right = ori_w / 3;
	int left = ori_w / 3;

	//create base node.
	auto node = Node::create();
	node->setContentSize(Size(w, h));

	//convert skin type to integer.
	if (!(cut_mask & CUT_MASK_UP))
	{
		//Upper left (not cut up and left)
		if (!(cut_mask & CUT_MASK_LEFT))
		{
			auto addSkin = Sprite::create(file, Rect(0, 0, ori_w / 3, ori_h / 3));
			addSkin->setPosition(Vec2(ori_w / 6, h - ori_h / 6));
			addSkin->setOpacity(opacity);
			node->addChild(addSkin);
		}
		//Upper right (not cut up and right)
		if (!(cut_mask & CUT_MASK_RIGHT))
		{
			auto addSkin = Sprite::create(file, Rect(ori_w / 3 * 2, 0, ori_w / 3, ori_h / 3));
			addSkin->setPosition(Vec2(w - ori_w / 6, h - ori_h / 6));
			addSkin->setOpacity(opacity);
			node->addChild(addSkin);
		}
	}
	else
	{
		//top is zero for cut up.
		top = 0;
	}

	if (!(cut_mask & CUT_MASK_DOWN))
	{
		//Lower left (not cut down and left)
		if (!(cut_mask & CUT_MASK_LEFT))
		{
			auto addSkin = Sprite::create(file, Rect(0, ori_h / 3 * 2, ori_w / 3, ori_h / 3));
			addSkin->setPosition(Vec2(ori_w / 6, ori_h / 6));
			addSkin->setOpacity(opacity);
			node->addChild(addSkin);
		}
		//Lower right (not cut down and right)
		if (!(cut_mask & CUT_MASK_RIGHT))
		{
			auto addSkin = Sprite::create(file, Rect(ori_w / 3 * 2, ori_h / 3 * 2, ori_w / 3, ori_h / 3));
			addSkin->setPosition(Vec2(w - ori_w / 6, ori_h / 6));
			addSkin->setOpacity(opacity);
			node->addChild(addSkin);
		}
	}
	else
	{
		//under is zero for cut down.
		under = 0;
	}

	//In the following, scaling in some cases.

	//Left (not cut left)
	if (!(cut_mask & CUT_MASK_LEFT))
	{
		auto addSkin = Sprite::create(file, Rect(0, ori_h / 3, ori_w / 3, ori_h / 3));
		addSkin->setScaleY((h - top - under) / (ori_h / 3));
		addSkin->setPosition(Vec2(ori_w / 6, (h - top + under) / 2));
		addSkin->setOpacity(opacity);
		node->addChild(addSkin);
	}
	else
	{
		//left is zero for cut left.
		left = 0;
	}
	//Right (not cut right)
	if (!(cut_mask & CUT_MASK_RIGHT))
	{
		auto addSkin = Sprite::create(file, Rect(ori_w / 3 * 2, 0 + ori_h / 3, ori_w / 3, ori_h / 3));
		addSkin->setScaleY((h - top - under) / (ori_h / 3));
		addSkin->setPosition(Vec2(w - ori_w / 6, (h - top + under) / 2));
		addSkin->setOpacity(opacity);
		node->addChild(addSkin);
	}
	else
	{
		//right is zero for cut right.
		right = 0;
	}

	//Upper (not cut up)
	if (!(cut_mask & CUT_MASK_UP))
	{
		auto addSkin = Sprite::create(file, Rect(ori_w / 3, 0, ori_w / 3, ori_h / 3));
		addSkin->setScaleX((w - left - right) / (ori_w / 3));
		addSkin->setPosition(Vec2((w + left - right) / 2, h - ori_h / 6));
		addSkin->setOpacity(opacity);
		node->addChild(addSkin);
	}
	//Lower (not cut down)
	if (!(cut_mask & CUT_MASK_DOWN))
	{
		auto addSkin = Sprite::create(file, Rect(ori_w / 3, ori_h / 3 * 2, ori_w / 3, ori_h / 3));
		addSkin->setScaleX((w - left - right) / (ori_w / 3));
		addSkin->setPosition(Vec2((w + left - right) / 2, ori_h / 6));
		addSkin->setOpacity(opacity);
		node->addChild(addSkin);
	}

	//main skin(center)
	auto skin = Sprite::create(file, Rect(ori_w / 3, ori_h / 3, ori_w / 3, ori_h / 3));
	skin->setScaleX((w - left - right) / (ori_w / 3));
	skin->setScaleY((h - top - under) / (ori_h / 3));
	skin->setPosition(Vec2((w + left - right) / 2, (h - top + under) / 2));
	skin->setOpacity(opacity);
	node->addChild(skin);

	//return node
	return node;
}

Node* util::createCutSkinAndAnimation(const std::string &file, int w, int h, int wnum, int hnum, int cut_mask, float duration, int opacity)
{
	auto origine = Sprite::create(file);

	auto ori_w = origine->getContentSize().width / wnum;
	auto ori_h = origine->getContentSize().height / hnum;

	int top = ori_h / 3;
	int under = ori_h / 3;
	int right = ori_w / 3;
	int left = ori_w / 3;

	//create base node.
	auto node = Node::create();
	node->setContentSize(Size(w, h));

	//convert skin type to integer.
	if (!(cut_mask & CUT_MASK_UP))
	{
		//Upper left (not cut up and left)
		if (!(cut_mask & CUT_MASK_LEFT))
		{
			auto addSkin = Sprite::create(file, Rect(0, 0, ori_w / 3, ori_h / 3));
			addSkin->setPosition(Vec2(ori_w / 6, h - ori_h / 6));
			addSkin->setOpacity(opacity);
			node->addChild(addSkin);
			auto animation = Animation::create();
			animation->setDelayPerUnit(duration);
			animation->setRestoreOriginalFrame(true);
			for (int x = 0; x < wnum; x++)
				for (int y = 0; y < hnum; y++)
					animation->addSpriteFrame(SpriteFrame::create(file, Rect(0 + x * ori_w, 0 + y * ori_h, ori_w / 3, ori_h / 3)));
			addSkin->runAction(RepeatForever::create(Animate::create(animation)));
		}
		//Upper right (not cut up and right)
		if (!(cut_mask & CUT_MASK_RIGHT))
		{
			auto addSkin = Sprite::create(file, Rect(ori_w / 3 * 2, 0, ori_w / 3, ori_h / 3));
			addSkin->setPosition(Vec2(w - ori_w / 6, h - ori_h / 6));
			addSkin->setOpacity(opacity);
			node->addChild(addSkin);
			auto animation = Animation::create();
			animation->setDelayPerUnit(duration);
			animation->setRestoreOriginalFrame(true);
			for (int x = 0; x < wnum; x++)
				for (int y = 0; y < hnum; y++)
					animation->addSpriteFrame(SpriteFrame::create(file, Rect(ori_w / 3 * 2 + x * ori_w, 0 + y * ori_h, ori_w / 3, ori_h / 3)));
			addSkin->runAction(RepeatForever::create(Animate::create(animation)));
		}
	}
	else
	{
		//top is zero for cut up.
		top = 0;
	}

	if (!(cut_mask & CUT_MASK_DOWN))
	{
		//Lower left (not cut down and left)
		if (!(cut_mask & CUT_MASK_LEFT))
		{
			auto addSkin = Sprite::create(file, Rect(0, ori_h / 3 * 2, ori_w / 3, ori_h / 3));
			addSkin->setPosition(Vec2(ori_w / 6, ori_h / 6));
			addSkin->setOpacity(opacity);
			node->addChild(addSkin);
			auto animation = Animation::create();
			animation->setDelayPerUnit(duration);
			animation->setRestoreOriginalFrame(true);
			for (int x = 0; x < wnum; x++)
				for (int y = 0; y < hnum; y++)
					animation->addSpriteFrame(SpriteFrame::create(file, Rect(0 + x * ori_w, ori_h / 3 * 2 + y * ori_h, ori_w / 3, ori_h / 3)));
			addSkin->runAction(RepeatForever::create(Animate::create(animation)));
		}
		//Lower right (not cut down and right)
		if (!(cut_mask & CUT_MASK_RIGHT))
		{
			auto addSkin = Sprite::create(file, Rect(ori_w / 3 * 2, ori_h / 3 * 2, ori_w / 3, ori_h / 3));
			addSkin->setPosition(Vec2(w - ori_w / 6, ori_h / 6));
			addSkin->setOpacity(opacity);
			node->addChild(addSkin);
			auto animation = Animation::create();
			animation->setDelayPerUnit(duration);
			animation->setRestoreOriginalFrame(true);
			for (int x = 0; x < wnum; x++)
				for (int y = 0; y < hnum; y++)
					animation->addSpriteFrame(SpriteFrame::create(file, Rect(ori_w / 3 * 2 + x * ori_w, ori_h / 3 * 2 + y * ori_h, ori_w / 3, ori_h / 3)));
			addSkin->runAction(RepeatForever::create(Animate::create(animation)));
		}
	}
	else
	{
		//under is zero for cut down.
		under = 0;
	}

	//In the following, scaling in some cases.

	//Left (not cut left)
	if (!(cut_mask & CUT_MASK_LEFT))
	{
		auto addSkin = Sprite::create(file, Rect(0, ori_h / 3, ori_w / 3, ori_h / 3));
		addSkin->setScaleY((h - top - under) / (ori_h / 3));
		addSkin->setPosition(Vec2(ori_w / 6, (h - top + under) / 2));
		addSkin->setOpacity(opacity);
		node->addChild(addSkin);
		auto animation = Animation::create();
		animation->setDelayPerUnit(duration);
		animation->setRestoreOriginalFrame(true);
		for (int x = 0; x < wnum; x++)
			for (int y = 0; y < hnum; y++)
				animation->addSpriteFrame(SpriteFrame::create(file, Rect(0 + x * ori_w, ori_h / 3 + y * ori_h, ori_w / 3, ori_h / 3)));
		addSkin->runAction(RepeatForever::create(Animate::create(animation)));
	}
	else
	{
		//left is zero for cut left.
		left = 0;
	}
	//Right (not cut right)
	if (!(cut_mask & CUT_MASK_RIGHT))
	{
		auto addSkin = Sprite::create(file, Rect(ori_w / 3 * 2, 0 + ori_h / 3, ori_w / 3, ori_h / 3));
		addSkin->setScaleY((h - top - under) / (ori_h / 3));
		addSkin->setPosition(Vec2(w - ori_w / 6, (h - top + under) / 2));
		addSkin->setOpacity(opacity);
		node->addChild(addSkin);
		auto animation = Animation::create();
		animation->setDelayPerUnit(duration);
		animation->setRestoreOriginalFrame(true);
		for (int x = 0; x < wnum; x++)
			for (int y = 0; y < hnum; y++)
				animation->addSpriteFrame(SpriteFrame::create(file, Rect(ori_w / 3 * 2 + x * ori_w, ori_h / 3 + y * ori_h, ori_w / 3, ori_h / 3)));
		addSkin->runAction(RepeatForever::create(Animate::create(animation)));
	}
	else
	{
		//right is zero for cut right.
		right = 0;
	}

	//Upper (not cut up)
	if (!(cut_mask & CUT_MASK_UP))
	{
		auto addSkin = Sprite::create(file, Rect(ori_w / 3, 0, ori_w / 3, ori_h / 3));
		addSkin->setScaleX((w - left - right) / (ori_w / 3));
		addSkin->setPosition(Vec2((w + left - right) / 2, h - ori_h / 6));
		addSkin->setOpacity(opacity);
		node->addChild(addSkin);
		auto animation = Animation::create();
		animation->setDelayPerUnit(duration);
		animation->setRestoreOriginalFrame(true);
		for (int x = 0; x < wnum; x++)
			for (int y = 0; y < hnum; y++)
				animation->addSpriteFrame(SpriteFrame::create(file, Rect(ori_w / 3 + x * ori_w, 0 + y * ori_h, ori_w / 3, ori_h / 3)));
		addSkin->runAction(RepeatForever::create(Animate::create(animation)));
	}
	//Lower (not cut down)
	if (!(cut_mask & CUT_MASK_DOWN))
	{
		auto addSkin = Sprite::create(file, Rect(ori_w / 3, ori_h / 3 * 2, ori_w / 3, ori_h / 3));
		addSkin->setScaleX((w - left - right) / (ori_w / 3));
		addSkin->setPosition(Vec2((w + left - right) / 2, ori_h / 6));
		addSkin->setOpacity(opacity);
		node->addChild(addSkin);
		auto animation = Animation::create();
		animation->setDelayPerUnit(duration);
		animation->setRestoreOriginalFrame(true);
		for (int x = 0; x < wnum; x++)
			for (int y = 0; y < hnum; y++)
				animation->addSpriteFrame(SpriteFrame::create(file, Rect(ori_w / 3 + x * ori_w, ori_h / 3 * 2 + y * ori_h, ori_w / 3, ori_h / 3)));
		addSkin->runAction(RepeatForever::create(Animate::create(animation)));
	}

	//main skin(center)
	auto skin = Sprite::create(file, Rect(ori_w / 3, ori_h / 3, ori_w / 3, ori_h / 3));
	skin->setScaleX((w - left - right) / (ori_w / 3));
	skin->setScaleY((h - top - under) / (ori_h / 3));
	skin->setPosition(Vec2((w + left - right) / 2, (h - top + under) / 2));
	skin->setOpacity(opacity);
	node->addChild(skin);
	auto animation = Animation::create();
	animation->setDelayPerUnit(duration);
	animation->setRestoreOriginalFrame(true);
	for (int x = 0; x < wnum; x++)
		for (int y = 0; y < hnum; y++)
			animation->addSpriteFrame(SpriteFrame::create(file, Rect(ori_w / 3 + x * ori_w, ori_h / 3 + y * ori_h, ori_w / 3, ori_h / 3)));
	skin->runAction(RepeatForever::create(Animate::create(animation)));

	//return node
	return node;
}

void util::initRand()
{
	std::random_device rndDev;
	_mt = std::mt19937(rndDev());
}

int util::getRand(int min, int max)
{
	std::uniform_int_distribution<int> dist(min, max);
	return dist(_mt);
}

double util::getRand(double min, double max)
{
	std::uniform_real_distribution<double> dist(min, max);
	return dist(_mt);
}
