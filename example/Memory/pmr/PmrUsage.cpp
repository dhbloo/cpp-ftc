#include "FTC/Memory/pmr/ProfileResource.hpp"

#include <chrono>
#include <forward_list>
#include <iostream>
#include <memory_resource>

#pragma comment(linker, "/STACK:60000000")

using namespace ftc;

int main()
{
    std::pmr::memory_resource * mr = std::pmr::new_delete_resource();
    std::pmr::forward_list<int> intList {mr};

    std::chrono::time_point t_start = std::chrono::steady_clock::now();

    for (int i = 0; i < 100000; i++)
        intList.push_front(i);

    std::chrono::time_point                   t_end = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> dt    = t_end - t_start;
    std::cout << "new_delete  elapsed time: " << dt.count() << "ms\n";

    int buffer[400000];

    {
        std::pmr::monotonic_buffer_resource mbr {buffer, sizeof(buffer)};
        std::pmr::forward_list<int> intList2 {&mbr};

        t_start = std::chrono::steady_clock::now();

        for (int i = 0; i < 100000; i++)
            intList2.push_front(i);

        t_end = std::chrono::steady_clock::now();
        dt    = t_end - t_start;
        std::cout << "monotonic  elapsed time: " << dt.count() << "ms\n";
    }

    {
        pmr::profile_resource       pr {std::pmr::new_delete_resource()};
        std::pmr::forward_list<int> intList3 {&pr};

        t_start = std::chrono::steady_clock::now();

        for (int i = 0; i < 100000; i++)
            intList3.push_front(i);

        t_end = std::chrono::steady_clock::now();
        dt    = t_end - t_start;
        std::cout << "new_delete(profile)  elapsed time: " << dt.count() << "ms\n";
        std::cout << "-bytes allocated: " << pr.get_stat().bytes_allocated << "\n";
    }

    {
        std::pmr::monotonic_buffer_resource mbr2 {buffer, sizeof(buffer)};
        pmr::profile_resource               pmbr {&mbr2};
        std::pmr::forward_list<int>         intList4 {&pmbr};

        t_start = std::chrono::steady_clock::now();

        for (int i = 0; i < 100000; i++)
            intList4.push_front(i);

        t_end = std::chrono::steady_clock::now();
        dt    = t_end - t_start;
        std::cout << "monotonic(profile)  elapsed time: " << dt.count() << "ms\n";
        std::cout << "-bytes allocated: " << pmbr.get_stat().bytes_allocated << "\n";
    }
}
