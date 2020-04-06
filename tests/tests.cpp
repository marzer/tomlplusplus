#include "tests.h"

template void parse_expected_value(std::string_view, uint32_t, std::string_view, const int&) noexcept;
template void parse_expected_value(std::string_view, uint32_t, std::string_view, const unsigned int&) noexcept;
template void parse_expected_value(std::string_view, uint32_t, std::string_view, const bool&) noexcept;
template void parse_expected_value(std::string_view, uint32_t, std::string_view, const float&) noexcept;
template void parse_expected_value(std::string_view, uint32_t, std::string_view, const double&) noexcept;
template void parse_expected_value(std::string_view, uint32_t, std::string_view, const toml::string_view&) noexcept;

namespace std
{
	template class unique_ptr<const Catch::IExceptionTranslator>;
}
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


