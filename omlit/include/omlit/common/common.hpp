#pragma once

#include <cstdint>
#include <string>
#include <sstream>
#include <memory>
#include <vector>
#include <deque>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <functional>
#include <filesystem>
#include <chrono>

namespace omlit
{
    enum Error
    {
        SUCCEED = 0,
        INVALID_ID,
        INCORRECT_INTERFACE,
        INCORRECT_INPUT_TYPE,
        INCORRECT_OUTPUT_TYPE,
        INNER_ERROR,
    };
}