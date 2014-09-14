



template<typename C>
struct container_traits;


struct linear_container_tag{};
struct container_adapter_tag{};
struct asociative_container_tag{};


template<template<typename> class C , typename T>
struct container_traits<C<T>>
{
	typedef linear_container_tag container_cathegory;

	typedef T value_type;
};

template<template<typename,template<typename>class> class A , template<typename> class C , typename T>
struct container_traits<A<T,C<T>>>
{
	typedef container_adapter_tag container_cathegory;

	typedef T value_type;
};

template<template<typename,typename,template<typename>class> class C , template<typename> class UC , typename KEY , typename VALUE>
struct container_traits<C<KEY,VALUE,UC<std::pair<const KEY,VALUE>>>>
{
	typedef asociative_container_tag container_cathegory;

	typedef std::pair<const KEY,VALUE> value_type;
	typedef KEY                        key_type;
	typedef VALUE                      mapped_type;
};




template<typename C>
struct iterator_traits
{
	typedef typename C::Iterator java_iterator;
	typedef typename util::iterator_cathegory<java_iterator>::type iterator_cathegory;
};



template<typename C>
struct edatocpp_container_adapter : public C , public iterator_traits<C> , public container_traits<C>
{
	using C::C;

	typedef C edalib_container;
	typedef iterator_traits<C> iterator_traits;
	typedef container_traits<C> container_traits;

	struct iterator : public iterator_traits::java_iterator
	{
		typedef typename iterator_traits::java_iterator java_iterator;

		using java_iterator::java_iterator;



		const typename container_traits::value_type& operator*() const
		{
			return java_iterator::elem();
		}

		typename container_traits::value_type& operator*()
		{
			return java_iterator::elem();
		}

		iterator& operator++()
		{
			java_iterator::next();
			return *this;
		}

		iterator operator++(int)
		{
			iterator cpy{*this};
			++(*this);
			return cpy;
		}

		template<typename SFINAE_FLAG = typename iterator_traits::iterator_cathegory,
				 typename = typename std::enable_if<std::is_same<SFINAE_FLAG,std::bidirectional_iterator_tag>::value>::type
				>
		iterator& operator--()
		{
			java_iterator::prev();
			return *this;
		}

		template<typename SFINAE_FLAG = typename iterator_traits::iterator_cathegory,
				 typename = typename std::enable_if<std::is_same<SFINAE_FLAG,std::bidirectional_iterator_tag>::value>::type
				>
		iterator operator--(int)
		{
			iterator cpy{*this};
			--(*this);
			return cpy;
		}
	};

	typedef iterator const_iterator;

	const_iterator begin() const
	{
		return const_iterator{ edalib_container::begin() };
	}

	iterator begin()
	{
		return iterator{ edalib_container::begin() };
	}
	
	const_iterator end() const
	{
		return const_iterator{ edalib_container::end() };
	}
	
	iterator end()
	{
		return iterator{ edalib_container::end() };
	}

};

