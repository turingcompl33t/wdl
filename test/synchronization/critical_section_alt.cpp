// critical_section_alt.cpp
// Unit Test: wdl::synchronization::critical_section_alt
//
// Demonstration of use of alternative critical section implementation.
//
// Build
//  cl /EHsc /nologo /std:c++17 /W4 /I C:\Dev\WDL critical_section_alt_test.cpp

#include <thread>
#include <chrono>
#include <iostream>

#include "wdl/synchronization/critical_section_alt.hpp"

#pragma comment(lib, "synchronization.lib")

unsigned long long g_data = 0;
wdl::synchronization::critical_section_alt g_critical_section{};

void worker()
{
    for (auto i = 0; i < 10000; ++i)
    {
        g_critical_section.enter();
        g_data++;
        g_critical_section.exit();
    }
}

int main()
{
    auto t1 = std::thread{worker};
    auto t2 = std::thread{worker};

    t1.join();
    t2.join();

    std::cout << "Total Count: " << g_data << std::endl;
}