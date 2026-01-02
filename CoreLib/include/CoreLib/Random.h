#pragma once
#include <random>
#include <unordered_map>

#include "IDManager.h"
#include "CoreID.h"
#include "FormatUtils.h"

const uint32_t RANDOM_DEVICE_INVALID_ID = std::numeric_limits<uint32_t>::max();

struct RandomDeviceTag {};
using RandomDeviceID = CoreID<uint32_t, RANDOM_DEVICE_INVALID_ID, RandomDeviceTag>;

class Random {
public:
	Random() = delete;

	static void SetSeed(uint32_t seed);
	static uint32_t GetSeed();
	static void SetSeed(RandomDeviceID id, uint32_t seed);
	static uint32_t GetSeed(RandomDeviceID id);

	static RandomDeviceID CreateRandomDevice(uint32_t seed = 0);
	static void DeleteRandomDevice(RandomDeviceID id);

	template<typename T>
	static T GetNumber();

	template<typename T>
	static T GetPositiveNumber();

	template<typename T>
	static T GetRangeNumber(T start, T end);

	template<typename T>
	static T GetNumber(RandomDeviceID id);

	template<typename T>
	static T GetPositiveNumber(RandomDeviceID id);

	template<typename T>
	static T GetRangeNumber(RandomDeviceID id, T start, T end);

private:
	struct Device {
		std::mt19937 generator;
		uint32_t seed = 0;
	};

	static inline IDManager<uint32_t, RANDOM_DEVICE_INVALID_ID> m_idManager;
	static inline std::unordered_map<RandomDeviceID, Device> m_devices;
	static inline Device m_defaultDevice{ std::mt19937(std::random_device{}()), 0 };
};

template<>
static inline std::string FormatUtils::toString<RandomDeviceID>(RandomDeviceID id) {
	return id.ToString();
}