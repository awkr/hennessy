#pragma once

// Unsigned int types.
using u8  = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

// Signed int types.
using i8  = signed char;
using i16 = signed short;
using i32 = signed int;
using i64 = signed long long;

// Floating point types.
using f32 = float;
using f64 = double;

// Boolean types.
using b8  = char;
using b32 = int;

// Platform detection.
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define PLATFORM_WINDOWS 1
#elif defined(__linux__) || defined(__gnu_linux__)
#define PLATFORM_LINUX 1
#if defined(__ANDROID__)
#define PLATFORM_ANDROID 1
#endif
#elif defined(__unix__)
#define PLATFORM_UNIX 1
#elif defined(_POSIX_VERSION)
#define PLATFORM_POSIX 1
#elif __APPLE__
#define PLATFORM_APPLE 1
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR
#define PLATFORM_IOS           1
#define PLATFORM_IOS_SIMULATOR 1
#elif TARGET_OS_IPHONE
#define PLATFORM_IOS 1
#endif
#endif
