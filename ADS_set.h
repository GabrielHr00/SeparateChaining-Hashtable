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


#endif
