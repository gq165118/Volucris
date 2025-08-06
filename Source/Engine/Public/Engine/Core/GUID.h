#ifndef __volucris_guid_h__
#define __volucris_guid_h__

#include <Engine/Core/TypesHelp.h>
#include <string>
#include <fmt/format.h>
#include <array>
#include <boost/serialization/access.hpp>
#include <boost/serialization/array.hpp>

namespace volucris
{
	struct GUID
	{
		unsigned long  Data1 = 0;
		unsigned short Data2 = 0;
		unsigned short Data3 = 0;
		std::array<unsigned char, 8> Data4;

		std::string toString() const {
			char buffer[64];
			snprintf(buffer, sizeof(buffer),
				"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
				Data1, Data2, Data3,
				Data4[0], Data4[1],
				Data4[2], Data4[3],
				Data4[4], Data4[5],
				Data4[6], Data4[7]);
			return std::string(buffer);
		}

		static GUID generate();

		template <class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& Data1;
			ar& Data2;
			ar& Data3;
			ar& Data4;
		}	

	};

	inline bool operator==(const GUID& lhs, const GUID& rhs) {
		return lhs.Data1 == rhs.Data1 &&
			lhs.Data2 == rhs.Data2 &&
			lhs.Data3 == rhs.Data3 &&
			lhs.Data4 == rhs.Data4;
	}

}

namespace std {
	template <>
	struct hash<volucris::GUID> {
		size_t operator()(const volucris::GUID& guid) const noexcept {
			size_t h1 = hash<unsigned long>()(guid.Data1);
			size_t h2 = hash<unsigned short>()(guid.Data2);
			size_t h3 = hash<unsigned short>()(guid.Data3);
			size_t h4 = 0;
			for (auto b : guid.Data4) {
				h4 ^= hash<unsigned char>()(b) + 0x9e3779b9 + (h4 << 6) + (h4 >> 2);
			}
			return h1 ^ (h2 << 1) ^ (h3 << 2) ^ h4;
		}
	};
}

// 格式化vector
template <>
struct fmt::formatter<volucris::GUID> {
	constexpr auto parse(format_parse_context& ctx) {
		return ctx.begin();
	}

	auto format(const volucris::GUID& guid, format_context& ctx) const {
		auto out = ctx.out();
		return fmt::format_to(out, "{{{}}}", guid.toString());
	}
};

#endif // !__volucris_guid_h__
