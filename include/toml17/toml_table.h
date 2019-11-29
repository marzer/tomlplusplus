#pragma once
#include "toml_common.h"

namespace TOML17_NAMESPACE
{
	class table final
		: public node
	{
		private:
			std::map<string, std::shared_ptr<node>, std::less<>> values;
	};
}
