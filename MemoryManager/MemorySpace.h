#ifndef LAB3_MEMORYSPACE_H
#define LAB3_MEMORYSPACE_H

#include <utility>
#include <stdexcept>


namespace lab3 {
    class MemorySpace {
    private:
        uint size;
        char* memory;
        uint space;
    public:
        explicit MemorySpace(uint size)
        {
            this->size = size;
            memory = new char[size];
            space = 0;
        }
        ~MemorySpace()
        {
            delete[] memory;
        }
        [[nodiscard]] char *read(uint start, uint datasize) const
        {
            if(datasize == 0) return nullptr;
            auto data = new char[datasize];
            for(int i = 0; i < datasize; i++)
            {
                data[i] = memory[i+start];
            }
            return data;
        }
        void write(uint start, uint datasize, const char*data)
        {
            for(int i = 0; i < datasize; i++)
            {
                memory[i + start] = data[i];
            }
        }
        [[nodiscard]] uint get_space() const
        {
            return space;
        }
        void set_space(uint newplace)
        {
            space = newplace;
        }
        [[nodiscard]] uint get_size() const
        {
            return size;
        }
    };
    class MemoryException: public std::runtime_error
    {
    public:
        explicit MemoryException(const std::string &message) : runtime_error(message) {}
    };
}

#endif //LAB3_MEMORYSPACE_H
