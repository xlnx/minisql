#pragma once

#include <debug/debug.hpp>
#include <vector>
#include <variant>
#include <string>
#include <queue>

#define BLOCK_SIZE (1024 * 4)

#define SQL_INT 0x100000U
#define SQL_CHAR(N) (BufferElem(0x200000U | N))
#define SQL_FLOAT 0x400000U

#define SQL_POINTER 0x800000U
#define SQL_POINTER_DATA 0x800001U
#define SQL_POINTER_NODE 0x800002U

#define SQL_NONE_TYPE 0x0

#define MAX_CACHE_BLOCK_COUNT (1024 * 32) // 4kb * 1024 = 4 MB

#define SQL_NULL (-1)
#define SQL_NAP (-2)

#define SizeOf(x) SizeType(sizeof(x))

namespace minisql
{

namespace __buffer
{

using std::nullptr_t;

using BufferType = unsigned short;
using BufferElem = unsigned;
using OffType = long long;
using SizeType = int;
using ItemIndex = int;
using RefCount = ItemIndex;

using ItemType = std::vector<BufferElem>;

template <typename ...Args>
using heap = std::deque<Args...>;

}

using __buffer::ItemType;
using __buffer::BufferElem;
using __buffer::SizeType;

}