#pragma once
#include <vector>
#include <forward_list>
#include "StellarChronicles/stella.h"

#ifndef Entity
#define Entity galaxy

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

		if (this->isLeaf() && this->_contain_size >= this->_MaxCapacity)
		{
			this->subDivide();
		}

		if (this->isLeaf())
		{
			this->_contain.emplace_front(&entity);
			_contain_size++;
		}
		else
		{
			for (auto &child : this->_children)
				child.insert(entity);
		}
	}

	std::vector<Entity *> query(const Rect &range)
	{
		std::forward_list<Entity *> entitiesInRange;
		this->search(range, entitiesInRange);
		size_t size = std::distance(entitiesInRange.cbegin(), entitiesInRange.cend());
		std::vector<Entity*> results(size);
		std::copy(entitiesInRange.begin(), entitiesInRange.end(), results.begin());
		return results;
	}

	bool isEntityInRange(const Entity & entity, const Rect &rect)
	{
		auto& x = entity.Position.x;
		auto& y = entity.Position.y;
		auto& r = entity.mainStar.radius;
		float minX = x - r;
		float minY = y - r;
		float maxX = x + r;
		float maxY = y + r;

		return !(minX > rect.centerX + rect.halfW || maxX < rect.centerX - rect.halfW || minY > rect.centerY + rect.halfH || maxY < rect.centerY - rect.halfH);
	}

private:
	int _depth = 0;
	std::forward_list<QuadTree> _children;
	std::forward_list<Entity *> _contain;
	Rect _range;
	int _contain_size = 0;

	int _MaxDepth = 7;
	int _MaxCapacity = 4;

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

		return !((ArightX < BleftX || AleftX > BrightX) || (AupY < BlowY || AlowY > BupY));
	}




	inline bool isLeaf()
	{
		return this->_children.empty();
	}

	void subDivide()
	{
		if (this->_depth >= this->_MaxDepth)
			return;

		Rect subDivideRange[4] = {
			Rect{_range.centerX + _range.halfW * 0.5f, _range.centerY + _range.halfH * 0.5f, _range.halfW * 0.5f, _range.halfH * 0.5f},
			Rect{_range.centerX - _range.halfW * 0.5f, _range.centerY + _range.halfH * 0.5f, _range.halfW * 0.5f, _range.halfH * 0.5f},
			Rect{_range.centerX - _range.halfW * 0.5f, _range.centerY - _range.halfH * 0.5f, _range.halfW * 0.5f, _range.halfH * 0.5f},
			Rect{_range.centerX + _range.halfW * 0.5f, _range.centerY - _range.halfH * 0.5f, _range.halfW * 0.5f, _range.halfH * 0.5f}};

		for (int i = 0; i < 4; i++)
		{
			this->_children.emplace_front(subDivideRange[i]);
			this->_children.front()._depth = this->_depth + 1;
		}

		for (const auto &entity : this->_contain)
		{
			for (auto &child : this->_children)
				child.insert(*entity);
		}

		this->_contain.clear();
		_contain_size = 0;
	}

	void search(const Rect &range, std::forward_list<Entity *> &entitiesInRange)
	{
		if (!isRangeInRange(range, _range))
			return;

		if (this->isLeaf())
		{
			for (const auto &entity : this->_contain)
				if (isEntityInRange(*entity, range))
					entitiesInRange.push_front(entity);
		}
		else
			for (auto &child : this->_children)
				child.search(range, entitiesInRange);
	}


};

#undef Entity

#endif // !Entity
