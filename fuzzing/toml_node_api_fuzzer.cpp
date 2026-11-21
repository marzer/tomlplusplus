// SPDX-License-Identifier: MIT
//
// Fuzz harness: toml_node_api_fuzzer targets the node api

#include <cstddef>
#include <cstdint>
#include <sstream>
#include <string>
#include <utility>

#include <fuzzer/FuzzedDataProvider.h>

#include <toml++/toml.hpp>

namespace
{
	constexpr std::uint8_t max_entries = 12u;
	constexpr std::uint8_t max_key_len = 24u;
	constexpr std::uint8_t max_elems   = 6u;
	constexpr std::uint8_t max_ops	   = 40u;
	constexpr unsigned max_depth	   = 3u;

	std::string make_key(FuzzedDataProvider& fdp);
	toml::date make_date(FuzzedDataProvider& fdp);
	toml::time make_time(FuzzedDataProvider& fdp);
	toml::array make_array(FuzzedDataProvider& fdp, unsigned depth);
	toml::table make_table(FuzzedDataProvider& fdp, unsigned depth);

	std::string make_key(FuzzedDataProvider& fdp)
	{
		return fdp.ConsumeBytesAsString(fdp.ConsumeIntegralInRange<std::uint8_t>(1u, max_key_len));
	}

	toml::date make_date(FuzzedDataProvider& fdp)
	{
		toml::date d;
		d.year	= static_cast<std::uint16_t>(fdp.ConsumeIntegralInRange<int>(1600, 2400));
		d.month = static_cast<std::uint8_t>(fdp.ConsumeIntegralInRange<int>(1, 12));
		d.day	= static_cast<std::uint8_t>(fdp.ConsumeIntegralInRange<int>(1, 28));
		return d;
	}

	toml::time make_time(FuzzedDataProvider& fdp)
	{
		toml::time t;
		t.hour		 = static_cast<std::uint8_t>(fdp.ConsumeIntegralInRange<int>(0, 23));
		t.minute	 = static_cast<std::uint8_t>(fdp.ConsumeIntegralInRange<int>(0, 59));
		t.second	 = static_cast<std::uint8_t>(fdp.ConsumeIntegralInRange<int>(0, 59));
		t.nanosecond = static_cast<std::uint32_t>(fdp.ConsumeIntegralInRange<int>(0, 999'999'999));
		return t;
	}

	toml::array make_array(FuzzedDataProvider& fdp, unsigned depth)
	{
		toml::array arr;
		const auto count = fdp.ConsumeIntegralInRange<std::uint8_t>(0u, max_elems);
		for (std::uint8_t i = 0u; i < count && fdp.remaining_bytes() > 2u; i++)
		{
			switch (fdp.ConsumeIntegralInRange<std::uint8_t>(0u, 6u))
			{
				case 0u:
					arr.emplace_back<std::int64_t>(fdp.ConsumeIntegralInRange<std::int64_t>(-1'000'000, 1'000'000));
					break;
				case 1u: arr.emplace_back<double>(fdp.ConsumeFloatingPointInRange(-1.0e6, 1.0e6)); break;
				case 2u: arr.emplace_back<bool>(fdp.ConsumeBool()); break;
				case 3u:
					arr.emplace_back<std::string>(
						fdp.ConsumeBytesAsString(fdp.ConsumeIntegralInRange<std::uint8_t>(0u, 32u)));
					break;
				case 4u: arr.emplace_back<toml::date>(make_date(fdp)); break;
				case 5u: arr.emplace_back<toml::time>(make_time(fdp)); break;
				default:
					if (depth < max_depth)
					{
						if (fdp.ConsumeBool())
							arr.push_back(make_array(fdp, depth + 1u));
						else
							arr.push_back(make_table(fdp, depth + 1u));
					}
					break;
			}
		}
		return arr;
	}

	toml::table make_table(FuzzedDataProvider& fdp, unsigned depth)
	{
		toml::table tbl;
		const auto count = fdp.ConsumeIntegralInRange<std::uint8_t>(0u, max_entries);
		for (std::uint8_t i = 0u; i < count && fdp.remaining_bytes() > 2u; i++)
		{
			const auto key = make_key(fdp);
			switch (fdp.ConsumeIntegralInRange<std::uint8_t>(0u, 7u))
			{
				case 0u:
					tbl.insert_or_assign(key, fdp.ConsumeIntegralInRange<std::int64_t>(-1'000'000, 1'000'000));
					break;
				case 1u: tbl.insert_or_assign(key, fdp.ConsumeFloatingPointInRange(-1.0e6, 1.0e6)); break;
				case 2u: tbl.insert_or_assign(key, fdp.ConsumeBool()); break;
				case 3u:
					tbl.insert_or_assign(key,
										 fdp.ConsumeBytesAsString(fdp.ConsumeIntegralInRange<std::uint8_t>(0u, 32u)));
					break;
				case 4u: tbl.insert_or_assign(key, make_date(fdp)); break;
				case 5u: tbl.insert_or_assign(key, make_time(fdp)); break;
				default:
					if (depth < max_depth)
					{
						if (fdp.ConsumeBool())
							tbl.insert_or_assign(key, make_array(fdp, depth + 1u));
						else
							tbl.insert_or_assign(key, make_table(fdp, depth + 1u));
					}
					break;
			}
		}
		return tbl;
	}

	toml::array* find_array(toml::table& tbl)
	{
		for (auto&& [key, node] : tbl)
		{
			(void)key;
			if (auto* arr = node.as_array())
				return arr;
		}
		return nullptr;
	}
}

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, const std::size_t size)
{
	FuzzedDataProvider fdp{ data, size };
	std::ostringstream oss;

	// phase A: construct a document from typed, bounded operations
	toml::table root;
	const auto entries = fdp.ConsumeIntegralInRange<std::uint8_t>(1u, max_entries);
	for (std::uint8_t i = 0u; i < entries && fdp.remaining_bytes() > 2u; i++)
	{
		const auto key = make_key(fdp);
		switch (fdp.ConsumeIntegralInRange<std::uint8_t>(0u, 8u))
		{
			case 0u: root.insert_or_assign(key, fdp.ConsumeIntegralInRange<std::int64_t>(-1'000'000, 1'000'000)); break;
			case 1u: root.insert_or_assign(key, fdp.ConsumeFloatingPointInRange(-1.0e6, 1.0e6)); break;
			case 2u: root.insert_or_assign(key, fdp.ConsumeBool()); break;
			case 3u:
				root.insert_or_assign(key, fdp.ConsumeBytesAsString(fdp.ConsumeIntegralInRange<std::uint8_t>(0u, 48u)));
				break;
			case 4u: root.insert_or_assign(key, make_date(fdp)); break;
			case 5u: root.insert_or_assign(key, make_time(fdp)); break;
			case 6u:
			{
				const auto d = make_date(fdp);
				const auto t = make_time(fdp);
				if (fdp.ConsumeBool())
					root.insert_or_assign(key, toml::date_time{ d, t });
				else
					root.insert_or_assign(
						key,
						toml::date_time{ d,
										 t,
										 toml::time_offset{ fdp.ConsumeIntegralInRange<int>(-23, 23),
															fdp.ConsumeIntegralInRange<int>(-59, 59) } });
				break;
			}
			case 7u: root.insert_or_assign(key, make_array(fdp, 0u)); break;
			default: root.insert_or_assign(key, make_table(fdp, 0u)); break;
		}
	}

	// phase B: bounded mutation and navigation
	const auto op_count = fdp.ConsumeIntegralInRange<std::uint8_t>(1u, max_ops);
	for (std::uint8_t i = 0u; i < op_count && fdp.remaining_bytes() > 2u; i++)
	{
		const auto key = make_key(fdp);
		switch (fdp.ConsumeIntegralInRange<std::uint8_t>(0u, 9u))
		{
			case 0u: // table erase at lower_bound
			{
				const auto it = root.lower_bound(key);
				if (it != root.end())
					root.erase(it);
				break;
			}
			case 1u: // table insert / emplace
			{
				(void)root.insert(key, fdp.ConsumeIntegralInRange<std::int64_t>(-1000, 1000));
				(void)root.emplace<std::int64_t>(key, fdp.ConsumeIntegralInRange<std::int64_t>(-1000, 1000));
				break;
			}
			case 2u: // array truncate
			{
				if (auto* arr = find_array(root); arr && arr->size())
					arr->truncate(fdp.ConsumeIntegralInRange<std::size_t>(0u, arr->size()));
				break;
			}
			case 3u: // array emplace_back / resize
			{
				if (auto* arr = find_array(root))
				{
					const auto n = arr->size();
					if (fdp.ConsumeBool())
						arr->emplace_back<std::int64_t>(fdp.ConsumeIntegralInRange<std::int64_t>(-1000, 1000));
					else if (n)
						arr->resize(fdp.ConsumeIntegralInRange<std::size_t>(0u, n + 2u),
									fdp.ConsumeIntegralInRange<std::int64_t>(-1000, 1000));
				}
				break;
			}
			case 4u: // array erase at a clamped position
			{
				if (auto* arr = find_array(root); arr && arr->size())
				{
					const auto n = arr->size();
					arr->erase(arr->cbegin()
							   + static_cast<std::ptrdiff_t>(fdp.ConsumeIntegralInRange<std::size_t>(0u, n - 1u)));
				}
				break;
			}
			case 5u: // flatten + prune
			{
				if (auto* arr = find_array(root))
				{
					arr->flatten();
					arr->prune(fdp.ConsumeBool());
				}
				else
					root.prune(fdp.ConsumeBool());
				break;
			}
			case 6u: // table prune + homogeneity + value comparison
			{
				root.prune(fdp.ConsumeBool());
				(void)root.is_homogeneous(fdp.ConsumeBool() ? toml::node_type::table : toml::node_type::array);
				if (auto* v = root.get_as<std::int64_t>(key))
					(void)(*v == fdp.ConsumeIntegralInRange<std::int64_t>(-1000, 1000));
				break;
			}
			case 7u: // date/time comparisons and printing
			{
				if (const auto* d = root.get_as<toml::date>(key))
				{
					(void)(*d == *d);
					(void)(*d < *d);
					oss << *d;
				}
				if (const auto* t = root.get_as<toml::time>(key))
				{
					(void)(*t == *t);
					(void)(*t < *t);
					oss << *t;
				}
				if (const auto* dt = root.get_as<toml::date_time>(key))
				{
					(void)(*dt == *dt);
					oss << *dt;
				}
				break;
			}
			case 8u: // node_view navigation and comparisons
			{
				const toml::node_view<toml::node> view{ root };
				const auto sub = view[key];
				(void)(sub == view);
				(void)(sub == fdp.ConsumeIntegralInRange<std::int64_t>(-1000, 1000));
				(void)sub.is_table();
				(void)sub.is_array();
				const toml::path p{ make_key(fdp) };
				(void)view[p];
				break;
			}
			default: // visitation + key and node_type printing
			{
				const toml::key k1{ key }, k2{ make_key(fdp) };
				(void)(k1 == k2);
				(void)(k1 != k2);
				(void)(k1 == key);
				(void)k1.str();
				oss << k1 << toml::node_type::table << toml::node_type::array;
				root.for_each(
					[&oss](const toml::key& k, const toml::node& n)
					{
						oss << k << n.type();
						n.visit([&oss](auto&& el) { oss << el; });
					});
				break;
			}
		}
	}

	return 0;
}
