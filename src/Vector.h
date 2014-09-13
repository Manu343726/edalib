/**
 * @file Vector.h
 *
 * A dynamic vector container. Similar to std::vector
 *
 * Estructura de Datos y Algoritmos
 *
 * Copyright (C) 2014
 * Facultad de Informática, Universidad Complutense de Madrid
 * This software is licensed under the Simplified BSD licence:
 *    (see the LICENSE file or
 *    visit opensource.org/licenses/BSD-3-Clause)
 */

#ifndef __VECTOR_H
#define __VECTOR_H

// to access std::sort and std::random_shuffle
#include <algorithm>

#include "Util.h"
#include "iterator_adapters.hpp"

DECLARE_EXCEPTION(VectorInvalidIndex)

/**
 * A dynamic vector container. Fast random access,
 * but fast insertion/removal only at the back.
 * 
 * @author mfreire
 */
template <class Type>
class Vector {

    /// initial size to reserve for an empty vector
    static const std::size_t INITIAL_SIZE = 16;

    Type* _v;   ///< dynamically-reserved array of elements
    std::size_t _used; ///< number of slots used
    std::size_t _max;  ///< total number of slots in _v
   
public:
   
    /**  */
    Vector() : _used(0), _max(INITIAL_SIZE) {
        _v = new Type[_max];
    }
    
    /**  */
    Vector(const Vector& other) :
        _used(other._used), _max(other._max) {

        _v = new Type[other._max];
        for (std::size_t i=0; i<_used; i++) {
            _v[i] = other._v[i];
        }        
    }
    
    /**  */
    ~Vector() {
        delete[] _v;
        _v = 0;
    }
    
    /** */
    const Vector& operator=(const Vector& other) {
        delete _v;
        _max = other._max;
        _v = new Type[_max];
        _used = other.size();
        for (std::size_t i=0; i<other.size(); i++) {
            _v[i] = other._v[i];
        }
        return (*this);
    }    

    /**  */
    std::size_t size() const {
        return _used;
    }

    class Iterator : public util::edatocpp_iterator_adapter<Iterator,Type> {
    public:
        void next() {
            _pos ++;
        }
        
        const Type& elem() const {
            return _dv->at(_pos);
        }
        
        Type& elem() { NON_CONST_VARIANT(Type, Iterator, elem()); }

        bool operator==(const Iterator &other) const {
            return _pos == other._pos;
        }
        
        bool operator!=(const Iterator &other) const {
            return _pos != other._pos;
        }
    protected:
        friend class Vector;
        
        const Vector* _dv;
        
        std::size_t _pos;
        
        Iterator(const Vector *dv, std::size_t pos)
            : _dv(dv), _pos(pos) {}
    };    
    
    /** */
    const Iterator find(const Type& e) const {
        for (std::size_t i=0; i<_used; i++) {
            if (e == _v[i]) {
                return Iterator(this, i);
            }
        }
        return end();
    }
    
    /** */
    Iterator begin() const {
        return Iterator(this, 0);
    }
    
    /** */
    Iterator end() const {
        return Iterator(this, _used);
    }
    
    /** */
    void sort() {
        std::sort(_v + 0, _v + _used);
    }   
    
    /** */
    void shuffle() {
        std::random_shuffle(_v + 0, _v + _used);
    } 
    
    /** */
    const Type& at(std::size_t pos) const {        
        if (pos < 0 || pos >= _used) {
            throw VectorInvalidIndex("at");
        }
        return _v[pos];
    }

    /** */
    Type& at(std::size_t pos) {
        NON_CONST_VARIANT(Type,Vector,at(pos));
    }
    
    /** */
    void push_back(const Type& e) {
        if (_used == _max) {
            _grow();
        }
        _v[_used++] = e;
    }

    /** */
    const Type& back() const {
        return (const Type &)at(_used - 1);
    }

    /** */
    Type& back() {
        NON_CONST_VARIANT(Type,Vector,back());
    }

    /**  */
    void pop_back() {
        if (_used == 0) {
            throw VectorInvalidIndex("pop_back");
        }        
        _used --;
    }

    /**  */
    void push_front(const Type& e) {
        if (_used == _max) {
            _grow();
        }
        for (std::size_t i=_used; i>0; i--) {
            _v[i] = _v[i-1];
        }
        _v[0] = e;
        _used ++;
    }
    
    /**  */
    const Type& front() const {
        (const Type &)at(0);
    }
    
    /**  */
    Type& front() {
        NON_CONST_VARIANT(Type,Vector,front());
    }    

    /**  */
    void pop_front() {
        if (_used == 0) {
            throw VectorInvalidIndex("pop_front");
        }
        for (int i=0; i<=_used; i++) {
            _v[i] = _v[i+1];
        }
        _used --;
    }    
    
private:

    void _grow() {
        _max *= 2;
        Type *old = _v;
        _v = new Type[_max];
        for (std::size_t i=0; i<_used; i++) {
            _v[i] = old[i];
        }
        delete[] old;
        old = 0;
    }    
};

#endif // __VECTOR_H
