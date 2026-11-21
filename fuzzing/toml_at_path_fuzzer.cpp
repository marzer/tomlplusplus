// SPDX-License-Identifier: MIT
//
// Fuzz harness: toml_at_path_fuzzer targets path string

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>

#include <toml++/toml.hpp>

using namespace std::string_view_literals;

namespace
{
	// each half of the input is capped at this size; the .options max_len
	// already bounds the combined input, this is just a defensive guard
	constexpr std::size_t max_size = 4096;

	[[gnu::noinline]] void exercise_path(std::string_view str)
	{
		toml::path p{ str };

		// component introspection (type-checked before the typed accessors,
		// which carry internal assumptions about the component type)
		for (std::size_t i = 0u; i < p.size(); i++)
		{
			const auto& component = p[i];
			if (component.type() == toml::path_component_type::array_index)
			{
				(void)component.index();
				(void)static_cast<std::size_t>(component);
			}
			else if (component.type() == toml::path_component_type::key)
			{
				(void)component.key();
				(void)static_cast<const std::string&>(component);
			}
		}

		// derived paths (the API tolerates out-of-range values)
		(void)p.str();
		(void)p.empty();
		const auto n = p.size();
		(void)p.parent();
		if (n)
		{
			(void)p.leaf(n);
			(void)p.truncated(n - 1u);
			for (std::size_t start = 0u; start < n; start++)
				(void)p.subpath(start, n - start);
		}

		// composition and comparison
		toml::path q = p;
		q += str;
		q.append(str);
		q.prepend(str);
		q.assign(str);
		(void)(q == p);
		(void)(q != p);
		(void)q.str();
		(void)q.size();
		q.truncate(n);
		q.clear();

		// component construction and comparison
		toml::path_component key_component{ "some.key"sv };
		toml::path_component index_component{ std::size_t{ 42u } };
		key_component	= "another.key"sv;
		index_component = std::size_t{ 7u };
		(void)(key_component == index_component);
		(void)(key_component != index_component);
	}

	[[gnu::noinline]] void exercise_at_path(const toml::table& tbl, std::string_view str, const toml::path& p)
	{
		(void)tbl.at_path(str);
		(void)toml::at_path(tbl, str);
		(void)toml::at_path(tbl, p);

		const auto n = p.size();
		if (n > 1u)
		{
			(void)tbl.at_path(p.parent());
			(void)tbl.at_path(p.subpath(0u, n - 1u));
		}
	}

	struct outcome
	{
		bool parsed = {};
		toml::table table;
	};

	template <typename Source>
	outcome parse(Source&& source)
	{
#if TOML_EXCEPTIONS
		try
		{
			return { true, toml::parse(static_cast<Source&&>(source)) };
		}
		catch (const toml::parse_error&)
		{
			return {};
		}
#else
		auto result = toml::parse(static_cast<Source&&>(source));
		if (!result)
			return {};
		return { true, std::move(result).table() };
#endif
	}
}

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, const std::size_t size)
{
	if (size > max_size * 2u)
		return 0;

	// first NUL byte separates the path string from the TOML document
	const std::string_view input{ reinterpret_cast<const char*>(data), size };
	const auto sep					= input.find('\0');
	const std::string_view path_str = input.substr(0u, sep == std::string_view::npos ? input.size() : sep);
	const std::string_view doc_str	= sep == std::string_view::npos ? std::string_view{} : input.substr(sep + 1u);

	exercise_path(path_str);

	const auto doc = parse(doc_str);
	if (!doc.parsed)
		return 0;

	exercise_at_path(doc.table, path_str, toml::path{ path_str });
	return 0;
}
