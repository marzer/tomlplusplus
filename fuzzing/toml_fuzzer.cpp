#include <cstdint>
#include <sstream>
#include <fuzzer/FuzzedDataProvider.h>

#include <toml++/toml.hpp>

enum class SerializationTest
{
	NONE = 0,
	JSON,
	YAML,
	TOML,
	kMaxValue = TOML
};

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, const std::size_t size)
{
	FuzzedDataProvider fdp{ data, size };
	try
	{
		const toml::table tbl = toml::parse(fdp.ConsumeRandomLengthString());

		std::ostringstream ss;
		switch (fdp.ConsumeEnum<SerializationTest>())
		{
			case SerializationTest::JSON: ss << toml::json_formatter{ tbl }; break;
			case SerializationTest::YAML: ss << toml::yaml_formatter{ tbl }; break;
			case SerializationTest::TOML: ss << toml::toml_formatter{ tbl }; break;
			default: break;
		}
	}
	catch (const toml::parse_error&)
	{}
	return 0;
}
