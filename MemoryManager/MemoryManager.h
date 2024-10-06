#ifndef LAB3_MEMORYMANAGER_H
#define LAB3_MEMORYMANAGER_H

#include <utility>
#include <stack>
#include <functional>
#include <optional>
#include <memory>

#include "MemorySpace.h"
#include "MemoryTable.h"
#include "MemoryElements.h"

namespace lab3 {
    class Handler{
    private:
        MemoryElement *el;
        std::multimap<uint, MemoryException> *errors;
        uint id;
        auto my_try(auto&& func);
    public:
        Handler(uint i, MemoryElement *e, std::multimap<uint, MemoryException> *err) : el(e), errors(err), id(i) {}
        [[nodiscard]] const MemoryElement* get_pointer() const {return el;};
        [[nodiscard]] std::string get_name();
        [[nodiscard]] uint get_size();
        [[nodiscard]] uint get_address();
        [[nodiscard]] std::string get();
        [[nodiscard]] std::string get(uint index);
        [[nodiscard]] std::string get_multiple(uint start, uint end);
        void set(std::string data);
        void set(uint index, std::string data);
        void set(char *data);
        void set(uint index, char *data);
        [[nodiscard]] bool shared() const;
    };
    class MemoryManager {
    private:
        MemorySpace memorySpace;
        MemoryTable table;
        std::vector<std::shared_ptr<MemoryElement>> elements;
        std::multimap<uint, MemoryException> errors;
        uint last_id;
        //friend class ManagerViewer;

        static SharedElement *cast_to_shared(MemoryElement *from);
        void generate_error(uint id, const MemoryException& er);
        bool id_exists(uint id);
    public:
        explicit MemoryManager(uint memorySize): memorySpace(MemorySpace(memorySize)), table(MemoryTable()), elements(std::vector<std::shared_ptr<MemoryElement>>()), last_id(0) { }
        void add_program(std::string dir, uint maxMemory);
        void kill_program(uint id);
        void add_element(uint caller, std::string name, uint size, uint sizeOfElement=0, bool isPublic=false);
        void delete_element(uint caller, const std::string& name);
        Handler use_element(uint caller, const std::string& name);
        Handler get_reference(uint caller, std::string refName, const std::string& name);
        std::multimap<uint, MemoryException> get_errors() {return errors;}
        std::stack<MemoryException> get_errors(uint id);
        void defragmentation(bool consoleLog=false);
        template<typename T>
        static char *make_bytes(T* val) { return static_cast<char*>(static_cast<void*>(val)); }
        [[nodiscard]] const MemoryTable* get_memory_table() const{
            return &table;
        }
        void give_access(uint caller, const std::string& name, uint target);
        void withdraw_access(uint caller, const std::string& name, uint target);
        bool check_access(uint caller, const std::string& name, uint target);
        uint memory_usage(uint caller, bool multithreading = false){
            return table.memory_usage(caller, multithreading);
        }
    };
}

#endif //LAB3_MEMORYMANAGER_H
