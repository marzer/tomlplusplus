//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "forward_declarations.h"
#include "header_start.h"

/// \cond
TOML_IMPL_NAMESPACE_START
{
	template <typename T>
	TOML_NODISCARD
	TOML_ATTR(returns_nonnull)
	auto* make_node_specialized(T && val) noexcept
	{
		using type = unwrap_node<remove_cvref_t<T>>;
		static_assert(!std::is_same_v<type, node>);
		static_assert(!is_node_view<type>);

		if constexpr (is_one_of<type, array, table>)
		{
			return new type{ static_cast<T&&>(val) };
		}
		else if constexpr (is_native<type> && !std::is_same_v<remove_cvref_t<T>, type>)
		{
			return new value<type>{ static_cast<T&&>(val) };
		}
		else
		{
			static_assert(!is_wide_string<T> || TOML_WINDOWS_COMPAT,
						  "Instantiating values from wide-character strings is only "
						  "supported on Windows with TOML_WINDOWS_COMPAT enabled.");
			static_assert(is_native<type> || is_losslessly_convertible_to_native<type>,
						  "Value initializers must be (or be promotable to) one of the TOML value types");

			using value_type = native_type_of<remove_cvref_t<T>>;
			if constexpr (is_wide_string<T>)
			{
#if TOML_WINDOWS_COMPAT
				return new value<value_type>{ narrow(static_cast<T&&>(val)) };
#else
				static_assert(dependent_false<T>, "Evaluated unreachable branch!");
#endif
			}
			else
				return new value<value_type>{ static_cast<T&&>(val) };
		}
	}

	template <typename T>
	TOML_NODISCARD
	auto* make_node(T && val) noexcept
	{
		using type = unwrap_node<remove_cvref_t<T>>;
		if constexpr (std::is_same_v<type, node> || is_node_view<type>)
		{
			if constexpr (is_node_view<type>)
			{
				if (!val)
					return static_cast<toml::node*>(nullptr);
			}

			return static_cast<T&&>(val).visit(
				[](auto&& concrete) noexcept {
					return static_cast<toml::node*>(make_node_specialized(static_cast<decltype(concrete)&&>(concrete)));
				});
		}
		else
			return make_node_specialized(static_cast<T&&>(val));
	}

	template <typename T>
	TOML_NODISCARD
	auto* make_node(inserter<T> && val) noexcept
	{
		return make_node(static_cast<T&&>(val.value));
	}

	template <typename T, bool = (is_node<T> || is_node_view<T> || is_value<T> || can_partially_represent_native<T>)>
	struct inserted_type_of_
	{
		using type = std::remove_pointer_t<decltype(make_node(std::declval<T>()))>;
	};

	template <typename T>
	struct inserted_type_of_<inserter<T>, false>
	{
		using type = typename inserted_type_of_<T>::type;
	};

	template <typename T>
	struct inserted_type_of_<T, false>
	{
		using type = void;
	};
}
TOML_IMPL_NAMESPACE_END;
/// \endcond

TOML_NAMESPACE_START
{
	/// \brief	Metafunction for determining which node type would be constructed
	///			if an object of this type was inserted into a toml::table or toml::array.
	///
	/// \detail \cpp
	/// static_assert(std::is_same_v<toml::inserted_type_of<const char*>, toml::value<std::string>);
	/// static_assert(std::is_same_v<toml::inserted_type_of<int>,         toml::value<int64_t>);
	/// static_assert(std::is_same_v<toml::inserted_type_of<float>,       toml::value<double>);
	/// static_assert(std::is_same_v<toml::inserted_type_of<bool>,        toml::value<bool>);
	/// \ecpp
	///
	/// \note	This will return toml::node for nodes and node_views, even though a more specific node subclass
	///			would actually be inserted. There is no way around this in a compile-time metafunction.
	template <typename T>
	using inserted_type_of =
		POXY_IMPLEMENTATION_DETAIL(typename impl::inserted_type_of_<impl::remove_cvref_t<T>>::type);
}
TOML_NAMESPACE_END;

#include "header_end.h"
