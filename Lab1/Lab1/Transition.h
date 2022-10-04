#pragma once
#include <tuple>

struct Transition
{
	int state;
	int output;
	int mooreState{ -1 };

	friend bool operator==(const Transition& left, const Transition& right)
	{
		return std::tie(left.state, left.output) == std::tie(right.state, right.output);
	}
};

struct TransitionLessComparator
{
public:
	bool operator()(const Transition& left, const Transition& right) const
	{
		if (left.state == right.state)
		{
			return left.output < right.output;
		}

		return left.state < right.state;
	}
};