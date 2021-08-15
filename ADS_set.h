#ifndef ADS_SET_H
#define ADS_SET_H

#include <cstddef>
#include <functional>
#include <algorithm>
#include <iostream>
#include <stdexcept>


template <typename Key, size_t N = 7>
class ADS_set {
public:
    class Iterator;
    using value_type = Key;
    using key_type = Key;
    using reference = key_type&;
    using const_reference = const key_type&;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    using iterator = Iterator;
    using const_iterator = Iterator;
    using key_equal = std::equal_to<key_type>;
    using hasher = std::hash<key_type>;
private:
    struct Node;
    size_type sz;
    size_type max_sz;
    Node *array_ll;

    // insert a given value (Key)
    void insert_hash(const key_type &value){
        Node* new_elem = new Node;

        auto k = hasher{}(value) % max_sz;
        new_elem->key = value;
        new_elem->next = nullptr;

        new_elem->next = array_ll[k].head;
        array_ll[k].head = new_elem;

        ++sz;
    }

    // resize the table with all its values
    void resize() {
        size_t new_sz = 2*max_sz;
        Node* new_array_ll {new Node[new_sz]};

        size_t ind = 0;
        value_type* help_cont = new value_type[max_sz];
        
        for(size_t i{0}; i < max_sz; i++){
            Node *elem = array_ll[i].head;

            while(elem != nullptr){
                help_cont[ind++] = elem->key;
                elem = elem->next;
            }
            delete elem;
        }
        
        delete[] array_ll;
        array_ll = new_array_ll;
        max_sz = new_sz;
        sz = 0;

        for (size_t i = 0; i < ind; i++) {
            insert_hash(help_cont[i]);
        }
        delete[] help_cont;
    }

    // check if the table should be resized
   void check_load_factor(){
       size_t load_factor = 100*(sz/max_sz);
       if(load_factor > 70){
           resize();
       }
   }
    
    // delete a given value (Key)
       void delete_hash(const key_type &key){
           auto k = hasher{}(key) % max_sz;
           Node *current = array_ll[k].head;
           Node *previous = nullptr;


           while(current != nullptr && !key_equal{}(current->key,key)){
               previous = current;
               current = current->next;
           }

           if(current == nullptr){
               return;
           }
           sz--;
           if(previous != nullptr){
               previous->next = current->next;
               current->next = nullptr;
               delete current;
           }
           else{
               array_ll[k].head = current->next;
               current->next = nullptr;
               delete current;
           }
        }

public:
    ADS_set(): sz{0}, max_sz{N}{
        array_ll = new Node[max_sz];
        for(size_t i{0}; i < max_sz; i++){
            array_ll[i].head = nullptr;
        }
    }

    ADS_set(std::initializer_list<key_type> ilist): ADS_set{std::begin(ilist),std::end(ilist)} {}

    template<typename InputIt> ADS_set(InputIt first, InputIt last): ADS_set{} {
        insert(first, last);
    }

    size_type size() const{
        return this->sz;
    }

    bool empty() const{
        return !sz;
    }

    void insert(std::initializer_list<key_type> ilist) {
        insert(std::begin(ilist),std::end(ilist));
    }

    template<typename InputIt>
    void insert(InputIt first, InputIt last){
        for(auto it=first; it != last; it++){
            if(count(*it) == 1) {}
            else{
                insert_hash(*it);
                check_load_factor();
            }
        }
    }

    size_type count(const key_type &value) const {
        auto key = hasher{}(value) % max_sz;

        Node *help = array_ll[key].head;
        while(help != nullptr){
            if(key_equal{}(help->key,value)){
                return 1;
            }
            help=help->next;
        }
        delete help;
        return 0;
    }

    void dump(std::ostream &o = std::cerr) const{
        o << "Separate Chaining: " << typeid(Key).name() << ',' << N << "\n";
        o << "Size of the Hash Table: " << sz << "\n";
        for(size_t i{0}; i < max_sz; i++){
            Node *temp = array_ll[i].head;

            o << "Position[" << i << "] -->";
            if(temp == nullptr){o << "NULL" << "\n";}
            else{
                while(temp){
                    o << "[" << temp->key << "] -->";
                    temp = temp->next;
                }
                o << "NULL" << "\n";
            }
            delete temp;
        }
    }
    // -----------------------------------------   2. Phaseeee  ----------------------------------------------
        // -------------------------------------------------------------------------------------------------------


        ADS_set(const ADS_set& other): ADS_set{other.begin(), other.end()} {}

        ~ADS_set(){
            delete[] array_ll;
        }

        ADS_set &operator=(const ADS_set &other) {
            if(this == &other) return *this;

            ADS_set new_set{other};
            swap(new_set);
            return *this;
        }

        ADS_set &operator=(std::initializer_list<key_type> ilist) {
            ADS_set new_set{ilist};
            swap(new_set);
            return *this;
        }

        void clear(){
            ADS_set empty_set;
            swap(empty_set);
        }

        size_type erase(const key_type &key){
           if(count(key) == 0){
               return 0;
           }
           delete_hash(key);
           return 1;
        }

        void swap(ADS_set &other){
            using std::swap;
            swap(array_ll, other.array_ll);
            swap(max_sz, other.max_sz);
            swap(sz, other.sz);
        }

        std::pair<iterator,bool> insert(const key_type &key){
            if(count(key) == 1){
                return {find(key),false};
            }
            insert_hash(key);
            return {find(key), true};
        }

       iterator find(const key_type &key) const{
           if(count(key) == 0){return end();}

           auto k = hasher{}(key) % max_sz;
           Node *help = array_ll[k].head;
           if(key_equal{}(help->key, key)){
               return iterator{max_sz, array_ll, help, k};
           }
           while(!key_equal{}(help->key, key) && help != nullptr){
              help = help->next;
           }
           return iterator{max_sz, array_ll, help, k};
       }

       const_iterator begin() const{
        if(this->empty()){return end();}
        return iterator{max_sz, array_ll};
        }

        const_iterator end() const{
          return const_iterator{};
        }

        friend bool operator==(const ADS_set &lhs, const ADS_set &rhs){
            if(lhs.sz != rhs.sz){return false;}

            for(const auto &value: rhs){
                if(lhs.count(value) == 0){
                   return false;
                }
            }
            return true;
        }

        friend bool operator!=(const ADS_set &lhs, const ADS_set &rhs){
            return !(lhs == rhs);
        }
};

template <typename Key, size_t N>
struct ADS_set<Key,N>::Node{
    Key key;
    Node *head {nullptr};
    Node *next {nullptr};
    ~Node() {
       delete head;
       delete next;
    }
};

template <typename Key, size_t N>
class ADS_set<Key,N>::Iterator {
public:
   using value_type = Key;
   using difference_type = std::ptrdiff_t;
   using reference = const value_type &;
   using pointer = const value_type *;
   using iterator_category = std::forward_iterator_tag;
private:
    // max size of the table
    size_t maxy_sz;
    // an array with linked lists
    Node *table;
    // help pointer to traverse the table with
    Node *ptr;
    // the index/position of the current element/pointer
    size_t index;

    void skip(){
        for (size_t i = 0; i < maxy_sz; i++){
            if (table[i].head != nullptr){
                ptr = table[i].head;
                index = i;
                break;
            }
        }
    }
public:
  explicit Iterator(size_t maxy_sz, Node *table, Node *ptr, size_t index): maxy_sz{maxy_sz}, table{table}, ptr{ptr}, index{index} {}

  explicit Iterator(size_t maxy_sz, Node *table): maxy_sz{maxy_sz}, table{table}{
      skip();
  }
  
  Iterator(): maxy_sz{N}, table{nullptr}, ptr{nullptr}, index{NULL} {}

  reference operator*() const {
      pointer p = &ptr->key;
      return *p;
  }
  pointer operator->() const {
      pointer p = &ptr->key;
      return p;
  }
  // check if theres a next element -> point to it
  // if not - traverse the indexes and point to the first one
  // loop until the index is valid
  Iterator& operator++() {
    if (ptr->next != nullptr){
        ptr = ptr->next;
        return *this;
    }
    else{
        while(index < maxy_sz){
            if (index+1 >= maxy_sz) {
                ptr = nullptr;
                return *this;
            }
            else{
                index += 1;

                if(table[index].head != nullptr) {
                    ptr = table[index].head;
                    return *this;
                }
            }
        }
    }
    return *this;
  }

  Iterator operator++(int) {
      Iterator tmp{*this};
      ++*this;
      return tmp;
  }
    
  friend bool operator==(const Iterator &lhs, const Iterator &rhs){
      return (lhs.ptr == rhs.ptr);
  }
  friend bool operator!=(const Iterator &lhs, const Iterator &rhs) {
      return !(lhs == rhs);
  }
};

template <typename Key, size_t N>
void swap(ADS_set<Key,N> &lhs, ADS_set<Key,N> &rhs) {
    lhs.swap(rhs);
}


#endif
