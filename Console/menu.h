#ifndef LAB3_MENU_H
#define LAB3_MENU_H

#include <limits>
#include <iostream>
#include <string>
#include <cstring>

namespace lab3 {
    /*template<class T>
    T getNum(T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max()) {
        T a;
        while(true) {
            std::cin >> a;
            if(std::cin.eof()) // обнаружен конец файла
                throw std::runtime_error("Failed to read number: EOF detected");
            else if(std::cin.bad()) // обнаружена невосстановимая ошибка входного потока
                throw std::runtime_error(std::string("Failed to read number: ") + strerror(errno));
            else if(std::cin.fail()) { // прочие ошибки (неправильный формат ввода)
                std::cin.clear(); // очищаем флаги состояния потока
                // игнорируем все символы до конца строки
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Failed to read number: Incorrect input" << std::endl;
            }
            else if(a >= min && a <= max)
                return a;
            else
                std::cout << "Number is out of range" << std::endl;
        }
    }*/
    uint getNum(uint min = std::numeric_limits<uint>::min(), uint max = std::numeric_limits<uint>::max()) {
        uint a;
        std::string b;
        while(true) {
            std::cin >> std::ws;
            std::getline(std::cin, b);
            if (std::cin.eof()) // обнаружен конец файла
                throw std::runtime_error("Failed to read: EOF detected");
            if (std::cin.bad()) // обнаружена невосстановимая ошибка входного потока
                throw std::runtime_error(std::string("Failed to read: ") + strerror(errno));
            try{
                a = std::stoul(b);
            }catch(...){
                std::cerr << "Incorrect input" << std::endl;
                continue;
            }
            if (a >= min && a <= max)
                return a;
            else
                std::cerr << "Number is out of range" << std::endl;
        }
    }
    std::string getString(){
        std::string a;
        std::cin >> std::ws;
        std::getline(std::cin, a, '\n');
        if(std::cin.eof()) // обнаружен конец файла
            throw std::runtime_error("Failed to read: EOF detected");
        if(std::cin.bad()) // обнаружена невосстановимая ошибка входного потока
            throw std::runtime_error(std::string("Failed to read: ") + strerror(errno));
        return a;
    }

    struct menu{
        static uint base_menu(){
            std::cout << "Choose a command:\n";
            std::cout << "0. Exit\n";
            std::cout << "1. Add process\n";
            std::cout << "2. Kill process\n";
            std::cout << "3. Choose process\n";
            std::cout << "4. Print errors\n";
            std::cout << "5. Defragmentation\n";
            return getNum(0,5);
        }
        static uint custom_read_number(const std::string& msg){
            std::cout << msg;
            return getNum();
        }
        static uint choose_command_process() {
            std::cout << "Choose a command:\n";
            std::cout << "0. Back\n";
            std::cout << "1. Add element\n";
            std::cout << "2. Delete element\n";
            std::cout << "3. Use element\n";
            std::cout << "4. Give access\n";
            std::cout << "5. Withdraw_access\n";
            return getNum(0,5);
        }
        static std::string enter_element_name(){
            std::cout << "Enter element name:\n";
            return getString();
        }
        static uint choose_command_element() {
            std::cout << "Choose a command:\n";
            std::cout << "0. Back\n";
            std::cout << "1. Set\n";
            std::cout << "2. Set by index\n";
            std::cout << "3. Print stored data(Use only when data is set)\n";
            return getNum(0,3);
        }
        static std::string custom_read_string(const std::string& msg) {
            std::cout << msg;
            return getString();
        }
        static uint choose_type() {
            std::cout << "Choose type:\n";
            std::cout << "1. Variable\n";
            std::cout << "2. Array\n";
            std::cout << "3. Shared array\n";
            std::cout << "4. Reference\n";
            return getNum(1,4);
        }
        static auto safetry(auto&& func){
            try {
                func();
            }
            catch(MemoryException &e) {
                std::cerr << e.what() << std::endl;
            }
        }
    };
}
#endif //LAB3_MENU_H
