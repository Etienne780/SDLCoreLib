#include "BinarySerializer.h"
#include "ConversionUtils.h"

std::vector<uint8_t> BinarySerializer::ToBuffer() const {
	return m_buffer;
}

std::string BinarySerializer::ToBase64() const {
	return ConversionUtils::ToBase64(m_buffer);
}