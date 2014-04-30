#pragma once

#include <chrono>
 
constexpr auto operator "" _days (unsigned long long n) { return std::chrono::hours(24 * n); }
constexpr auto operator "" _h    (unsigned long long n) { return std::chrono::hours(n); }
constexpr auto operator "" _min  (unsigned long long n) { return std::chrono::minutes(n); }
constexpr auto operator "" _s    (unsigned long long n) { return std::chrono::seconds(n); }
constexpr auto operator "" _ms   (unsigned long long n) { return std::chrono::milliseconds(n); }
constexpr auto operator "" _µs   (unsigned long long n) { return std::chrono::microseconds(n); }
constexpr auto operator "" _ns   (unsigned long long n) { return std::chrono::nanoseconds(n); }