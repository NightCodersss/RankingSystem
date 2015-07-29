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

	std::pair<ID, Rank> upper_bound(Rank rank)
	{
		return all.upper_bound(rank);
	}

	void cutOff()
	{
		all.cutOff(top_const);
		top.cutOff(bottom_const);
	}

	void setTopConst(Rank top_)
	{
		top_const = top_;
	}	

	void setBottomConst(Rank bottom_)
	{
		bottom_const = bottom_;
	}	

private:
	SortByRankGetById<ID, Rank> top;
	SortByRankGetById<ID, Rank> all;

	Rank top_const;
	Rank bottom_const;
};

#endif
