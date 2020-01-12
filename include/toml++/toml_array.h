#pragma once
#include "toml_node.h"

namespace toml::impl
{
	template <bool is_const>
	class array_iterator final
	{
		private:
			friend class toml::array;

			using raw_iterator = std::conditional_t<
				is_const,
				std::vector<std::unique_ptr<node>>::const_iterator,
				std::vector<std::unique_ptr<node>>::iterator
			>;
				
			mutable raw_iterator raw_;

			array_iterator(const raw_iterator& raw) noexcept
				: raw_{ raw }
			{}

			array_iterator(raw_iterator&& raw) noexcept
				: raw_{ std::move(raw) }
			{}

		public:

			using reference = std::conditional_t<is_const, const node&, node&>;

			array_iterator() noexcept = default;

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
			reference operator* () const noexcept
			{
				return *raw_->get();
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
	};
}

namespace toml
{
	/// \brief	A TOML array.
	/// \detail The interface of this type is modeled after std::vector so things
	/// 		mostly work as you'd expect them to with a vector: \cpp
	/// 		
	/// auto table = toml::parse("arr = [1, 2, 3, 4, 'five']"sv);
	/// 
	/// auto& arr = *table.get_as<toml::array>("arr");
	/// 
	/// for (size_t i = 0; i < arr.size(); i++)
	/// {
	///		arr[i].visit([=](auto&& el) noexcept
	///		{
	///			std::cout << el << ", ";
	///		});
	/// }
	/// 
	/// // prints: 1, 2, 3, 4, "five"
	/// 
	/// \ecpp
	class array final
		: public node
	{
		private:
			friend class impl::parser;
			std::vector<std::unique_ptr<node>> values;

		public:

			using value_type = node;
			using size_type = size_t;
			using difference_type = ptrdiff_t;
			using reference = node&;
			using const_reference = const node&;
			using iterator = impl::array_iterator<false>;
			using const_iterator = impl::array_iterator<true>;

			TOML_NODISCARD_CTOR
			array() noexcept = default;

			TOML_NODISCARD_CTOR
			array(array&& other) noexcept
				: node{ std::move(other) },
				values{ std::move(other.values) }
			{}

			array& operator= (array&& rhs) noexcept
			{
				node::operator=(std::move(rhs));
				values = std::move(rhs.values);
				return *this;
			}

			[[nodiscard]] node_type type() const noexcept override { return node_type::array; }
			[[nodiscard]] bool is_table() const noexcept override { return false; }
			[[nodiscard]] bool is_array() const noexcept override { return true; }
			[[nodiscard]] bool is_value() const noexcept override { return false; }
			[[nodiscard]] array* as_array() noexcept override { return this; }
			[[nodiscard]] const array* as_array() const noexcept override { return this; }

			/// \brief	Checks if the array contains values of only one type.
			///
			/// \tparam	T	A TOML node type. Provide an explicit type for "is every element a T?".
			/// 			Leave it as the default `void` for "is every element the same type?".
			///
			/// \returns	True if the array was homogeneous.
			/// 
			/// \attention	Empty arrays are _not_ regarded as homogeneous.
			template <typename T = void>
			[[nodiscard]] bool is_homogeneous() const noexcept
			{
				if (values.empty())
					return false;

				if constexpr (std::is_same_v<T, void>)
				{
					const auto type = values[0]->type();
					for (size_t i = 1; i < values.size(); i++)
						if (values[i]->type() != type)
							return false;
				}
				else
				{
					for (auto& v : values)
						if (!v->is<T>())
							return false;
				}
				return true;
			}

			/// \brief	Returns true if this array contains only tables.
			[[nodiscard]] TOML_ALWAYS_INLINE
			bool is_array_of_tables() const noexcept override
			{
				return is_homogeneous<toml::table>();
			}

			/// \brief	Gets a reference to the node element at a specific index.
			[[nodiscard]] node& operator[] (size_t index) noexcept { return *values[index]; }
			/// \brief	Gets a reference to the node element at a specific index.
			[[nodiscard]] const node& operator[] (size_t index) const noexcept { return *values[index]; }

			/// \brief	Returns a reference to the first element in the array.
			[[nodiscard]] node& front() noexcept { return *values.front(); }
			/// \brief	Returns a reference to the first element in the array.
			[[nodiscard]] const node& front() const noexcept { return *values.front(); }
			/// \brief	Returns a reference to the last element in the array.
			[[nodiscard]] node& back() noexcept { return *values.back(); }
			/// \brief	Returns a reference to the last element in the array.
			[[nodiscard]] const node& back() const noexcept { return *values.back(); }

			/// \brief	Returns an iterator to the first element.
			[[nodiscard]] iterator begin() noexcept { return { values.begin() }; }
			/// \brief	Returns an iterator to the first element.
			[[nodiscard]] const_iterator begin() const noexcept { return { values.begin() }; }
			/// \brief	Returns an iterator to the first element.
			[[nodiscard]] const_iterator cbegin() const noexcept { return { values.cbegin() }; }

			/// \brief	Returns an iterator to one-past-the-last element.
			[[nodiscard]] iterator end() noexcept { return { values.end() }; }
			/// \brief	Returns an iterator to one-past-the-last element.
			[[nodiscard]] const_iterator end() const noexcept { return { values.end() }; }
			/// \brief	Returns an iterator to one-past-the-last element.
			[[nodiscard]] const_iterator cend() const noexcept { return { values.cend() }; }

			/// \brief	Returns true if the array is empty.
			[[nodiscard]] bool empty() const noexcept { return values.empty(); }
			/// \brief	Returns the number of elements in the array.
			[[nodiscard]] size_t size() const noexcept { return values.size(); }
			/// \brief	Reserves internal storage capacity up to a pre-determined number of elements.
			void reserve(size_t new_capacity) TOML_MAY_THROW { values.reserve(new_capacity); }


			/// \brief	Removes all elements from the array.
			void clear() noexcept { values.clear(); }

			// insert()
			// emplace()

			/// \brief	Removes the specified element from the array.
			/// 		
			/// \returns The position following the Iterator following the removed element.
			iterator erase(const_iterator pos) noexcept
			{
				return iterator{ values.erase(pos.raw_) };
			}

			/// \brief	Removes the specified range of elements elements from the array.
			/// 
			/// \returns The position following the Iterator following the last removed element.
			iterator erase(const_iterator first, const_iterator last) noexcept
			{
				return iterator{ values.erase(first.raw_, last.raw_) };
			}

			// push_back()
			// emplace_back()

			/// \brief	Removes the last element from the array.
			void pop_back() noexcept { values.pop_back(); }

			/// \brief	Gets an element at a specific index if it is a particular type.
			///
			/// \tparam	T	The node's type.
			/// \param 	index	The element index.
			///
			/// \returns	A pointer to the selected element if it was of the specified type, or nullptr.
			template <typename T>
			[[nodiscard]] node_of<T>* get_as(size_t index) noexcept
			{
				return values[index]->as<T>();
			}

			/// \brief	Gets an element at a specific index if it is a particular type.
			///
			/// \tparam	T	The node's type.
			/// \param 	index	The element index.
			///
			/// \returns	A pointer to the selected element if it was of the specified type, or nullptr.
			template <typename T>
			[[nodiscard]] const node_of<T>* get_as(size_t index) const noexcept
			{
				return values[index]->as<T>();
			}

			template <typename CHAR>
			friend inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>&, const array&) TOML_MAY_THROW;
	};
}
