#ifndef LAB3_HASHTABLE_H
#define LAB3_HASHTABLE_H

#include <cstdlib>
#include <utility>
#include <functional>
#include <memory>
#include <limits>
#include <iostream>
#include <stack>

namespace lab3{
    template<typename T>
    struct Node{
        std::shared_ptr<T> val;
        Node<T>* next;
        Node() : val(std::shared_ptr<T>(nullptr)), next(nullptr) {};
        explicit Node(T v) : next(nullptr) {
            T *value = new T(std::move(v));
            val = std::shared_ptr<T>(value);
        }
    };

    template<typename T, bool is_const>
    class TableIterator{
        typedef Node<T> node_t;
        typedef std::conditional_t<
                is_const, const node_t, node_t>
                *node_ptr_t;
        node_ptr_t node;
        //friend HashTable<typename T::first_type, typename T::second_type>;
        friend TableIterator<T, !is_const>;
        typedef ptrdiff_t difference_type;
        typedef T value_type;
        typedef std::conditional_t<
                is_const, const value_type , value_type>* pointer;
        typedef std::conditional_t<
                is_const, const value_type , value_type>& reference;
        typedef std::forward_iterator_tag iterator_category;
    public:
        explicit TableIterator(node_ptr_t node): node(node) {}
        template<bool other_const>
        explicit TableIterator(TableIterator<T, other_const>& o) noexcept
        requires (is_const >= other_const)
                : node(o.node) {}
        template<bool other_const>
        TableIterator& operator = (const TableIterator<T, other_const>& o) noexcept
        requires (is_const >= other_const) {
            node = o.node;
            return *this;
        }
        TableIterator& operator ++ () noexcept {
            node = node->next;
            while(node){
                if(node->val) break;
                node = node->next;
            }
            return *this;
        }
        TableIterator operator ++ (int) noexcept {
            node = node->next;
            while(node){
                if(node->val) break;
                node = node->next;
            }
            TableIterator res(node);
            return res;
        }
        reference operator * () noexcept {
            return *(node->val.get());
        }
        pointer operator -> () noexcept {
            return node->val.get();
        }
        TableIterator() noexcept : node(nullptr) {}
        template<bool other_const>
        bool operator == (const TableIterator<T, other_const>& o) const noexcept {
            return node == o.node;
        }
    };

    template<typename T>
    struct List{
        Node<T>* head;
        Node<T>* tail;
        int size;
        explicit List() : size(0) {
            tail = new Node<T>();
            head = new Node<T>();
            head->next = tail;
        }
        void insert(Node<T> *newn) {
            newn->next = head->next;
            head->next = newn;
            size++;
        }
        void clear(){
            Node<T> *prev;
            if(!head) return;
            while(head != tail)
            {
                prev = head;
                head = head->next;
                delete prev;
            }
            delete tail;
            head = nullptr;
            tail = nullptr;
        }
    };

    template<typename Key, typename Data>
    class HashTable{
    private:
        typedef std::pair<Key, Data> value_type;
        typedef std::pair<Key, Data>& reference;
        typedef const std::pair<Key, Data>& const_reference;
        typedef TableIterator<value_type, false> iterator;
        typedef TableIterator<value_type, true> const_iterator;
        typedef ptrdiff_t difference_type;
        typedef size_t size_type;
        typedef Node<value_type> node_t;
        typedef List<value_type> list_t;

        size_type vector_size;
        list_t *vector;
        float mlf;
        size_type overall_size;
        static size_type hash(Key k){
            return std::hash<Key>{}(k);
        }
        void _insert(node_t *n, bool reh=true){
            size_type hk = hash(n->val->first) % vector_size;
            vector[hk].insert(n);
            overall_size++;
            if(reh)
                auto_rehash();
        }
        node_t* _find(const Key& k) const{
            if(empty()) return nullptr;
            size_type hk = hash(k) % vector_size;
            node_t *p = vector[hk].head->next;
            while(p != vector[hk].tail){
                if(p->val->first == k) return p;
                p = p->next;
            }
            return nullptr;
        }
        void reform(size_type newSize){
            std::stack<node_t*> nodes;
            node_t *p = nullptr;
            for(size_type i = 0; i < vector_size; i++){
                if(!vector[i].size){
                    continue;
                }
                p = vector[i].head->next;
                while(p != vector[i].tail){
                    nodes.push(p);
                    p = p->next;
                }
            }
            for(int i = 0; i < vector_size; i++)
            {
                delete vector[i].head;
                delete vector[i].tail;
            }
            delete[] vector;
            vector_size = newSize;
            vector = new list_t[vector_size]();
            for(int i = 0; i < vector_size - 1; i++)
            {
                vector[i].tail->next = vector[i+1].head;
            }
            while(!nodes.empty())
            {
                _insert(nodes.top(), false);
                nodes.pop();
            }
        }
        void auto_rehash()
        {
            //std::cout << load_factor() << std::endl;
            if(load_factor() > mlf)
            {
                reform(vector_size+1);
            }
        }
        void copy(const HashTable& other){
            clear();
            mlf = other.mlf;
            for(auto const&it : other){
                insert(std::make_pair(it.first,it.second));
            }
        }
    public:
        ///
        ///Clear all heap memory associated with table. Set all parameters to 0
        void clear(){
            for(size_type i = 0; i < vector_size; i++)
                vector[i].clear();
            delete[] vector;
            vector = nullptr;
            vector_size = 0;
            overall_size = 0;
        }
        ///
        ///Erase all elements
        void soft_clear(){
            *this = HashTable(vector_size, mlf);
        }
        ///
        /// \param size Size
        /// \param maxLoadFactor Maximum average number of elements per bucket
        explicit HashTable(size_type size=1, float maxLoadFactor=2) : vector_size(size), mlf(std::abs(maxLoadFactor)), overall_size(0) {
            if(!size)
            {
                vector_size = 1;
            }
            vector = new list_t[vector_size]();
            for(size_type i = 0; i < vector_size - 1; i++){
                vector[i].tail->next = vector[i+1].head;
            }
        };
        ///
        /// Copy constructor
        HashTable(HashTable const &other){
            copy(other);
        }
        ///
        /// Move constructor
        HashTable(HashTable&& other) noexcept {
            clear();
            vector = other.vector;
            vector_size = other.vector_size;
            mlf = other.mlf;
            overall_size = other.overall_size;
            other.clear();
        }
        ///
        /// Move assignment operator
        HashTable<Key, Data>& operator=(HashTable<Key, Data> &&other) noexcept {
            if(this == &other) return *this;
            clear();
            swap(other);
            return *this;
        }
        ///
        /// Copy assignment operator
        HashTable<Key, Data>& operator=(const HashTable<Key, Data>& other) {
            if(this == &other) return *this;
            clear();
            copy(other);
            return *this;
        }
        ///
        /// Comparing operator
        bool operator==(const HashTable<Key, Data>& other) {
            if(size() != other.size())
                return false;
            for(auto const& it : other){
                if(find(it.first) == cend())
                    return false;
            }
            return true;
        }
        ~HashTable(){
            clear();
        }
        /// Inserts elements
        /// \param p std::pair<Key,Data>
        /// \return std::pair consisting of an iterator to the inserted element (or to the element that prevented the insertion) and a bool denoting whether the insertion took place (true if insertion happened, false if it did not).
        std::pair<iterator, bool> insert(value_type p){
            auto search = find(p.first);
            if(search != end()) return std::make_pair(search, false);
            auto newnode = new Node(std::move(p));
            _insert(newnode);
            return std::make_pair(iterator(newnode), true);
        }
        /// Erases elements
        /// \param k key of element to erase
        /// \return Number of elements removed (0 or 1).
        size_type erase(const Key& k) {
            size_type hk = hash(k) % vector_size;
            node_t *p = vector[hk].head;
            node_t *prev = p;
            p = p->next;
            while(p != vector[hk].tail){
                if(p->val->first == k){
                    prev->next = p->next;
                    vector[hk].size--;
                    overall_size--;
                    delete p;
                    return 1;
                }
                prev = p;
                p = p->next;
            }
            return 0;
        }
        /// Finds element with specific key
        /// \param k key
        /// \return Iterator to an element with key equivalent to key. If no such element is found, past-the-end (see end()) iterator is returned.
        const_iterator find(const Key& k) const {
            node_t *res = _find(k);
            return res ? const_iterator(res) : end();
        }
        /// Finds element with specific key
        /// \param k key
        /// \return Iterator to an element with key equivalent to key. If no such element is found, past-the-end (see end()) iterator is returned.
        iterator find(const Key& k) {
            node_t *res = _find(k);
            return res ? iterator(res) : end();
        }
        ///
        /// \return Overall amount of elements.
        [[nodiscard]] size_type size() const noexcept
        {
            return overall_size;
        }
        ///
        /// \return Max amount of elements.
        [[nodiscard]] size_type max_size() const noexcept
        {
            return std::numeric_limits<size_type>::max();
        }
        ///
        /// \return True if container is empty. False if has elements.
        [[nodiscard]] bool empty() const noexcept
        {
            return size() == 0;
        }
        ///
        /// Swaps with other table.
        /// \param o  other table
        void swap(HashTable& o) noexcept{
            std::swap(mlf, o.mlf);
            std::swap(overall_size, o.overall_size);
            std::swap(vector_size, o.vector_size);
            std::swap(vector, o.vector);
        }
        ///
        /// \return An iterator to the beginning.
        iterator begin() noexcept {
            node_t *p = nullptr;
            if(empty()) return end();
            for(int i = 0; i < vector_size; i++){
                if(vector[i].size){
                    p = vector[i].head->next;
                    break;
                }
            }
            return iterator(p);
        }
        ///
        /// \return An iterator to the end
        iterator end() noexcept {
            //if(empty()) return iterator(begin());
            //return iterator(vector[vector_size-1].tail ? vector[vector_size-1].tail->next : vector[vector_size-1].tail);
            return iterator(nullptr);
        }
        ///
        /// \return A constant iterator to the beginning.
        const_iterator begin() const noexcept {
            node_t *p;
            if(empty()) return end();
            for(int i = 0; i < vector_size; i++){
                if(vector[i].size){
                    p = vector[i].head->next;
                    break;
                }
            }
            return const_iterator(p);
        }
        ///
        /// \return A constant iterator to the end.
        const_iterator end() const noexcept {
            //if(empty()) return const_iterator(begin());
            //return const_iterator(vector[vector_size-1].tail ? vector[vector_size-1].tail->next : vector[vector_size-1].tail);
            return const_iterator(nullptr);
        }
        ///
        /// \return A constant iterator to the beginning.
        const_iterator cbegin() const noexcept {
            return begin();
        }
        ///
        /// \return A constant iterator to the end.
        const_iterator cend() const noexcept {
            return end();
        }
        /// Constructs element in-place.
        /// \tparam Args arguments types
        /// \param args arguments
        /// \return std::pair consisting of an iterator to the inserted element (or to the element that prevented the insertion) and a bool denoting whether the insertion took place (true if insertion happened, false if it did not).
        template<typename ...Args>
        std::pair<iterator, bool> emplace(Args && ... args)
        requires std::constructible_from<value_type, Args...>
        {
            auto v = value_type(std::forward<Args>(args)...);
            auto search = find(v.first);
            if(search != end()) return std::make_pair(search, false);
            auto node = new node_t(std::move(v));
            _insert(node);
            return std::make_pair(iterator(node), true);
        }
        ///
        /// \return Amount of buckets.
        [[nodiscard]] size_type bucket_count() const {
            return vector_size;
        }
        ///
        /// \param n number of bucket
        /// \return Amount of elements in the bucket of index n.
        /// \throw std::out_of_range the bucket of index n does not exist
        [[nodiscard]] size_type bucket_size(size_type n) const {
            if(n>=vector_size) throw std::out_of_range("Out of range");
            return vector[n].size;
        }
        ///
        /// \return Current load factor (Overall size divided by amount of buckets).
        float load_factor()
        {
            return static_cast<float>(overall_size) / static_cast<float>(vector_size);
        }
        ///
        /// \return Max load factor.
        float max_load_factor()
        {
            return mlf;
        }
        /// Sets max load factor.
        /// \param n new value
        void max_load_factor(float n)
        {
            if(n<=0) return;
            mlf = n;
        }
        /// Reserves at least the specified number of buckets and regenerates the hash table.
        /// \param count lower bound for the new number of buckets
        void rehash(size_type count){
            size_type n = size()/mlf;
            n = count > n ? count : n;
            reform(n);
        }
        /*
        void print(){
            for(size_type i = 0; i < vector_size; i++)
            {
                std::cout << i << "(" << vector[i].size << ")"  << ": ";
                auto p = vector[i].head->next;
                while(p != vector[i].tail){
                    std::cout << p->val->first << " - " << p->val->second << " ";
                    p = p->next;
                }
                std::cout << std::endl;
            }
        }*/
    };
}

#endif //LAB3_HASHTABLE_H
