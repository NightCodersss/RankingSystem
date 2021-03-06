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

	void set(ID key, Rank value) 
	{
		all.set(key, value);
		top.set(key, value);
		cutOff(); // WARN cutoff may work wrong, if rank may decrease, but really do not know
	}

	auto all_upper_bound(Rank rank) const
	{
		return all.upper_bound(rank);
	}

	auto all_begin() const
	{
		return all.begin();		
	}

	auto all_end() const
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

	auto top_begin() const
	{
		return top.begin();
	}

	auto top_end() const
	{
		return top.end();
	}

	auto top_size() const
	{
		return top.size();
	}

	auto all_size() const
	{
		return all.size();
	}

	void deleteTheTopDocument()
	{
		top.deleteTheTopDocument();
		all.deleteTheTopDocument();
	}

private:
	SortByRankGetById<ID, Rank> top;
	SortByRankGetById<ID, Rank> all;

	Rank top_const;
	Rank bottom_const;
};

#endif
