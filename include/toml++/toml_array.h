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

			array_iterator() noexcept = default;

			using reference = std::conditional_t<is_const, const node&, node&>;

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
	class array final
		: public node
	{
		private:
			friend class impl::parser;
			std::vector<std::unique_ptr<node>> values;

		public:

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

			[[nodiscard]] bool is_array_of_tables() const noexcept override
			{
				if (values.empty())
					return false;

				for (auto& val : values)
					if (!val->is_table())
						return false;

				return true;
			}

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

			[[nodiscard]] array* as_array() noexcept override { return this; }
			[[nodiscard]] const array* as_array() const noexcept override { return this; }

			[[nodiscard]] bool empty() const noexcept { return values.empty(); }
			[[nodiscard]] size_t size() const noexcept { return values.size(); }

			[[nodiscard]] node& operator[] (size_t index) & noexcept { return *values[index].get(); }
			[[nodiscard]] node&& operator[] (size_t index) && noexcept { return std::move(*values[index].get()); }
			[[nodiscard]] const node& operator[] (size_t index) const& noexcept { return *values[index].get(); }

			template <typename T>
			[[nodiscard]] node_of<T>* get_as(size_t index) noexcept
			{
				return values[index]->as<T>();
			}

			template <typename T>
			[[nodiscard]] const node_of<T>* get_as(size_t index) const noexcept
			{
				return values[index]->as<T>();
			}

			[[nodiscard]] iterator begin() noexcept { return { values.begin() }; }
			[[nodiscard]] const_iterator begin() const noexcept { return { values.begin() }; }
			[[nodiscard]] const_iterator cbegin() const noexcept { return { values.cbegin() }; }

			[[nodiscard]] iterator end() noexcept { return { values.end() }; }
			[[nodiscard]] const_iterator end() const noexcept { return { values.end() }; }
			[[nodiscard]] const_iterator cend() const noexcept { return { values.cend() }; }
	};
}
