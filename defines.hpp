#pragma once

#include <string>
#include <boost/asio.hpp>

#define DO_PRAGMA(x) _Pragma (#x)
#define TODO(x) DO_PRAGMA(message ("TODO - " #x))

using TextID = std::string;
static const TextID EmptyTextID = "";
using DocID = long long;
using TextIndex = int; // enumerating for bitsets
using SocketStream = boost::asio::ip::tcp::iostream;
