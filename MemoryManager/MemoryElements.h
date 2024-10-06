#ifndef LAB3_MEMORYELEMENTS_H
#define LAB3_MEMORYELEMENTS_H

#include <list>
#include <utility>
#include <string>
#include <vector>
#include "MemorySpace.h"

namespace lab3 {
    typedef unsigned int uint;

    class MemoryElement {
    private:
        std::string name;
        uint size;
        uint address;
        MemorySpace *mspace;
    public:
        ///
        /// \param memorySpace pointer to MemorySpace
        /// \param n name
        /// \param s size
        /// \param a address
        MemoryElement(MemorySpace *memorySpace, std::string  n, uint const &s, uint const &a) : mspace(memorySpace), name(std::move(n)), size(s), address(a) {}
        virtual ~MemoryElement() = default;
        ///
        /// \return Name.
        [[nodiscard]] std::string get_name() const {return name;}
        ///
        /// \return Size.
        [[nodiscard]] uint get_size() const {return size;}
        ///
        /// \return Address.
        [[nodiscard]] uint get_address() const {return address;}
        ///
        /// \return Pointer to MemorySpace.
        [[nodiscard]] MemorySpace *get_space() const {return mspace;}
        ///
        /// \return Stored data.
        [[nodiscard]] virtual char* get() const { return mspace->read(address, size); }
        ///
        /// \param index index
        /// \return Stored data by index.
        /// \throws MemoryException Index out of range
        [[nodiscard]] virtual char* get(uint index) const;
        ///
        /// \param start starting index
        /// \param end last index
        /// \return Stored data in [start, end) indexes.
        /// \throws MemoryException Index out of range
        [[nodiscard]] virtual char* get_multiple(uint start, uint end) const;
        ///
        /// \return Size of one element.
        [[nodiscard]] virtual uint get_element_size() const {return size;}
        /// Writes data to the element`s memory
        /// \param data data to write
        virtual void set(char*data) const { mspace->write(address, size, data); }
        /// Writes data to the element`s memory by index
        /// \param data data to write
        /// \throws MemoryException Index out of range
        virtual void set(uint index, char*data) const;
        /// Moves element to a new address with it`s data.
        /// \param to new address
        void move(uint to);
    };
    class Reference : public MemoryElement {
    private:
        MemoryElement *element;
    public:
        ///
        /// \param memorySpace pointer to MemorySpace
        /// \param n name
        /// \param a address
        /// \param m pointer to referred memory element
        Reference(MemorySpace *memorySpace, std::string n, uint a, MemoryElement *m) : MemoryElement(memorySpace, std::move(n), sizeof(uint), a), element(m) {}
        ///
        /// \return Pointer to referred memory element.
        const MemoryElement *get_element(){return element;}
        ///
        /// \return Data stored in referred memory element.
        [[nodiscard]] char *get() const override { return element->get(); }
        ///
        /// \param index index
        /// \return Data stored in referred memory element by index.
        /// \throws MemoryException Index out of range
        [[nodiscard]] char *get(uint index) const override {return element->get(index);}
        ///
        /// \param start starting index
        /// \param end last index
        /// \return Stored data in [start, end) indexes of referred memory element.
        /// \throws MemoryException Index out of range
        [[nodiscard]] char* get_multiple(uint start, uint end) const override {return element->get_multiple(start, end);}
        ///
        /// \return Size of referred element
        [[nodiscard]] uint get_element_size() const override {return element->get_element_size();}
        /// Writes data to the referred element`s memory.
        /// \param data data
        void set(char*data) const override{ element->set(data); }
        /// Writes data to the referred element`s memory by index.
        /// \param index index
        /// \param data data
        void set(uint index, char*data) const override { element->set(index, data); }
    };
    class Variable : public MemoryElement {
    public:
        Variable(MemorySpace *memorySpace, std::string n, uint s, uint a) : MemoryElement(memorySpace, std::move(n), s, a) {}
    };
    class Array : public MemoryElement {
    private:
        uint elementSize;
    public:
        ///
        /// \param memorySpace pointer to MemorySpace
        /// \param n name
        /// \param s size
        /// \param a address
        /// \param e size of one element
        Array(MemorySpace* memorySpace, std::string n, uint s, uint a, uint e) : MemoryElement(memorySpace, std::move(n), s, a), elementSize(e) {}
        [[nodiscard]] char* get(uint index) const override;
        [[nodiscard]] char* get_multiple(uint start, uint end) const override;
        void set(uint index, char* data) const override;
        [[nodiscard]] uint get_element_size() const override {return elementSize;}
    };
    class PublicSegment {
    public:
        virtual ~PublicSegment() = default;
        /// Give access to another process.
        /// \param id id of other process
        virtual void give_access(uint id) = 0;
        /// Withdraw access to another process.
        /// \param id id of other process
        virtual void withdraw_access(uint id) = 0;
        /// Check if process can access this element
        /// \param id id of other process
        /// \return True if can, false if can not.
        [[nodiscard]] virtual bool check_access(uint id) const = 0;
    };
    class SharedElement : public Array, public PublicSegment {
    private:
        std::vector<uint> programs;
    public:
        /// \param memorySpace pointer to MemorySpace
        /// \param n name
        /// \param s size
        /// \param a address
        /// \param e size of one element
        SharedElement(MemorySpace *memorySpace, std::string n, uint s, uint a, uint e) : Array(memorySpace, std::move(n), s, a, e), programs({}) {}
        void give_access(uint id) override;
        void withdraw_access(uint id) override;
        [[nodiscard]] bool check_access(uint id) const override;
    };
}
#endif
