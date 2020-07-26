//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#pragma once
#include "toml_value.h"

TOML_PUSH_WARNINGS
TOML_DISABLE_MISC_WARNINGS

TOML_IMPL_NAMESPACE_START
{
	template <bool IsConst>
	class TOML_TRIVIAL_ABI array_iterator final
	{
		private:
			friend class ::toml::array;

			using raw_mutable_iterator = std::vector<std::unique_ptr<node>>::iterator;
			using raw_const_iterator = std::vector<std::unique_ptr<node>>::const_iterator;
			using raw_iterator = std::conditional_t<IsConst, raw_const_iterator, raw_mutable_iterator>;
			
			mutable raw_iterator raw_;

			array_iterator(raw_mutable_iterator raw) noexcept
				: raw_{ raw }
			{}

			template <bool C = IsConst, typename = std::enable_if_t<C>>
			TOML_NODISCARD_CTOR
			array_iterator(raw_const_iterator raw) noexcept
				: raw_{ raw }
			{}

		public:

			using value_type = std::conditional_t<IsConst, const node, node>;
			using reference = value_type&;
			using pointer = value_type*;
			using difference_type = ptrdiff_t;

			array_iterator() noexcept = default;
			array_iterator(const array_iterator&) noexcept = default;
			array_iterator& operator = (const array_iterator&) noexcept = default;

			array_iterator& operator++() noexcept // ++pre
			{
				++raw_;
				return *this;
			}

			array_iterator operator++(int) noexcept // post++
			{
				array_iterator out{ raw_ };
				++raw_;
				return out;
			}

			array_iterator& operator--() noexcept // --pre
			{
				--raw_;
				return *this;
			}

			array_iterator operator--(int) noexcept // post--
			{
				array_iterator out{ raw_ };
				--raw_;
				return out;
			}

			[[nodiscard]]
			reference operator * () const noexcept
			{
				return *raw_->get();
			}

			[[nodiscard]]
			pointer operator -> () const noexcept
			{
				return raw_->get();
			}

			array_iterator& operator += (ptrdiff_t rhs) noexcept
			{
				raw_ += rhs;
				return *this;
			}

			array_iterator& operator -= (ptrdiff_t rhs) noexcept
			{
				raw_ -= rhs;
				return *this;
			}

			[[nodiscard]]
			friend constexpr array_iterator operator + (const array_iterator& lhs, ptrdiff_t rhs) noexcept
			{
				return { lhs.raw_ + rhs };
			}

			[[nodiscard]]
			friend constexpr array_iterator operator + (ptrdiff_t lhs, const array_iterator& rhs) noexcept
			{
				return { rhs.raw_ + lhs };
			}

			[[nodiscard]]
			friend constexpr array_iterator operator - (const array_iterator& lhs, ptrdiff_t rhs) noexcept
			{
				return { lhs.raw_ - rhs };
			}

			[[nodiscard]]
			friend constexpr ptrdiff_t operator - (const array_iterator& lhs, const array_iterator& rhs) noexcept
			{
				return lhs.raw_ - rhs.raw_;
			}

			[[nodiscard]]
			friend constexpr bool operator == (const array_iterator& lhs, const array_iterator& rhs) noexcept
			{
				return lhs.raw_ == rhs.raw_;
			}

			[[nodiscard]]
			friend constexpr bool operator != (const array_iterator& lhs, const array_iterator& rhs) noexcept
			{
				return lhs.raw_ != rhs.raw_;
			}

			[[nodiscard]]
			friend constexpr bool operator < (const array_iterator& lhs, const array_iterator& rhs) noexcept
			{
				return lhs.raw_ < rhs.raw_;
			}

			[[nodiscard]]
			friend constexpr bool operator <= (const array_iterator& lhs, const array_iterator& rhs) noexcept
			{
				return lhs.raw_ <= rhs.raw_;
			}

			[[nodiscard]]
			friend constexpr bool operator > (const array_iterator& lhs, const array_iterator& rhs) noexcept
			{
				return lhs.raw_ > rhs.raw_;
			}

			[[nodiscard]]
			friend constexpr bool operator >= (const array_iterator& lhs, const array_iterator& rhs) noexcept
			{
				return lhs.raw_ >= rhs.raw_;
			}

			[[nodiscard]]
			reference operator[] (ptrdiff_t idx) const noexcept
			{
				return *(raw_ + idx)->get();
			}

			TOML_PUSH_WARNINGS
			TOML_DISABLE_ALL_WARNINGS

			template <bool C = IsConst, typename = std::enable_if_t<!C>>
			[[nodiscard]]
			operator array_iterator<true>() const noexcept
			{
				return array_iterator<true>{ raw_ };
			}

			TOML_POP_WARNINGS
	};

	template <typename T>
	[[nodiscard]]
	TOML_ATTR(returns_nonnull)
	TOML_ALWAYS_INLINE
	auto* make_node(T&& val) noexcept
	{
		using type = unwrap_node<remove_cvref_t<T>>;
		if constexpr (is_one_of<type, array, table>)
		{
			static_assert(
				std::is_rvalue_reference_v<decltype(val)>,
				"Tables and arrays may only be moved (not copied)."
			);
			return new type{ std::forward<T>(val) };
		}
		else
		{
			static_assert(
				!is_wide_string<T> || TOML_WINDOWS_COMPAT,
				"Instantiating values from wide-character strings is only "
				"supported on Windows with TOML_WINDOWS_COMPAT enabled."
			);
			static_assert(
				is_native<type> || is_losslessly_convertible_to_native<type>,
				"Value initializers must be (or be promotable to) one of the TOML value types"
			);
			if constexpr (is_wide_string<T>)
			{
				#if TOML_WINDOWS_COMPAT
				return new value{ narrow(std::forward<T>(val)) };
				#else
				static_assert(dependent_false<T>, "Evaluated unreachable branch!");
				#endif
			}
			else
				return new value{ std::forward<T>(val) };
		}
	}

	template <typename T>
	[[nodiscard]]
	TOML_ATTR(returns_nonnull)
	TOML_ALWAYS_INLINE
	auto* make_node(inserter<T>&& val) noexcept
	{
		return make_node(std::move(val.value));
	}
}
TOML_IMPL_NAMESPACE_END

TOML_NAMESPACE_START
{
	/// \brief A RandomAccessIterator for iterating over elements in a toml::array.
	using array_iterator = impl::array_iterator<false>;

	/// \brief A RandomAccessIterator for iterating over const elements in a toml::array.
	using const_array_iterator = impl::array_iterator<true>;

	/// \brief	A TOML array.
	///
	/// \detail The interface of this type is modeled after std::vector, with some
	/// 		additional considerations made for the heterogeneous nature of a
	/// 		TOML array.
	/// 
	/// \godbolt{sjK4da}
	/// 
	/// \cpp
	/// 
	/// toml::table tbl = toml::parse(R"(
	///     arr = [1, 2, 3, 4, 'five']
	/// )"sv);
	/// 
	/// // get the element as an array
	/// toml::array& arr = *tbl.get_as<toml::array>("arr");
	/// std::cout << arr << "\n";
	/// 
	/// // increment each element with visit()
	/// for (auto&& elem : arr)
	/// {
	/// 	elem.visit([](auto&& el) noexcept
	/// 	{
	/// 		if constexpr (toml::is_number<decltype(el)>)
	/// 			(*el)++;
	/// 		else if constexpr (toml::is_string<decltype(el)>)
	/// 			el = "six"sv;
	/// 	});
	/// }
	/// std::cout << arr << "\n";
	/// 
	/// // add and remove elements
	/// arr.push_back(7);
	/// arr.push_back(8.0f);
	/// arr.push_back("nine"sv);
	/// arr.erase(arr.cbegin());
	/// std::cout << arr << "\n";
	/// 
	/// // emplace elements
	/// arr.emplace_back<std::string>("ten");
	/// arr.emplace_back<toml::array>(11, 12.0);
	/// std::cout << arr << "\n";
	/// 
	/// \ecpp
	/// 
	/// \out
	/// [ 1, 2, 3, 4, 'five' ]
	/// [ 2, 3, 4, 5, 'six' ]
	/// [ 3, 4, 5, 'six', 7, 8.0, 'nine' ]
	/// [ 3, 4, 5, 'six', 7, 8.0, 'nine', 'ten', [ 11, 12.0 ] ]
	/// \eout
	class TOML_API array final
		: public node
	{
		private:
			friend class TOML_PARSER_TYPENAME;
			std::vector<std::unique_ptr<node>> elements;

			void preinsertion_resize(size_t idx, size_t count) noexcept;

		public:

			using value_type = node;
			using size_type = size_t;
			using difference_type = ptrdiff_t;
			using reference = node&;
			using const_reference = const node&;

			/// \brief A RandomAccessIterator for iterating over elements in a toml::array.
			using iterator = array_iterator;
			/// \brief A RandomAccessIterator for iterating over const elements in a toml::array.
			using const_iterator = const_array_iterator;

			/// \brief	Default constructor.
			TOML_NODISCARD_CTOR
			array() noexcept;

			/// \brief	Move constructor.
			TOML_NODISCARD_CTOR
			array(array&& other) noexcept;

			/// \brief	Constructs an array with one or more initial elements.
			///
			/// \detail \cpp
			/// auto arr = toml::array{ 1, 2.0, "three"sv, toml::array{ 4, 5 } };
			/// std::cout << arr << "\n";
			/// 
			/// \ecpp
			/// 
			/// \out
			/// [ 1, 2.0, 'three', [ 4, 5 ] ]
			/// \eout
			/// 
			/// \remark	\parblock If you need to construct an array with one child array element, the array's move constructor
			/// 		will take precedence and perform a move-construction instead. You can use toml::inserter to
			/// 		suppress this behaviour: \cpp
			/// // desired result: [ [ 42 ] ]
			/// auto bad = toml::array{ toml::array{ 42 } }
			/// auto good = toml::array{ toml::inserter{ toml::array{ 42 } } }
			/// std::cout << "bad: " << bad << "\n";
			/// std::cout << "good:" << good << "\n";
			/// \ecpp
			/// 
			/// \out
			/// bad:  [ 42 ]
			/// good: [ [ 42 ] ]
			/// \eout
			/// 
			/// \endparblock
			/// 
			/// \tparam	ElemType	One of the TOML node or value types (or a type promotable to one).
			/// \tparam	ElemTypes	One of the TOML node or value types (or a type promotable to one).
			/// \param 	val 	The node or value used to initialize element 0.
			/// \param 	vals	The nodes or values used to initialize elements 1...N.
			template <typename ElemType, typename... ElemTypes>
			TOML_NODISCARD_CTOR
			explicit array(ElemType&& val, ElemTypes&&... vals)
			{
				elements.reserve(sizeof...(ElemTypes) + 1_sz);
				elements.emplace_back(impl::make_node(std::forward<ElemType>(val)));
				if constexpr (sizeof...(ElemTypes) > 0)
				{
					(
						elements.emplace_back(impl::make_node(std::forward<ElemTypes>(vals))),
						...
					);
				}
			}

			/// \brief	Move-assignment operator.
			array& operator= (array&& rhs) noexcept;

			array(const array&) = delete;
			array& operator= (const array&) = delete;

			/// \brief	Always returns node_type::array for array nodes.
			[[nodiscard]] node_type type() const noexcept override;
			/// \brief	Always returns `false` for array nodes.
			[[nodiscard]] bool is_table() const noexcept override;
			/// \brief	Always returns `true` for array nodes.
			[[nodiscard]] bool is_array() const noexcept override;
			/// \brief	Always returns `false` for array nodes.
			[[nodiscard]] bool is_value() const noexcept override;
			[[nodiscard]] array* as_array() noexcept override;
			[[nodiscard]] const array* as_array() const noexcept override;

			/// \brief	Checks if the array contains elements of only one type.
			///
			/// \detail \cpp
			/// auto arr = toml::array{ 1, 2, 3 };
			/// std::cout << "homogenous: "sv << arr.is_homogeneous(toml::node_type::none) << "\n";
			/// std::cout << "all floats: "sv << arr.is_homogeneous(toml::node_type::floating_point) << "\n";
			/// std::cout << "all arrays: "sv << arr.is_homogeneous(toml::node_type::array) << "\n";
			/// std::cout << "all ints:   "sv << arr.is_homogeneous(toml::node_type::integer) << "\n";
			/// 
			/// \ecpp
			/// 
			/// \out
			/// homogeneous: true
			/// all floats: false
			/// all arrays: false
			/// all ints:   true
			/// \eout
			/// 
			/// \param	type	A TOML node type. <br>
			/// 				<strong><em>toml::node_type::none: </em></strong> "is every node the same type?" <br>
			/// 				<strong><em>Anything else:</em></strong> "is every node one of these?"
			///
			/// \returns	True if the array was homogeneous.
			/// 
			/// \remarks	Always returns `false` for empty arrays.
			[[nodiscard]] bool is_homogeneous(node_type type) const noexcept;

			/// \brief	Checks if the array contains elements of only one type.
			///
			/// \detail \cpp
			/// auto arr = toml::array{ 1, 2, 3 };
			/// std::cout << "homogenous:   "sv << arr.is_homogeneous() << "\n";
			/// std::cout << "all doubles:  "sv << arr.is_homogeneous<double>() << "\n";
			/// std::cout << "all arrays:   "sv << arr.is_homogeneous<toml::array>() << "\n";
			/// std::cout << "all integers: "sv << arr.is_homogeneous<int64_t>() << "\n";
			/// 
			/// \ecpp
			/// 
			/// \out
			/// homogeneous:  true
			/// all doubles:  false
			/// all arrays:   false
			/// all integers: true
			/// \eout
			/// 
			/// \tparam	ElemType	A TOML node or value type. <br>
			/// 					<strong><em>Left as `void`:</em></strong> "is every node the same type?" <br>
			/// 					<strong><em>Explicitly specified:</em></strong> "is every node a T?"
			///
			/// \returns	True if the array was homogeneous.
			/// 
			/// \remarks	Always returns `false` for empty arrays.
			template <typename ElemType = void>
			[[nodiscard]]
			bool is_homogeneous() const noexcept
			{
				using type = impl::unwrap_node<ElemType>;
				static_assert(
					std::is_void_v<type>
					|| ((impl::is_native<type> || impl::is_one_of<type, table, array>) && !impl::is_cvref<type>),
					"The template type argument of array::is_homogeneous() must be void or one of:"
					TOML_SA_UNWRAPPED_NODE_TYPE_LIST
				);
				
				if constexpr (std::is_void_v<type>)
					return is_homogeneous(node_type::none);
				else
					return is_homogeneous(impl::node_type_of<type>);
			}

			/// \brief	Returns true if this array contains only tables.
			[[nodiscard]] bool is_array_of_tables() const noexcept override;

			/// \brief	Gets a reference to the element at a specific index.
			[[nodiscard]] node& operator[] (size_t index) noexcept;
			/// \brief	Gets a reference to the element at a specific index.
			[[nodiscard]] const node& operator[] (size_t index) const noexcept;

			/// \brief	Returns a reference to the first element in the array.
			[[nodiscard]] node& front() noexcept;
			/// \brief	Returns a reference to the first element in the array.
			[[nodiscard]] const node& front() const noexcept;
			/// \brief	Returns a reference to the last element in the array.
			[[nodiscard]] node& back() noexcept;
			/// \brief	Returns a reference to the last element in the array.
			[[nodiscard]] const node& back() const noexcept;

			/// \brief	Returns an iterator to the first element.
			[[nodiscard]] iterator begin() noexcept;
			/// \brief	Returns an iterator to the first element.
			[[nodiscard]] const_iterator begin() const noexcept;
			/// \brief	Returns an iterator to the first element.
			[[nodiscard]] const_iterator cbegin() const noexcept;

			/// \brief	Returns an iterator to one-past-the-last element.
			[[nodiscard]] iterator end() noexcept;
			/// \brief	Returns an iterator to one-past-the-last element.
			[[nodiscard]] const_iterator end() const noexcept;
			/// \brief	Returns an iterator to one-past-the-last element.
			[[nodiscard]] const_iterator cend() const noexcept;

			/// \brief	Returns true if the array is empty.
			[[nodiscard]] bool empty() const noexcept;
			/// \brief	Returns the number of elements in the array.
			[[nodiscard]] size_t size() const noexcept;
			/// \brief	Reserves internal storage capacity up to a pre-determined number of elements.
			void reserve(size_t new_capacity);
			/// \brief	Removes all elements from the array.
			void clear() noexcept;

			/// \brief	Returns the maximum number of elements that can be stored in an array on the current platform.
			[[nodiscard]] size_t max_size() const noexcept;
			/// \brief	Returns the current max number of elements that may be held in the array's internal storage.
			[[nodiscard]] size_t capacity() const noexcept;
			/// \brief	Requests the removal of any unused internal storage capacity.
			void shrink_to_fit();

			/// \brief	Inserts a new element at a specific position in the array.
			///
			/// \detail \cpp
			/// auto arr = toml::array{ 1, 3 };
			///	arr.insert(arr.cbegin() + 1, "two");
			///	arr.insert(arr.cend(), toml::array{ 4, 5 });
			/// std::cout << arr << "\n";
			/// 
			/// \ecpp
			/// 
			/// \out
			/// [ 1, 'two', 3, [ 4, 5 ] ]
			/// \eout
			/// 
			/// \tparam	ElemType	One of the TOML node or value types (or a type promotable to one).
			/// \param 	pos			The insertion position.
			/// \param 	val			The node or value being inserted.
			///
			/// \returns	An iterator to the newly-inserted element.
			template <typename ElemType>
			iterator insert(const_iterator pos, ElemType&& val) noexcept
			{
				return { elements.emplace(pos.raw_, impl::make_node(std::forward<ElemType>(val))) };
			}

			/// \brief	Repeatedly inserts a new element starting at a specific position in the array.
			///
			/// \detail \cpp
			/// auto arr = toml::array{
			///		"with an evil twinkle in its eye the goose said",
			///		"and immediately we knew peace was never an option."
			///	};
			///	arr.insert(arr.cbegin() + 1, 3, "honk");
			/// std::cout << arr << "\n";
			/// 
			/// \ecpp
			/// 
			/// \out
			/// [
			/// 	'with an evil twinkle in its eye the goose said',
			/// 	'honk',
			/// 	'honk',
			/// 	'honk',
			/// 	'and immediately we knew peace was never an option.'
			/// ]
			/// \eout
			/// 
			/// \tparam	ElemType	One of the TOML node or value types (or a type promotable to one).
			/// \param 	pos			The insertion position.
			/// \param 	count		The number of times the node or value should be inserted.
			/// \param 	val			The node or value being inserted.
			///
			/// \returns	An iterator to the first newly-inserted element (or a copy of `pos` if count was 0).
			template <typename ElemType>
			iterator insert(const_iterator pos, size_t count, ElemType&& val) noexcept
			{
				switch (count)
				{
					case 0: return { elements.begin() + (pos.raw_ - elements.cbegin()) };
					case 1: return insert(pos, std::forward<ElemType>(val));
					default:
					{
						const auto start_idx = static_cast<size_t>(pos.raw_ - elements.cbegin());
						preinsertion_resize(start_idx, count);
						size_t i = start_idx;
						for (size_t e = start_idx + count - 1_sz; i < e; i++)
							elements[i].reset(impl::make_node(val));

						//# potentially move the initial value into the last element
						elements[i].reset(impl::make_node(std::forward<ElemType>(val)));
						return { elements.begin() + static_cast<ptrdiff_t>(start_idx) };
					}
				}
			}

			/// \brief	Inserts a range of elements into the array at a specific position.
			///
			/// \tparam	Iter	An iterator type. Must satisfy ForwardIterator.
			/// \param 	pos		The insertion position.
			/// \param 	first	Iterator to the first node or value being inserted.
			/// \param 	last	Iterator to the one-past-the-last node or value being inserted.
			///
			/// \returns	An iterator to the first newly-inserted element (or a copy of `pos` if `first` >= `last`).
			template <typename Iter>
			iterator insert(const_iterator pos, Iter first, Iter last) noexcept
			{
				const auto count = std::distance(first, last);
				if (count <= 0)
					return { elements.begin() + (pos.raw_ - elements.cbegin()) };
				else if (count == 1)
					return insert(pos, *first);
				else
				{
					const auto start_idx = static_cast<size_t>(pos.raw_ - elements.cbegin());
					preinsertion_resize(start_idx, static_cast<size_t>(count));
					size_t i = start_idx;
					for (auto it = first; it != last; it++)
						elements[i++].reset(impl::make_node(*it));
					return { elements.begin() + static_cast<ptrdiff_t>(start_idx) };
				}
			}

			/// \brief	Inserts a range of elements into the array at a specific position.
			///
			/// \tparam	ElemType	One of the TOML node or value types (or a type promotable to one).
			/// \param 	pos			The insertion position.
			/// \param 	ilist		An initializer list containing the values to be inserted.
			///
			/// \returns	An iterator to the first newly-inserted element (or a copy of `pos` if `ilist` was empty).
			template <typename ElemType>
			iterator insert(const_iterator pos, std::initializer_list<ElemType> ilist) noexcept
			{
				switch (ilist.size())
				{
					case 0: return { elements.begin() + (pos.raw_ - elements.cbegin()) };
					case 1: return insert(pos, *ilist.begin());
					default:
					{
						const auto start_idx = static_cast<size_t>(pos.raw_ - elements.cbegin());
						preinsertion_resize(start_idx, ilist.size());
						size_t i = start_idx;
						for (auto& val : ilist)
							elements[i++].reset(impl::make_node(val));
						return { elements.begin() + static_cast<ptrdiff_t>(start_idx) };
					}
				}
			}

			/// \brief	Emplaces a new element at a specific position in the array.
			///
			/// \detail \cpp
			/// auto arr = toml::array{ 1, 2 };
			///	
			///	//add a string using std::string's substring constructor
			///	arr.emplace<std::string>(arr.cbegin() + 1, "this is not a drill"sv, 14, 5);
			/// std::cout << arr << "\n";
			/// 
			/// \ecpp
			/// 
			/// \out
			/// [ 1, 'drill', 2 ]
			/// \eout
			/// 
			/// \tparam	ElemType	One of the TOML node or value types.
			/// \tparam	Args		Value constructor argument types.
			/// \param 	pos			The insertion position.
			/// \param 	args		Arguments to forward to the value's constructor.
			///
			/// \returns	An iterator to the inserted element.
			/// 
			/// \remarks There is no difference between insert() and emplace()
			/// 		 for trivial value types (floats, ints, bools).
			template <typename ElemType, typename... Args>
			iterator emplace(const_iterator pos, Args&&... args) noexcept
			{
				using type = impl::unwrap_node<ElemType>;
				static_assert(
					(impl::is_native<type> || impl::is_one_of<type, table, array>) && !impl::is_cvref<type>,
					"Emplacement type parameter must be one of:"
					TOML_SA_UNWRAPPED_NODE_TYPE_LIST
				);

				return { elements.emplace(pos.raw_, new impl::wrap_node<type>{ std::forward<Args>(args)...} ) };
			}

			/// \brief	Removes the specified element from the array.
			///
			/// \detail \cpp
			/// auto arr = toml::array{ 1, 2, 3 };
			/// std::cout << arr << "\n";
			/// 
			/// arr.erase(arr.cbegin() + 1);
			/// std::cout << arr << "\n";
			/// 
			/// \ecpp
			/// 
			/// \out
			/// [ 1, 2, 3 ]
			/// [ 1, 3 ]
			/// \eout
			/// 
			/// \param 	pos		Iterator to the element being erased.
			/// 
			/// \returns Iterator to the first element immediately following the removed element.
			iterator erase(const_iterator pos) noexcept;

			/// \brief	Removes the elements in the range [first, last) from the array.
			///
			/// \detail \cpp
			/// auto arr = toml::array{ 1, "bad", "karma" 2 };
			/// std::cout << arr << "\n";
			/// 
			/// arr.erase(arr.cbegin() + 1, arr.cbegin() + 3);
			/// std::cout << arr << "\n";
			/// 
			/// \ecpp
			/// 
			/// \out
			/// [ 1, 'bad', 'karma', 3 ]
			/// [ 1, 3 ]
			/// \eout
			/// 
			/// \param 	first	Iterator to the first element being erased.
			/// \param 	last	Iterator to the one-past-the-last element being erased.
			/// 
			/// \returns Iterator to the first element immediately following the last removed element.
			iterator erase(const_iterator first, const_iterator last) noexcept;

			/// \brief	Resizes the array.
			/// 
			/// \detail \godbolt{W5zqx3}
			/// 
			/// \cpp
			/// auto arr = toml::array{ 1, 2, 3 };
			/// std::cout << arr << "\n";
			/// 
			/// arr.resize(6, 42);
			/// std::cout << arr << "\n";
			/// 
			/// arr.resize(2, 0);
			/// std::cout << arr << "\n";
			/// 
			/// \ecpp
			/// 
			/// \out
			/// [ 1, 2, 3 ]
			/// [ 1, 2, 3, 42, 42, 42 ]
			/// [ 1, 2 ]
			/// \eout
			/// 
			/// \tparam	ElemType			One of the TOML node or value types (or a type promotable to one).
			/// 
			/// \param 	new_size			The number of elements the array will have after resizing.
			/// \param 	default_init_val	The node or value used to initialize new elements if the array needs to grow.
			template <typename ElemType>
			void resize(size_t new_size, ElemType&& default_init_val) noexcept
			{
				if (!new_size)
					elements.clear();
				else if (new_size < elements.size())
					elements.resize(new_size);
				else if (new_size > elements.size())
					insert(cend(), new_size - elements.size(), std::forward<ElemType>(default_init_val));
			}

			/// \brief	Shrinks the array to the given size.
			/// 
			/// \detail \godbolt{rxEzK5}
			/// 
			/// \cpp
			/// auto arr = toml::array{ 1, 2, 3 };
			/// std::cout << arr << "\n";
			/// 
			/// arr.truncate(5); // no-op
			/// std::cout << arr << "\n";
			/// 
			/// arr.truncate(1);
			/// std::cout << arr << "\n";
			/// 
			/// \ecpp
			/// 
			/// \out
			/// [ 1, 2, 3 ]
			/// [ 1, 2, 3 ]
			/// [ 1]
			/// \eout
			/// 
			/// \remarks	Does nothing if the requested size is larger than or equal to the current size.
			void truncate(size_t new_size);

			/// \brief	Appends a new element to the end of the array.
			///
			/// \detail \cpp
			/// auto arr = toml::array{ 1, 2 };
			///	arr.push_back(3);
			///	arr.push_back(4.0);
			///	arr.push_back(toml::array{ 5, "six"sv });
			/// std::cout << arr << "\n";
			/// 
			/// \ecpp
			/// 
			/// \out
			/// [ 1, 2, 3, 4.0, [ 5, 'six' ] ]
			/// \eout
			/// 
			/// \tparam	ElemType	One of the TOML node or value types (or a type promotable to one).
			/// \param 	val			The node or value being added.
			/// 
			/// \returns A reference to the newly-constructed element.
			template <typename ElemType>
			decltype(auto) push_back(ElemType&& val) noexcept
			{
				auto nde = impl::make_node(std::forward<ElemType>(val));
				elements.emplace_back(nde);
				return *nde;
			}

			/// \brief	Emplaces a new element at the end of the array.
			///
			/// \detail \cpp
			/// auto arr = toml::array{ 1, 2 };
			///	arr.emplace_back<toml::array>(3, "four"sv);
			/// std::cout << arr << "\n";
			/// 
			/// \ecpp
			/// 
			/// \out
			/// [ 1, 2, [ 3, 'four' ] ]
			/// \eout
			/// 
			/// \tparam	ElemType	One of the TOML node or value types.
			/// \tparam	Args		Value constructor argument types.
			/// \param 	args		Arguments to forward to the value's constructor.
			/// 
			/// \returns A reference to the newly-constructed element.
			///
			/// \remarks There is no difference between push_back() and emplace_back()
			/// 		 For trivial value types (floats, ints, bools).
			template <typename ElemType, typename... Args>
			decltype(auto) emplace_back(Args&&... args) noexcept
			{
				using type = impl::unwrap_node<ElemType>;
				static_assert(
					(impl::is_native<type> || impl::is_one_of<type, table, array>) && !impl::is_cvref<type>,
					"Emplacement type parameter must be one of:"
					TOML_SA_UNWRAPPED_NODE_TYPE_LIST
				);

				auto nde = new impl::wrap_node<type>{ std::forward<Args>(args)... };
				elements.emplace_back(nde);
				return *nde;
			}

			/// \brief	Removes the last element from the array.
			void pop_back() noexcept;

			/// \brief	Gets the element at a specific index.
			///
			/// \detail \cpp
			/// auto arr = toml::array{ 99, "bottles of beer on the wall" };
			///	std::cout << "element [0] exists: "sv << !!arr.get(0) << "\n";
			///	std::cout << "element [1] exists: "sv << !!arr.get(1) << "\n";
			///	std::cout << "element [2] exists: "sv << !!arr.get(2) << "\n";
			/// if (toml::node* val = arr.get(0))
			///		std::cout << "element [0] is an "sv << val->type() << "\n";
			/// 
			/// \ecpp
			/// 
			/// \out
			/// element [0] exists: true
			/// element [1] exists: true
			/// element [2] exists: false
			/// element [0] is an integer
			/// \eout
			/// 
			/// \param 	index	The element's index.
			///
			/// \returns	A pointer to the element at the specified index if one existed, or nullptr.
			[[nodiscard]] node* get(size_t index) noexcept;

			/// \brief	Gets the element at a specific index (const overload).
			///
			/// \param 	index	The element's index.
			///
			/// \returns	A pointer to the element at the specified index if one existed, or nullptr.
			[[nodiscard]] const node* get(size_t index) const noexcept;

			/// \brief	Gets the element at a specific index if it is a particular type.
			///
			/// \detail \cpp
			/// auto arr = toml::array{ 42, "is the meaning of life, apparently."sv };
			/// if (toml::value<int64_t>* val = arr.get_as<int64_t>(0))
			///		std::cout << "element [0] is an integer with value "sv << *val << "\n";
			/// 
			/// \ecpp
			/// 
			/// \out
			/// element [0] is an integer with value 42
			/// \eout
			/// 
			/// \tparam	ElemType	The element's type.
			/// \param 	index		The element's index.
			///
			/// \returns	A pointer to the selected element if it existed and was of the specified type, or nullptr.
			template <typename ElemType>
			[[nodiscard]]
			impl::wrap_node<ElemType>* get_as(size_t index) noexcept
			{
				if (auto val = get(index))
					return val->as<ElemType>();
				return nullptr;
			}

			/// \brief	Gets the element at a specific index if it is a particular type (const overload).
			///
			/// \tparam	ElemType	The element's type.
			/// \param 	index		The element's index.
			///
			/// \returns	A pointer to the selected element if it existed and was of the specified type, or nullptr.
			template <typename ElemType>
			[[nodiscard]]
			const impl::wrap_node<ElemType>* get_as(size_t index) const noexcept
			{
				if (auto val = get(index))
					return val->as<ElemType>();
				return nullptr;
			}

			/// \brief	Equality operator.
			///
			/// \param 	lhs	The LHS array.
			/// \param 	rhs	The RHS array.
			///
			/// \returns	True if the arrays contained the same elements.
			friend bool operator == (const array& lhs, const array& rhs) noexcept;

			/// \brief	Inequality operator.
			///
			/// \param 	lhs	The LHS array.
			/// \param 	rhs	The RHS array.
			///
			/// \returns	True if the arrays did not contain the same elements.
			friend bool operator != (const array& lhs, const array& rhs) noexcept;

		private:

			template <typename T>
			[[nodiscard]]
			static bool container_equality(const array& lhs, const T& rhs) noexcept
			{
				using element_type = std::remove_const_t<typename T::value_type>;
				static_assert(
					impl::is_native<element_type> || impl::is_losslessly_convertible_to_native<element_type>,
					"Container element type must be (or be promotable to) one of the TOML value types"
				);

				if (lhs.size() != rhs.size())
					return false;
				if (rhs.size() == 0_sz)
					return true;

				size_t i{};
				for (auto& list_elem : rhs)
				{
					const auto elem = lhs.get_as<impl::native_type_of<element_type>>(i++);
					if (!elem || *elem != list_elem)
						return false;
				}

				return true;
			}

			[[nodiscard]] size_t total_leaf_count() const noexcept;

			void flatten_child(array&& child, size_t& dest_index) noexcept;

		public:

			/// \brief	Initializer list equality operator.
			template <typename T>
			[[nodiscard]]
			friend bool operator == (const array& lhs, const std::initializer_list<T>& rhs) noexcept
			{
				return container_equality(lhs, rhs);
			}
			TOML_ASYMMETRICAL_EQUALITY_OPS(const array&, const std::initializer_list<T>&, template <typename T>)

			/// \brief	Vector equality operator.
			template <typename T>
			[[nodiscard]]
			friend bool operator == (const array& lhs, const std::vector<T>& rhs) noexcept
			{
				return container_equality(lhs, rhs);
			}
			TOML_ASYMMETRICAL_EQUALITY_OPS(const array&, const std::vector<T>&, template <typename T>)

			/// \brief	Flattens this array, recursively hoisting the contents of child arrays up into itself.
			/// 
			/// \detail \cpp
			/// 
			/// auto arr = toml::array{ 1, 2, toml::array{ 3, 4, toml::array{ 5 } }, 6, toml::array{} };
			/// std::cout << arr << "\n";
			/// 
			/// arr.flatten();
			/// std::cout << arr << "\n";
			/// 
			/// \ecpp
			/// 
			/// \out
			/// [ 1, 2, [ 3, 4, [ 5 ] ], 6, [] ]
			/// [ 1, 2, 3, 4, 5, 6 ]
			/// \eout
			/// 
			/// \remarks	Arrays inside child tables are not flattened.
			/// 
			/// \returns A reference to the array.
			array& flatten() &;

			/// \brief	 Flattens this array, recursively hoisting the contents of child arrays up into itself (rvalue overload).
			/// \returns An rvalue reference to the array.
			array&& flatten() &&
			{
				return static_cast<toml::array&&>(static_cast<toml::array&>(*this).flatten());
			}

			/// \brief	Prints the array out to a stream as formatted TOML.
			template <typename Char>
			friend std::basic_ostream<Char>& operator << (std::basic_ostream<Char>&, const array&);
	};
}
TOML_NAMESPACE_END

TOML_POP_WARNINGS //TOML_DISABLE_MISC_WARNINGS
