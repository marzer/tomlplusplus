//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#pragma once
//# {{
#include "toml_preprocessor.h"
#if !TOML_PARSER
	#error This header cannot not be included when TOML_PARSER is disabled.
#endif
//# }}
#include "toml_utf8.h"
#include "toml_parse_error.h"

TOML_PUSH_WARNINGS
TOML_DISABLE_PADDING_WARNINGS

namespace toml::impl
{
	template <typename T>
	class utf8_byte_stream;

	inline constexpr auto utf8_byte_order_mark = "\xEF\xBB\xBF"sv;

	template <typename Char>
	class utf8_byte_stream<std::basic_string_view<Char>> final
	{
		static_assert(sizeof(Char) == 1_sz);

		private:
			std::basic_string_view<Char> source;
			size_t position = {};

		public:
			explicit constexpr utf8_byte_stream(std::basic_string_view<Char> sv) noexcept
				: source{ sv }
			{
				if (source.length() >= 3_sz && memcmp(utf8_byte_order_mark.data(), source.data(), 3_sz) == 0)
					position += 3_sz;
			}

			[[nodiscard]] TOML_ALWAYS_INLINE
			constexpr bool eof() const noexcept
			{
				return position >= source.length();
			}

			[[nodiscard]] TOML_ALWAYS_INLINE
			constexpr bool peek_eof() const noexcept
			{
				return eof();
			}

			[[nodiscard]] TOML_ALWAYS_INLINE
			constexpr bool error() const noexcept
			{
				return false;
			}

			[[nodiscard]]
			constexpr unsigned int operator() () noexcept
			{
				if (position >= source.length())
					return 0xFFFFFFFFu;
				return static_cast<unsigned int>(static_cast<uint8_t>(source[position++]));
			}
	};

	template <typename Char>
	class utf8_byte_stream<std::basic_istream<Char>> final
	{
		static_assert(sizeof(Char) == 1_sz);

		private:
			std::basic_istream<Char>* source;

		public:
			explicit utf8_byte_stream(std::basic_istream<Char>& stream)
				: source{ &stream }
			{
				if (!*source)
					return;

				using stream_traits = typename std::remove_pointer_t<decltype(source)>::traits_type;
				const auto initial_pos = source->tellg();
				size_t bom_pos{};
				Char bom[3];
				for (; bom_pos < 3_sz && *source; bom_pos++)
				{
					const auto next = source->get();
					if (next == stream_traits::eof())
						break;
					bom[bom_pos] = static_cast<Char>(next);
				}
				if (!*source || bom_pos < 3_sz || memcmp(utf8_byte_order_mark.data(), bom, 3_sz) != 0)
					source->seekg(initial_pos);
			}

			[[nodiscard]] TOML_ALWAYS_INLINE
			bool eof() const noexcept
			{
				return source->eof();
			}

			[[nodiscard]] TOML_ALWAYS_INLINE
			bool peek_eof() const
			{
				using stream_traits = typename std::remove_pointer_t<decltype(source)>::traits_type;
				return eof() || source->peek() == stream_traits::eof();
			}

			[[nodiscard]] TOML_ALWAYS_INLINE
			bool error() const noexcept
			{
				return !(*source);
			}

			[[nodiscard]]
			unsigned int operator() ()
			{
				auto val = source->get();
				if (val == std::basic_istream<Char>::traits_type::eof())
					return 0xFFFFFFFFu;
				return static_cast<unsigned int>(val);
			}
	};

	#if TOML_LARGE_FILES
		TOML_ABI_NAMESPACE_START(impl_lf)
	#else
		TOML_ABI_NAMESPACE_START(impl_sf)
	#endif

	struct utf8_codepoint final
	{
		char32_t value;
		string_char bytes[4];
		source_position position;

		template <typename Char = string_char>
		[[nodiscard]]
		TOML_ALWAYS_INLINE
		std::basic_string_view<Char> as_view() const noexcept
		{
			static_assert(
				sizeof(Char) == 1,
				"The string view's underlying character type must be 1 byte in size."
			);

			return bytes[3]
				? std::basic_string_view<Char>{ reinterpret_cast<const Char*>(bytes), 4_sz }
				: std::basic_string_view<Char>{ reinterpret_cast<const Char*>(bytes) };
		}

		[[nodiscard]]
		TOML_GNU_ATTR(pure)
		TOML_ALWAYS_INLINE
		constexpr operator char32_t& () noexcept
		{
			return value;
		}

		[[nodiscard]]
		TOML_GNU_ATTR(pure)
		TOML_ALWAYS_INLINE
		constexpr operator const char32_t& () const noexcept
		{
			return value;
		}

		[[nodiscard]]
		TOML_GNU_ATTR(pure)
		TOML_ALWAYS_INLINE
		constexpr const char32_t& operator* () const noexcept
		{
			return value;
		}
	};
	static_assert(std::is_trivial_v<utf8_codepoint>);
	static_assert(std::is_standard_layout_v<utf8_codepoint>);

	TOML_ABI_NAMESPACE_END // TOML_LARGE_FILES

	#if TOML_EXCEPTIONS
		#define TOML_ERROR_CHECK	(void)0
		#define TOML_ERROR			throw parse_error
		TOML_ABI_NAMESPACE_START(impl_ex)
	#else
		#define TOML_ERROR_CHECK	if (err) return nullptr
		#define TOML_ERROR			err.emplace
		TOML_ABI_NAMESPACE_START(impl_noex)
	#endif

	TOML_PUSH_WARNINGS
	TOML_DISABLE_VTABLE_WARNINGS

	struct TOML_INTERFACE utf8_reader_interface
	{
		[[nodiscard]]
		virtual const source_path_ptr& source_path() const noexcept = 0;

		[[nodiscard]]
		virtual const utf8_codepoint* read_next() = 0;

		[[nodiscard]]
		virtual bool peek_eof() const = 0;

		#if !TOML_EXCEPTIONS

		[[nodiscard]]
		virtual optional<parse_error>&& error() noexcept = 0;

		#endif

		virtual ~utf8_reader_interface() noexcept = default;
	};

	template <typename T>
	class TOML_EMPTY_BASES utf8_reader final
		: public utf8_reader_interface
	{
		private:
			utf8_byte_stream<T> stream;
			utf8_decoder decoder;
			utf8_codepoint codepoints[2];
			size_t cp_idx = 1;
			uint8_t current_byte_count{};
			source_path_ptr source_path_;
			#if !TOML_EXCEPTIONS
			optional<parse_error> err;
			#endif

		public:

			template <typename U, typename String = std::string_view>
			explicit utf8_reader(U && source, String&& source_path = {})
				noexcept(std::is_nothrow_constructible_v<utf8_byte_stream<T>, U&&>)
				: stream{ std::forward<U>(source) }
			{
				std::memset(codepoints, 0, sizeof(codepoints));
				codepoints[0].position = { 1, 1 };
				codepoints[1].position = { 1, 1 };

				if (!source_path.empty())
					source_path_ = std::make_shared<const std::string>(std::forward<String>(source_path));
			}

			[[nodiscard]]
			const source_path_ptr& source_path() const noexcept override
			{
				return source_path_;
			}

			[[nodiscard]]
			const utf8_codepoint* read_next() override
			{
				TOML_ERROR_CHECK;

				auto& prev = codepoints[(cp_idx - 1_sz) % 2_sz];

				if (stream.eof())
					return nullptr;
				else if (stream.error())
					TOML_ERROR("An error occurred while reading from the underlying stream", prev.position, source_path_ );
				else if (decoder.error())
					TOML_ERROR( "Encountered invalid utf-8 sequence", prev.position, source_path_ );

				TOML_ERROR_CHECK;

				while (true)
				{
					uint8_t next_byte;
					{
						unsigned int next_byte_raw{ 0xFFFFFFFFu };
						if constexpr (noexcept(stream()) || !TOML_EXCEPTIONS)
						{
							next_byte_raw = stream();
						}
						#if TOML_EXCEPTIONS
						else
						{
							try
							{
								next_byte_raw = stream();
							}
							catch (const std::exception& exc)
							{
								throw parse_error{ exc.what(), prev.position, source_path_ };
							}
							catch (...)
							{
								throw parse_error{ "An unspecified error occurred", prev.position, source_path_ };
							}
						}
						#endif

						if (next_byte_raw >= 256u)
						{
							if (stream.eof())
							{
								if (decoder.needs_more_input())
									TOML_ERROR("Encountered EOF during incomplete utf-8 code point sequence",
										prev.position, source_path_);
								return nullptr;
							}
							else
								TOML_ERROR("An error occurred while reading from the underlying stream",
									prev.position, source_path_);
						}

						TOML_ERROR_CHECK;
						next_byte = static_cast<uint8_t>(next_byte_raw);
					}

					decoder(next_byte);
					if (decoder.error())
						TOML_ERROR( "Encountered invalid utf-8 sequence", prev.position, source_path_ );

					TOML_ERROR_CHECK;

					auto& current = codepoints[cp_idx % 2_sz];
					current.bytes[current_byte_count++] = static_cast<string_char>(next_byte);
					if (decoder.has_code_point())
					{
						//store codepoint
						current.value = decoder.codepoint;

						//reset prev (will be the next 'current')
						std::memset(prev.bytes, 0, sizeof(prev.bytes));
						current_byte_count = {};
						if (is_line_break<false>(current.value))
							prev.position = { static_cast<source_index>(current.position.line + 1), 1 };
						else
							prev.position = { current.position.line, static_cast<source_index>(current.position.column + 1) };
						cp_idx++;
						return &current;
					}
				}

				TOML_UNREACHABLE;
			}

			[[nodiscard]]
			bool peek_eof() const override
			{
				return stream.peek_eof();
			}

			#if !TOML_EXCEPTIONS

			[[nodiscard]]
			optional<parse_error>&& error() noexcept override
			{
				return std::move(err);
			}

			#endif
	};

	template <typename Char>
	utf8_reader(std::basic_string_view<Char>, std::string_view) -> utf8_reader<std::basic_string_view<Char>>;

	template <typename Char>
	utf8_reader(std::basic_istream<Char>&, std::string_view) -> utf8_reader<std::basic_istream<Char>>;

	template <typename Char>
	utf8_reader(std::basic_string_view<Char>, std::string&&) -> utf8_reader<std::basic_string_view<Char>>;

	template <typename Char>
	utf8_reader(std::basic_istream<Char>&, std::string&&) -> utf8_reader<std::basic_istream<Char>>;

	#if !TOML_EXCEPTIONS
		#undef TOML_ERROR_CHECK
		#define TOML_ERROR_CHECK	if (reader.error()) return nullptr
	#endif

	class TOML_EMPTY_BASES utf8_buffered_reader final
		: public utf8_reader_interface
	{
		public:
			static constexpr size_t max_history_length = 64;

		private:
			static constexpr size_t history_buffer_size = max_history_length - 1; //'head' is stored in the reader
			utf8_reader_interface& reader;
			struct
			{
				
				utf8_codepoint buffer[history_buffer_size];
				size_t count, first;
			}
			history = {};
			const utf8_codepoint* head = {};
			size_t negative_offset = {};

		public:

			explicit utf8_buffered_reader(utf8_reader_interface& reader_) noexcept
				: reader{ reader_ }
			{}

			[[nodiscard]]
			const source_path_ptr& source_path() const noexcept override
			{
				return reader.source_path();
			}

			[[nodiscard]]
			const utf8_codepoint* read_next() override
			{
				TOML_ERROR_CHECK;

				if (negative_offset)
				{
					negative_offset--;

					// an entry negative offset of 1 just means "replay the current head"
					if (!negative_offset) 
						return head;

					// otherwise step back into the history buffer
					else
						return history.buffer + ((history.first + history.count - negative_offset) % history_buffer_size);
				}
				else
				{
					// first character read from stream
					if TOML_UNLIKELY(!history.count && !head)
						head = reader.read_next();

					// subsequent characters and not eof
					else if (head)
					{
						if TOML_UNLIKELY(history.count < history_buffer_size)
							history.buffer[history.count++] = *head;
						else
							history.buffer[(history.first++ + history_buffer_size) % history_buffer_size] = *head;

						head = reader.read_next();
					}

					return head;
				}
			}

			[[nodiscard]]
			const utf8_codepoint* step_back(size_t count) noexcept
			{
				TOML_ERROR_CHECK;
				TOML_ASSERT(history.count);
				TOML_ASSERT(negative_offset + count <= history.count);

				negative_offset += count;

				return negative_offset
					? history.buffer + ((history.first + history.count - negative_offset) % history_buffer_size)
					: head;
			}

			[[nodiscard]]
			bool peek_eof() const override
			{
				return reader.peek_eof();
			}

			#if !TOML_EXCEPTIONS

			[[nodiscard]]
			optional<parse_error>&& error() noexcept override
			{
				return reader.error();
			}

			#endif
	};


	#undef TOML_ERROR_CHECK
	#undef TOML_ERROR
	TOML_ABI_NAMESPACE_END // TOML_EXCEPTIONS
	TOML_POP_WARNINGS
}

TOML_POP_WARNINGS // TOML_DISABLE_PADDING_WARNINGS
