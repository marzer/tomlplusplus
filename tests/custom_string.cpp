// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"

namespace
{
	// a user-defined string type supporting both directions
	struct udstring
	{
		std::string value;

		explicit udstring(std::string_view sv) //
			: value{ sv }
		{}

		/*implicit*/ operator std::string_view() const noexcept
		{
			return std::string_view{ value };
		}

		TOML_NODISCARD
		friend bool operator==(const udstring& lhs, std::string_view rhs) noexcept
		{
			return lhs.value == rhs;
		}
	};

	// ... usable as an initializer only: convertible to a view, but constructible from
	// neither std::string_view nor std::string
	struct udstring_in_only
	{
		std::string value;

		explicit udstring_in_only(const char* v) //
			: value{ v }
		{}

		/*implicit*/ operator std::string_view() const noexcept
		{
			return std::string_view{ value };
		}
	};

	// ... usable as a retrieval target only (no conversion to string_view)
	struct udstring_out_only
	{
		std::string value;

		explicit udstring_out_only(std::string_view sv) //
			: value{ sv }
		{}
	};

	// ... converts via std::string rather than std::string_view in *both* directions.
	// note this type is deliberately NOT convertible to/from std::string_view: a chain through
	// operator std::string() would need two user-defined conversions, which is ill-formed.
	struct udstring_via_string
	{
		std::string value;

		explicit udstring_via_string(std::string v) //
			: value{ std::move(v) }
		{}

		/*implicit*/ operator std::string() const
		{
			return value;
		}
	};

	// ... convertible both ways, but deliberately NOT opted in
	struct udstring_unregistered
	{
		std::string value;

		explicit udstring_unregistered(std::string_view sv) //
			: value{ sv }
		{}

		/*implicit*/ operator std::string_view() const noexcept
		{
			return std::string_view{ value };
		}
	};

	// ... opted in, and nothrow-constructible from a view
	struct udstring_view
	{
		std::string_view value;

		/*implicit*/ udstring_view(std::string_view sv) noexcept //
			: value{ sv }
		{}

		/*implicit*/ operator std::string_view() const noexcept
		{
			return value;
		}
	};
}

namespace toml
{
	template <>
	struct string_like<udstring> : std::true_type
	{};
	template <>
	struct string_like<udstring_in_only> : std::true_type
	{};
	template <>
	struct string_like<udstring_out_only> : std::true_type
	{};
	template <>
	struct string_like<udstring_view> : std::true_type
	{};
	template <>
	struct string_like<udstring_via_string> : std::true_type
	{};
}

TEST_CASE("custom string types - traits")
{
	using impl::value_traits;

	// opting in is necessary...
	static_assert(!toml::string_like<udstring_unregistered>::value);
	static_assert(value_traits<udstring_unregistered>::type == node_type::none);
	static_assert(!value_traits<udstring_unregistered>::is_losslessly_convertible_to_native);
	static_assert(!value_traits<udstring_unregistered>::can_represent_native);
	static_assert(std::is_same_v<toml::inserted_type_of<udstring_unregistered>, void>);

	// ... and the built-in types are untouched by any of this
	static_assert(value_traits<std::string>::is_native);
	static_assert(value_traits<std::string_view>::type == node_type::string);
	static_assert(!value_traits<int>::is_native);
	static_assert(value_traits<int>::type == node_type::integer);
	static_assert(value_traits<double>::type == node_type::floating_point);
	static_assert(std::is_same_v<toml::inserted_type_of<const char*>, toml::value<std::string>>);

	// registered types are strings, but never 'native' - storage stays std::string
	static_assert(value_traits<udstring>::type == node_type::string);
	static_assert(!value_traits<udstring>::is_native);
	static_assert(std::is_same_v<impl::native_type_of<udstring>, std::string>);
	static_assert(std::is_same_v<toml::inserted_type_of<udstring>, toml::value<std::string>>);

	// ... so toml::is_string, which asks whether a type IS a string node, stays false for them
	static_assert(!toml::is_string<udstring>);
	static_assert(toml::is_string<std::string>);

	// the two directions are tracked independently
	static_assert(value_traits<udstring>::is_losslessly_convertible_to_native); // in
	static_assert(value_traits<udstring>::can_represent_native);			   // out

	static_assert(!std::is_constructible_v<udstring_in_only, std::string_view>);
	static_assert(!std::is_constructible_v<udstring_in_only, const std::string&>);
	static_assert(value_traits<udstring_in_only>::is_losslessly_convertible_to_native);
	static_assert(!value_traits<udstring_in_only>::can_represent_native);
	static_assert(!value_traits<udstring_in_only>::can_partially_represent_native);

	static_assert(!value_traits<udstring_out_only>::is_losslessly_convertible_to_native);
	static_assert(value_traits<udstring_out_only>::can_represent_native);

	// cvref-qualified forms resolve the same way
	static_assert(value_traits<const udstring&>::type == node_type::string);
	static_assert(value_traits<udstring&&>::can_represent_native);

	// a type that converts via std::string rather than std::string_view is supported in both directions,
	// even though it is convertible to neither a view nor constructible from one
	static_assert(!std::is_convertible_v<const udstring_via_string&, std::string_view>);
	static_assert(!std::is_constructible_v<udstring_via_string, std::string_view>);
	static_assert(std::is_convertible_v<const udstring_via_string&, std::string>);
	static_assert(std::is_constructible_v<udstring_via_string, const std::string&>);

	static_assert(value_traits<udstring_via_string>::type == node_type::string);
	static_assert(value_traits<udstring_via_string>::is_losslessly_convertible_to_native);
	static_assert(value_traits<udstring_via_string>::can_represent_native);
}

TEST_CASE("custom string types - noexcept propagation")
{
	// udstring allocates, so retrieving one can throw
	static_assert(!impl::value_retrieval_is_nothrow<udstring>);
	static_assert(!noexcept(std::declval<const toml::node&>().value<udstring>()));

	// udstring_view does not
	static_assert(impl::value_retrieval_is_nothrow<udstring_view>);
	static_assert(noexcept(std::declval<const toml::node&>().value<udstring_view>()));

	// unchanged for the built-ins
	static_assert(!impl::value_retrieval_is_nothrow<std::string>);
	static_assert(impl::value_retrieval_is_nothrow<std::string_view>);
	static_assert(impl::value_retrieval_is_nothrow<const char*>);
	static_assert(impl::value_retrieval_is_nothrow<int64_t>);
	static_assert(impl::value_retrieval_is_nothrow<double>);
}

TEST_CASE("custom string types - as initializers")
{
	// direct construction of a value
	{
		auto v = toml::value{ udstring{ "hello"sv } };
		static_assert(std::is_same_v<decltype(v), toml::value<std::string>>);
		CHECK(*v == "hello"sv);
	}

	// insertion into a table
	{
		toml::table tbl;
		tbl.insert("a", udstring{ "kek"sv });
		tbl.insert_or_assign("b", udstring_in_only{ "foo" });
		tbl.emplace<std::string>("c", udstring{ "bar"sv });

		REQUIRE(tbl["a"].is_string());
		CHECK(tbl["a"] == "kek"sv);
		CHECK(tbl["b"] == "foo"sv);
		CHECK(tbl["c"] == "bar"sv);
	}

	// pushing into an array
	{
		toml::array arr;
		arr.push_back(udstring{ "one"sv });
		arr.emplace_back<std::string>(udstring{ "two"sv });

		REQUIRE(arr.size() == 2u);
		REQUIRE(arr[0].as_string() != nullptr);
		REQUIRE(arr[1].as_string() != nullptr);
		CHECK(arr[0].as_string()->get() == "one"sv);
		CHECK(arr[1].as_string()->get() == "two"sv);
	}

	// overwriting an existing key
	{
		toml::table tbl;
		tbl.insert("x", "original"sv);
		tbl.insert_or_assign("x", udstring{ "replaced"sv });
		CHECK(tbl["x"] == "replaced"sv);
	}

	// assigning to an existing value
	{
		toml::value<std::string> v{ "before"sv };
		v = udstring{ "after"sv };
		CHECK(*v == "after"sv);
	}

	// a non-owning view type works as an initializer too (the stored value is a copy)
	{
		static_assert(impl::string_like_is_initializer<udstring_view>);

		toml::table tbl;
		{
			const std::string owner{ "borrowed" };
			tbl.insert("v", udstring_view{ owner });
		}
		CHECK(tbl["v"] == "borrowed"sv);
	}

	// a type that has *not* opted in is still usable, just not implicitly - the caller
	// converts at the call site, which is exactly the status quo this feature removes
	{
		const udstring_unregistered u{ "manual"sv };

		toml::table tbl;
		tbl.insert("k", std::string_view{ u });
		CHECK(tbl["k"] == "manual"sv);
	}
}

TEST_CASE("custom string types - as retrieval targets")
{
	static constexpr auto toml_text = R"(
		name = "toml++"
		count = 42
	)"sv;

	auto res = toml::parse(toml_text);
#if !TOML_EXCEPTIONS
	REQUIRE(res.succeeded());
#endif
	table& tbl = res;

	// node::value()
	{
		const auto val = tbl["name"].value<udstring>();
		REQUIRE(val.has_value());
		CHECK(*val == "toml++"sv);
	}

	// node::value_exact()
	{
		const auto val = tbl["name"].value_exact<udstring>();
		REQUIRE(val.has_value());
		CHECK(*val == "toml++"sv);
	}

	// node::value_or()
	{
		CHECK(tbl["name"].value_or(udstring{ "fallback"sv }) == "toml++"sv);
		CHECK(tbl["nope"].value_or(udstring{ "fallback"sv }) == "fallback"sv);
	}

	// wrong node type yields nullopt, same as for std::string
	{
		CHECK(!tbl["count"].value<udstring>().has_value());
		CHECK(!tbl["count"].value_exact<udstring>().has_value());
		CHECK(tbl["count"].value_or(udstring{ "fallback"sv }) == "fallback"sv);
	}

	// a retrieval-target-only type
	{
		const auto val = tbl["name"].value<udstring_out_only>();
		REQUIRE(val.has_value());
		CHECK(val->value == "toml++"sv);
	}

	// a non-allocating view type sees the stored string
	{
		const auto val = tbl["name"].value<udstring_view>();
		REQUIRE(val.has_value());
		CHECK(val->value == "toml++"sv);
		CHECK(val->value.data() == tbl["name"].as_string()->get().data());
	}

	// a type that only converts via std::string, in both directions
	{
		const auto val = tbl["name"].value<udstring_via_string>();
		REQUIRE(val.has_value());
		CHECK(val->value == "toml++"sv);

		toml::table out;
		out.insert("k", udstring_via_string{ "round-tripped" });
		CHECK(out["k"] == "round-tripped"sv);
	}
}
