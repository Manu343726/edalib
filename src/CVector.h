/**
 * @file CVector.h
 *
 * A circular vector, also called a circular buffer.
 *
 * Estructura de Datos y Algoritmos
 *
 * Copyright (C) 2014
 * Facultad de Informática, Universidad Complutense de Madrid
 * This software is licensed under the Simplified BSD licence:
 *    (see the LICENSE file or
 *    visit opensource.org/licenses/BSD-3-Clause)
 */

#ifndef __CVECTOR_H
#define __CVECTOR_H

#include <iomanip>
#include <iterator>

#include "Util.h"

DECLARE_EXCEPTION(CVectorInvalidIndex)

/**
 * A circular vector, also called a circular buffer.
 * Random access, slightly slower than for a normal vector.
 * Efficient insertion and removal at both ends.
 * 
 * @author mfreire
 */
template <class Type>
class CVector : public util::container_traits<Type> {

    /// initial size to reserve for an empty vector
    static const std::size_t INITIAL_SIZE = 16;
    
    Type* _v;    ///< dynamically-reserved array of elements
    std::size_t _start; ///< index of first slot used
    std::size_t _end;   ///< index of first free slot after start
    std::size_t _used;  ///< number of slots used
    std::size_t _max;   ///< total number of slots in _v
    
public:
    
    /**  */
    CVector() : _start(0), _end(0), _used(0), _max(INITIAL_SIZE) {
        _v = new Type[_max];
    }
    
    /**  */
    CVector(const CVector& other) :
        _start(other._start), _end(other._end),
        _used(other._used), _max(other._max) {
            
        _v = new Type[other._max];
        for (std::size_t i=0; i<_max; i++) {
            _v[i] = other._v[i];
        }        
    }    
    
    /**  */
    ~CVector() {
        delete[] _v;
        _v = 0;
    }

    /** */
    CVector& operator=(const CVector& other) {
        delete _v;
        _max = other._max;
        _v = new Type[_max];
        _used = other.size();
        for (std::size_t i=other._start, j=0; i!=other._end; i=other._inc(i)) {
            _v[j++] = other._v[i];
        }
        _start = 0;
        _end = _used;
        return (*this);
    }    
    
    /**  */
    std::size_t size() const {
        return _used;
    }
    
    class Iterator : public std::iterator<std::forward_iterator_tag,Type> {
    public:
        void next() {
            _pos = _cv->_inc(_pos);
        }
        
        Iterator& operator++()
        {
            next();
            return *this;
        }
        
        Iterator operator++(int)
        {
            Iterator cpy{ *this };
            ++(*this);
            return cpy;
        }
        
        
        
        const Type& elem() const {
            return _cv->_v[_pos];
        }
        
        Type& elem() { NON_CONST_VARIANT(Type, Iterator, elem()); }
        
        const Type& operator*() const
        {
            return elem();
        }
        
        Type& operator*()
        {
            return elem();
        }

        bool operator==(const Iterator &other) const {
            return _pos == other._pos;
        }
        
        bool operator!=(const Iterator &other) const {
            return _pos != other._pos;
        }
    protected:
        friend class CVector;
        
        const CVector* _cv;
        
        std::size_t _pos;
        
        Iterator(const CVector *cv, std::size_t pos)
            : _cv(cv), _pos(pos) {}
    };    
    
    /** */
    const Iterator find(const Type& e) const {
        for (std::size_t i=_start, j=0; i!=_end; i=_inc(i)) {
            if (e == _v[i]) {
                return Iterator(this, i);
            }
        }
        return end();
    }
    
    /** */
    Iterator begin() const {
        return Iterator(this, _start);
    }
    
    /** */
    Iterator end() const {
        return Iterator(this, _end);
    }
    
    /** */
    const Type& at(std::size_t pos) const {        
        if (pos < 0 || pos >= _used) {
            throw CVectorInvalidIndex("at");
        }
        return _v[_adjust(pos)];
    }

    /** */
    Type& at(std::size_t pos) {
        NON_CONST_VARIANT(Type,CVector,at(pos));
    }

    /** */
    void push_back(const Type& e) {
        if (_inc(_end) == _start) {
            _grow();
        }
        _v[_end] = e;
        _end = _inc(_end);
        _used ++;
    }

    /** */
    const Type& back() const {
        if (_used == 0) {
            throw CVectorInvalidIndex("back");
        }
        return (const Type &)_v[_dec(_end)];
    }

    /** */
    Type& back() {
        NON_CONST_VARIANT(Type,CVector,back());
    }

    /**  */
    void pop_back() {
        if (_used == 0) {
            throw CVectorInvalidIndex("pop_back");
        }
        _end = _dec(_end);
        _used --;
    }

    /**  */
    void push_front(const Type& e) {
        if (_dec(_start) == _end) {
            _grow();
        }
        _start = _dec(_start);
        _v[_start] = e;
        _used ++;
    }
    
    /**  */
    const Type& front() const {
        if (_used == 0) {
            throw CVectorInvalidIndex("front");
        }
        return (const Type &)_v[_start];
    }

    /**  */
    Type& front() {
        NON_CONST_VARIANT(Type,CVector,front());
    }    

    /**  */
    void pop_front() {
        if (_used == 0) {
            throw CVectorInvalidIndex("pop_front");
        }
        _start = _inc(_start);
        _used --;
    }            
    
    /** */
    void print(std::ostream &out=std::cout) {
        for (std::size_t i=0; i<_max; i++) {
            out << std::setw(2) << i << ": ";
            // NOTE: _inside() is O(n), which makes the metod O(n^2)
            if (_inside(i)) {
                out << _v[i];
            } else {
                out << "? ";
            }
            if (i == _start) out << "<-(start)";
            if (i == _end) out << "<-(end)";
            out << std::endl;
        }
    }    
    
private:

    void _grow() {
        Type *old = _v;
        _v = new Type[_max*2];
        
        for (std::size_t i=_start, j=0; i!=_end; i=_inc(i)) {
            _v[j++] = old[i];
        }
        // if _max is changed before the copy is over, inc() fails
        _max *= 2;
        _start = 0;
        _end = _used;
        delete[] old;
    }

    std::size_t _dec(std::size_t i) const {
        return (i == 0) ? _max-1 : i-1;
    }

    std::size_t _inc(std::size_t i) const {
        return (i == _max-1) ? 0 : i+1;
    }

    bool _inside(std::size_t i) const {
        for (std::size_t j=_start; j!=_end; j=_inc(j)) {
            if (j == i) return true;
        }
        return false;
    }
    
    std::size_t _adjust(std::size_t external) const {
        std::size_t internal = (external + _start);
        return (internal >= _max) ? internal - _max : internal;
    }
};

#endif // __CVECTOR_H
