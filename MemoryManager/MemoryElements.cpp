#include <iostream>
#include "MemoryElements.h"

namespace lab3 {

    char *MemoryElement::get(uint index) const {
        if (index > 0) throw MemoryException("Index out of range");
        else return get();
    }

    char *MemoryElement::get_multiple(uint start, uint end) const {
        if (start + end > 0) throw MemoryException("Index out of range");
        else return get();
    }

    void MemoryElement::set(uint index, char *data) const {
        if (index > 0) throw MemoryException("Index out of range");
        else set(data);
    }

    void MemoryElement::move(uint to) {
        char *data = get();
        address = to;
        set(data);
        delete[] data;
    }

    char *Array::get(uint index) const {
        if(index*elementSize > get_size()) throw MemoryException("Index out of range");
        return get_space()->read(get_address()+index*elementSize, elementSize);
    }

    char *Array::get_multiple(uint start, uint end) const {
        if(start > end) std::swap(start, end);
        if(start*elementSize > get_size() || end*elementSize > get_size()) throw MemoryException("Index out of range");
        return get_space()->read(get_address()+start*elementSize, (end-start)*elementSize);
    }

    void Array::set(uint index, char *data) const {
        if(index*elementSize > get_size()) throw MemoryException("Index out of range");
        get_space()->write(get_address()+index*elementSize, elementSize, data);
    }

    void SharedElement::give_access(uint id) {
        if(std::find(programs.begin(), programs.end(), id) != programs.end()) return;
        programs.push_back(id);
    }

    void SharedElement::withdraw_access(uint id) {
        programs.erase(std::remove(programs.begin(), programs.end(), id),programs.end());
    }

    bool SharedElement::check_access(uint id) const {
        return std::find(begin(programs), end(programs),id) != programs.end();
    }
}