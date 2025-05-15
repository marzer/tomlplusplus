/**
 * @file tomlpp.cppm
 * @brief File containing the module declaration for toml++.
 */

module;

#define TOML_UNDEF_MACROS 0
#include <toml++/toml.hpp>

export module tomlplusplus;

/**
 * @namespace toml
 * @brief The toml++ namespace toml:: 
 */
export namespace toml {
    /**
     * @namespace literals
     * @brief The toml++ namespace toml::literals::
     */
    inline namespace literals {
        using TOML_NAMESPACE::literals::operator""_toml;
        using TOML_NAMESPACE::literals::operator""_tpath;
    }

    using TOML_NAMESPACE::array;
    using TOML_NAMESPACE::date;
    using TOML_NAMESPACE::date_time;
    using TOML_NAMESPACE::inserter;
    using TOML_NAMESPACE::json_formatter;
    using TOML_NAMESPACE::key;
    using TOML_NAMESPACE::node;
    using TOML_NAMESPACE::node_view;
    using TOML_NAMESPACE::parse_error;
    using TOML_NAMESPACE::parse_result;
    using TOML_NAMESPACE::path;
    using TOML_NAMESPACE::path_component;
    using TOML_NAMESPACE::source_position;
    using TOML_NAMESPACE::source_region;
    using TOML_NAMESPACE::table;
    using TOML_NAMESPACE::time;
    using TOML_NAMESPACE::time_offset;
    using TOML_NAMESPACE::toml_formatter;
    using TOML_NAMESPACE::value;
    using TOML_NAMESPACE::yaml_formatter;
    using TOML_NAMESPACE::format_flags;
    using TOML_NAMESPACE::node_type;
    using TOML_NAMESPACE::path_component_type;
    using TOML_NAMESPACE::value_flags;
    using TOML_NAMESPACE::array_iterator;
    using TOML_NAMESPACE::const_array_iterator;
    using TOML_NAMESPACE::const_table_iterator;
    using TOML_NAMESPACE::default_formatter;
    using TOML_NAMESPACE::inserted_type_of;
    using TOML_NAMESPACE::optional;
    using TOML_NAMESPACE::source_index;
    using TOML_NAMESPACE::source_path_ptr;
    using TOML_NAMESPACE::table_iterator;

    using TOML_NAMESPACE::at_path;
    using TOML_NAMESPACE::get_line;
    using TOML_NAMESPACE::operator""_toml;
    using TOML_NAMESPACE::operator""_tpath;
    using TOML_NAMESPACE::operator<<;
    using TOML_NAMESPACE::parse;
    using TOML_NAMESPACE::parse_file;

    using TOML_NAMESPACE::is_array;
    using TOML_NAMESPACE::is_boolean;
    using TOML_NAMESPACE::is_chronological;
    using TOML_NAMESPACE::is_container;
    using TOML_NAMESPACE::is_date;
    using TOML_NAMESPACE::is_date_time;
    using TOML_NAMESPACE::is_floating_point;
    using TOML_NAMESPACE::is_integer;
    using TOML_NAMESPACE::is_key;
    using TOML_NAMESPACE::is_key_or_convertible;
    using TOML_NAMESPACE::is_node;
    using TOML_NAMESPACE::is_node_view;
    using TOML_NAMESPACE::is_number;
    using TOML_NAMESPACE::is_string;
    using TOML_NAMESPACE::is_table;
    using TOML_NAMESPACE::is_time;
    using TOML_NAMESPACE::is_value;

	using TOML_NAMESPACE::preserve_source_value_flags;
}
