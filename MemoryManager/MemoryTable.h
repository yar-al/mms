#ifndef LAB3_MEMORYTABLE_H
#define LAB3_MEMORYTABLE_H

#include <string>
#include <map>
#include <utility>
#include <vector>
#include <unordered_map>
#include "MemoryElements.h"
#include "HashTable.h"

namespace lab3 {
    class Container {
    private:
        std::string dir;
        uint max;
        std::vector<MemoryElement*> elements;
        typedef std::vector<MemoryElement*>::const_iterator iterator_t;
        static size_t sum(iterator_t first, iterator_t last);
        static size_t sum_mt(iterator_t first, iterator_t last);
    public:
        Container(std::string d, uint m) : dir(std::move(d)), max(m) {}
        [[nodiscard]] std::string get_directory() const{ return dir; }
        [[nodiscard]] uint get_max() const{ return max; }
        void set_max(uint newMax) { max = newMax; }
        void insert_element(MemoryElement *e) { elements.push_back(e); }
        void delete_element(const std::string& name);
        [[nodiscard]] MemoryElement* find_element(const std::string& name) const;
        [[nodiscard]] uint memory_usage() const;
        [[nodiscard]] uint memory_usage_multithreading() const;
        [[nodiscard]] std::vector<MemoryElement const*> get_elements() const{
            std::vector<MemoryElement const*> els;
            const MemoryElement* el;
            for(const auto& it : elements)
            {
                el = it;
                els.push_back(el);
            }
            return els;
        }
    };

    class MemoryTable{
    private:
        //std::unordered_map<uint, Container> table;
        HashTable<uint, Container> table;
        uint last_id;
    public:
        MemoryTable(): table(), last_id(0) {}
        void new_program(std::string dir, uint maxmem);
        void remove_program(uint id);
        bool new_element(uint id, MemoryElement *el);
        [[nodiscard]] MemoryElement *find_element_local(uint start, const std::string& name) const;
        [[nodiscard]] MemoryElement *find_element_global(const std::string& name) const;
        void delete_program(uint id);
        void delete_element(uint id, const std::string& name);
        [[nodiscard]] uint memory_max() const;
        [[nodiscard]] uint memory_usage(uint id, bool multi=false) const;
        [[nodiscard]] uint full_memory_usage() const;
        [[nodiscard]] uint memory_limit(uint id);
        [[nodiscard]] HashTable<uint, Container> const* get_table() const{
            return &table;
        }
    };
}
#endif //LAB3_MEMORYTABLE_H
