#ifndef INC_CPPUTIL_H_
#define INC_CPPUTIL_H_
/**
@file cpputil.h
@author t-sakai

# License
This software is distributed under two licenses, choose whichever you like.

## MIT License
Copyright (c) 2020 Takuro Sakai

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

## Public Domain
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>
```
# Usage
Put '#define CPPUTIL_IMPLEMENTATION' before including this file to create the implementation.
*/
#include <cstdint>
#include <functional>

namespace cpputil
{
#ifndef CPPUTIL_NULL
#    ifdef __cplusplus
#        if 201103L <= __cplusplus || 1700 <= _MSC_VER
#            define CPPUTIL_NULL nullptr
#        else
#            define CPPUTIL_NULL 0
#        endif
#    else
#        define CPPUTIL_NULL (void*)0
#    endif
#endif

#ifndef CPPUTIL_TYPES
#    define CPPUTIL_TYPES
using s8 = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = int64_t;

using f32 = float;
using f64 = double;

using size_t = ::size_t;

using Char = char;

#endif // CPPUTIL_TYPES

#ifndef CPPUTIL_ASSERT
#    define CPPUTIL_ASSERT(exp) assert(exp)
#endif

/**
 @brief Address for IP v4
 */
class IPAddress4
{
public:
    static constexpr s32 Size = 16;

    IPAddress4();
    IPAddress4(const IPAddress4& other);
    explicit IPAddress4(u32 address);

    u32 address() const
    {
        return integer_;
    }

    constexpr s32 length() const
    {
        return Size-1;
    }

    const Char* c_str() const
    {
        return string_;
    }

    IPAddress4& operator=(const IPAddress4& other);
private:
    inline static Char toChar(u32 x);
    s32 print(s32 pos, u32 x);

    u32 integer_;
    Char string_[Size];
};

class Arguments
{
public:
    struct Context
    {
        template<class T>
        T* getData() const{return reinterpret_cast<T*>(data_);};
        const Char* current() const{return argv_[count_];}

        s32 count_;
        s32 argc_;
        const Char** argv_;
        void* data_;
    };

    static void parse(s32 argc, const Char* argv[], std::function<void(const Context& context)> func, void* data);
    static const Char* requestNext(const Context& context);
    static s32 requestNextInt(const Context& context, s32 defaultValue);
    static f32 requestNextFloat(const Context& context, f32 defaultValue);

    static bool compare(const Char* s0, const Char* s1);
    static s32 toInt(const Char* s);
    static f32 toFloat(const Char* s);

protected:
    Arguments() = delete;
    ~Arguments() = delete;
    Arguments(const Arguments&) = delete;
    Arguments& operator=(const Arguments&) = delete;
};

} //namespace cpputil
#endif // INC_CPPUTIL_H_

#ifdef CPPUTIL_IMPLEMENTATION
#include <cassert>
#include <immintrin.h>

namespace cpputil
{

IPAddress4::IPAddress4()
    : integer_(0)
    , string_{'0', '.', '0', '.', '0', '.', '0', '\0'}
{
}

IPAddress4::IPAddress4(const IPAddress4& other)
{
    integer_ = other.integer_;
    _mm_storeu_ps(reinterpret_cast<f32*>(string_), _mm_loadu_ps(reinterpret_cast<const f32*>(other.string_)));
}

IPAddress4::IPAddress4(u32 address)
    : integer_(address)
{
    s32 pos = 0;
    pos = print(pos, (address >> 24) & 0xFFU);
    string_[pos++] = '.';
    pos = print(pos, (address >> 16) & 0xFFU);
    string_[pos++] = '.';
    pos = print(pos, (address >> 8) & 0xFFU);
    string_[pos++] = '.';
    pos = print(pos, (address >> 0) & 0xFFU);
    string_[pos] = '\0';
    CPPUTIL_ASSERT(pos < Size);
}

IPAddress4& IPAddress4::operator=(const IPAddress4& other)
{
    integer_ = other.integer_;
    _mm_storeu_ps(reinterpret_cast<f32*>(string_), _mm_loadu_ps(reinterpret_cast<const f32*>(other.string_)));
    return *this;
}

inline Char IPAddress4::toChar(u32 x)
{
    CPPUTIL_ASSERT(0 <= x && x < 10);
    return static_cast<Char>('0' + x);
}

s32 IPAddress4::print(s32 pos, u32 x)
{
    if(x < 10) {
        string_[pos++] = toChar(x);
    } else if(x < 100) {
        u32 i;
        i = x / 10;
        string_[pos++] = toChar(i);
        x -= i * 10;
        string_[pos++] = toChar(i);
    } else {
        u32 i;
        i = x / 100;
        string_[pos++] = toChar(i);
        x -= i * 100;
        i = x / 10;
        string_[pos++] = toChar(i);
        x -= i * 10;
        string_[pos++] = toChar(i);
    }
    return pos;
}

void Arguments::parse(s32 argc, const Char* argv[], std::function<void(const Context&)> func, void* data)
{
    Context context = {0, argc, argv, data};
    for(; context.count_<argc; ++context.count_){
        func(context);
    }
}

const Char* Arguments::requestNext(const Context& context)
{
    s32 next = context.count_ + 1;
    if(context.argc_<=next){
        return CPPUTIL_NULL;
    }
    const_cast<s32&>(context.count_) = next;
    return context.argv_[next];
}

s32 Arguments::requestNextInt(const Context& context, s32 defaultValue)
{
    const Char* next = Arguments::requestNext(context);
    return (CPPUTIL_NULL != next)? Arguments::toInt(next) : defaultValue;
}

f32 Arguments::requestNextFloat(const Context& context, f32 defaultValue)
{
    const Char* next = Arguments::requestNext(context);
    return (CPPUTIL_NULL != next)? Arguments::toFloat(next) : defaultValue;
}

bool Arguments::compare(const Char* s0, const Char* s1)
{
    return 0 == ::strcmp(s0, s1);
}

s32 Arguments::toInt(const Char* s)
{
    return atol(s);
}

f32 Arguments::toFloat(const Char* s)
{
    return static_cast<f32>(atof(s));
}

} // namespace cpputil
#endif // CPPUTIL_IMPLEMENTATION

