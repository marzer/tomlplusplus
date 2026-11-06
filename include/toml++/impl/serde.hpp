//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT
#pragma once

#include "forward_declarations.hpp"
#if TOML_ENABLE_REFLECTION

#include "std_meta.hpp"
#include "std_optional.hpp"
#include "std_string.hpp"
#if !TOML_EXCEPTIONS
#include "std_variant.hpp"
#endif
#include "make_node.hpp"
#include "array.hpp"
#include "table.hpp"
#include "header_start.hpp"

/// \cond
TOML_IMPL_NAMESPACE_START
{
	struct transient_
	{};
	struct flatten_
	{};
	struct reflectable_
	{};
}
TOML_IMPL_NAMESPACE_END;
/// \endcond

TOML_NAMESPACE_START
{
	/// \brief	Annotation marking a data member to be skipped during (de)serialization.
	/// \detail	\cpp
	/// struct Config
	/// {
	///		std::string host;
	///		[[=toml::transient]] int cached_fd; // never written, never read
	/// };
	/// \ecpp
	/// \availability This is only available when #TOML_ENABLE_REFLECTION is enabled.
	inline constexpr impl::transient_ transient;

	/// \brief	Annotation inlining a nested struct's members into the parent table rather than nesting a sub-table.
	/// \availability This is only available when #TOML_ENABLE_REFLECTION is enabled.
	inline constexpr impl::flatten_ flatten;

	/// \brief	Annotation granting (de)serialization access to a type's private members.
	/// \detail	Plain structs with only public data members need no annotation. Apply this to a type with private
	///			members to permit the serializer to read and write them.
	/// \warning Deserializing into private members writes them directly, bypassing constructors and setters.
	/// \availability This is only available when #TOML_ENABLE_REFLECTION is enabled.
	inline constexpr impl::reflectable_ reflectable;

	/// \brief	Annotation overriding the TOML key used for a data member.
	/// \detail	\cpp
	/// struct Config
	/// {
	///		[[=toml::rename("listen-port")]] int port; // keyed as "listen-port", not "port"
	/// };
	/// \ecpp
	/// \availability This is only available when #TOML_ENABLE_REFLECTION is enabled.
	template <size_t N>
	struct rename
	{
		char value[N]{};

		consteval rename(const char (&str)[N]) noexcept
		{
			for (size_t i = 0; i < N; ++i)
				value[i] = str[i];
		}
	};
	template <size_t N>
	rename(const char (&)[N]) -> rename<N>;

	/// \brief	Error thrown (or returned, when exceptions are disabled) when deserialization fails.
	/// \availability This is only available when #TOML_ENABLE_REFLECTION is enabled.
	class deserialization_error
	{
	  private:
		std::string desc;
		std::string where;

	  public:
		TOML_NODISCARD_CTOR
		deserialization_error(std::string desc, std::string key) noexcept
			: desc{ static_cast<std::string&&>(desc) },
			  where{ static_cast<std::string&&>(key) }
		{}

		/// \brief	A human-readable description of what went wrong.
		TOML_NODISCARD
		std::string_view description() const noexcept
		{
			return desc;
		}

		/// \brief	The TOML key that was being processed when the error occurred (may be empty).
		TOML_NODISCARD
		std::string_view key() const noexcept
		{
			return where;
		}
	};

#if !TOML_EXCEPTIONS
	/// \brief	The result of a #toml::table::deserialize call when exceptions are disabled: either a `T` or a
	///			#toml::deserialization_error.
	/// \availability This is only available when #TOML_ENABLE_REFLECTION is enabled and exceptions are disabled.
	template <typename T>
	class serde_result
	{
	  private:
		std::variant<T, deserialization_error> storage;

	  public:
		TOML_NODISCARD_CTOR
		serde_result() noexcept(std::is_nothrow_default_constructible_v<T>) : storage{ std::in_place_index<0> }
		{}

		TOML_NODISCARD_CTOR
		serde_result(deserialization_error err) noexcept : storage{ static_cast<deserialization_error&&>(err) }
		{}

		TOML_NODISCARD
		explicit operator bool() const noexcept
		{
			return storage.index() == 0;
		}

		TOML_NODISCARD
		bool has_value() const noexcept
		{
			return storage.index() == 0;
		}

		TOML_NODISCARD
		T& value() noexcept
		{
			return std::get<0>(storage);
		}

		TOML_NODISCARD
		const T& value() const noexcept
		{
			return std::get<0>(storage);
		}

		TOML_NODISCARD
		const deserialization_error& error() const noexcept
		{
			return std::get<1>(storage);
		}
	};
#endif // !TOML_EXCEPTIONS
}
TOML_NAMESPACE_END;

/// \cond
TOML_IMPL_NAMESPACE_START
{
	template <typename>
	struct is_optional_ : std::false_type
	{};
	template <typename U>
	struct is_optional_<optional<U>> : std::true_type
	{};
	template <typename T>
	inline constexpr bool is_optional = is_optional_<remove_cvref<T>>::value;

	consteval std::meta::info annotation_type(std::meta::info a)
	{
		return std::meta::remove_const(std::meta::dealias(std::meta::type_of(a)));
	}

	template <typename Tag, std::meta::info Mem>
	consteval bool has_annotation()
	{
		template for (constexpr auto a : std::define_static_array(
						  std::meta::annotations_of(Mem))) if (annotation_type(a) == ^^Tag) return true;
		return false;
	}

	template <std::meta::info Mem>
	consteval std::string_view member_key()
	{
		template for (constexpr auto a : std::define_static_array(std::meta::annotations_of(Mem)))
		{
			constexpr std::meta::info t = annotation_type(a);
			if constexpr (std::meta::has_template_arguments(t) && std::meta::template_of(t) == ^^rename)
			{
				constexpr auto r = std::meta::extract<typename[:t:]>(a);
				return std::define_static_string(std::string_view{ r.value, sizeof(r.value) - 1 });
			}
		}
		return std::meta::identifier_of(Mem);
	}

	template <typename T>
	consteval bool reflectable_type()
	{
		return TOML_SERDE_ALLOW_PRIVATE != 0 || has_annotation<reflectable_, ^^T>();
	}

	template <typename T>
	consteval std::meta::access_context serde_access()
	{
		if constexpr (reflectable_type<T>())
			return std::meta::access_context::unchecked();
		else
			return std::meta::access_context::current();
	}

	template <typename T>
	inline constexpr bool is_serde_struct =
		std::is_class_v<remove_cvref<T>> && !is_one_of<remove_cvref<T>, table, array, std::string> && !is_wide_string<T>
		&& !is_string_like<remove_cvref<T>> && !can_partially_represent_native<remove_cvref<T>>;

	template <typename T>
	node_ptr serde_to_node(const T& val);

	template <typename T>
	table serde_struct_to_table(const T& obj)
	{
		table out;
		template for (constexpr auto member :
					  std::define_static_array(std::meta::nonstatic_data_members_of(^^T, serde_access<T>())))
		{
			if constexpr (!has_annotation<transient_, member>())
			{
				using member_type = typename[:std::meta::type_of(member):];
				const auto& field = obj.[:member:];

				if constexpr (is_optional<member_type>)
				{
					if (field)
						out.insert(member_key<member>(), std::move(*serde_to_node(*field)));
				}
				else if constexpr (has_annotation<flatten_, member>() && is_serde_struct<member_type>)
				{
					table sub = serde_struct_to_table(field);
					for (auto&& [k, v] : sub)
						out.insert(k, std::move(v));
				}
				else
				{
					out.insert(member_key<member>(), std::move(*serde_to_node(field)));
				}
			}
		}
		return out;
	}

	template <typename T>
	node_ptr serde_to_node(const T& val)
	{
		if constexpr (is_serde_struct<T>)
			return node_ptr{ new table{ serde_struct_to_table(val) } };
		else
			return make_node(val);
	}

	struct serde_sink
	{
#if !TOML_EXCEPTIONS
		optional<deserialization_error> err;
#endif

		void fail(std::string desc, std::string key)
		{
#if TOML_EXCEPTIONS
			throw deserialization_error{ static_cast<std::string&&>(desc), static_cast<std::string&&>(key) };
#else
			if (!err)
				err.emplace(static_cast<std::string&&>(desc), static_cast<std::string&&>(key));
#endif
		}

		TOML_NODISCARD
		bool failed() const noexcept
		{
#if TOML_EXCEPTIONS
			return false;
#else
			return err.has_value();
#endif
		}
	};

	template <typename T>
	void serde_from_node(const node& n, T& out, std::string_view key, serde_sink& sink);

	template <typename T>
	void serde_table_to_struct(const table& tbl, T& out, serde_sink& sink)
	{
		template for (constexpr auto member :
					  std::define_static_array(std::meta::nonstatic_data_members_of(^^T, serde_access<T>())))
		{
			if (sink.failed())
				return;

			if constexpr (!has_annotation<transient_, member>())
			{
				using member_type			   = typename[:std::meta::type_of(member):];
				constexpr std::string_view key = member_key<member>();

				if constexpr (has_annotation<flatten_, member>() && is_serde_struct<member_type>)
				{
					serde_table_to_struct(tbl, out.[:member:], sink);
				}
				else if (const node* n = tbl.get(key))
				{
					serde_from_node(*n, out.[:member:], key, sink);
				}
				else if constexpr (!is_optional<member_type>)
				{
					sink.fail("missing required key", std::string{ key });
				}
			}
		}
	}

	// write a single node into a destination field
	template <typename T>
	void serde_from_node(const node& n, T& out, std::string_view key, serde_sink& sink)
	{
		if constexpr (is_optional<T>)
		{
			out.emplace();
			serde_from_node(n, *out, key, sink);
		}
		else if constexpr (is_serde_struct<T>)
		{
			const table* sub = n.as_table();
			if (!sub)
			{
				sink.fail("expected a table", std::string{ key });
				return;
			}
			serde_table_to_struct(*sub, out, sink);
		}
		else
		{
			auto v = n.value<T>();
			if (!v)
				sink.fail("type mismatch", std::string{ key });
			else
				out = static_cast<T&&>(*v);
		}
	}
}
TOML_IMPL_NAMESPACE_END;
/// \endcond

TOML_NAMESPACE_START
{
	/// \brief	Serializes an object into a #toml::table using C++26 static reflection.
	///
	/// \detail	Public data members are serialized by default; no annotation is required for plain structs.
	///			Nested structs become sub-tables (or are inlined with #toml::flatten), members annotated with
	///			#toml::transient are skipped, and #toml::rename overrides the key. \cpp
	///
	/// struct Point { int x; int y; };
	/// toml::table t = toml::serialize(Point{ 3, 4 });
	/// // x = 3
	/// // y = 4
	/// \ecpp
	///
	/// \availability This is only available when #TOML_ENABLE_REFLECTION is enabled.
	template <typename T>
	TOML_NODISCARD
	table serialize(const T& obj)
	{
		static_assert(impl::is_serde_struct<T>, "toml::serialize requires a struct or class type");
		return impl::serde_struct_to_table(obj);
	}

	template <typename T>
	auto table::deserialize() const
	{
		static_assert(impl::is_serde_struct<T>, "table::deserialize requires a struct or class type");
		impl::serde_sink sink;
#if TOML_EXCEPTIONS
		T out{};
		impl::serde_table_to_struct(*this, out, sink);
		return out;
#else
		serde_result<T> res;
		impl::serde_table_to_struct(*this, res.value(), sink);
		if (sink.err)
			return serde_result<T>{ static_cast<deserialization_error&&>(*sink.err) };
		return res;
#endif
	}
}
TOML_NAMESPACE_END;

#include "header_end.hpp"
#endif // TOML_ENABLE_REFLECTION
