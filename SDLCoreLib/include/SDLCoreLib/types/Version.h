#pragma once
#include <string>
#include <CoreLib/FormatUtils.h>

namespace SDLCore {

	enum class VersionType
	{
		SINGLE_COMPONENT,   // e.g., "1"
		DUAL_COMPONENT,     // e.g., "1.2"
		SEMANTIC            // e.g., "1.2.3"
	};

	class Version {
	public:
		Version() = default;
		Version(uint32_t version);
		Version(uint32_t major, uint32_t minor);
		Version(uint32_t major, uint32_t minor, uint32_t patch);

		std::string ToString() const;

		VersionType GetVersionType() const;
		uint32_t GetVersion() const;
		uint32_t GetMajor() const;
		uint32_t GetMinor() const;
		uint32_t GetPatch() const;

	private:
		VersionType m_versionType = VersionType::SINGLE_COMPONENT;
		uint32_t m_major = 0;
		uint32_t m_minor = 0;
		uint32_t m_patch = 0;
	};

}

template<>
static inline std::string FormatUtils::toString<SDLCore::Version>(SDLCore::Version value) {
	return value.ToString();
}

template<>
static inline std::string FormatUtils::toString<SDLCore::VersionType>(SDLCore::VersionType value) {
	switch (value) {
	case SDLCore::VersionType::SINGLE_COMPONENT: return "SINGLE_COMPONENT";
	case SDLCore::VersionType::DUAL_COMPONENT: return "DUAL_COMPONENT";
	case SDLCore::VersionType::SEMANTIC: return "SEMANTIC";
	}
	return "UNKNOWN";
}
