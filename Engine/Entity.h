#pragma once

#include <vector>
#include "Vec2.h"

class Entity
{
public: 
	Entity(std::vector<Vec2> model)
		:
		model(std::move(model))
	{}
	const Vec2& GetPos() const
	{
		return pos; 
	}
	void SetPos(const Vec2& newPos)
	{
		pos = newPos;
	}
	void TranslateBy(const Vec2& offset)
	{
		pos += offset;
	}
	void SetScale(const float& scalar)
	{
		scale = scalar;
	}
	float GetScale() const
	{
		return scale; 
	}
	std::vector<Vec2> GetPolyLine() const
	{
		auto poly = model;
		for (auto& v : poly)
		{
			v *= scale;
			v += pos; 
		}
		return poly;
	}
private:
	float scale = 1.0f;
	Vec2 pos = { 0.0f, 0.0f };
	std::vector<Vec2> model;
};