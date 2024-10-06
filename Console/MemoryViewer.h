#ifndef LAB3_MEMORYVIEWER_H
#define LAB3_MEMORYVIEWER_H

#include "../MemoryManager/MemoryManager.h"
namespace lab3{
    class ManagerViewer{
        MemoryManager & manager;
        std::unordered_map<uint, std::string> shared;
    public:
        explicit ManagerViewer(MemoryManager& m): manager(m) {};
        void new_shared(uint id, std::string name){
            shared.insert({id, std::move(name)});
        }
        void print_process_list() const{
            auto t = manager.get_memory_table()->get_table();
            std::cout << "Process List" << "("  << t->size() << "):" << std::endl;
            if(t->empty()) return;
            uint full_usage = manager.get_memory_table()->full_memory_usage();
            float usage_percent;
            uint i, j;
            for(const auto& it : *t){
                i = it.first;
                j = 0;
                while(i > 0){
                    j++;
                    i /= 10;
                }
                j = j ? j : 1;
                usage_percent = full_usage ? static_cast<float>(it.second.memory_usage()) / static_cast<float>(full_usage) : 0;
                std::cout << it.first;
                for(int k = 0; k < 5-j; k++){
                    std::cout << " ";
                }
                std::cout << " | ";
                i = static_cast<int>(usage_percent*100);
                std::cout << i;
                j = 0;
                while(i > 0){
                    j++;
                    i /= 10;
                }
                j = j ? j : 1;
                for(int k = 0; k < 3-j; k++){
                    std::cout << " ";
                }
                std::cout << " | " << it.second.get_directory() << "\n";

            }
        }
        bool print_process_info(uint id) const {
            if(manager.get_memory_table()->get_table()->find(id) == manager.get_memory_table()->get_table()->end())
                return false;
            auto container = manager.get_memory_table()->get_table()->find(id)->second;
            std::cout << "Process ID: " << manager.get_memory_table()->get_table()->find(id)->first << "\n";
            std::cout << "Directory: " << container.get_directory() << "\n";
            std::cout << "Memory usage: " << container.memory_usage() << " bytes\n";
            std::cout << "Elements:" << "\n";
            for(const auto& it : container.get_elements())
            {
                std::cout << it->get_name() << "\n";
            }
            std::cout << "Shared:\n";
            for(const auto& it : shared){
                if(manager.check_access(it.first, it.second, id)){
                    std::cout << it.first << " " << it.second << "\n";
                }
            }
            return true;
        }
        void print_element_info(uint id, const std::string& name) const {
            auto el = manager.use_element(id, name);
            std::cout << name << ":" << "\n";
            std::cout << "Address: " << el.get_address() << "\n";
            std::cout << "Size: " << el.get_size() << "\n";
            std::cout << "Shareable: " << (manager.use_element(id, name).shared() ? "Yes" : "No") << "\n";
        }
        void print_element_data(uint id, const std::string& name){
            auto el = manager.use_element(id, name);
            if(el.get_pointer()->get_element_size() == 0){
                std::cout << "Stored data: " << el.get() << "\n";
            }
            else{
                std::cout << "Stored data:\n";
                for(uint i = 0; i < el.get_pointer()->get_size()/el.get_pointer()->get_element_size(); i++){
                    std::cout << "[" << i << "]: " << el.get(i) << "\n";
                }
            }
        }
        void print_errors(){
            auto errs = manager.get_errors();
            for(const auto& it : errs){
                std::cout << it.first << " - " << it.second.what() << std::endl;
            }
        }
    };
}


#endif //LAB3_MEMORYVIEWER_H
