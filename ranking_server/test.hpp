#ifndef _TEST_HPP_
#define _TEST_HPP_

#include <iostream>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>

#include "sortbyrankgetbyidwithtop.hpp"


void testSortByRankGetById()
{
	BOOST_LOG_TRIVIAL(info) << "Testing SortByRankGetById structure\n";

	SortByRankGetById<int, int> s{}; // ID, rank
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

	s.cutOff(50);
	assert(s.size() == 1);
	assert(s.upper_bound(101)->second == 2228);
	s.cutOff(102);
	assert(s.size() == 0);

	BOOST_LOG_TRIVIAL(info) << "It seems to be ok\n";
}

void testSortByRankGetByIdWithTop()
{
	BOOST_LOG_TRIVIAL(info) << "Testing SortByRankGetByIdWithTop structure\n";

	SortByRankGetByIdWithTop<int, int> s{10000, -10};

	s.increment(228, 3);
	assert(s.all_size() == 1);
	assert(s.all_upper_bound(4)->first == 3);
	assert(s.all_upper_bound(4)->second == 228);
	assert(s.top_size() != s.all_size());

	s.increment(228, 4);
	assert(s.all_size() == 1);
	assert(s.top_size() != s.all_size());
	assert(s.all_upper_bound(8)->first == 7);
	assert(s.all_upper_bound(8)->second == 228);

	s.increment(2228, 100);
	assert(s.all_size() == 2);
	assert(s.top_size() != s.all_size());
	assert(s.all_upper_bound(8)->first == 7);
	assert(s.all_upper_bound(8)->second == 228);
	assert(s.all_upper_bound(500)->first == 100);
	assert(s.all_upper_bound(500)->second == 2228);

	s.cutOff();
	assert(s.top_size() == 0);

	s.increment(2228, 100000);
	assert(s.top_size() == 1);
	assert(s.all_size() == 2);

	BOOST_LOG_TRIVIAL(info) << "It seems to be ok\n";
}

void runTests() // TODO: Why these function are in hpp?
{
	testSortByRankGetById();
	testSortByRankGetByIdWithTop();
}
#endif
