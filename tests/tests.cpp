#include "tests.h"

#if TESTS_MANUAL_INSTANTIATIONS

template void parse_expected_value(std::string_view, const int&) noexcept;
template void parse_expected_value(std::string_view, const unsigned int&) noexcept;
template void parse_expected_value(std::string_view, const bool&) noexcept;
template void parse_expected_value(std::string_view, const float&) noexcept;
template void parse_expected_value(std::string_view, const double&) noexcept;
template void parse_expected_value(std::string_view, const toml::string_view&) noexcept;

namespace toml::impl
{
	template class formatter<char>;
}


namespace toml
{
	template class default_formatter<char>;

	template std::ostream& operator<< (std::ostream&, const table&);
	template std::ostream& operator<< (std::ostream&, const array&);
	template std::ostream& operator<< (std::ostream&, const value<string>&);
	template std::ostream& operator<< (std::ostream&, const value<int64_t>&);
	template std::ostream& operator<< (std::ostream&, const value<double>&);
	template std::ostream& operator<< (std::ostream&, const value<bool>&);
	template std::ostream& operator<< (std::ostream&, const value<date>&);
	template std::ostream& operator<< (std::ostream&, const value<time>&);
	template std::ostream& operator<< (std::ostream&, const value<date_time>&);
	template std::ostream& operator<< (std::ostream&, const node_view<node>&);
	template std::ostream& operator<< (std::ostream&, const node_view<const node>&);
	template std::ostream& operator<< (std::ostream&, node_type);
	template std::ostream& operator<< (std::ostream&, const source_region&);
	template std::ostream& operator<< (std::ostream&, const source_position&);
	template std::ostream& operator<< (std::ostream&, const parse_error&);
	template std::ostream& operator<< (std::ostream&, const date&);
	template std::ostream& operator<< (std::ostream&, const time&);
	template std::ostream& operator<< (std::ostream&, const time_offset&);
	template std::ostream& operator<< (std::ostream&, const date_time&);
	template std::ostream& operator<< (std::ostream&, default_formatter<char>&);
	template std::ostream& operator<< (std::ostream&, default_formatter<char>&&);
}


template class std::unique_ptr<const Catch::IExceptionTranslator>;
namespace Catch
{
	template struct StringMaker<node_view<node>>;
	template struct StringMaker<node_view<const node>>;
	template ReusableStringStream& ReusableStringStream::operator << (toml::node_view<toml::node> const&);
	template ReusableStringStream& ReusableStringStream::operator << (toml::node_view<const toml::node> const&);
	namespace Detail
	{
		template std::string stringify(const node_view<node>&);
		template std::string stringify(const node_view<const node>&);
	}
}

#endif // TESTS_MANUAL_INSTANTIATIONS

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

