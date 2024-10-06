#include <iostream>
#include "MemoryManager.h"

/*
static const std::unordered_map<std::string, std::function<lab3::MemoryElement()>> types = {
        std::make_pair(std::string("base"), std::function([](){return new lab3::MemoryElement(...)})),
        std::make_pair(std::string("shared"), std::function([](){return new lab3::SharedElement(...)}))
};
*/
void lab3::MemoryManager::add_element(uint caller, std::string name, uint size, uint sizeOfElement, bool isPublic) {
    if(!id_exists(caller))
        generate_error(caller, MemoryException("Caller does not exist"));
    if(table.memory_usage(caller) + size > table.memory_limit(caller))
        generate_error(caller, MemoryException("Element can not be added due to memory limitation for this process"));
    MemoryElement *nel;
    //MemoryElement *nel = types[type](&memorySpace,  std::move(name), vector_size, memorySpace.get_space(), sizeOfElement, );
    if(sizeOfElement == 0)
        nel = new Variable(&memorySpace,  std::move(name), size, memorySpace.get_space());
    else if(isPublic)
        nel = new SharedElement(&memorySpace,  std::move(name), size, memorySpace.get_space(), sizeOfElement);
    else
        nel = new Array(&memorySpace,  std::move(name), size, memorySpace.get_space(), sizeOfElement);
    if(!table.new_element(caller, nel)){
        generate_error(caller, MemoryException("Element with this name already exists"));
    }
    elements.push_back(std::shared_ptr<MemoryElement>(nel));
    memorySpace.set_space(nel->get_address()+size);
}

std::stack<lab3::MemoryException> lab3::MemoryManager::get_errors(uint id) {
    std::stack<MemoryException> res;
    for(auto & error : errors)
    {
        if(error.first == id)
            res.push(error.second);
    }
    return res;
}

void lab3::MemoryManager::delete_element(uint caller, const std::string& name) {
    if(!id_exists(caller))
        generate_error(caller, MemoryException("Caller does not exist"));
    table.delete_element(caller, name);
    elements.erase(std::remove_if(elements.begin(), elements.end(),
                                  [&name](const std::shared_ptr<MemoryElement>& val) {return val->get_name() == name; }),
                   elements.end());
}

lab3::Handler lab3::MemoryManager::use_element(uint caller, const std::string& name) {
    if(!id_exists(caller))
        generate_error(caller, MemoryException("Caller does not exist"));
    MemoryElement *el = table.find_element_local(caller, name);
    if(el)
        return Handler{caller, el, &errors};
    el = table.find_element_global(name);
    if(el == nullptr)
        generate_error(caller,MemoryException("Element with this name either does not exist or does not provide access to the calling process"));

    auto *elcast = cast_to_shared(el);
    if(!elcast)
        generate_error(caller, MemoryException("Element with this name either does not exist or does not provide access to the calling process"));
    if(!elcast->check_access(caller))
        generate_error(caller, MemoryException("Element with this name either does not exist or does not provide access to the calling process"));
    return Handler{caller, el, &errors};
}

auto lab3::Handler::my_try(auto&& func) {
    try {
        return func();
    }
    catch(MemoryException &e) {
        errors->insert({id, e});
        throw;
    }
}

std::string lab3::Handler::get_name() {
    return my_try([&](){return el->get_name();});
}

uint lab3::Handler::get_size() {
    return my_try([&](){return el->get_size();});
}

uint lab3::Handler::get_address() {
    return my_try([&](){return el->get_address();});
}

std::string lab3::Handler::get() {
    auto d = std::shared_ptr<char[]>(el->get());
    auto res = my_try([&](){return std::string(d.get(), el->get_size());});
    return res;
}

std::string lab3::Handler::get(uint index) {
    auto d = std::shared_ptr<char[]>(el->get(index));
    auto res = my_try([&](){return std::string(d.get(), el->get_element_size());});
    return res;
}

std::string lab3::Handler::get_multiple(uint start, uint end) {
    auto d = std::shared_ptr<char[]>(el->get_multiple(start, end));
    auto res = my_try([&](){return std::string(d.get(),el->get_element_size()*(end-start));});
    return res;
}

void lab3::Handler::set(std::string data) {
    my_try([&](){if(data.size() != el->get_size()) throw MemoryException("Incorrect input size");});
    my_try([&](){set(data.data());});
}

void lab3::Handler::set(uint index, std::string data) {
    std::cout << data.size() << " " << el->get_element_size() << std::endl;
    my_try([&](){if(data.size() != el->get_element_size()) throw MemoryException("Incorrect input size");});
    my_try([&](){set(index, data.data());});
}

void lab3::Handler::set(char *data) {
    my_try([&](){el->set(data);});
}

void lab3::Handler::set(uint index, char *data) {
    my_try([&](){el->set(index, data);});
}

bool lab3::Handler::shared() const {
    auto* to = dynamic_cast<PublicSegment*>(el);
    if(to)
        return true;
    else
        return false;
}

lab3::Handler lab3::MemoryManager::get_reference(uint caller, std::string refName, const std::string& elName) {
    if(!id_exists(caller))
        generate_error(caller, MemoryException("Caller does not exist"));
    Reference *ref;
    MemoryElement *el = table.find_element_local(caller, elName);
    if (el) {
        ref = new Reference(&memorySpace, std::move(refName), memorySpace.get_space(), el);
    }
    else{
        el = table.find_element_global(elName);
        if(el == nullptr)
            generate_error(caller, MemoryException("Element with this name either does not exist or does not provide access to the calling process"));
        auto elcast = cast_to_shared(el);
        if(!elcast)
            generate_error(caller, MemoryException("Element with this name either does not exist or does not provide access to the calling process"));
        if(!elcast->check_access(caller))
            generate_error(caller, MemoryException("Element with this name either does not exist or does not provide access to the calling process"));
        if(table.memory_usage(caller) > table.memory_limit(caller) - sizeof(uint))
            generate_error(caller, MemoryException("Element can not be added due to memory limitation for this process"));
        ref = new Reference(&memorySpace, std::move(refName), memorySpace.get_space(), elcast);

    }
    table.new_element(caller, ref);
    elements.push_back(std::shared_ptr<MemoryElement>(ref));
    uint val = el->get_address();
    ref->set(static_cast<char*>(static_cast<void*>(&val)));
    memorySpace.set_space(ref->get_address()+ref->get_size());
    return Handler{caller, ref, &errors};
}

void lab3::MemoryManager::defragmentation(bool consoleLog) {
    uint space = 0;
    for(auto &el : elements)
    {
        if(el->get_address() > space){
            el->move(space);
            if(consoleLog) std::cout << "Moving " << el->get_name() << std::endl;
        }
        space = el->get_address() + el->get_size();
    }
    memorySpace.set_space(space);
}

lab3::SharedElement* lab3::MemoryManager::cast_to_shared(MemoryElement *from) {
    auto* to = dynamic_cast<PublicSegment*>(from);
    if(to)
        return dynamic_cast<SharedElement*>(from);
    else
        return nullptr;
}

void lab3::MemoryManager::add_program(std::string dir, uint maxMemory) {
    if(table.memory_max() + maxMemory < memorySpace.get_size())
        table.new_program(std::move(dir), maxMemory);
    else
        throw MemoryException("Device memory overflow. Try decreasing max memory for this process");
}

void lab3::MemoryManager::generate_error(uint id, const lab3::MemoryException &er) {
    errors.insert({id, er});
    throw er;
}

void lab3::MemoryManager::give_access(uint caller, const std::string &name, uint target) {
    if(!id_exists(caller))
        generate_error(caller, MemoryException("Caller does not exist"));
    if(!id_exists(target))
        generate_error(caller, MemoryException("Target does not exist"));
    MemoryElement *el = table.find_element_local(caller, name);
    if(el == nullptr)
        generate_error(caller, MemoryException("Element with this name does not exist in this process"));
    auto *elcast = cast_to_shared(el);
    if(elcast)
        elcast->give_access(target);
    else
        generate_error(caller, MemoryException("Element is not shareable"));
}

void lab3::MemoryManager::withdraw_access(uint caller, const std::string& name, uint target) {
    MemoryElement *el = table.find_element_local(caller, name);
    if(el == nullptr)
        generate_error(caller, MemoryException("Element with this name does not exist in this process"));
    auto *elcast = cast_to_shared(el);
    if(elcast)
        elcast->withdraw_access(target);
    else
        generate_error(caller, MemoryException("Element is not shareable"));
}

bool lab3::MemoryManager::check_access(uint caller, const std::string &name, uint target) {
    if(!id_exists(caller))
        generate_error(caller, MemoryException("Caller does not exist"));
    if(!id_exists(target))
        generate_error(caller, MemoryException("Target does not exist"));
    MemoryElement *el = table.find_element_local(caller, name);
    if(el == nullptr)
        generate_error(caller, MemoryException("Element with this name does not exist in this process"));
    auto *elcast = cast_to_shared(el);
    if(elcast)
        return elcast->check_access(target);
    else
        generate_error(caller, MemoryException("Element is not shareable"));
    return false;
}

bool lab3::MemoryManager::id_exists(uint id) {
    return table.get_table()->find(id) != table.get_table()->end();
}

void lab3::MemoryManager::kill_program(uint id) {
    if(!id_exists(id))
        generate_error(id, MemoryException("Caller does not exist"));
    for( auto it : table.get_table()->find(id)->second.get_elements()){
        delete_element(id, it->get_name());
    }
    table.remove_program(id);
}
