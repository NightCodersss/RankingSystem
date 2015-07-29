#ifndef _SORTBYRANKGETBYID_HPP_
#define _SORTBYRANKGETBYID_HPP_

#include <map>

template <typename ID, typename Rank>
class SortByRankGetById
{
public:

	void increment(ID key, Rank value) 
	{
		rank_and_id.erase({rank_by_id[key], key});
		rank_by_id[key] += value;
		rank_and_id.insert({rank_by_id[key], key)};		
	}

	std::pair<ID, Rank> upper_bound(Rank rank)
	{
		return *rank_and_id.upper_bound({rank, 0});
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
