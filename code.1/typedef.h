/*
    Created BY 谢子飏 19307130037
*/
#pragma once
#ifndef TYPEDEF
#define TYPEDEF
#include <iterator>
#include <deque>

namespace DS
{
// Type Declare
typedef int value_type;
typedef long long key_type;
typedef std::deque<value_type> node_type;
typedef value_type* node_ptr;
typedef std::ptrdiff_t difference_type;
typedef std::size_t size_type;
}

#endif