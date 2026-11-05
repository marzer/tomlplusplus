// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.hpp"

#include <vector>

namespace
{
	// a user-defined string type detected via its data() + length() members.
	// deliberately NOT convertible to std::string_view - detection needs no conversions.
	struct UserDefinedString
	{
		std::string value;

		explicit UserDefinedString(std::string_view sv) //
			: value{ sv }
		{}

		const char* data() const noexcept
		{
			return value.data();
		}

		size_t length() const noexcept
		{
			return value.length();
		}

		TOML_NODISCARD
		friend bool operator==(const UserDefinedString& lhs, std::string_view rhs) noexcept
		{
			return lhs.value == rhs;
		}
	};

	// ... detected via c_str() + length() (no data()), and only constructible from (const char*, size_t)
	struct UserDefinedCString
	{
		std::string value;

		UserDefinedCString(const char* str, size_t len) //
			: value{ str, len }
		{}

		const char* c_str() const noexcept
		{
			return value.c_str();
		}

		size_t length() const noexcept
		{
			return value.length();
		}
	};

	// ... only constructible from const std::string&
	struct UserDefinedStringFromString
	{
		std::string value;

		explicit UserDefinedStringFromString(const std::string& str) //
			: value{ str }
		{}

		const char* data() const noexcept
		{
			return value.data();
		}

		size_t length() const noexcept
		{
			return value.length();
		}
	};

	// ... usable as an initializer only: detected, but with no constructor suitable for retrieval
	struct UserDefinedStringInOnly
	{
		std::string value;

		explicit UserDefinedStringInOnly(const char* v) //
			: value{ v }
		{}

		const char* data() const noexcept
		{
			return value.data();
		}

		size_t length() const noexcept
		{
			return value.length();
		}
	};

	// ... a non-owning view type, nothrow-constructible from a view
	struct UserDefinedStringView
	{
		std::string_view value;

		/*implicit*/ UserDefinedStringView(std::string_view sv) noexcept //
			: value{ sv }
		{}

		const char* data() const noexcept
		{
			return value.data();
		}

		size_t length() const noexcept
		{
			return value.length();
		}
	};

	// ... deriving from std::string inherits everything needed for detection.
	// (types implicitly convertible to std::string_view are deliberately not excluded - support is
	// dispatched via traits rather than overloads, so the conversion introduces no ambiguity)
	struct UserDefinedStringDerived : std::string
	{
		using std::string::string;
	};

	// NOT string-like: no length() (size() doesn't count)
	struct NonStringNoLength
	{
		std::string value;

		const char* data() const noexcept
		{
			return value.data();
		}

		size_t size() const noexcept
		{
			return value.size();
		}
	};

	// the types below exist only to be probed by the detection traits; unevaluated SFINAE contexts
	// never odr-use their members, so they are left undefined and marked [[maybe_unused]] to satisfy
	// clang's -Wunneeded-member-function and -Wunused-member-function

	// NOT string-like: character data is not char-based
	struct NonStringWrongChar
	{
		[[maybe_unused]] const unsigned char* data() const noexcept;
		[[maybe_unused]] size_t length() const noexcept;
	};

	// NOT string-like: members are not const-qualified
	struct NonStringNonConst
	{
		[[maybe_unused]] const char* data() noexcept;
		[[maybe_unused]] size_t length() noexcept;
	};

	// NOT string-like: members are not public
	class NonStringPrivate
	{
	  public:
		[[maybe_unused]] NonStringPrivate() noexcept = default;

	  private:
		[[maybe_unused]] const char* data() const noexcept;
		[[maybe_unused]] size_t length() const noexcept;
	};
}

TEST_CASE("custom string types - detection")
{
	using impl::value_traits;

	// a type is detected as string-like when it has data() or c_str(), and length()
	static_assert(toml::is_string_like<UserDefinedString>);
	static_assert(toml::is_string_like<UserDefinedCString>);
	static_assert(toml::is_string_like<UserDefinedStringFromString>);
	static_assert(toml::is_string_like<UserDefinedStringInOnly>);
	static_assert(toml::is_string_like<UserDefinedStringView>);
	static_assert(toml::is_string_like<UserDefinedStringDerived>);

	// ... no conversion to std::string_view required
	static_assert(!std::is_convertible_v<const UserDefinedString&, std::string_view>);

	// ... and types missing either member (or with the wrong shape) are not
	static_assert(!toml::is_string_like<NonStringNoLength>);
	static_assert(!toml::is_string_like<NonStringWrongChar>);
	static_assert(!toml::is_string_like<NonStringNonConst>);
	static_assert(!toml::is_string_like<NonStringPrivate>);
	static_assert(!toml::is_string_like<std::vector<char>>);
	static_assert(value_traits<NonStringNoLength>::type == node_type::none);
	static_assert(std::is_same_v<toml::inserted_type_of<NonStringNoLength>, void>);

	// ... while the built-in types are untouched by any of this
	static_assert(!toml::is_string_like<std::string>);
	static_assert(!toml::is_string_like<std::string_view>);
	static_assert(value_traits<std::string>::is_native);
	static_assert(value_traits<std::string_view>::type == node_type::string);
	static_assert(!value_traits<int>::is_native);
	static_assert(value_traits<int>::type == node_type::integer);
	static_assert(value_traits<double>::type == node_type::floating_point);
	static_assert(std::is_same_v<toml::inserted_type_of<const char*>, toml::value<std::string>>);

	// detected types are strings, but never 'native' - storage stays std::string
	static_assert(value_traits<UserDefinedString>::type == node_type::string);
	static_assert(!value_traits<UserDefinedString>::is_native);
	static_assert(std::is_same_v<impl::native_type_of<UserDefinedString>, std::string>);
	static_assert(std::is_same_v<toml::inserted_type_of<UserDefinedString>, toml::value<std::string>>);

	// ... so toml::is_string, which asks whether a type IS a string node, stays false for them
	static_assert(!toml::is_string<UserDefinedString>);
	static_assert(toml::is_string<std::string>);

	// every detected type is usable as an initializer; being a retrieval target additionally
	// requires a constructor taking std::string_view, (const char*, size_t) or const std::string&
	static_assert(value_traits<UserDefinedString>::is_losslessly_convertible_to_native); // in
	static_assert(value_traits<UserDefinedString>::can_represent_native);				 // out

	static_assert(value_traits<UserDefinedStringInOnly>::is_losslessly_convertible_to_native);
	static_assert(!value_traits<UserDefinedStringInOnly>::can_represent_native);
	static_assert(!value_traits<UserDefinedStringInOnly>::can_partially_represent_native);

	// ... and each of the three constructor forms is sufficient on its own
	static_assert(std::is_constructible_v<UserDefinedString, std::string_view>);
	static_assert(!std::is_constructible_v<UserDefinedCString, std::string_view>);
	static_assert(std::is_constructible_v<UserDefinedCString, const char*, size_t>);
	static_assert(!std::is_constructible_v<UserDefinedStringFromString, std::string_view>);
	static_assert(!std::is_constructible_v<UserDefinedStringFromString, const char*, size_t>);
	static_assert(std::is_constructible_v<UserDefinedStringFromString, const std::string&>);
	static_assert(value_traits<UserDefinedCString>::can_represent_native);
	static_assert(value_traits<UserDefinedStringFromString>::can_represent_native);

	// cvref-qualified forms resolve the same way
	static_assert(toml::is_string_like<const UserDefinedString&>);
	static_assert(value_traits<const UserDefinedString&>::type == node_type::string);
	static_assert(value_traits<UserDefinedString&&>::can_represent_native);
}

TEST_CASE("custom string types - noexcept propagation")
{
	// UserDefinedString allocates, so retrieving one can throw
	static_assert(!impl::value_retrieval_is_nothrow<UserDefinedString>);
	static_assert(!noexcept(std::declval<const toml::node&>().value<UserDefinedString>()));

	// UserDefinedStringView does not
	static_assert(impl::value_retrieval_is_nothrow<UserDefinedStringView>);
	static_assert(noexcept(std::declval<const toml::node&>().value<UserDefinedStringView>()));

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
		auto v = toml::value{ UserDefinedString{ "hello"sv } };
		static_assert(std::is_same_v<decltype(v), toml::value<std::string>>);
		CHECK(*v == "hello"sv);
	}

	// insertion into a table
	{
		toml::table tbl;
		tbl.insert("a", UserDefinedString{ "kek"sv });
		tbl.insert_or_assign("b", UserDefinedStringInOnly{ "foo" });
		tbl.emplace<std::string>("c", UserDefinedString{ "bar"sv });
		tbl.insert("d", UserDefinedCString{ "qux", 3u }); // character data provided by c_str()
		tbl.insert("e", UserDefinedStringFromString{ "baz" });

		REQUIRE(tbl["a"].is_string());
		CHECK(tbl["a"] == "kek"sv);
		CHECK(tbl["b"] == "foo"sv);
		CHECK(tbl["c"] == "bar"sv);
		CHECK(tbl["d"] == "qux"sv);
		CHECK(tbl["e"] == "baz"sv);
	}

	// pushing into an array
	{
		toml::array arr;
		arr.push_back(UserDefinedString{ "one"sv });
		arr.emplace_back<std::string>(UserDefinedString{ "two"sv });

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
		tbl.insert_or_assign("x", UserDefinedString{ "replaced"sv });
		CHECK(tbl["x"] == "replaced"sv);
	}

	// a non-owning view type works as an initializer too (the stored value is a copy)
	{
		toml::table tbl;
		{
			const std::string owner{ "borrowed" };
			tbl.insert("v", UserDefinedStringView{ owner });
		}
		CHECK(tbl["v"] == "borrowed"sv);
	}

	// a type derived from std::string is detected like any other string-like
	{
		toml::table tbl;
		tbl.insert("d", UserDefinedStringDerived{ "inherited" });
		CHECK(tbl["d"] == "inherited"sv);
	}

	// a type that is *not* detected is still usable, just not implicitly - the caller
	// converts at the call site, which is exactly the status quo this feature removes
	{
		const NonStringNoLength n{ "manual" };

		toml::table tbl;
		tbl.insert("k", std::string_view{ n.data(), n.size() });
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
		const auto val = tbl["name"].value<UserDefinedString>();
		REQUIRE(val.has_value());
		CHECK(*val == "toml++"sv);
	}

	// node::value_exact()
	{
		const auto val = tbl["name"].value_exact<UserDefinedString>();
		REQUIRE(val.has_value());
		CHECK(*val == "toml++"sv);
	}

	// node::value_or()
	{
		CHECK(tbl["name"].value_or(UserDefinedString{ "fallback"sv }) == "toml++"sv);
		CHECK(tbl["nope"].value_or(UserDefinedString{ "fallback"sv }) == "fallback"sv);
	}

	// wrong node type yields nullopt, same as for std::string
	{
		CHECK(!tbl["count"].value<UserDefinedString>().has_value());
		CHECK(!tbl["count"].value_exact<UserDefinedString>().has_value());
		CHECK(tbl["count"].value_or(UserDefinedString{ "fallback"sv }) == "fallback"sv);
	}

	// a type constructed via (const char*, size_t)
	{
		const auto val = tbl["name"].value<UserDefinedCString>();
		REQUIRE(val.has_value());
		CHECK(val->value == "toml++"sv);
	}

	// a type constructed via const std::string&
	{
		const auto val = tbl["name"].value<UserDefinedStringFromString>();
		REQUIRE(val.has_value());
		CHECK(val->value == "toml++"sv);
	}

	// a non-allocating view type sees the stored string
	{
		const auto val = tbl["name"].value<UserDefinedStringView>();
		REQUIRE(val.has_value());
		CHECK(val->value == "toml++"sv);
		CHECK(val->value.data() == tbl["name"].as_string()->get().data());
	}

	// a type derived from std::string
	{
		const auto val = tbl["name"].value<UserDefinedStringDerived>();
		REQUIRE(val.has_value());
		CHECK(*val == "toml++"sv);
	}
}
