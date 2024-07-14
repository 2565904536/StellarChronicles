#pragma once
#include <vector>

template <class Entity>
class QuadTree
{
public:
	struct Rect
	{
		float centerX;
		float centerY;
		float halfW;
		float halfH;
	};

	explicit QuadTree(Rect range) : _range(range) {}
	~QuadTree() = default;
	void insert(Entity &entity)
	{
		if (!isEntityInRange(entity, _range))
		{
			return;
		}

		if (this->isLeaf() && this->_contain.size() >= this->_MaxCpacity)
		{
			this->subDivide();
		}

		if (this->isLeaf())
		{
			this->_contain.push_back(&entity);
		}
		else
		{
			for (QuadTree<Entity> &child : this->_children)
			{
				child.insert(entity);
			}
		}
	}

	void remove(Entity& entity)
	{
		if (this->isLeaf())
		{
			std::erase(this->_contain, &entity);
		}
		else
		{
			for (QuadTree<Entity>& child : this->_children)
			{
				child.remove(entity);
			}
		}
	}

	std::vector<Entity *> query(const Rect &range)
	{
		std::vector<Entity *> entitiesInRange;
		this->search(range, entitiesInRange);
		return entitiesInRange;
	}

private:
	int _depth;
	int _MaxDepth;
	int _MaxCapacity;
	virtual bool isEntityInRange(const Entity &, const Rect &) = 0;
	bool isRangeInRange(const Rect &A, const Rect &B)
	{
		float AleftX = A.centerX - A.halfW;
		float ArightX = A.centerX + A.halfW;
		float AlowY = A.centerY - A.halfH;
		float AupY = A.centerY + A.halfH;

		float BleftX = B.centerX - B.halfW;
		float BrightX = B.centerX + B.halfW;
		float BlowY = B.centerY - B.halfH;
		float BupY = B.centerY + B.halfH;

		return !(ArightX < BleftX || AleftX > BrightX || AupY < BlowY || AlowY > BupY);
	}
	Rect _range;
	std::vector<Entity *> _contain;
	std::vector<QuadTree<Entity>> _children;

	inline bool isLeaf()
	{
		return this->_children.empty();
	}

	void subDivide()
	{
		if (this->_depth >= this->_MaxDepth)
		{
			return;
		}

		Rect subDivideRange[4] =
			{
				Rect{_range.centerX + _range.halfW * 0.5f, _range.centerY + _range.halfH * 0.5f, _range.halfW * 0.5f, _range.halfH * 0.5f},
				Rect{_range.centerX - _range.halfW * 0.5f, _range.centerY + _range.halfH * 0.5f, _range.halfW * 0.5f, _range.halfH * 0.5f},
				Rect{_range.centerX - _range.halfW * 0.5f, _range.centerY - _range.halfH * 0.5f, _range.halfW * 0.5f, _range.halfH * 0.5f},
				Rect{_range.centerX + _range.halfW * 0.5f, _range.centerY - _range.halfH * 0.5f, _range.halfW * 0.5f, _range.halfH * 0.5f}};

		for (int i = 0; i < 4; i++)
		{
			this->_children.emplace_back(subDivideRange[i]);
			this->_children[i]._depth++;
		}

		for (const Entity *&entity : this->_contain)
		{
			this->_children[0].insert(*entity);
			this->_children[1].insert(*entity);
			this->_children[2].insert(*entity);
			this->_children[3].insert(*entity);
		}

		this->_contain.clear();
	}

	void search(const Rect &range, std::vector<Entity *> &entitiesInRange)
	{
		if (!isRangeInRange(range, _range))
		{
			return;
		}

		if (this->isLeaf())
		{
			for (const Entity &entity : this->_contain)
			{
				if (isEntityInRange(entity, range))
				{
					entitiesInRange.push_back(entity);
				}
			}
		}
		else
		{
			for (QuadTree<Entity> &child : this->_children)
			{
				child.search(range, entitiesInRange);
			}
		}
	}
};