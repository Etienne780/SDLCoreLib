#include <unordered_set>
#include <queue>
#include <CoreLib/Log.h>

#include "SDLCoreTypes.h"
#include "IDManager.h"

namespace SDLCore {

	IDManager::IDManager(IDOrder order) 
		: IDManager(0, order) {
	}

	IDManager::IDManager(uint32_t startCount, IDOrder order) {
		m_idCounter = startCount;
		SetIDOrder(order);
	}

	bool IDManager::IsIDFallback() {
		return m_idFallback;
	}

	uint32_t IDManager::GetNewUniqueIdentifier() {
		switch (m_order) {
		case IDOrder::RANDOME: return GetNewUniqueIdentifierRandom();
		case IDOrder::ASCENDING: return GetNewUniqueIdentifierAscending();
		case IDOrder::DESCENDING: return GetNewUniqueIdentifierDescending();
		}

		return SDLCORE_INVALID_ID;
	}

	void IDManager::FreeUniqueIdentifier(uint32_t id) {
		if (id == SDLCORE_INVALID_ID)
			return;

		// Find the first range with start > id
		auto it = m_rangeFreeIDs.upper_bound(id);

		uint32_t newStart = id;
		uint32_t newEnd = id;

		bool mergedLeft = false;
		bool mergedRight = false;

		// check left neighbor (previous range) to see if id connects
		if (it != m_rangeFreeIDs.begin()) {
			auto prev = std::prev(it);
			if (prev->second + 1 == id) {
				// extend previous range to include id
				prev->second = id;
				newStart = prev->first;
				newEnd = prev->second;
				mergedLeft = true;
			}
			else if (prev->first <= id && id <= prev->second) {
				// id already contained in a free range -> nothing to do
				return;
			}
		}


		// check right neighbor (it) to see if id connects
		if (it != m_rangeFreeIDs.end()) {
			if (it->first == id + 1) {
				// extend right range to include id
				if (mergedLeft) {
					// we previously extended left; now merge both ranges
					auto leftIt = m_rangeFreeIDs.find(newStart);
					if (leftIt != m_rangeFreeIDs.end()) {
						leftIt->second = it->second; // merge
						m_rangeFreeIDs.erase(it);
						return;
					}
				}
				else {
					// extend right range downward
					uint32_t oldEnd = it->second;
					uint32_t oldStart = it->first;
					m_rangeFreeIDs.erase(it);
					m_rangeFreeIDs.emplace(id, oldEnd);
					return;
				}
				mergedRight = true;
			}
			else if (it->first <= id && id <= it->second) {
				// id already contained in right range -> nothing to do
				return;
			}
		}


		if (!mergedLeft && !mergedRight) {
			// create new single-id range
			m_rangeFreeIDs.emplace(id, id);
		}
	}

	uint32_t IDManager::GetNewUniqueIdentifierFallback() {
		if (!m_idFallback) {
			m_idFallback = true;
			Log::Warn("SDLCore::IDManager: Max ID limit reached, using fallback IDs from free pool");
		}

		if (!m_rangeFreeIDs.empty()) {
			return GetRangeFreeID();
		}

		Log::Warn("SDLCore::IDManager: Can't find any free IDs, free pool is empty!");
		return SDLCORE_INVALID_ID;
	}

	uint32_t IDManager::GetRangeFreeID() {
		// return the lowest available id from the first range
		auto it = m_rangeFreeIDs.begin();
		uint32_t id = it->first;
		if (it->first == it->second) {
			// single element range -> erase
			m_rangeFreeIDs.erase(it);
		}
		else {
			// increment start of range
			uint32_t newStart = it->first + 1;
			uint32_t oldEnd = it->second;
			m_rangeFreeIDs.erase(it);
			m_rangeFreeIDs.emplace(newStart, oldEnd);
		}
		return id;
	}

	void IDManager::Reset(uint32_t startValue) {
		m_rangeFreeIDs.clear();

		switch (m_order) {
		case IDOrder::RANDOME:
			m_idCounter = startValue;
			m_idFallback = true;
			break;
		case IDOrder::ASCENDING:
			m_idCounter = startValue;
			m_idFallback = false;
			break;
		case IDOrder::DESCENDING:
			m_idCounter = startValue;
			m_idFallback = false;
			break;
		}
	}

	void IDManager::SetIDOrder(IDOrder value) {
		m_order = value;
		Reset((value == IDOrder::DESCENDING) ? idLimit : 0);
	}

#ifdef DEBUG
	const std::map<uint32_t, uint32_t>& IDManager::DebugGetRanges() const {
		return m_rangeFreeIDs; 
	}
#else
	const std::map<uint32_t, uint32_t>& IDManager::DebugGetRanges() const {
		Log::Warn("SDLCore::IDManager::DebugGetRanges: Called DebugGetRanges outside of debug builds!");
		static std::map<uint32_t, uint32_t> dummy; 
		return dummy;
	}
#endif

	uint32_t IDManager::GetNewUniqueIdentifierRandom() {
		uint32_t id;
		
		if (!m_rangeFreeIDs.empty()) {
			id = GetRangeFreeID();
		}
		else {
			if (m_idCounter != idLimit) {
				id = m_idCounter++;
			}
			else {
				id = idLimit;// will trigger fallback next call
			}
		}

		return id;
	}

	uint32_t IDManager::GetNewUniqueIdentifierAscending() {
		uint32_t id;

		if (m_idFallback) {
			id = GetNewUniqueIdentifierFallback();
		}
		else {
			if (m_idCounter != idLimit) {
				id = m_idCounter++;
			}
			else {
				id = GetNewUniqueIdentifierFallback();
			}
		}

		return id;
	}

	uint32_t IDManager::GetNewUniqueIdentifierDescending() {
		uint32_t id;

		if (m_idFallback) {
			id = GetNewUniqueIdentifierFallback();
		}
		else {
			if (m_idCounter != 0) {
				id = m_idCounter--;
			}
			else {
				id = GetNewUniqueIdentifierFallback();
			}
		}

		return id;
	}

}