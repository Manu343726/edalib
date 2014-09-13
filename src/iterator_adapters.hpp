/* 
 * File:   iterator_adapters.hpp
 * Author: Manu Sánchez (Manu343726 @ twitter, github, stackoverflow, etc)
 *
 * Created on 13 de septiembre de 2014, 19:34
 * 
 * This file is published under the BSD License, see the LICENSE file for more info.
 */

#ifndef ITERATOR_ADAPTERS_HPP
#define	ITERATOR_ADAPTERS_HPP

#include <iterator>
#include <type_traits>

/**
 * Since you write iterators in a Java way, instead of the C++ way, this header provides several adaptors to
 * translate an Iterator class from your Java-like syntax (Next(), elem(), set(), etc) to the C++ one (operator++ , operator* , etc) and vice-versa.
 * 
 * First define some constraints:
 * 
 *  - Forward iterators and bidirectional iterators only. C++ defines some tags to identify such cathegories (std::forward_iterator_tag , etc). 
 *    Note that I discarded the random access iterator cathegory, so expressions like "it + 4" will not be supported.
 * 
 *    In your Java-like iterators, I will suppose that:
 *        a) An iterator type with elem() (Read/Write, avoid set() please) and next() members only is a forward iterator.
 *        b) An iterator type with elem() , next() , and prev() members is a bidirectional iterator.
 * 
 * 
 * The point of these adaptors is to leave the iterators readable for your students (You are teaching Java and C++ in a Java way, even if its a very bad idea IMHO), using
 * constructs like set(), get(), next(), etc, while those iterators work with C++ too, since they define a C++-ready interface automatically based on your Java-like interface.
 * 
 * Also note this has no runtime overhead at all, everything is done at compile-time in a way which allows the compiler to optimize away all the abstractions.
 */

namespace util
{
   /*
    * First we need to get the cathegory of a given iterator type. For a C++ iterator its easy, since they follow the conventions and define an iterator_cathegory member type.
    * For an EDA iterator, we need to do a little obscure template metaprogramming (Don't worry, leave this to the C++ freak) to figure out the cathegory from its members.
    */

   /*
    * This is an old trick to check it a given type has a member, but updated to C++11 (Its easier to write and read)
    */
   #define HAS_MEMBER(MEMBER) template<typename T>                                             \
                              struct has_member_##MEMBER                                       \
                              {                                                                \
                                  template<typename U> static std::true_type  test(U*);        \
                                  template<typename U> static std::false_type test(...);       \
                                                                                               \
                                  static const bool value = decltype(test<T>(nullptr))::value; \
                              }

   HAS_MEMBER(elem);
   HAS_MEMBER(next);
   HAS_MEMBER(prev);


   template<typename T>
   using is_eda_forward_iterator = std::integral_constant<bool, has_member_elem<T>::value &&
                                                                has_member_next<T>::value &&
                                                               !has_member_prev<T>::value
                                                         >;
   
   template<typename T>
   using is_eda_bidirectional_iterator = std::integral_constant<bool,has_member_elem<T>::value &&
                                                                     has_member_next<T>::value &&
                                                                     has_member_prev<T>::value
                                                               >; 
   
   template<typename T ,
            bool E = util::has_member_elem<T>::value ,
            bool N = util::has_member_next<T>::value ,
            bool P = util::has_member_prev<T>::value
           >
   struct eda_iterator_cathegory
   {
       static_assert( sizeof(T) != sizeof(T) , "Unknown EDA iterator cathegory" );
   };
   
   template<typename T>
   struct eda_iterator_cathegory<T,true,true,false>
   {
       using type = std::forward_iterator_tag;
   };
   
   template<typename T>
   struct eda_iterator_cathegory<T,true,true,true>
   {
       using type = std::bidirectional_iterator_tag;
   };
   
   
   /*
    * We don't know the value type of an EDA iterator type T (Since its not following the conventions), so here is a workaround:
    * (A non-working workaround... Thats why the adapters have a second parameter, the value type)
    */
   template<typename T>
   using eda_iterator_value_type = typename std::decay<decltype(&T::elem)>::type;
   
   
   
   /*
    * So we know the cathegory of an EDA iterator. Ok, now define the adapters, specialized for each cathegory
    */
   
   template<typename T , typename VT = util::eda_iterator_value_type<T> , typename C = typename util::eda_iterator_cathegory<T>::type>
   struct edatocpp_iterator_adapter;
   
   template<typename T , typename VT>
   struct edatocpp_iterator_adapter<T,VT,std::forward_iterator_tag> : public std::iterator<std::forward_iterator_tag,VT>
   {
       using iterator_traits = std::iterator<std::forward_iterator_tag,VT>;
       
       
       
       //Using C++14 this is easier, since we can use function return type deduction, but in this case we will use these traits defined above.
       
       const typename iterator_traits::value_type& operator*() const
       {
           return static_cast<T*>(this)->elem();
       }
       
       typename iterator_traits::value_type& operator*()
       {
           return static_cast<T*>(this)->elem();
       }
       
       T& operator++()
       {
           static_cast<T*>(this)->next();
           return *static_cast<T*>(this);
       }
       
       T operator++(int)
       {
           T cpy{ *static_cast<T*>(this) };
           ++(*this);
           return cpy;
       }
   };
   
   
   /*
    * Is there a way to automatize this avoiding copy-pasting code, but its done via typelists, tye-maps, etc; and I don't want to add dependencies
    * to my Turbo library here... Lets get simpler, just good old clipboard.
    */
   
    template<typename T , typename VT>
    struct edatocpp_iterator_adapter<T,VT,std::bidirectional_iterator_tag> : public std::iterator<std::bidirectional_iterator_tag,VT>
    {
        using iterator_traits = std::iterator<std::bidirectional_iterator_tag,VT>;
       
       

        //Using C++14 this is easier, since we can use function return type deduction, but in this case we will use those traits defined above.

        const typename iterator_traits::value_type& operator*( ) const
        {
            return static_cast<T*>( this )->elem( );
        }

        typename iterator_traits::value_type& operator*( )
        {
            return static_cast<T*>( this )->elem( );
        }

        T& operator++( )
        {
            static_cast<T*>( this )->next( );
            return *static_cast<T*>( this );
        }

        T operator++(int)
        {
            T cpy{ *static_cast<T*>( this ) };
            ++( *this );
            return cpy;
        }
        
        T& operator--( )
        {
            static_cast<T*>( this )->prev( );
            return *static_cast<T*>( this );
        }

        T operator--(int)
        {
            T cpy{ *static_cast<T*>( this ) };
            --( *this );
            return cpy;
        }
    };
   
}





#endif	/* ITERATOR_ADAPTERS_HPP */

