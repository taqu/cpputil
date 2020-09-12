#include "../cpputil.h"

#include <string>
#include <iostream>
#include <random>
#include <chrono>
#include <array>
#include "utest.h"

using namespace cpputil;

IPAddress4 getIP4(u32 ip)
{
    return IPAddress4(ip);
}

std::string getIPAddress(u32 ip)
{
    std::array<Char, 16> buffer;
    s32 l = sprintf_s(&buffer[0], buffer.size(), "%u.%u.%u.%u",
              ((ip >> 24) & 0xFFU),
              ((ip >> 16) & 0xFFU),
              ((ip >> 8) & 0xFFU),
              ((ip >> 0) & 0xFFU));
    return std::string(&buffer[0], l);
}

template<class T>
void copy(s32 count, s32 size, std::function<T(u32)> func)
{
    u32* data = new u32[size];

    std::random_device seed;
    std::mt19937 engine(seed());
    for(s32 i=0; i<size; ++i){
        data[i] = engine();
    }

    s64 accTime = 0;
    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point end;
    for(s32 i = 0; i < count; ++i) {
        for(s32 j = 0; j < size; ++j) {
            data[j] = engine();
        }
        T* result = new T[size];

        start = std::chrono::high_resolution_clock::now();
        for(s32 j = 0; j < size; ++j) {
            result[j] = func(data[j]);
            //std::cout << result[j].c_str() << std::endl;
        }
        end = std::chrono::high_resolution_clock::now();
        accTime += std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
        delete[] result;
    }

    double time = 0.001 * accTime / count;
    std::cout << time << " micro seconds" << std::endl;
    delete[] data;
}

UTEST(IPAddress, V4)
{
    static const int Count = 10;
    static const int Size = 4096;
    copy<std::string>(Count, Size, getIPAddress);
    copy<IPAddress4>(Count, Size, getIP4);
}
