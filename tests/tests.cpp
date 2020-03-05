#include "tests.h"

#if TESTS_MANUAL_INSTANTIATIONS

template void parse_expected_value(std::string_view, const int&) noexcept;
template void parse_expected_value(std::string_view, const unsigned int&) noexcept;
template void parse_expected_value(std::string_view, const bool&) noexcept;
template void parse_expected_value(std::string_view, const float&) noexcept;
template void parse_expected_value(std::string_view, const double&) noexcept;
template void parse_expected_value(std::string_view, const toml::string_view&) noexcept;

TOML_IMPL_START
{
	template class formatter<char>;
}
TOML_IMPL_END

TOML_START
{
	template class default_formatter<char>;

	template std::ostream& operator<< (std::ostream&, const table&) TOML_MAY_THROW;
	template std::ostream& operator<< (std::ostream&, const array&) TOML_MAY_THROW;
	template std::ostream& operator<< (std::ostream&, const value<string>&) TOML_MAY_THROW;
	template std::ostream& operator<< (std::ostream&, const value<int64_t>&) TOML_MAY_THROW;
	template std::ostream& operator<< (std::ostream&, const value<double>&) TOML_MAY_THROW;
	template std::ostream& operator<< (std::ostream&, const value<bool>&) TOML_MAY_THROW;
	template std::ostream& operator<< (std::ostream&, const value<date>&) TOML_MAY_THROW;
	template std::ostream& operator<< (std::ostream&, const value<time>&) TOML_MAY_THROW;
	template std::ostream& operator<< (std::ostream&, const value<date_time>&) TOML_MAY_THROW;
	template std::ostream& operator<< (std::ostream&, const node_view<node>&) TOML_MAY_THROW;
	template std::ostream& operator<< (std::ostream&, const node_view<const node>&) TOML_MAY_THROW;
	template std::ostream& operator<< (std::ostream&, node_type) TOML_MAY_THROW;
	template std::ostream& operator<< (std::ostream&, const source_region&) TOML_MAY_THROW;
	template std::ostream& operator<< (std::ostream&, const source_position&) TOML_MAY_THROW;
	template std::ostream& operator<< (std::ostream&, const parse_error&) TOML_MAY_THROW;
	template std::ostream& operator<< (std::ostream&, const date&) TOML_MAY_THROW;
	template std::ostream& operator<< (std::ostream&, const time&) TOML_MAY_THROW;
	template std::ostream& operator<< (std::ostream&, const time_offset&) TOML_MAY_THROW;
	template std::ostream& operator<< (std::ostream&, const date_time&) TOML_MAY_THROW;
	template std::ostream& operator<< (std::ostream&, default_formatter<char>&) TOML_MAY_THROW;
	template std::ostream& operator<< (std::ostream&, default_formatter<char>&&) TOML_MAY_THROW;
}
TOML_END

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

TOML_START
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
TOML_END
