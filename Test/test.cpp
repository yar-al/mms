#define CATCH_CONFIG_MAIN

#include "../MemoryManager/MemoryManager.h"
#include <catch2/catch.hpp>
#include <random>

using namespace lab3;

TEST_CASE("Adding")
{
    MemoryManager manager(1000);
    REQUIRE_THROWS(manager.add_program("C:/googoogaga/zhizh.exe", 1000000));
    manager.add_program("C:/googoogaga/zhizh.exe", 50);
    manager.add_element(0, "x", 10);
    manager.add_element(0, "y", 10);
    auto ref = manager.get_reference(0, "xref", "x");
    REQUIRE(manager.use_element(0, "y").get_name() == "y");
    REQUIRE(ref.get_name() == "xref");
    REQUIRE_THROWS(manager.add_element(0, "too_much", 100));
    REQUIRE(manager.get_errors(0).size() == 1);
}

TEST_CASE("Get/Set memory elements")
{
    MemoryManager manager(1000);
    manager.add_program("C:/googoogaga/zhizh.exe", 30);
    manager.add_program("C:/googoogaga/parapapa.exe", 30);
    manager.add_element(0, "x", sizeof(int));
    manager.add_element(0, "y", 10);
    manager.add_element(0, "z", sizeof(int)*2, sizeof(int), true);
    manager.delete_element(0, "y");
    //auto ref = manager.get_reference(0, "xref", "x");
    int a = 12132321;
    manager.use_element(0, "x").set(lab3::MemoryManager::make_bytes(&a));
    REQUIRE(*static_cast<int*>(static_cast<void*>(manager.use_element(0, "x").get().data())) == 12132321);
    auto prev = manager.use_element(0, "z").get_address();
    manager.defragmentation();
    //REQUIRE(manager.use_element(0, "z").get_address() < prev);
    int c = 8;
    manager.use_element(0, "z").set(1, MemoryManager::make_bytes(&c));
    //std::cout << manager.use_element(0, "z").get(1);
    REQUIRE(*static_cast<int*>(static_cast<void*>(manager.use_element(0, "z").get(1).data())) == 8);
    manager.give_access(0, "z", 1);
    REQUIRE(*static_cast<int*>(static_cast<void*>(manager.use_element(1, "z").get(1).data())) == 8);
    manager.withdraw_access(0, "z", 1);
    REQUIRE_THROWS(manager.use_element(1, "z"));
}

TEST_CASE("Memory usage")
{
    MemoryManager manager(1000000000);
    manager.add_program("C:/googoogaga/zhizh.exe", 1000000000-1);
    std::string letters("abcdefgijk1234567890");
    uint s;
    std::random_device rd;  // a seed source for the random number engine
    std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<size_t> distrib(1, 100);
    for(int i = 0; i < 1000; i++){
        std::string n;
        for(int j = 0; j < 10; j++)
            n.push_back(letters[distrib(gen)%letters.size()]);
        s = distrib(gen)%10;
        //std::cout << s << std::endl;
        manager.add_element(0, n, s);
    }
    std::cout << std::endl;
    size_t r1, r2;
    clock_t start, end;
    double dtime;
    start = std::clock();
    r1 = manager.memory_usage(0, false);
    end = std::clock();
    dtime = (double)(end - start) / CLOCKS_PER_SEC;
    dtime *= 1000;
    std::cout << "Multithreading off: " << dtime << std::endl;
    start = std::clock();
    r2 = manager.memory_usage(0, true);
    end = std::clock();
    dtime = (double)(end - start) / CLOCKS_PER_SEC;
    dtime *= 1000;
    std::cout << "Multithreading on:  " << dtime << std::endl;
    REQUIRE(r1 == r2);
}

HashTable<int, int> htable(1);

TEST_CASE("Table")
{
    htable.insert({2,1});
    //htable.print();
    htable.insert({10, 2});
    //htable.print();
    htable.insert({11, 3});
    //htable.print();
    htable.insert({12, 4});
    htable.insert({22, 2});
    htable.emplace(35, 41);
    //htable.print();
    //for(auto it : htable)
    //   std::cout << it.first << std::endl;
    htable.erase(11);
    htable.erase(12);
    for(auto it : htable){
        //std::cout << it.first << std::endl;
        REQUIRE(it.second == htable.find(it.first)->second);
    }
    REQUIRE(htable.find(11) == htable.end());
    REQUIRE(htable.load_factor() <= htable.max_load_factor());
    htable.max_load_factor(1);
    htable.rehash(0);
    REQUIRE(htable.load_factor() <= htable.max_load_factor());
    //htable.print();
    std::cout << std::endl;
    REQUIRE(htable.bucket_count() > 0);
    REQUIRE(htable.bucket_size(2) > 0);
    htable = std::move(HashTable<int, int>());
    REQUIRE(htable.empty());
}