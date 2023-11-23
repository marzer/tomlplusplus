#include <cstdint>
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
    FuzzedDataProvider fdp{data, size};
    try
    {
        const toml::table tbl = toml::parse(fdp.ConsumeRandomLengthString());

        switch (fdp.ConsumeEnum<SerializationTest>())
        {
            case SerializationTest::JSON:
                static_cast<void>(toml::json_formatter{tbl});
                break;
            case SerializationTest::YAML:
                static_cast<void>(toml::yaml_formatter{tbl});
                break;
            case SerializationTest::TOML:
                static_cast<void>(toml::toml_formatter{tbl});
            default:
                break;
        }
    }
    catch (const toml::parse_error&)
    {
        return -1;
    }
    return 0;
}
