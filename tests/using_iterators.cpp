#include "tests.h"

TOML_DISABLE_WARNINGS;
#include <algorithm>
TOML_ENABLE_WARNINGS;

TEST_CASE("using iterators")
{
	constexpr auto data = R"(array=[1,"Foo",true]
string="Bar"
number=5)"sv;
	parsing_should_succeed(FILE_LINE_ARGS, data, [](auto&& table)
	{
		const auto table_begin = table.begin();
		const auto table_end = table.end();

		auto count_table_lambda = [table_begin, table_end](node_type type) noexcept
		{
			return std::count_if(table_begin, table_end, [type](const auto& pair) noexcept
			{
				return pair.second.type() == type;
			});
		};

		CHECK(std::distance(table_begin, table_end) == 3);
		CHECK(count_table_lambda(node_type::table) == 0);
		CHECK(count_table_lambda(node_type::integer) == 1);
		CHECK(count_table_lambda(node_type::string) == 1);
		CHECK(std::next(table_begin, 3) == table_end);

		const auto array_iter = std::find_if(table_begin, table_end, [](const auto& pair) noexcept
		{
			return pair.second.is_array();
		});

		REQUIRE(array_iter != table_end);
		const auto& array = array_iter->second.as_array();
		const auto array_begin = array->begin();
		const auto array_end = array->end();

		auto count_array_lambda = [array_begin, array_end](node_type type) noexcept
		{
			return std::count_if(array_begin, array_end, [type](const auto& node) noexcept
			{
				return node.type() == type;
			});
		};

		CHECK(std::distance(array_begin, array_end) == 3);
		CHECK(count_array_lambda(node_type::table) == 0);
		CHECK(count_array_lambda(node_type::integer) == 1);
		CHECK(count_array_lambda(node_type::string) == 1);
		CHECK(std::next(array_begin, 2) != array_end);
	});
}
