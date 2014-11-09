/**
 * @file Set.h
 *
 * Set
 * 
 * Estructura de Datos y Algoritmos
 * 
 * Copyright (C) 2014
 * Facultad de Inform�tica, Universidad Complutense de Madrid
 * This software is licensed under the Simplified BSD licence:
 *    (see the LICENSE file or
 *     visit opensource.org/licenses/BSD-3-Clause)
 */

#ifndef __SET_H
#define __SET_H

#include "HashTable.h"
#include "TreeMap.h"

struct EmptyClass {};
/// std::ostream output
std::ostream& operator<<(std::ostream& out, const EmptyClass &e) {
    return out;
}
    
/**
 * Sets allow quick insertion, lookup and removal of elements.
 * Duplicate insertions are ignored.
 * 
 * Built on top of an associative container.
 * 
 * @author mfreire
 */
template <class KeyType, template<typename,typename> class Container>
class BaseSet {
    
    /**
     * Internal associative container.
     * Must support iteration (begin, end, find), lookup by key
     * (find and contains, but does not need 'at'), removal by key (remove), 
     * insertion by key(insert), and size
     */
    Container<KeyType,EmptyClass> _m;

public:

    /** */
    typedef typename Container<KeyType,EmptyClass>::Iterator Iterator;    
    
    /**  */
    Iterator begin() const {
        return _m.begin();
    }
    
    /**  */
    Iterator end() const {
        return _m.end();
    }
    
    /**  */
    bool contains(const KeyType& key) const {
        return _m.find(key) != _m.end();
    }
    
    /**  */
    void insert(const KeyType& key) {
        _m.insert(key, EmptyClass());
    }

    /**  */
    void erase(const KeyType& key) {
        _m.erase(key);
    }

    /**  */    
    std::size_t size() const {
        return _m.size();
    }
};

/**
 * Pre-built sets using a HashTable and a TreeMap as backup containers
 */
template <class KeyType>
struct Set {
    /// Set::H is a HashTable-backed set, and is not ordered
    typedef BaseSet<KeyType, HashTable> H;
    /// Set::M is a TreeMap-backed set, and is always ordered
    typedef BaseSet<KeyType, TreeMap> T;    
};

#if __cplusplus >= 201103L //C++11

template<typename K>
using ordered_set = typename Set<K>::T;

template<typename K>
using unordered_set = typename Set<K>::H;


#endif //C++11

#endif // __SET_H
