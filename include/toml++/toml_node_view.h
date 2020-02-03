#pragma once
#include "toml_table.h"
#include "toml_array.h"
#include "toml_value.h"

namespace toml::impl
{
	template <typename T>
	struct node_view_traits;

	template <>
	struct node_view_traits<const table>
	{
		using haystack_type = const table*;
		using key_type = string_view;

		[[nodiscard]] static const node* get(const table* tbl, key_type key) noexcept
		{
			return tbl->get(key);
		}

		template <typename U>
		[[nodiscard]] static const node_of<U>* as(const table* tbl, key_type key) noexcept
		{
			return tbl->get_as<U>(key);
		}
	};

	template <>
	struct node_view_traits<table>
	{
		using haystack_type = table*;
		using key_type = string_view;

		[[nodiscard]] static node* get(table* tbl, key_type key) noexcept
		{
			return tbl->get(key);
		}

		[[nodiscard]] static const node* get(const table* tbl, key_type key) noexcept
		{
			return tbl->get(key);
		}

		template <typename T>
		[[nodiscard]] static node_of<T>* as(table* tbl, key_type key) noexcept
		{
			return tbl->get_as<T>(key);
		}

		template <typename T>
		[[nodiscard]] static const node_of<T>* as(const table* tbl, key_type key) noexcept
		{
			return tbl->get_as<T>(key);
		}
	};

	template <typename T, typename K>
	struct sub_view final { };

	template <typename T>
	struct node_view_traits<sub_view<T, string_view>>
	{
		using haystack_type = T;
		using key_type = string_view;

		[[nodiscard]] static auto get(haystack_type& view, string_view key) noexcept
		{
			auto parent = view.as_table();
			return parent ? parent->get(key) : nullptr;
		}

		[[nodiscard]] static const node* get(const haystack_type& view, string_view key) noexcept
		{
			auto parent = view.as_table();
			return parent ? parent->get(key) : nullptr;
		}

		template <typename U>
		[[nodiscard]] static auto as(haystack_type& view, string_view key) noexcept
		{
			auto parent = view.as_table();
			return parent ? parent->template get_as<U>(key) : nullptr;
		}

		template <typename U>
		[[nodiscard]] static const node_of<U>* as(const haystack_type& view, string_view key) noexcept
		{
			auto parent = view.as_table();
			return parent ? parent->template get_as<U>(key) : nullptr;
		}
	};

	template <typename T>
	struct node_view_traits<sub_view<T, size_t>>
	{
		using haystack_type = T;
		using key_type = size_t;

		[[nodiscard]] static auto get(haystack_type& view, size_t index) noexcept
		{
			auto parent = view.as_array();
			return parent ? parent->get(index) : nullptr;
		}

		[[nodiscard]] static const node* get(const haystack_type& view, size_t index) noexcept
		{
			auto parent = view.as_array();
			return parent ? parent->get(index) : nullptr;
		}

		template <typename U>
		[[nodiscard]] static auto as(haystack_type& view, size_t index) noexcept
		{
			auto parent = view.as_array();
			return parent ? parent->template get_as<U>(index) : nullptr;
		}

		template <typename U>
		[[nodiscard]] static const node_of<U>* as(const haystack_type& view, size_t index) noexcept
		{
			auto parent = view.as_array();
			return parent ? parent->template get_as<U>(index) : nullptr;
		}
	};
}

namespace toml
{
	template <typename T>
	class node_view final
	{
		public:
			using traits = impl::node_view_traits<T>;
			using key_type = typename traits::key_type;

		private:
			using haystack_type = typename traits::haystack_type;
			haystack_type haystack_;
			key_type key_;

		public:

			TOML_NODISCARD_CTOR
			node_view(haystack_type obj, key_type key) noexcept
				: haystack_{ obj },
				key_{ key }
			{}

			[[nodiscard]] auto get() noexcept { return traits::get(haystack_, key_); }
			[[nodiscard]] const node* get() const noexcept { return traits::get(haystack_, key_); }

			[[nodiscard]] explicit operator bool() const noexcept { return !!get(); }

			template <typename U>
			[[nodiscard]] auto as() noexcept
			{
				static_assert(
					impl::is_value_or_node<impl::unwrapped<U>>,
					"Template type parameter must be one of the basic value types, a toml::table, or a toml::array"
				);

				return traits::template as<U>(haystack_, key_);
			}

			template <typename U>
			[[nodiscard]] const node_of<U>* as() const noexcept
			{
				static_assert(
					impl::is_value_or_node<impl::unwrapped<U>>,
					"Template type parameter must be one of the basic value types, a toml::table, or a toml::array"
				);

				return traits::template as<U>(haystack_, key_);
			}

			[[nodiscard]] auto as_string() noexcept { return as<string>(); }
			[[nodiscard]] auto as_integer() noexcept { return as<int64_t>(); }
			[[nodiscard]] auto as_floating_point() noexcept { return as<double>(); }
			[[nodiscard]] auto as_boolean() noexcept { return as<bool>(); }
			[[nodiscard]] auto as_date() noexcept { return as<date>(); }
			[[nodiscard]] auto as_time() noexcept { return as<time>(); }
			[[nodiscard]] auto as_date_time() noexcept { return as<date_time>(); }
			[[nodiscard]] auto as_array() noexcept { return as<array>(); }
			[[nodiscard]] auto as_table() noexcept { return as<table>(); }

			[[nodiscard]] const value<string>* as_string() const noexcept { return as<string>(); }
			[[nodiscard]] const value<int64_t>* as_integer() const noexcept { return as<int64_t>(); }
			[[nodiscard]] const value<double>* as_floating_point() const noexcept { return as<double>(); }
			[[nodiscard]] const value<bool>* as_boolean() const noexcept { return as<bool>(); }
			[[nodiscard]] const value<date>* as_date() const noexcept { return as<date>(); }
			[[nodiscard]] const value<time>* as_time() const noexcept { return as<time>(); }
			[[nodiscard]] const value<date_time>* as_date_time() const noexcept { return as<date_time>(); }
			[[nodiscard]] const array* as_array() const noexcept { return as<array>(); }
			[[nodiscard]] const table* as_table() const noexcept { return as<table>(); }

		private:

			template <typename U>
			[[nodiscard]] static bool value_equality(const node_view& lhs, const U& rhs) noexcept
			{
				const auto val = lhs.as<impl::promoted<U>>();
				return val && val->get() == rhs;
			}

			template <typename U>
			[[nodiscard]] static bool container_equality(const node_view& lhs, const U& rhs) noexcept
			{
				using elem_t = std::remove_const_t<typename U::value_type>;
				static_assert(
					impl::is_value_or_promotable<elem_t>,
					"Container element type must be (or be promotable to) one of the TOML value types"
				);

				const array* arr = lhs.as<array>();
				if (!arr || arr->size() != rhs.size())
					return false;
				if (rhs.size() == 0_sz)
					return true;

				size_t i{};
				for (auto& list_elem : rhs)
				{
					const auto elem = arr->get_as<impl::promoted<elem_t>>(i++);
					if (!elem || elem->get() != list_elem)
						return false;
				}

				return true;
			}

		public:

			[[nodiscard]] bool operator == (string_view rhs) const noexcept { return value_equality(*this, rhs); }
			[[nodiscard]] bool operator == (int64_t rhs) const noexcept { return value_equality(*this, rhs); }
			[[nodiscard]] bool operator == (int32_t rhs) const noexcept { return value_equality(*this, rhs); }
			[[nodiscard]] bool operator == (int16_t rhs) const noexcept { return value_equality(*this, rhs); }
			[[nodiscard]] bool operator == (int8_t rhs) const noexcept { return value_equality(*this, rhs); }
			[[nodiscard]] bool operator == (uint32_t rhs) const noexcept { return value_equality(*this, rhs); }
			[[nodiscard]] bool operator == (uint16_t rhs) const noexcept { return value_equality(*this, rhs); }
			[[nodiscard]] bool operator == (uint8_t rhs) const noexcept { return value_equality(*this, rhs); }
			[[nodiscard]] bool operator == (double rhs) const noexcept { return value_equality(*this, rhs); }
			[[nodiscard]] bool operator == (float rhs) const noexcept { return value_equality(*this, rhs); }
			[[nodiscard]] bool operator == (bool rhs) const noexcept { return value_equality(*this, rhs); }
			[[nodiscard]] bool operator == (const date& rhs) const noexcept { return value_equality(*this, rhs); }
			[[nodiscard]] bool operator == (const time& rhs) const noexcept { return value_equality(*this, rhs); }
			[[nodiscard]] bool operator == (const date_time& rhs) const noexcept { return value_equality(*this, rhs); }

			template <typename U>
			[[nodiscard]] bool operator == (const std::initializer_list<U>& rhs) const noexcept
			{
				return container_equality(*this, rhs);
			}

			template <typename U>
			[[nodiscard]] bool operator == (const std::vector<U>& rhs) const noexcept
			{
				return container_equality(*this, rhs);
			}

			template <typename U>
			[[nodiscard]] friend bool operator == (const U& lhs, const node_view& rhs) noexcept
			{
				return rhs == lhs;
			}

			[[nodiscard]] node_view<impl::sub_view<node_view<T>, string_view>> operator[] (string_view key) noexcept
			{
				return { *this, key };
			}

			[[nodiscard]] node_view<impl::sub_view<node_view<T>, size_t>> operator[] (size_t index) noexcept
			{
				return { *this, index };
			}
	};

	inline node_view<table> table::operator[] (string_view key) noexcept
	{
		return { this, key };
	}

	inline node_view<const table> table::operator[] (string_view key) const noexcept
	{
		return { this, key };
	}
}
