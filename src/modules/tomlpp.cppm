/**
 * @file tomlpp.cppm
 * @brief File containing the module declaration for toml++.
 */

module;

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
        using toml::literals::operator""_toml;
        using toml::literals::operator""_tpath;
    }

    using toml::array;
    using toml::date;
    using toml::date_time;
    using toml::inserter;
    using toml::json_formatter;
    using toml::key;
    using toml::node;
    using toml::node_view;
    using toml::parse_error;
    using toml::parse_result;
    using toml::path;
    using toml::path_component;
    using toml::source_position;
    using toml::source_region;
    using toml::table;
    using toml::time;
    using toml::time_offset;
    using toml::toml_formatter;
    using toml::value;
    using toml::yaml_formatter;
    using toml::format_flags;
    using toml::node_type;
    using toml::path_component_type;
    using toml::value_flags;
    using toml::array_iterator;
    using toml::const_array_iterator;
    using toml::const_table_iterator;
    using toml::default_formatter;
    using toml::inserted_type_of;
    using toml::optional;
    using toml::source_index;
    using toml::source_path_ptr;
    using toml::table_iterator;

    using toml::at_path;
    using toml::get_line;
    using toml::operator""_toml;
    using toml::operator""_tpath;
    using toml::operator<<;
    using toml::parse;
    using toml::parse_file;
}
