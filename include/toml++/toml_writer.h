#pragma once
#include "toml_utf8.h"
#include "toml_value.h"
#include "toml_array.h"
#include "toml_table_array.h"

namespace toml::impl
{
	class TOML_INTERFACE writer
	{
		private:
			[[maybe_unused]] const table& tab;

		protected:
			virtual void write(const void*, size_t) = 0;

		public:
			writer(const table& table_) noexcept
				: tab{ table_ }
			{
			}

			virtual ~writer() noexcept = default;

			void operator() ()
			{

			}
	};

	template <typename T>
	class output_writer;

	template <typename CHAR>
	class output_writer<std::basic_string<CHAR>> final : public writer
	{
		static_assert(sizeof(CHAR) == 1_sz);

		private:
			std::basic_string<CHAR>& target;

			void write(const void* data, size_t size) override
			{
				TOML_ASSERT(data && size);
				target.append(std::basic_string_view<CHAR>{ reinterpret_cast<const CHAR*>(data), size });
			}

		public:
			explicit output_writer(std::basic_ostream<CHAR>& target_, const toml::table& tab) noexcept
				: writer(tab),
				target{ target_ }
			{
			}
	};

	template <typename CHAR>
	output_writer(std::basic_string<CHAR>&, const toml::table&) -> output_writer<std::basic_string<CHAR>>;

	template <typename CHAR>
	class output_writer<std::basic_ostream<CHAR>> final : public writer
	{
		static_assert(sizeof(CHAR) == 1_sz);

		private:
			std::basic_ostream<CHAR>& target;

			void write(const void* data, size_t size) override
			{
				TOML_ASSERT(data && size);
				target.write(reinterpret_cast<const CHAR*>(data), static_cast<std::streamsize>(size));
			}

		public:
			explicit output_writer(std::basic_ostream<CHAR>& target_, const toml::table& tab) noexcept
				: writer(tab),
				target{ target_ }
			{
			}
	};

	template <typename CHAR>
	output_writer(std::basic_ostream<CHAR>&, const toml::table&) -> output_writer<std::basic_ostream<CHAR>>;
}

namespace toml
{
	template <typename CHAR>
	inline std::basic_ostream<CHAR>& operator << (std::basic_ostream<CHAR>& lhs, const table& rhs)
	{
		static_assert(
			sizeof(CHAR) == 1,
			"The stream's underlying character type must be 1 byte in size."
		);

		impl::output_writer{ lhs, rhs }();
		return lhs;
	}
}
