// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#include "tests.h"

#ifdef _WIN32
TOML_DISABLE_WARNINGS;
#include <windows.h>
TOML_ENABLE_WARNINGS;
#endif

TOML_DISABLE_SPAM_WARNINGS;

template <typename T>
static constexpr T one = static_cast<T>(1);

TEST_CASE("values - construction")
{
	#define CHECK_VALUE_INIT2(initializer, target_type, equiv)				\
	do {																	\
		auto v = value{ initializer };										\
		static_assert(std::is_same_v<decltype(v), value<target_type>>);		\
		CHECK(v == equiv);													\
		CHECK(equiv == v);													\
		CHECK(*v == equiv);													\
		CHECK(v.get() == equiv);											\
		CHECK(v.is_homogeneous());											\
		CHECK(v.is_homogeneous<target_type>());								\
		CHECK(v.is_homogeneous(impl::node_type_of<target_type>));			\
	} while (false)

	#define CHECK_VALUE_INIT(initializer, target_type)						\
		CHECK_VALUE_INIT2(initializer, target_type, initializer)

	CHECK_VALUE_INIT(one<signed char>,			int64_t);
	CHECK_VALUE_INIT(one<signed short>,			int64_t);
	CHECK_VALUE_INIT(one<signed int>,			int64_t);
	CHECK_VALUE_INIT(one<signed long>,			int64_t);
	CHECK_VALUE_INIT(one<signed long long>,		int64_t);
	CHECK_VALUE_INIT2(one<unsigned char>,		int64_t,		1u);
	CHECK_VALUE_INIT2(one<unsigned short>,		int64_t,		1u);
	CHECK_VALUE_INIT2(one<unsigned int>,		int64_t,		1u);
	CHECK_VALUE_INIT2(one<unsigned long>,		int64_t,		1u);
	CHECK_VALUE_INIT2(one<unsigned long long>,	int64_t,		1u);
	CHECK_VALUE_INIT(true,						bool);
	CHECK_VALUE_INIT(false,						bool);
	CHECK_VALUE_INIT("kek",						std::string);
	CHECK_VALUE_INIT("kek"s,					std::string);
	CHECK_VALUE_INIT("kek"sv,					std::string);
	CHECK_VALUE_INIT2("kek"sv.data(),			std::string,	"kek"sv);
	#if TOML_HAS_CHAR8
	CHECK_VALUE_INIT2(u8"kek",					std::string,	"kek"sv);
	CHECK_VALUE_INIT2(u8"kek"s,					std::string,	"kek"sv);
	CHECK_VALUE_INIT2(u8"kek"sv,				std::string,	"kek"sv);
	CHECK_VALUE_INIT2(u8"kek"sv.data(),			std::string,	"kek"sv);
	#endif

	#ifdef _WIN32
	CHECK_VALUE_INIT(one<BOOL>,					int64_t);
	CHECK_VALUE_INIT(one<SHORT>,				int64_t);
	CHECK_VALUE_INIT(one<INT>,					int64_t);
	CHECK_VALUE_INIT(one<LONG>,					int64_t);
	CHECK_VALUE_INIT(one<INT_PTR>,				int64_t);
	CHECK_VALUE_INIT(one<LONG_PTR>,				int64_t);
	CHECK_VALUE_INIT2(one<USHORT>,				int64_t,		1u);
	CHECK_VALUE_INIT2(one<UINT>,				int64_t,		1u);
	CHECK_VALUE_INIT2(one<ULONG>,				int64_t,		1u);
	CHECK_VALUE_INIT2(one<UINT_PTR>,			int64_t,		1u);
	CHECK_VALUE_INIT2(one<ULONG_PTR>,			int64_t,		1u);
	CHECK_VALUE_INIT2(one<WORD>,				int64_t,		1u);
	CHECK_VALUE_INIT2(one<DWORD>,				int64_t,		1u);
	CHECK_VALUE_INIT2(one<DWORD32>,				int64_t,		1u);
	CHECK_VALUE_INIT2(one<DWORD64>,				int64_t,		1u);
	CHECK_VALUE_INIT2(one<DWORDLONG>,			int64_t,		1u);

	#if TOML_WINDOWS_COMPAT
 
	CHECK_VALUE_INIT2(L"kek",					std::string,	"kek"sv);
	CHECK_VALUE_INIT2(L"kek"s,					std::string,	"kek"sv);
	CHECK_VALUE_INIT2(L"kek"sv,					std::string,	"kek"sv);
	CHECK_VALUE_INIT2(L"kek"sv.data(),			std::string,	"kek"sv);
	
	#endif // TOML_WINDOWS_COMPAT

	#endif
}

TEST_CASE("values - printing")
{
	static constexpr auto print_value = [](auto&& raw)
	{
		auto val = toml::value{ std::forward<decltype(raw)>(raw) };
		std::stringstream ss;
		ss << val;
		return ss.str();
	};

	CHECK(print_value(1) == "1");
	CHECK(print_value(1.0f) == "1.0");
	CHECK(print_value(1.0) == "1.0");

	CHECK(print_value(1.5f) == "1.5");
	CHECK(print_value(1.5) == "1.5");

	CHECK(print_value(10) == "10");
	CHECK(print_value(10.0f) == "10.0");
	CHECK(print_value(10.0) == "10.0");

	CHECK(print_value(100) == "100");
	CHECK(print_value(100.0f) == "100.0");
	CHECK(print_value(100.0) == "100.0");

	CHECK(print_value(1000) == "1000");
	CHECK(print_value(1000.0f) == "1000.0");
	CHECK(print_value(1000.0) == "1000.0");

	CHECK(print_value(10000) == "10000");
	CHECK(print_value(10000.0f) == "10000.0");
	CHECK(print_value(10000.0) == "10000.0");

	CHECK(print_value(std::numeric_limits<double>::infinity()) == "inf");
	CHECK(print_value(-std::numeric_limits<double>::infinity()) == "-inf");
	CHECK(print_value(std::numeric_limits<double>::quiet_NaN()) == "nan");

	// only integers for large values;
	// large floats might get output as scientific notation and that's fine
	CHECK(print_value(10000000000) == "10000000000");
	CHECK(print_value(100000000000000) == "100000000000000");
}

TEST_CASE("nodes - value() int/float/bool conversions")
{
	#define CHECK_VALUE_PASS(type, v) \
		CHECK(n.value<type>() == static_cast<type>(v))
	#define CHECK_VALUE_FAIL(type) \
		CHECK(!n.value<type>())

	// bools
	{
		value val{ false };
		const node& n = val;
		CHECK_VALUE_PASS(bool, false);
		CHECK_VALUE_PASS(int8_t, 0);
		CHECK_VALUE_PASS(uint8_t, 0);
		CHECK_VALUE_PASS(int16_t, 0);
		CHECK_VALUE_PASS(uint16_t, 0);
		CHECK_VALUE_PASS(int32_t, 0);
		CHECK_VALUE_PASS(uint32_t, 0);
		CHECK_VALUE_PASS(int64_t, 0);
		CHECK_VALUE_PASS(uint64_t, 0);
		CHECK_VALUE_FAIL(float);
		CHECK_VALUE_FAIL(double);
		CHECK_VALUE_FAIL(std::string);
		CHECK_VALUE_FAIL(std::string_view);
		CHECK_VALUE_FAIL(toml::date);
		CHECK_VALUE_FAIL(toml::time);
		CHECK_VALUE_FAIL(toml::date_time);

		*val = true;
		CHECK_VALUE_PASS(bool, true);
		CHECK_VALUE_PASS(int8_t, 1);
		CHECK_VALUE_PASS(uint8_t, 1);
		CHECK_VALUE_PASS(int16_t, 1);
		CHECK_VALUE_PASS(uint16_t, 1);
		CHECK_VALUE_PASS(int32_t, 1);
		CHECK_VALUE_PASS(uint32_t, 1);
		CHECK_VALUE_PASS(int64_t, 1);
		CHECK_VALUE_PASS(uint64_t, 1);
		CHECK_VALUE_FAIL(float);
		CHECK_VALUE_FAIL(double);
		CHECK_VALUE_FAIL(std::string);
		CHECK_VALUE_FAIL(std::string_view);
		CHECK_VALUE_FAIL(toml::date);
		CHECK_VALUE_FAIL(toml::time);
		CHECK_VALUE_FAIL(toml::date_time);
	}

	// ints
	{
		value val{ 0 };
		const node& n = val;
		CHECK_VALUE_PASS(bool, false); // int -> bool coercion
		CHECK_VALUE_PASS(int8_t, 0);
		CHECK_VALUE_PASS(uint8_t, 0);
		CHECK_VALUE_PASS(int16_t, 0);
		CHECK_VALUE_PASS(uint16_t, 0);
		CHECK_VALUE_PASS(int32_t, 0);
		CHECK_VALUE_PASS(uint32_t, 0);
		CHECK_VALUE_PASS(int64_t, 0);
		CHECK_VALUE_PASS(uint64_t, 0);
		CHECK_VALUE_PASS(float, 0);
		CHECK_VALUE_PASS(double, 0);
		CHECK_VALUE_FAIL(std::string);
		CHECK_VALUE_FAIL(std::string_view);
		CHECK_VALUE_FAIL(toml::date);
		CHECK_VALUE_FAIL(toml::time);
		CHECK_VALUE_FAIL(toml::date_time);

		*val = 100;
		CHECK_VALUE_PASS(bool, true); // int -> bool coercion
		CHECK_VALUE_PASS(int8_t, 100);
		CHECK_VALUE_PASS(uint8_t, 100);
		CHECK_VALUE_PASS(int16_t, 100);
		CHECK_VALUE_PASS(uint16_t, 100);
		CHECK_VALUE_PASS(int32_t, 100);
		CHECK_VALUE_PASS(uint32_t, 100);
		CHECK_VALUE_PASS(int64_t, 100);
		CHECK_VALUE_PASS(uint64_t, 100);
		CHECK_VALUE_PASS(float, 100);
		CHECK_VALUE_PASS(double, 100);
		CHECK_VALUE_FAIL(std::string);
		CHECK_VALUE_FAIL(std::string_view);
		CHECK_VALUE_FAIL(toml::date);
		CHECK_VALUE_FAIL(toml::time);
		CHECK_VALUE_FAIL(toml::date_time);

		*val = -100;
		CHECK_VALUE_PASS(bool, true); // int -> bool coercion
		CHECK_VALUE_PASS(int8_t, -100);
		CHECK_VALUE_FAIL(uint8_t);
		CHECK_VALUE_PASS(int16_t, -100);
		CHECK_VALUE_FAIL(uint16_t);
		CHECK_VALUE_PASS(int32_t, -100);
		CHECK_VALUE_FAIL(uint32_t);
		CHECK_VALUE_PASS(int64_t, -100);
		CHECK_VALUE_FAIL(uint64_t);
		CHECK_VALUE_PASS(float, -100);
		CHECK_VALUE_PASS(double, -100);
		CHECK_VALUE_FAIL(std::string);
		CHECK_VALUE_FAIL(std::string_view);
		CHECK_VALUE_FAIL(toml::date);
		CHECK_VALUE_FAIL(toml::time);
		CHECK_VALUE_FAIL(toml::date_time);

		*val = 1000;
		CHECK_VALUE_PASS(bool, true); // int -> bool coercion
		CHECK_VALUE_FAIL(int8_t);
		CHECK_VALUE_FAIL(uint8_t);
		CHECK_VALUE_PASS(int16_t, 1000);
		CHECK_VALUE_PASS(uint16_t, 1000);
		CHECK_VALUE_PASS(int32_t, 1000);
		CHECK_VALUE_PASS(uint32_t, 1000);
		CHECK_VALUE_PASS(int64_t, 1000);
		CHECK_VALUE_PASS(uint64_t, 1000);
		CHECK_VALUE_PASS(float, 1000);
		CHECK_VALUE_PASS(double, 1000);
		CHECK_VALUE_FAIL(std::string);
		CHECK_VALUE_FAIL(std::string_view);
		CHECK_VALUE_FAIL(toml::date);
		CHECK_VALUE_FAIL(toml::time);
		CHECK_VALUE_FAIL(toml::date_time);

		*val = -1000;
		CHECK_VALUE_PASS(bool, true); // int -> bool coercion
		CHECK_VALUE_FAIL(int8_t);
		CHECK_VALUE_FAIL(uint8_t);
		CHECK_VALUE_PASS(int16_t, -1000);
		CHECK_VALUE_FAIL(uint16_t);
		CHECK_VALUE_PASS(int32_t, -1000);
		CHECK_VALUE_FAIL(uint32_t);
		CHECK_VALUE_PASS(int64_t, -1000);
		CHECK_VALUE_FAIL(uint64_t);
		CHECK_VALUE_PASS(float, -1000);
		CHECK_VALUE_PASS(double, -1000);
		CHECK_VALUE_FAIL(std::string);
		CHECK_VALUE_FAIL(std::string_view);
		CHECK_VALUE_FAIL(toml::date);
		CHECK_VALUE_FAIL(toml::time);
		CHECK_VALUE_FAIL(toml::date_time);

		*val = (std::numeric_limits<int64_t>::max)();
		CHECK_VALUE_PASS(bool, true); // int -> bool coercion
		CHECK_VALUE_FAIL(int8_t);
		CHECK_VALUE_FAIL(uint8_t);
		CHECK_VALUE_FAIL(int16_t);
		CHECK_VALUE_FAIL(uint16_t);
		CHECK_VALUE_FAIL(int32_t);
		CHECK_VALUE_FAIL(uint32_t);
		CHECK_VALUE_PASS(int64_t, (std::numeric_limits<int64_t>::max)());
		CHECK_VALUE_PASS(uint64_t, (std::numeric_limits<int64_t>::max)());
		CHECK_VALUE_FAIL(float);
		CHECK_VALUE_FAIL(double);
		CHECK_VALUE_FAIL(std::string);
		CHECK_VALUE_FAIL(std::string_view);
		CHECK_VALUE_FAIL(toml::date);
		CHECK_VALUE_FAIL(toml::time);
		CHECK_VALUE_FAIL(toml::date_time);

		*val = (std::numeric_limits<int64_t>::min)();
		CHECK_VALUE_PASS(bool, true); // int -> bool coercion
		CHECK_VALUE_FAIL(int8_t);
		CHECK_VALUE_FAIL(uint8_t);
		CHECK_VALUE_FAIL(int16_t);
		CHECK_VALUE_FAIL(uint16_t);
		CHECK_VALUE_FAIL(int32_t);
		CHECK_VALUE_FAIL(uint32_t);
		CHECK_VALUE_PASS(int64_t, (std::numeric_limits<int64_t>::min)());
		CHECK_VALUE_FAIL(uint64_t);
		CHECK_VALUE_FAIL(float);
		CHECK_VALUE_FAIL(double);
		CHECK_VALUE_FAIL(std::string);
		CHECK_VALUE_FAIL(std::string_view);
		CHECK_VALUE_FAIL(toml::date);
		CHECK_VALUE_FAIL(toml::time);
		CHECK_VALUE_FAIL(toml::date_time);
	}

	// floats
	{
		value val{ 0.0 };
		const node& n = val;
		CHECK_VALUE_FAIL(bool);
		CHECK_VALUE_PASS(int8_t, 0);
		CHECK_VALUE_PASS(uint8_t, 0);
		CHECK_VALUE_PASS(int16_t, 0);
		CHECK_VALUE_PASS(uint16_t, 0);
		CHECK_VALUE_PASS(int32_t, 0);
		CHECK_VALUE_PASS(uint32_t, 0);
		CHECK_VALUE_PASS(int64_t, 0);
		CHECK_VALUE_PASS(uint64_t, 0);
		CHECK_VALUE_PASS(float, 0);
		CHECK_VALUE_PASS(double, 0);
		CHECK_VALUE_FAIL(std::string);
		CHECK_VALUE_FAIL(std::string_view);
		CHECK_VALUE_FAIL(toml::date);
		CHECK_VALUE_FAIL(toml::time);
		CHECK_VALUE_FAIL(toml::date_time);


		*val = 1.0;
		CHECK_VALUE_FAIL(bool);
		CHECK_VALUE_PASS(int8_t, 1);
		CHECK_VALUE_PASS(uint8_t, 1);
		CHECK_VALUE_PASS(int16_t, 1);
		CHECK_VALUE_PASS(uint16_t, 1);
		CHECK_VALUE_PASS(int32_t, 1);
		CHECK_VALUE_PASS(uint32_t, 1);
		CHECK_VALUE_PASS(int64_t, 1);
		CHECK_VALUE_PASS(uint64_t, 1);
		CHECK_VALUE_PASS(float, 1);
		CHECK_VALUE_PASS(double, 1);
		CHECK_VALUE_FAIL(std::string);
		CHECK_VALUE_FAIL(std::string_view);
		CHECK_VALUE_FAIL(toml::date);
		CHECK_VALUE_FAIL(toml::time);
		CHECK_VALUE_FAIL(toml::date_time);

		*val = -1.0;
		CHECK_VALUE_FAIL(bool);
		CHECK_VALUE_PASS(int8_t, -1);
		CHECK_VALUE_FAIL(uint8_t);
		CHECK_VALUE_PASS(int16_t, -1);
		CHECK_VALUE_FAIL(uint16_t);
		CHECK_VALUE_PASS(int32_t, -1);
		CHECK_VALUE_FAIL(uint32_t);
		CHECK_VALUE_PASS(int64_t, -1);
		CHECK_VALUE_FAIL(uint64_t);
		CHECK_VALUE_PASS(float, -1);
		CHECK_VALUE_PASS(double, -1);
		CHECK_VALUE_FAIL(std::string);
		CHECK_VALUE_FAIL(std::string_view);
		CHECK_VALUE_FAIL(toml::date);
		CHECK_VALUE_FAIL(toml::time);
		CHECK_VALUE_FAIL(toml::date_time);

		*val = 1.5;
		CHECK_VALUE_FAIL(bool);
		CHECK_VALUE_FAIL(int8_t);
		CHECK_VALUE_FAIL(uint8_t);
		CHECK_VALUE_FAIL(int16_t);
		CHECK_VALUE_FAIL(uint16_t);
		CHECK_VALUE_FAIL(int32_t);
		CHECK_VALUE_FAIL(uint32_t);
		CHECK_VALUE_FAIL(int64_t);
		CHECK_VALUE_FAIL(uint64_t);
		CHECK_VALUE_PASS(float, 1.5);
		CHECK_VALUE_PASS(double, 1.5);
		CHECK_VALUE_FAIL(std::string);
		CHECK_VALUE_FAIL(std::string_view);
		CHECK_VALUE_FAIL(toml::date);
		CHECK_VALUE_FAIL(toml::time);
		CHECK_VALUE_FAIL(toml::date_time);

		*val = -1.5;
		CHECK_VALUE_FAIL(bool);
		CHECK_VALUE_FAIL(int8_t);
		CHECK_VALUE_FAIL(uint8_t);
		CHECK_VALUE_FAIL(int16_t);
		CHECK_VALUE_FAIL(uint16_t);
		CHECK_VALUE_FAIL(int32_t);
		CHECK_VALUE_FAIL(uint32_t);
		CHECK_VALUE_FAIL(int64_t);
		CHECK_VALUE_FAIL(uint64_t);
		CHECK_VALUE_PASS(float, -1.5);
		CHECK_VALUE_PASS(double, -1.5);
		CHECK_VALUE_FAIL(std::string);
		CHECK_VALUE_FAIL(std::string_view);
		CHECK_VALUE_FAIL(toml::date);
		CHECK_VALUE_FAIL(toml::time);
		CHECK_VALUE_FAIL(toml::date_time);

		*val = std::numeric_limits<double>::infinity();
		CHECK_VALUE_FAIL(bool);
		CHECK_VALUE_FAIL(int8_t);
		CHECK_VALUE_FAIL(uint8_t);
		CHECK_VALUE_FAIL(int16_t);
		CHECK_VALUE_FAIL(uint16_t);
		CHECK_VALUE_FAIL(int32_t);
		CHECK_VALUE_FAIL(uint32_t);
		CHECK_VALUE_FAIL(int64_t);
		CHECK_VALUE_FAIL(uint64_t);
		CHECK_VALUE_PASS(float, std::numeric_limits<float>::infinity());
		CHECK_VALUE_PASS(double, std::numeric_limits<double>::infinity());
		CHECK_VALUE_FAIL(std::string);
		CHECK_VALUE_FAIL(std::string_view);
		CHECK_VALUE_FAIL(toml::date);
		CHECK_VALUE_FAIL(toml::time);
		CHECK_VALUE_FAIL(toml::date_time);

		*val = -std::numeric_limits<double>::infinity();
		CHECK_VALUE_FAIL(bool);
		CHECK_VALUE_FAIL(int8_t);
		CHECK_VALUE_FAIL(uint8_t);
		CHECK_VALUE_FAIL(int16_t);
		CHECK_VALUE_FAIL(uint16_t);
		CHECK_VALUE_FAIL(int32_t);
		CHECK_VALUE_FAIL(uint32_t);
		CHECK_VALUE_FAIL(int64_t);
		CHECK_VALUE_FAIL(uint64_t);
		CHECK_VALUE_PASS(float, -std::numeric_limits<float>::infinity());
		CHECK_VALUE_PASS(double, -std::numeric_limits<double>::infinity());
		CHECK_VALUE_FAIL(std::string);
		CHECK_VALUE_FAIL(std::string_view);
		CHECK_VALUE_FAIL(toml::date);
		CHECK_VALUE_FAIL(toml::time);
		CHECK_VALUE_FAIL(toml::date_time);

		*val = std::numeric_limits<double>::quiet_NaN();
		CHECK_VALUE_FAIL(bool);
		CHECK_VALUE_FAIL(int8_t);
		CHECK_VALUE_FAIL(uint8_t);
		CHECK_VALUE_FAIL(int16_t);
		CHECK_VALUE_FAIL(uint16_t);
		CHECK_VALUE_FAIL(int32_t);
		CHECK_VALUE_FAIL(uint32_t);
		CHECK_VALUE_FAIL(int64_t);
		CHECK_VALUE_FAIL(uint64_t);
		{
			auto fval = n.value<float>();
			REQUIRE(fval.has_value());
			CHECK(impl::fpclassify(*fval) == impl::fp_class::nan);
		}
		{
			auto fval = n.value<double>();
			REQUIRE(fval.has_value());
			CHECK(impl::fpclassify(*fval) == impl::fp_class::nan);
		}
		CHECK_VALUE_FAIL(std::string);
		CHECK_VALUE_FAIL(std::string_view);
		CHECK_VALUE_FAIL(toml::date);
		CHECK_VALUE_FAIL(toml::time);
		CHECK_VALUE_FAIL(toml::date_time);
	}
}
