#ifdef TARTANLLAMA_OPTIONAL
	#if __has_include(<tloptional/include/tl/optional.hpp>)
		#include <tloptional/include/tl/optional.hpp>
	#else
		#error TartanLlama/optional is missing! You probably need to fetch submodules ("git submodule update --init extern/tloptional")
	#endif
	#define TOML_OPTIONAL_TYPE tl::optional
#endif
#include <ostream>
#if !defined(_MSC_VER) || !defined(_M_IX86)
	#define TOML_ALL_INLINE 0
	#define TOML_IMPLEMENTATION
#endif
#include "../include/toml++/toml.h"

namespace toml
{
	using std::declval;
	using std::is_same_v;

	static_assert(is_same_v<decltype(declval<node&>().ref<double>()), double&>);
	static_assert(is_same_v<decltype(declval<node&&>().ref<double>()), double&&>);
	static_assert(is_same_v<decltype(declval<const node&>().ref<double>()), const double&>);
	static_assert(is_same_v<decltype(declval<node&>().ref<value<double>>()), double&>);
	static_assert(is_same_v<decltype(declval<node&&>().ref<value<double>>()), double&&>);
	static_assert(is_same_v<decltype(declval<const node&>().ref<value<double>>()), const double&>);
	static_assert(is_same_v<decltype(declval<node&>().ref<table>()), table&>);
	static_assert(is_same_v<decltype(declval<node&&>().ref<table>()), table&&>);
	static_assert(is_same_v<decltype(declval<const node&>().ref<table>()), const table&>);
	static_assert(is_same_v<decltype(declval<node&>().ref<array>()), array&>);
	static_assert(is_same_v<decltype(declval<node&&>().ref<array>()), array&&>);
	static_assert(is_same_v<decltype(declval<const node&>().ref<array>()), const array&>);

	static_assert(is_same_v<decltype(declval<node_view<node>>().ref<double>()), double&>);
	static_assert(is_same_v<decltype(declval<node_view<const node>>().ref<double>()), const double&>);
	static_assert(is_same_v<decltype(declval<node_view<node>>().ref<value<double>>()), double&>);
	static_assert(is_same_v<decltype(declval<node_view<const node>>().ref<value<double>>()), const double&>);
	static_assert(is_same_v<decltype(declval<node_view<node>>().ref<table>()), table&>);
	static_assert(is_same_v<decltype(declval<node_view<const node>>().ref<table>()), const table&>);
	static_assert(is_same_v<decltype(declval<node_view<node>>().ref<array>()), array&>);
	static_assert(is_same_v<decltype(declval<node_view<const node>>().ref<array>()), const array&>);
}
