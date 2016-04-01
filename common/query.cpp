#include "query.hpp"

Query::Query(const std::string& text) : text(text) { }
Query::Query(const std::string&& text) : text(text) { }

const std::string& Query::getText() const { return text; }
