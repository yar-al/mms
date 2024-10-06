#include "MemoryViewer.h"
#include "menu.h"
using namespace lab3;

MemoryManager start_preset(){
    auto man = MemoryManager(20000);
    man.add_program("C/LOVE/KITTENS.exe", 100);
    man.add_program("C/games/ManHunt.exe", 300);
    man.add_element(0, "x", 5, 0, false);
    man.add_element(0, "y", 10, 2, true);
    man.give_access(0, "y", 1);
    man.use_element(0, "x").set(std::string("12345").data());
    man.use_element(0, "y").set(std::string("1111111111").data());
    man.use_element(0, "y").set(1, std::string("22").data());
    return man;
}

int main()
{
    uint memorysize = menu::custom_read_number("Enter size of memory space: ");
    auto manager = memorysize ? MemoryManager(memorysize) : start_preset();
    auto viewer = ManagerViewer(manager);
    if(!memorysize) viewer.new_shared(0, "y");

    uint opt, id, type, size, amount, elopt, max_mem, ind;
    std::string name, data, name2;
    while(true){
        viewer.print_process_list();
        opt = menu::base_menu();
        if(!opt) return 0;
        if(opt == 1){
            max_mem = menu::custom_read_number("Enter max memory: ");
            name = menu::custom_read_string("Enter directory: ");
            menu::safetry([&](){manager.add_program(name, max_mem);});
            viewer.print_process_list();
            continue;
        }else if(opt == 4){
            viewer.print_errors();
            continue;
        }else if(opt == 5){
            manager.defragmentation(true);
            continue;
        }
        id = menu::custom_read_number("Enter process ID: ");
        if(opt == 2){
            manager.kill_program(id);
            continue;
        }
        while(true){
            if(!viewer.print_process_info(id)){
                std::cerr << "Process with this id does not exist" << std::endl;
                break;
            }
            opt = menu::choose_command_process();
            if(!opt) break;
            name = menu::enter_element_name();
            switch (opt) {
                case 1:
                    type = menu::choose_type();
                    switch (type) {
                        case 1:
                            menu::safetry([&](){manager.add_element(id, name, menu::custom_read_number("Enter size: "), 0, false);});
                            break;
                        case 2:
                            size = menu::custom_read_number("Enter size of one element: ");
                            amount = menu::custom_read_number("Enter amount of elements: ");
                            menu::safetry([&](){manager.add_element(id, name, size*amount, size, false);});
                            break;
                        case 3:
                            size = menu::custom_read_number("Enter size of one element: ");
                            amount = menu::custom_read_number("Enter amount of elements: ");
                            menu::safetry([&](){manager.add_element(id, name, size*amount, size, true);});
                            viewer.new_shared(id, name);
                            break;
                        case 4:
                            name2 = menu::custom_read_string("Enter name of element to refer: ");
                            menu::safetry([&](){manager.get_reference(id, name, name2);});
                        default:
                            break;
                    }
                    break;
                case 2:
                    menu::safetry([&](){manager.delete_element(id, name);});
                    break;
                case 3:
                    viewer.print_element_info(id, name);
                    elopt = menu::choose_command_element();
                    switch (elopt) {
                        case 0:
                            break;
                        case 1:
                            menu::safetry([&](){manager.use_element(id, name).set(menu::custom_read_string("Enter data: "));});
                            break;
                        case 2:
                            ind = menu::custom_read_number("Enter index: ");
                            data = menu::custom_read_string("Enter data: ");
                            menu::safetry([&](){manager.use_element(id, name).set(ind, data);});
                            break;
                        case 3:
                            viewer.print_element_data(id, name);
                            break;
                        default:
                            break;
                    }
                    break;
                case 4:
                    menu::safetry([&](){manager.give_access(id, name, menu::custom_read_number("Enter target process: "));});
                    break;
                case 5:
                    menu::safetry([&](){manager.withdraw_access(id, name, menu::custom_read_number("Enter target process: "));});
                    break;
                default:
                    break;
            }
        }
    }
}