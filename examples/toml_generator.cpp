// This file is a part of toml++ and is subject to the the terms of the MIT license.
// Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
// See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

// This example demonstrates the use of some more advanced features to generate a tree of random TOML data.

#include "examples.h"

#define TOML_ENABLE_PARSER 0
#include <toml++/toml.h>

using namespace std::string_view_literals;

namespace
{
	inline constexpr auto words = std::array{
		"acceptable"sv, "contain"sv,	  "ghost"sv,		 "mark"sv,		  "respect"sv,		 "taboo"sv,
		"actually"sv,	"cream"sv,		  "gleaming"sv,		 "meaty"sv,		  "rest"sv,			 "tacky"sv,
		"addition"sv,	"creature"sv,	  "glorious"sv,		 "memory"sv,	  "rice"sv,			 "tank"sv,
		"adhesive"sv,	"crime"sv,		  "gold"sv,			 "messy"sv,		  "rich"sv,			 "tent"sv,
		"adorable"sv,	"cross"sv,		  "grandfather"sv,	 "miss"sv,		  "righteous"sv,	 "terrible"sv,
		"advise"sv,		"crowded"sv,	  "gusty"sv,		 "modern"sv,	  "room"sv,			 "threatening"sv,
		"afraid"sv,		"crown"sv,		  "haircut"sv,		 "morning"sv,	  "rotten"sv,		 "three"sv,
		"ancient"sv,	"cure"sv,		  "hard-to-find"sv,	 "naughty"sv,	  "royal"sv,		 "ticket"sv,
		"anxious"sv,	"curious"sv,	  "harm"sv,			 "neck"sv,		  "run"sv,			 "title"sv,
		"aromatic"sv,	"curtain"sv,	  "heavy"sv,		 "night"sv,		  "satisfy"sv,		 "torpid"sv,
		"attempt"sv,	"cycle"sv,		  "helpless"sv,		 "nondescript"sv, "scary"sv,		 "train"sv,
		"babies"sv,		"deadpan"sv,	  "high-pitched"sv,	 "overjoyed"sv,	  "scatter"sv,		 "umbrella"sv,
		"bake"sv,		"decisive"sv,	  "hilarious"sv,	 "page"sv,		  "scene"sv,		 "unadvised"sv,
		"ball"sv,		"deeply"sv,		  "history"sv,		 "partner"sv,	  "scintillating"sv, "unbecoming"sv,
		"bat"sv,		"delightful"sv,	  "hook"sv,			 "party"sv,		  "self"sv,			 "unbiased"sv,
		"behave"sv,		"deserted"sv,	  "ignore"sv,		 "pause"sv,		  "selfish"sv,		 "unite"sv,
		"best"sv,		"draconian"sv,	  "imperfect"sv,	 "pear"sv,		  "silky"sv,		 "uptight"sv,
		"birds"sv,		"dreary"sv,		  "impossible"sv,	 "picture"sv,	  "sisters"sv,		 "used"sv,
		"blind"sv,		"dull"sv,		  "incandescent"sv,	 "place"sv,		  "ski"sv,			 "vengeful"sv,
		"blood"sv,		"enthusiastic"sv, "influence"sv,	 "playground"sv,  "skip"sv,			 "versed"sv,
		"blue-eyed"sv,	"equable"sv,	  "innocent"sv,		 "popcorn"sv,	  "snow"sv,			 "vessel"sv,
		"boiling"sv,	"excuse"sv,		  "insidious"sv,	 "prefer"sv,	  "soap"sv,			 "view"sv,
		"bore"sv,		"experience"sv,	  "itch"sv,			 "productive"sv,  "spare"sv,		 "voyage"sv,
		"borrow"sv,		"fabulous"sv,	  "jail"sv,			 "profuse"sv,	  "spicy"sv,		 "wall"sv,
		"broken"sv,		"familiar"sv,	  "kindhearted"sv,	 "protective"sv,  "spiritual"sv,	 "want"sv,
		"capable"sv,	"finger"sv,		  "lackadaisical"sv, "pumped"sv,	  "sprout"sv,		 "weary"sv,
		"charming"sv,	"finicky"sv,	  "laughable"sv,	 "rabbit"sv,	  "squirrel"sv,		 "week"sv,
		"cheerful"sv,	"fix"sv,		  "leather"sv,		 "rapid"sv,		  "stale"sv,		 "whip"sv,
		"chubby"sv,		"flagrant"sv,	  "legal"sv,		 "regret"sv,	  "step"sv,			 "wilderness"sv,
		"clean"sv,		"flat"sv,		  "lewd"sv,			 "reject"sv,	  "stingy"sv,		 "wistful"sv,
		"close"sv,		"flimsy"sv,		  "license"sv,		 "rejoice"sv,	  "string"sv,		 "worried"sv,
		"cobweb"sv,		"fuel"sv,		  "light"sv,		 "relation"sv,	  "sulky"sv,		 "wretched"sv,
		"complex"sv,	"furtive"sv,	  "march"sv,		 "remarkable"sv,  "surprise"sv,		 "zealous"sv,
		"consist"sv,	"geese"sv
	};

	template <typename T>
	[[nodiscard]] static T rand(T excl_max) noexcept
	{
		return static_cast<T>(static_cast<T>(std::rand()) % excl_max);
	}

	template <typename T>
	[[nodiscard]] static T rand(T incl_min, T excl_max) noexcept
	{
		return static_cast<T>(incl_min + rand(excl_max - incl_min));
	}

	static auto rand_date() noexcept
	{
		return toml::date{ rand(uint16_t{ 1900 }, uint16_t{ 2021 }),
						   rand(uint8_t{ 1 }, uint8_t{ 13 }),
						   rand(uint8_t{ 1 }, uint8_t{ 29 }) };
	}

	static auto rand_time() noexcept
	{
		return toml::time{ rand(uint8_t{ 24 }),
						   rand(uint8_t{ 60 }),
						   rand(uint8_t{ 60 }),
						   rand(100) > 80 ? rand(1000000000u) : 0u };
	}

	static auto rand_string(size_t word_count, char sep = ' ') noexcept
	{
		std::string val;
		while (word_count-- > 0u)
		{
			if (!val.empty())
				val += sep;
			val.append(words[rand(words.size())]);
		}
		return val;
	}
}

int main(int argc, char** argv)
{
	int node_budget{};
	for (int i = 1; i < argc; i++)
	{
		std::stringstream ss{ argv[i] };
		int nodes;
		if ((ss >> nodes))
			node_budget += nodes;
	}
	if (node_budget <= 0)
		node_budget = 100;

	toml::table root;
	std::vector<toml::node*> tree;
	tree.push_back(&root);
	constexpr size_t max_depth = 10u;
	int container_min_values   = 10;
	bool in_arr				   = false;

	const auto add = [&](auto&& val) noexcept -> auto&
	{
		using value_ref	 = decltype(val);
		using value_type = std::remove_reference_t<value_ref>;
		using node_type	 = toml::inserted_type_of<value_type>;

		toml::node* new_node{};

		// we're adding an element to an array
		if (auto arr = tree.back()->as_array())
		{
			arr->push_back(std::forward<value_ref>(val));
			new_node = &arr->back();
		}

		// we're adding a new kvp to a table
		else
		{
			auto& table = tree.back()->ref<toml::table>();

			const auto it =
				table.insert_or_assign(rand_string(rand<size_t>(1u, 4u), '-'), std::forward<value_ref>(val));

			new_node = &it.first->second;
		}

		// we added a new array or table, so now we need to step into it
		if constexpr (toml::is_container<value_type>)
		{
			tree.push_back(new_node);
			container_min_values = rand(1, 4);
			in_arr				 = toml::is_array<value_type>;
			if constexpr (toml::is_array<value_type>)
				tree.back()->as_array()->reserve(static_cast<size_t>(container_min_values));
		}
		else
			container_min_values--;

		node_budget--;
		return *new_node->as<node_type>();
	};

	while (node_budget)
	{
		if (!in_arr && rand(100) >= 75)
		{
			if (container_min_values <= 0 && tree.size() < max_depth)
				add(toml::table{}).is_inline(tree.size() >= max_depth - 2u && rand(100) >= 85);
		}
		else
		{
			toml::node_type new_node_type;
			if (auto arr = tree.back()->as_array(); arr && !arr->empty())
				new_node_type = arr->front().type();
			else
				new_node_type = static_cast<toml::node_type>((rand() % 8) + 2);

			switch (new_node_type)
			{
				case toml::node_type::array:
					if (container_min_values <= 0 && tree.size() < max_depth)
						add(toml::array{});
					break;

				case toml::node_type::string: add(rand_string(rand<size_t>(8u))); break;

				case toml::node_type::integer: add(rand()); break;

				case toml::node_type::floating_point: add(rand(10001u) / 10000.0); break;

				case toml::node_type::boolean: add(!rand(2u)); break;

				case toml::node_type::date: add(rand_date()); break;

				case toml::node_type::time: add(rand_time()); break;

				case toml::node_type::date_time:
					add(rand(100) >= 75
							? toml::date_time{ rand_date(), rand_time() }
							: toml::date_time{ rand_date(),
											   rand_time(),
											   toml::time_offset{ rand<int8_t>(-11, 12), rand<int8_t>(-45, +46) } });
					break;
			}
			if (container_min_values <= 0 && tree.size() >= 2u && rand(100) >= 85)
			{
				tree.pop_back();
				in_arr = !tree.empty() && tree.back()->type() == toml::node_type::array;
			}
		}
	}

	std::cout << root << "\n";
	return 0;
}
