#ifndef _SORTBYRANKGETBYID_HPP_
#define _SORTBYRANKGETBYID_HPP_

#include <map>
#include <set>

template <typename ID, typename Rank>
class SortByRankGetById
{
public:

	void increment(ID key, Rank value) 
	{
		rank_and_id.erase({rank_by_id[key], key});
		rank_by_id[key] += value;
		rank_and_id.insert({rank_by_id[key], key});
	}

	auto upper_bound(Rank rank) const
	{
		return rank_and_id.upper_bound({rank, std::numeric_limits<ID>::max()});
	}

	auto begin() const
	{
		return rank_and_id.begin();	
	}

	auto end() const
	{
		return rank_and_id.end();
	}

	auto size() const
	{
		return rank_by_id.size();
	}

	void cutOff(Rank rank)
	{
		for ( auto it = rank_and_id.upper_bound({rank, 0}); it != std::end(rank_and_id); )
		{
			rank_by_id.erase(it -> second);
			it = rank_and_id.erase(it);
		}
	}

private:
	std::map<ID, Rank> rank_by_id;
	std::set< std::pair<Rank, ID> , std::greater<std::pair<Rank, ID>>> rank_and_id;
};

#endif
