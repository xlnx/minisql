#pragma once

#include <debug/debug.hpp>
#include <vector>
#include <variant>
#include <string>

#define BLOCK_SIZE (1024 * 4)

#define SQL_INT 0x100000
#define SQL_CHAR(N) (0x200000 | N)
#define SQL_FLOAT 0x400000

#define SQL_POINTER 0x800000
#define SQL_POINTER_DATA 0x800001
#define SQL_POINTER_NODE 0x800002

#define SQL_NONE_TYPE 0x0

#define MAX_CACHE_BLOCK_COUNT (1024) // 4kb * 1024 = 4 MB

#define SQL_NULL (-1)
#define SQL_NAP (-2)

#define SizeOf(x) SizeType(sizeof(x))

namespace minisql
{

namespace __buffer
{

using BufferType = unsigned short;
using BufferElem = unsigned;
using OffType = long long;
using SizeType = int;
using ItemIndex = int;

using ItemType = std::vector<BufferElem>;

template <typename ...Args>
using heap = std::vector<Args...>;

}

using __buffer::ItemType;

}