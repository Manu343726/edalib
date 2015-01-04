

#ifndef CONTAINER_ADAPTERS_HPP
#define CONTAINER_ADAPTERS_HPP

#include "iterator_adapters.hpp"
#include "Util.h"

template<typename C>
struct container_traits;


struct linear_container_tag{};
struct container_adapter_tag{};
struct asociative_container_tag{};


/*
 * - A linear container is any template with one type parameter only.
 * - A container adapter is any template with a type parameter and a linear container template parameter,
 * - An asociative container is any template with two type parameters (key,value) and a linear container 
 *   template parameter.
 */

//Ugly macros to make the code much more readable for non-C++ers
#define LINEAR_CONTAINER template<typename> class
#define CONTAINER_ADAPTER template<typename,LINEAR_CONTAINER> class
#define ASSOCIATIVE_CONTAINER template<typename,typename,LINEAR_CONTAINER> class


template<LINEAR_CONTAINER C , typename T>
struct container_traits<C<T>>
{
    typedef linear_container_tag container_category;

    typedef T value_type;
};

template<CONTAINER_ADAPTER A , LINEAR_CONTAINER C , typename T>
struct container_traits<A<T, C>>
{
    typedef container_adapter_tag container_category;

    typedef T value_type;
};

template<ASSOCIATIVE_CONTAINER C , LINEAR_CONTAINER UC , typename KEY , typename VALUE>
struct container_traits<C<KEY, VALUE, UC>>
{
    typedef asociative_container_tag container_category;

    typedef std::pair<const KEY, VALUE> value_type;
    typedef KEY                        key_type;
    typedef VALUE                      mapped_type;
    typedef UC<value_type>             bucket_container;
};




template<typename C>
struct edatocpp_iterator_traits
{
    typedef typename C::Iterator java_iterator;
    typedef typename util::eda_iterator_category<java_iterator>::type iterator_category;
};


template<typename C>
struct edatocpp_container_adapter : public C , public container_traits<C>
{
    typedef C edalib_container;
    typedef edatocpp_iterator_traits<C> itraits;
    typedef container_traits<C> ctraits;
    
    INHERIT_CTORS(edatocpp_container_adapter,edalib_container)
    
    template<typename T>
    edatocpp_container_adapter(const std::initializer_list<T>& i) : C( i ) //Good old initialization to prevent C2797 on MSVC
    {}
    
#ifndef _MSC_VER
    edatocpp_container_adapter() = default;
#endif

    struct iterator : public itraits::java_iterator
    {  
        using java_iterator = typename itraits::java_iterator;
        
        INHERIT_CTORS(iterator,java_iterator)

        const typename ctraits::value_type& operator*( ) const
        {
            return itraits::java_iterator::elem( );
        }

        typename ctraits::value_type& operator*( )
        {
            return itraits::java_iterator::elem( );
        }

        iterator& operator++( )
        {
            itraits::java_iterator::next( );
            return *this;
        }

        iterator operator++(int)
        {
            iterator cpy{ *this };
            ++( *this );
            return cpy;
        }
        
        //SFINAE: Welcome to C++ type-based conditional code generation

        template<typename SFINAE_FLAG = typename itraits::iterator_category,
                 typename = typename std::enable_if<std::is_same<SFINAE_FLAG, std::bidirectional_iterator_tag>::value>::type
                >
        iterator& operator--( )
        {
            itraits::java_iterator::prev( );
            return *this;
        }

        template<typename SFINAE_FLAG = typename itraits::iterator_category,
                 typename = typename std::enable_if<std::is_same<SFINAE_FLAG, std::bidirectional_iterator_tag>::value>::type
                >
        iterator operator--(int)
        {
            iterator cpy{ *this };
            --( *this );
            return cpy;
        }
        
        //Standard types of the iterator interface:
        typedef typename ctraits::value_type  value_type;
        typedef typename ctraits::value_type& reference;
        typedef typename ctraits::value_type* pointer;
        typedef typename itraits::iterator_category iterator_category;
        typedef std::ptrdiff_t difference_type;
    };

    using const_iterator = iterator; //Having the same iterator both for mutable and const iterators is not a good idea, but it works...

    const_iterator begin( ) const
    {
        return const_iterator{ edalib_container::begin( ) };
    }

    iterator begin( )
    {
        return iterator{ edalib_container::begin( ) };
    }

    const_iterator end( ) const
    {
        return const_iterator{ edalib_container::end( ) };
    }

    iterator end( )
    {
        return iterator{ edalib_container::end( ) };
    }

};

#endif /* CONTAINER_ADAPTERS_HPP */

