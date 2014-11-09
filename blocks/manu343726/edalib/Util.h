/**
 * @file Util.h
 *
 * Common definitions, exceptions, and utility functions
 *
 * Estructura de Datos y Algoritmos
 *
 * Copyright (C) 2014
 * Facultad de Inform�tica, Universidad Complutense de Madrid
 * This software is licensed under the Simplified BSD licence:
 *    (see the LICENSE file or
 *    visit opensource.org/licenses/BSD-3-Clause)
 */

#ifndef UTIL_H //Avoid names starting with underscores, has UB (Are reserved names)
#define UTIL_H

#include <string>
#include <iostream>
#include <iosfwd>
#include <exception>
#include <stdexcept>
#include <iterator>
#include <algorithm>

//Macro to inherit constructors from a base class. MSVC12 doesn't implement this feature yet,
//so perfect forwarding to the base class is used instead. 
//The third optional parameter of the macro is the namespace of the base class, 
//to write the qualified name (See DECLARE_EXCEPTION() bellow for an example).
#ifdef _MSC_VER
#define INHERIT_CTORS(derived,base,...) template<typename... ARGS> derived(ARGS... args) : __VA_ARGS__##base{ std::forward<ARGS>(args)...} {}
#else
#define INHERIT_CTORS(derived,base,...) using __VA_ARGS__##base::base;
#endif

std::ostream& operator<<(std::ostream& out, const std::exception& e) {
    return out << e.what();
}

/// Macro to create subclasses of the base exception.
///     use as: DECLARE_EXCEPTION(UniqueExceptionName)
#define DECLARE_EXCEPTION(ExceptionSubclass) \
class ExceptionSubclass : public std::logic_error { \
public: \
	INHERIT_CTORS(ExceptionSubclass,logic_error,std::) \
};

/// Macro to constify an operation.
///     given an operation like const Something& MyClass::myOperation(...),
///     use to provide the implementation of a non-const variant as:
///        NON_CONST_VARIANT(Something,Myclass,myOperation(...));
#define NON_CONST_VARIANT(Return,Class,Call) \
return const_cast<Return&>(static_cast<const Class*>(this)->Call) //AVOID IF POSSIBLE, HAS UB. Provide different overloads

/**
 * Copies all elements between first and last at the back of a given container
 */
template<class It, class Container>
void copy_back(It first, It last, Container& target) {
    std::copy( first , last , std::back_inserter( target ) );
}

/**
 * Prints all elements between first and last to a given stream.
 *     Places a separator string between each pair of elements.
 *     Defaults to printing to std::cout with ", " as separator
 */ 
template<class It>
void print(It first, It last, std::ostream &out = std::cout,
           std::string separator = std::string(", ")) {
    std::copy( first , last , std::ostream_iterator<typename It::value_type>( out , separator ) );
}

/**
 * Prints all elements in an iterable collection to a given stream.
 *     Places a separator string between each pair of elements.
 *     Defaults to printing to std::cout with ", " as separator
 */
template<class Container>
void print(std::string message, const Container &c, std::ostream &out = std::cout,
           std::string separator = std::string(", ")) {
    out << message << " (size " << c.size() << "): ";
    print(c.begin(), c.end(), out, separator);
    out << std::endl;
}

namespace util
{
    /**
     * Provides the expected member types of a Standard Container
     */
    template<typename C , typename T>
    struct container_traits
    {
        typedef T  value_type;
        typedef T& reference_type;
        typedef T* pointer_type;
    };
}

/**
 * Adds the required iterator traits to a container
 */
#define ADD_ITERATOR_TRAITS() public:                            \
                                typedef Iterator iterator;       \
                                typedef Iterator const_iterator;
           
#endif // UTIL_H
