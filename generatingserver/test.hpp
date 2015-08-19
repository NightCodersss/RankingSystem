#ifndef _TEST_HPP_
#define _TEST_HPP_

#include <iostream>
#include "sortbyrankgetbyidwithtop.hpp"


void testSortByRankGetById()
{
	std::cout << "Testing SortByRankGetById structure\n";

	SortByRankGetById<int, int> s{};
	s.increment(228, 3);
	assert(s.size() == 1);
	assert(s.upper_bound(4)->first == 3);
	assert(s.upper_bound(4)->second == 228);
	s.increment(228, 4);
	assert(s.size() == 1);
	assert(s.upper_bound(8)->first == 7);
	assert(s.upper_bound(8)->second == 228);
	s.increment(2228, 100);
	assert(s.size() == 2);
	assert(s.upper_bound(8)->first == 7);
	assert(s.upper_bound(8)->second == 228);
	assert(s.upper_bound(500)->first == 100);
	assert(s.upper_bound(500)->second == 2228);
	std::cout << "It seems to be ok\n";
}

void testSortByRankGetByIdWithTop()
{
	std::cout << "Testing SortByRankGetByIdWithTop structure\n";

	SortByRankGetByIdWithTop<int, int> s{1000, -10};
	s.increment(228, 3);
	assert(s.size() == 1);
	assert(s.upper_bound(4)->first == 3);
	assert(s.upper_bound(4)->second == 228);
	s.increment(228, 4);
	assert(s.size() == 1);
	assert(s.upper_bound(8)->first == 7);
	assert(s.upper_bound(8)->second == 228);
	s.increment(2228, 100);
	assert(s.size() == 2);
	assert(s.upper_bound(8)->first == 7);
	assert(s.upper_bound(8)->second == 228);
	assert(s.upper_bound(500)->first == 100);
	assert(s.upper_bound(500)->second == 2228);
	s.cutOff();
	assert(s.topSize() == 0);
	s.increment(2228, 100);
	assert(s.topSize() == 1);
	assert(s.size() == 2);

	std::cout << "It seems to be ok\n";
}

void runTests()
{
	testSortByRankGetById();
	testSortByRankGetByIdWithTop();
}
#endif
