#pragma once

#ifdef ENABLE_CODE_RELOAD_WINDOWS
#    define NOMINMAX
#    include "API/LPP_API.h"
#    include <Windows.h>
#endif


#if defined(ENABLE_CODE_RELOAD_UNIX)

#    include <jet/live/Live.hpp>
#    include <jet/live/Utility.hpp>

#endif

//! C System Headers
#include <cctype>
#include <climits>
#include <cmath>
#include <csignal>
#include <cstddef>
#include <cstdlib>
#include <cstring>

//! C++ System Headers
#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <exception>
#include <filesystem>
#include <fstream>
#include <functional>
#include <future>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <new>
#include <numeric>
#include <optional>
#include <queue>
#include <random>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <thread>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

//! Global Helpers
namespace fs = std::filesystem;

constexpr std::size_t operator"" _sz(unsigned long long n) { return n; }

template <typename T>
constexpr std::size_t
bit_size() noexcept
{
    return sizeof(T) * CHAR_BIT;
}

template <class Key, class T, class Compare, class Alloc, class Pred>
void
erase_if(std::map<Key, T, Compare, Alloc>& c, Pred pred)
{
    for (auto i = c.begin(), last = c.end(); i != last;)
    {
        if (pred(*i))
        {
            i = c.erase(i);
        }
        else
        {
            ++i;
        }
    }
}

template <class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...)->overloaded<Ts...>;

//! Folly Headers
#include <folly/Memory.h>
#include <folly/SharedMutex.h>
#include <folly/concurrency/ConcurrentHashMap.h>

namespace folly
{
    // Explicitly instantiate SharedMutex here:
    template class SharedMutexImpl<true>;
    template class SharedMutexImpl<false>;
} // namespace folly

//! Boost Headers
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
//#include <boost/filesystem.hpp>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#include <boost/multiprecision/cpp_dec_float.hpp>
using t_float_50 = boost::multiprecision::cpp_dec_float_50;

inline std::string
get_formated_float(t_float_50 value)
{
    std::stringstream ss;
    ss.precision(8);
    ss << std::fixed << std::move(value);
    return ss.str();
}

inline std::string
adjust_precision(const std::string& current)
{
    std::string       result;
    std::stringstream ss;
    t_float_50        current_f(current);

    ss << std::fixed << std::setprecision(8) << current_f;
    result = ss.str();

    boost::trim_right_if(result, boost::is_any_of("0"));
    boost::trim_right_if(result, boost::is_any_of("."));
    // std::cerr << "WTF: " << result << std::endl;
    return result;
}

#pragma clang diagnostic pop

//! Ranges
#include <range/v3/view/iota.hpp> ///< ranges::view::ints
#include <range/v3/view/zip.hpp>  ///< ranges::view::zip

#ifdef __APPLE__
//! Other dependencies Headers
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wunused-function"
#    pragma clang diagnostic ignored "-Wunused-parameter"
#    pragma clang diagnostic ignored "-Wignored-qualifiers"
#    include <bitcoin/bitcoin/utility/pseudo_random.hpp>
#    include <bitcoin/bitcoin/wallet/mnemonic.hpp>
#    pragma clang diagnostic pop
#elif defined(_WIN32) || defined(WIN32)
#    include <wally.hpp>
#endif

#include <date/date.h>
#define ENTT_STANDARD_CPP
#include <entt/entity/helper.hpp>
#include <entt/signal/dispatcher.hpp>
#include <loguru.hpp>
#include <meta/detection/detection.hpp>
#if defined(_WIN32) || defined(WIN32)
#    define and &&
#    define or ||
#    define not !
#endif
#include <nlohmann/json.hpp>
#include <reproc++/reproc.hpp>
#include <restclient-cpp/restclient.h>
#include <sodium.h>

//! SDK Headers
#include <antara/gaming/core/open.url.browser.hpp>
#include <antara/gaming/core/real.path.hpp>
#include <antara/gaming/ecs/system.hpp>
#include <antara/gaming/ecs/virtual.input.system.hpp>
#include <antara/gaming/event/key.pressed.hpp>
#include <antara/gaming/event/quit.game.hpp>
#include <antara/gaming/graphics/component.canvas.hpp>
#include <antara/gaming/timer/time.step.hpp>
#include <antara/gaming/world/world.app.hpp>

namespace ag = antara::gaming;