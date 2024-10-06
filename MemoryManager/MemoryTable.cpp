#include <thread>
#include <numeric>
#include <execution>
#include "MemoryTable.h"

void lab3::Container::delete_element(const std::string &name) {
    elements.erase(std::remove_if(elements.begin(), elements.end(),
                                  [&name](MemoryElement* val) {return val->get_name() == name; }),
                   elements.end());
}

lab3::MemoryElement *lab3::Container::find_element(const std::string &name) const {
    auto a = std::find_if(begin(elements), end(elements),
                         [&name](MemoryElement* val) { return val->get_name() == name;});
    if (a != elements.end())
        return *a;
    else
        return nullptr;
}

uint lab3::Container::memory_usage() const { //one thread
    uint size = 0;
    for(auto &i : elements)
    {
        size += i->get_size();
    }
    return size;
}

template<typename It>
size_t temp_sum(It first, It last) {
    std::iter_value_t<It> res = 0;
    for(; first != last; ++first) {
        res += *first;
    }
    return res;
}

size_t lab3::Container::sum(iterator_t first, iterator_t last) {
    /*size_t res = 0;
    MemoryElement *i = *first;
    std::cout<<i->get_size()<<std::endl;
    while(first != last){
        res += i->get_size();
        first++;
    }
    return res;*/
    return std::accumulate(first, last, 0, [](size_t a, MemoryElement* b){return a + b->get_size(); });
}
size_t lab3::Container::sum_mt(iterator_t first, iterator_t last) {
    //return std::transform_reduce(std::execution::par_unseq, first, last, 0ul, [](size_t a, size_t b) {return a+b;}, [](const MemoryElement* b){return b->get_size(); });
    auto threadNum = std::thread::hardware_concurrency();
    auto elements = std::distance(first, last);
    std::vector<size_t> results(threadNum);
    std::vector<std::thread> threads(threadNum);
    for (long i = 0; i < threadNum; ++i) {
        long start_i = i * elements / threadNum;
        long end_i = (i + 1) * elements / threadNum;
        auto start = std::next(first, start_i);
        auto end = std::next(first, end_i);
        threads[i] = std::thread([start, end, i, &results](){
            results[i] = sum(start, end);
        });
    }
    for(auto& th : threads) {
        th.join();
    }
    return temp_sum(results.begin(), results.end());
}

uint lab3::Container::memory_usage_multithreading() const { //multithreading
    return sum_mt(elements.begin(), elements.end());
}

void lab3::MemoryTable::new_program(std::string dir, uint maxmem) {
    table.insert({last_id++, Container(std::move(dir), maxmem)});
}

bool lab3::MemoryTable::new_element(uint id, lab3::MemoryElement *el) {
    auto it = table.find(id);
    if(it == table.end()) return false;
    if(it->second.find_element(el->get_name())) return false;
    it->second.insert_element(el);
    return true;
}

void lab3::MemoryTable::delete_program(uint id) {
    table.erase(id);
}

void lab3::MemoryTable::delete_element(uint id, const std::string &name) {
    auto a = table.find(id);
    if(a != table.end())
        a->second.delete_element(name);
}

lab3::MemoryElement *lab3::MemoryTable::find_element_local(uint start, const std::string& name) const {
    MemoryElement *res = table.find(start)->second.find_element(name);
    return res;
}

lab3::MemoryElement *lab3::MemoryTable::find_element_global(const std::string &name) const {
    MemoryElement *res = nullptr;
    for (auto& it: table) {
        res = it.second.find_element(name);
        if(res) break;
    }
    return res;
}

uint lab3::MemoryTable::memory_usage(uint id, bool multi) const {
    if(multi)
        return table.find(id)->second.memory_usage_multithreading();
    return table.find(id)->second.memory_usage();
}

uint lab3::MemoryTable::memory_limit(uint id) {
    return table.find(id)->second.get_max();
}

uint lab3::MemoryTable::memory_max() const {
    uint sum = 0;
    for(auto &a : table)
        sum += a.second.get_max();
    return sum;
}

uint lab3::MemoryTable::full_memory_usage() const{
    uint res = 0;
    for(const auto& it : table){
        res += it.second.memory_usage();
    }
    return res;
}

void lab3::MemoryTable::remove_program(uint id) {
    table.erase(id);
}
