#ifndef _SORTBYRANKGETBYIDWITHTOP_HPP_
#define _SORTBYRANKGETBYIDWITHTOP_HPP_

#include "sortbyrankgetbyid.hpp"

template <typename ID, typename Rank>
class SortByRankGetByIdWithTop
{
public:

	SortByRankGetByIdWithTop(Rank top_const, Rank bottom_const) : top_const(top_const), bottom_const(bottom_const) { }

	void increment(ID key, Rank value) 
	{
		all.increment(key, value);
		top.increment(key, value);
		cutOff();
	}

	auto upper_bound(Rank rank) const
	{
		return all.upper_bound(rank);
	}

	auto allBegin() const
	{
		return all.begin();		
	}

	auto allEnd() const
	{
		return all.end();
	}

	void cutOff()
	{
		all.cutOff(bottom_const);
		top.cutOff(top_const);
	}

	void setTopConst(Rank top_)
	{
		top_const = top_;
	}	

	void setBottomConst(Rank bottom_)
	{
		bottom_const = bottom_;
	}	

	auto begin() const
	{
		return top.begin();
	}

	auto end() const
	{
		return top.end();
	}

	auto topSize() const
	{
		return top.size();
	}

	auto size() const
	{
		return all.size();
	}

private:
	SortByRankGetById<ID, Rank> top;
	SortByRankGetById<ID, Rank> all;

	Rank top_const;
	Rank bottom_const;
};

#endif
