#pragma once
#include <vector>
#include <functional>
#include <memory>

namespace Algorithm {

	namespace Search {

		#pragma region Binary

		/**
		* @brief Gets an element by a specific ID from a sorted list using binary search
		* @tparam T The type of elements stored in the list
		* @tparam getIDFn Callable that returns the ID for a given element reference
		* @param list Reference to a sorted vector containing elements of type T
		* @param getID Function that returns the ID of a given element
		* @param searchID The ID to search for
		* @return Pointer to the found element, or nullptr if not found
		*/
		template<typename T, typename getIDFn>
		inline T* GetBinary(
			std::vector<T>& list, 
			getIDFn getID,
			unsigned int searchID
		) {
			if (list.empty()) 
				return nullptr;

			size_t startIndex = 0;
			size_t endIndex = list.size() - 1;

			while (startIndex <= endIndex) {
				size_t mid = startIndex + (endIndex - startIndex) / 2;
				T& element = list[mid];
				unsigned int id = getID(element);

				if (id == searchID) {
					return &element;
				}
				else if (id > searchID) {
					if (mid == 0) break;
					endIndex = mid - 1;
				}
				else {
					startIndex = mid + 1;
				}
			}

			return nullptr;
		}

		/**
		* @brief Gets an element by a specific ID from a sorted list of pointers using binary search
		* @tparam T The type of elements pointed to by the vector
		* @tparam getIDFn Callable that returns the ID for a given element reference
		* @param list Reference to a sorted vector of pointers
		* @param getID Function returning the ID of a given element pointer
		* @param searchID The ID to search for
		* @return Pointer to the found element, or nullptr if not found
		*/
		template<typename T, typename getIDFn>
		inline T* GetBinary(std::vector<T*>& list, getIDFn getID, unsigned int searchID) {
			if (list.empty())
				return nullptr;

			size_t startIndex = 0;
			size_t endIndex = list.size() - 1;

			while (startIndex <= endIndex) {
				size_t mid = startIndex + (endIndex - startIndex) / 2;
				T* element = list[mid];
				unsigned int id = getID(*element);  // Pass the dereferenced pointer

				if (id == searchID) {
					return element;
				}
				else if (id > searchID) {
					if (mid == 0) break;
					endIndex = mid - 1;
				}
				else {
					startIndex = mid + 1;
				}
			}

			return nullptr;
		}

		/**
		* @brief Gets an element by a specific ID from a sorted list of unique_ptr using binary search
		* @tparam T The type of the managed elements
		* @tparam getIDFn Callable that returns the ID for a given element reference
		* @param list Reference to a sorted vector of unique_ptr<T>
		* @param getID Function that returns the ID of a given element
		* @param searchID The ID to search for
		* @return Pointer to the found element (non-owning), or nullptr if not found
		*/
		template<typename T, typename getIDFn>
		inline T* GetBinary(
			std::vector<std::unique_ptr<T>>& list,
			getIDFn getID,
			unsigned int searchID
		) {
			if (list.empty()) 
				return nullptr;

			size_t startIndex = 0;
			size_t endIndex = list.size() - 1;

			while (startIndex <= endIndex) {
				size_t mid = startIndex + (endIndex - startIndex) / 2;
				T& element = *list[mid];  // Dereference unique_ptr
				unsigned int id = getID(element);

				if (id == searchID) {
					return list[mid].get();  // Return raw pointer (non-owning)
				}
				else if (id > searchID) {
					if (mid == 0) break;
					endIndex = mid - 1;
				}
				else {
					startIndex = mid + 1;
				}
			}

			return nullptr;
		}

		/**
		* @brief Gets an element by a specific ID from a sorted list of shared_ptr using binary search
		* @tparam T The type of the managed elements
		* @tparam getIDFn Callable that returns the ID for a given element reference
		* @param list Reference to a sorted vector of shared_ptr<T>
		* @param getID Function that returns the ID of a given element
		* @param searchID The ID to search for
		* @return Shared pointer to the found element, or nullptr if not found
		*/
		template<typename T, typename getIDFn>
		inline std::shared_ptr<T> GetBinary(
			std::vector<std::shared_ptr<T>>& list,
			getIDFn getID,
			unsigned int searchID
		) {
			if (list.empty()) 
				return nullptr;

			size_t startIndex = 0;
			size_t endIndex = list.size() - 1;

			while (startIndex <= endIndex) {
				size_t mid = startIndex + (endIndex - startIndex) / 2;
				T& element = *list[mid];  // Dereference shared_ptr
				unsigned int id = getID(element);

				if (id == searchID) {
					return list[mid];
				}
				else if (id > searchID) {
					if (mid == 0) break;
					endIndex = mid - 1;
				}
				else {
					startIndex = mid + 1;
				}
			}

			return std::shared_ptr<T>{};
		}

		/**
		* @brief Recursively searches for an element in a sorted vector (and its hierarchical children) using binary search.
		* @tparam T The type of elements in the vector
		* @tparam getIDFn Function type that returns an ID from an element
		* @tparam ChildFn Function type that returns a reference to the children vector of an element
		* @param list Reference to a sorted vector of T
		* @param getID Callable that returns the ID of a given element
		* @param searchID The ID to search for
		* @param getChildren Callable that returns a reference to a vector of children elements for a given element
		* @return Pointer to the found element, or nullptr if not found
		*/
		template<typename T, typename getIDFn, typename ChildFn>
		inline T* GetBinaryRecursive(
			std::vector<T>& list,
			getIDFn getID,
			unsigned int searchID,
			ChildFn getChildren
		) {
			if (list.empty())
				return nullptr;

			size_t startIndex = 0;
			size_t endIndex = list.size() - 1;

			while (startIndex <= endIndex) {
				size_t mid = startIndex + (endIndex - startIndex) / 2;
				T& element = list[mid];
				unsigned int id = getID(element);

				if (id == searchID) {
					return &element;
				}
				else if (id > searchID) {
					if (mid == 0) break;
					endIndex = mid - 1;
				}
				else {
					startIndex = mid + 1;
				}

				auto& children = getChildren(element);
				if (!children.empty()) {
					T* found = GetBinaryRecursive(children, getID, searchID, getChildren);
					if (found) return found;
				}
			}

			return nullptr;
		}

		/**
		* @brief Recursively searches for an element in a sorted vector of unique_ptr (and its hierarchical children) using binary search.
		* @tparam T The type of elements managed by unique_ptr
		* @tparam getIDFn Function type that returns an ID from an element
		* @tparam ChildFn Function type that returns a reference to the children vector of an element
		* @param list Reference to a sorted vector of std::unique_ptr<T>
		* @param getID Callable that returns the ID of a given element
		* @param searchID The ID to search for
		* @param getChildren Callable that returns a reference to a vector of children elements for a given element
		* @return Non-owning raw pointer to the found element, or nullptr if not found
		*/
		template<typename T, typename getIDFn, typename ChildFn>
		inline T* GetBinaryRecursive(
			std::vector<std::unique_ptr<T>>& list,
			getIDFn getID,
			unsigned int searchID,
			ChildFn getChildren
		) {
			if (list.empty())
				return nullptr;

			size_t startIndex = 0;
			size_t endIndex = list.size() - 1;

			while (startIndex <= endIndex) {
				size_t mid = startIndex + (endIndex - startIndex) / 2;
				T& element = *list[mid];
				unsigned int id = getID(element);

				if (id == searchID) {
					return list[mid].get();  // non-owning raw pointer
				}
				else if (id > searchID) {
					if (mid == 0) break;
					endIndex = mid - 1;
				}
				else {
					startIndex = mid + 1;
				}

				auto& children = getChildren(element);
				if (!children.empty()) {
					T* found = GetBinaryRecursive(children, getID, searchID, getChildren);
					if (found) return found;
				}
			}

			return nullptr;
		}

		/**
		* @brief Recursively searches for an element in a sorted vector of shared_ptr (and its hierarchical children) using binary search.
		* @tparam T The type of elements managed by shared_ptr
		* @tparam getIDFn Function type that returns an ID from an element
		* @tparam ChildFn Function type that returns a reference to the children vector of an element
		* @param list Reference to a sorted vector of std::shared_ptr<T>
		* @param getID Callable that returns the ID of a given element
		* @param searchID The ID to search for
		* @param getChildren Callable that returns a reference to a vector of children elements for a given element
		* @return Shared pointer to the found element, or nullptr if not found
		*/
		template<typename T, typename getIDFn, typename ChildFn>
		inline std::shared_ptr<T> GetBinaryRecursive(
			std::vector<std::shared_ptr<T>>& list,
			getIDFn getID,
			unsigned int searchID,
			ChildFn getChildren
		) {
			if (list.empty())
				return nullptr;

			size_t startIndex = 0;
			size_t endIndex = list.size() - 1;

			while (startIndex <= endIndex) {
				size_t mid = startIndex + (endIndex - startIndex) / 2;
				T& element = *list[mid];
				unsigned int id = getID(element);

				if (id == searchID) {
					return list[mid];
				}
				else if (id > searchID) {
					if (mid == 0) break;
					endIndex = mid - 1;
				}
				else {
					startIndex = mid + 1;
				}

				// Recursive search in children
				auto& children = getChildren(element);
				if (!children.empty()) {
					std::shared_ptr<T> found = GetBinaryRecursive(children, getID, searchID, getChildren);
					if (found)
						return found;
				}
			}

			return std::shared_ptr<T>{};
		}

		#pragma endregion

		#pragma region Linear

		/**
		* @brief Gets an element that satisfies a condition from a list using linear search
		* @tparam T The type of elements stored in the list
		* @tparam CondFn Callable type that determines whether an element matches the search condition
		* @param list Reference to a vector containing elements of type T
		* @param condition Function that returns true for the element to find
		* @return Pointer to the found element, or nullptr if not found
		*/
		template<typename T, typename CondFn>
		inline T* GetLinear(std::vector<T>& list, CondFn condition) {
			for (auto& e : list)
				if (condition(e))
					return &e;

			return nullptr;
		}

		/**
		* @brief Performs a linear search on a vector of raw pointers.
		* @tparam T Type of elements pointed to by the pointers in the vector
		* @tparam CondFn Callable type that determines whether an element matches the search condition
		* @param list Reference to a vector containing raw pointers to elements of type T
		* @param condition Callable (e.g., lambda) that takes a T& and returns true if it matches the search condition
		* @return Pointer to the found element, or nullptr if no element satisfies the condition
		*/
		template<typename T, typename CondFn>
		inline T* GetLinear(std::vector<T*>& list, CondFn condition) {
			for (auto& e : list)
				if (condition(*e))
					return e;

			return nullptr;
		}

		/**
		* @brief Gets an element that satisfies a condition from a list of unique_ptr using linear search
		* @tparam T The type of the managed elements
		* @tparam CondFn Callable type that determines whether an element matches the search condition
		* @param list Reference to a vector of unique_ptr<T>
		* @param condition Function that returns true for the element to find
		* @return Pointer to the found element (non-owning), or nullptr if not found
		*/
		template<typename T, typename CondFn>
		inline T* GetLinear(std::vector<std::unique_ptr<T>>& list, CondFn condition) {
			for (auto& e : list)
				if (condition(*e))
					return e.get();

			return nullptr;
		}

		/**
		* @brief Gets an element that satisfies a condition from a list of shared_ptr using linear search
		* @tparam T The type of the managed elements
		* @tparam CondFn Callable type that determines whether an element matches the search condition
		* @param list Reference to a vector of shared_ptr<T>
		* @param condition Function that returns true for the element to find
		* @return Shared pointer to the found element, or nullptr if not found
		*/
		template<typename T, typename CondFn>
		inline std::shared_ptr<T> GetLinear(std::vector<std::shared_ptr<T>>& list, CondFn condition) {
			for (auto& e : list)
				if (condition(*e))
					return e;

			return std::shared_ptr<T>{};
		}

		/**
		* @brief Recursively searches a vector and its children for an element matching the given condition.
		* @tparam T Type of elements in the vector
		* @tparam CondFn Callable that checks if an element matches the search condition
		* @tparam ChildFn Callable that retrieves the children vector reference
		* @param list Vector of elements to search through
		* @param condition Callable that returns true if an element matches the search condition
		* @param getChildren Callable that returns a reference to the children vector of a given element
		* @return Pointer to the found element, or nullptr if not found
		*/
		template<typename T, typename CondFn, typename ChildFn>
		inline T* GetLinearRecursive(
			std::vector<T>& list,
			CondFn condition,
			ChildFn getChildren
		) {
			for (auto& element : list) {
				if (condition(element))
					return &element;

				auto& children = getChildren(element);
				if (!children.empty()) {
					T* found = GetLinearRecursive(children, condition, getChildren);
					if (found)
						return found;
				}
			}
			return nullptr;
		}

		/**
		* @brief Recursively searches a vector of unique_ptr and their children for an element matching the given condition.
		* @tparam T Type of the managed elements
		* @tparam CondFn Callable that checks if an element matches the search condition
		* @tparam ChildFn Callable that retrieves the children vector reference
		* @param list Vector of unique_ptr elements to search through
		* @param condition Callable that returns true if an element matches the search condition
		* @param getChildren Callable that returns a reference to the children vector of a given element
		* @return Pointer to the found element, or nullptr if not found
		*/
		template<typename T, typename CondFn, typename ChildFn>
		inline T* GetLinearRecursive(
			std::vector<std::unique_ptr<T>>& list,
			CondFn condition,
			ChildFn getChildren
		) {
			for (auto& element : list) {
				if (condition(*element))
					return element.get();

				auto& children = getChildren(*element);
				if (!children.empty()) {
					T* found = GetLinearRecursive(children, condition, getChildren);
					if (found)
						return found;
				}
			}
			return nullptr;
		}

		/**
		* @brief Recursively searches a vector of shared_ptr and their children for an element matching the given condition.
		* @tparam T Type of the managed elements
		* @tparam CondFn Callable that checks if an element matches the search condition
		* @tparam ChildFn Callable that retrieves the children vector reference
		* @param list Vector of shared_ptr elements to search through
		* @param condition Callable that returns true if an element matches the search condition
		* @param getChildren Callable that returns a reference to the children vector of a given element
		* @return Shared pointer to the found element, or empty shared_ptr if not found
		*/
		template<typename T, typename CondFn, typename ChildFn>
		inline std::shared_ptr<T> GetLinearRecursive(
			std::vector<std::shared_ptr<T>>& list,
			CondFn condition,
			ChildFn getChildren
		) {
			for (auto& element : list) {
				if (condition(*element))
					return element;

				auto& children = getChildren(*element);
				if (!children.empty()) {
					std::shared_ptr<T> found = GetLinearRecursive(children, condition, getChildren);
					if (found)
						return found;
				}
			}
			return std::shared_ptr<T>{};
		}

		#pragma endregion

	}

	namespace Sorting {
		
		/**
		* @brief Sorts a list in-place using a user-defined comparison function.
		* @tparam T Type of elements stored in the vector
		* @param list Reference to the vector to be sorted
		* @param condition Function returning true if the first element should come before the second
		*/
		template<typename T>
		inline void BubbleSort(std::vector<T>& list, std::function<bool(const T& a, const T& b)> condition) {
			size_t n = list.size();
			for (size_t i = 0; i < n; ++i) {
				bool swapped = false;
				for (size_t j = 0; j < n - 1 - i; ++j) {
					// Swap if the condition indicates list[j] should come before list[i]
					if (!condition(list[j], list[j + 1])) {
						std::swap(list[j], list[j + 1]);
						swapped = true;
					}
				}
				if (!swapped) 
					break;
			}
		}

		/**
		* @brief Performs Bubble Sort on 'list' and stores the sorted elements in 'outList'
		* @tparam T Type of elements
		* @param list The input vector to be sorted
		* @param outList Vector that will contain the sorted elements (must be resized to list.size())
		* @param condition Comparison function: returns true if first element should come before second
		*/
		template<typename T>
		inline void BubbleSort(std::vector<T>& list, std::vector<T>& outList, std::function<bool(const T& a, const T& b)> condition) {
			outList = list;

			for (size_t i = 0; i < outList.size(); ++i) {
				bool swapped = false;
				for (size_t j = 0; j < outList.size() - 1 - i; ++j) {
					if (!condition(outList[j], outList[j + 1])) {
						std::swap(outList[j], outList[j + 1]);
						swapped = true;
					}
				}
				if (!swapped) break;
			}
		}

	}

	namespace Easing {

		/// Constant for π
		constexpr double ALGORITHM_PI = 3.14159265358979323846;

		/**
		* @brief Ease-in sine function
		* @tparam T Numeric type (float, double, etc.)
		* @param t Normalized time [0,1]
		* @return Eased value
		*/
		template<typename T>
		T EaseInSine(T t) {
			return static_cast<T>(1.0) - std::cos((t * static_cast<T>(ALGORITHM_PI)) / static_cast<T>(2.0));
		}

		/**
		* @brief Ease-out sine function
		* @tparam T Numeric type (float, double, etc.)
		* @param t Normalized time [0,1]
		* @return Eased value
		*/
		template<typename T>
		T EaseOutSine(T t) {
			return std::sin((t * static_cast<T>(ALGORITHM_PI)) / static_cast<T>(2.0));
		}

		/**
		* @brief Ease-in-out sine function
		* @tparam T Numeric type (float, double, etc.)
		* @param t Normalized time [0,1]
		* @return Eased value
		*/
		template<typename T>
		T EaseInOutSine(T t) {
			return -(std::cos(static_cast<T>(ALGORITHM_PI) * t) - static_cast<T>(1.0)) / static_cast<T>(2.0);
		}

		/**
		* @brief Ease-in quadratic function
		* @tparam T Numeric type (float, double, etc.)
		* @param t Normalized time [0,1]
		* @return Eased value
		*/
		template<typename T>
		T EaseInQuad(T t) {
			return t * t;
		}

		/**
		* @brief Ease-out quadratic function
		* @tparam T Numeric type (float, double, etc.)
		* @param t Normalized time [0,1]
		* @return Eased value
		*/
		template<typename T>
		T EaseOutQuad(T t) {
			return t * (static_cast<T>(2) - t);
		}

		/**
		* @brief Ease-in-out quadratic function
		* @tparam T Numeric type (float, double, etc.)
		* @param t Normalized time [0,1]
		* @return Eased value
		*/
		template<typename T>
		T EaseInOutQuad(T t) {
			return t < static_cast<T>(0.5) ? static_cast<T>(2) * t * t : -static_cast<T>(1) + (static_cast<T>(4) - static_cast<T>(2) * t) * t;
		}

		/**
		* @brief Ease-in cubic function
		* @tparam T Numeric type (float, double, etc.)
		* @param t Normalized time [0,1]
		* @return Eased value
		*/
		template<typename T>
		T EaseInCubic(T t) {
			return t * t * t;
		}

		/**
		* @brief Ease-out cubic function
		* @tparam T Numeric type (float, double, etc.)
		* @param t Normalized time [0,1]
		* @return Eased value
		*/
		template<typename T>
		T EaseOutCubic(T t) {
			t -= static_cast<T>(1);
			return t * t * t + static_cast<T>(1);
		}

		/**
		* @brief Ease-in-out cubic function
		* @tparam T Numeric type (float, double, etc.)
		* @param t Normalized time [0,1]
		* @return Eased value
		*/
		template<typename T>
		T EaseInOutCubic(T t) {
			return t < static_cast<T>(0.5) ? static_cast<T>(4) * t * t * t : std::pow(static_cast<T>(2) * t - static_cast<T>(2), 3) / static_cast<T>(2) + static_cast<T>(1);
		}

		/**
		* @brief Ease-in quartic function
		* @tparam T Numeric type (float, double, etc.)
		* @param t Normalized time [0,1]
		* @return Eased value
		*/
		template<typename T>
		T EaseInQuart(T t) {
			return t * t * t * t;
		}

		/**
		* @brief Ease-out quartic function
		* @tparam T Numeric type (float, double, etc.)
		* @param t Normalized time [0,1]
		* @return Eased value
		*/
		template<typename T>
		T EaseOutQuart(T t) {
			t -= static_cast<T>(1);
			return static_cast<T>(1) - t * t * t * t;
		}

		/**
		* @brief Ease-in-out quartic function
		* @tparam T Numeric type (float, double, etc.)
		* @param t Normalized time [0,1]
		* @return Eased value
		*/
		template<typename T>
		T EaseInOutQuart(T t) {
			return t < static_cast<T>(0.5) ? static_cast<T>(8) * t * t * t * t : static_cast<T>(1) - static_cast<T>(8) * std::pow(t - static_cast<T>(1), 4);
		}

		/**
		* @brief Ease-in quintic function
		* @tparam T Numeric type (float, double, etc.)
		* @param t Normalized time [0,1]
		* @return Eased value
		*/
		template<typename T>
		T EaseInQuint(T t) {
			return t * t * t * t * t;
		}

		/**
		* @brief Ease-out quintic function
		* @tparam T Numeric type (float, double, etc.)
		* @param t Normalized time [0,1]
		* @return Eased value
		*/
		template<typename T>
		T EaseOutQuint(T t) {
			t -= static_cast<T>(1);
			return static_cast<T>(1) + t * t * t * t * t;
		}

		/**
		* @brief Ease-in-out quintic function
		* @tparam T Numeric type (float, double, etc.)
		* @param t Normalized time [0,1]
		* @return Eased value
		*/
		template<typename T>
		T EaseInOutQuint(T t) {
			return t < static_cast<T>(0.5) ? static_cast<T>(16) * t * t * t * t * t : static_cast<T>(1) + static_cast<T>(16) * std::pow(t - static_cast<T>(1), 5);
		}

		/**
		* @brief Ease-in exponential function
		* @tparam T Numeric type (float, double, etc.)
		* @param t Normalized time [0,1]
		* @return Eased value
		*/
		template<typename T>
		T EaseInExpo(T t) {
			return t == static_cast<T>(0) ? static_cast<T>(0) : std::pow(static_cast<T>(2), static_cast<T>(10) * (t - static_cast<T>(1)));
		}

		/**
		* @brief Ease-out exponential function
		* @tparam T Numeric type (float, double, etc.)
		* @param t Normalized time [0,1]
		* @return Eased value
		*/
		template<typename T>
		T EaseOutExpo(T t) {
			return t == static_cast<T>(1) ? static_cast<T>(1) : static_cast<T>(1) - std::pow(static_cast<T>(2), -static_cast<T>(10) * t);
		}

		/**
		* @brief Ease-in-out exponential function
		* @tparam T Numeric type (float, double, etc.)
		* @param t Normalized time [0,1]
		* @return Eased value
		*/
		template<typename T>
		T EaseInOutExpo(T t) {
			if (t == static_cast<T>(0)) {
				return static_cast<T>(0);
			}
			if (t == static_cast<T>(1)) {
				return static_cast<T>(1);
			}
			return t < static_cast<T>(0.5) ? std::pow(static_cast<T>(2), static_cast<T>(20) * t - static_cast<T>(10)) / static_cast<T>(2) : (static_cast<T>(2) - std::pow(static_cast<T>(2), -static_cast<T>(20) * t + static_cast<T>(10))) / static_cast<T>(2);
		}

		/**
		* @brief Ease-in circular function
		* @tparam T Numeric type (float, double, etc.)
		* @param t Normalized time [0,1]
		* @return Eased value
		*/
		template<typename T>
		T EaseInCirc(T t) {
			return static_cast<T>(1) - std::sqrt(static_cast<T>(1) - t * t);
		}

		/**
		* @brief Ease-out circular function
		* @tparam T Numeric type (float, double, etc.)
		* @param t Normalized time [0,1]
		* @return Eased value
		*/
		template<typename T>
		T EaseOutCirc(T t) {
			t -= static_cast<T>(1);
			return std::sqrt(static_cast<T>(1) - t * t);
		}

		/**
		* @brief Ease-in-out circular function
		* @tparam T Numeric type (float, double, etc.)
		* @param t Normalized time [0,1]
		* @return Eased value
		*/
		template<typename T>
		T EaseInOutCirc(T t) {
			if (t < static_cast<T>(0.5)) {
				return (static_cast<T>(1) - std::sqrt(static_cast<T>(1) - static_cast<T>(4) * t * t)) / static_cast<T>(2);
			}
			t = static_cast<T>(2) * t - static_cast<T>(1);
			return (std::sqrt(static_cast<T>(1) - t * t) + static_cast<T>(1)) / static_cast<T>(2);
		}

		/**
		* @brief Ease-in back function
		* @tparam T Numeric type (float, double, etc.)
		* @param t Normalized time [0,1]
		* @return Eased value
		*/
		template<typename T>
		T EaseInBack(T t) {
			const T c1 = static_cast<T>(1.70158);
			return t * t * ((c1 + static_cast<T>(1)) * t - c1);
		}

		/**
		* @brief Ease-out back function
		* @tparam T Numeric type (float, double, etc.)
		* @param t Normalized time [0,1]
		* @return Eased value
		*/
		template<typename T>
		T EaseOutBack(T t) {
			const T c1 = static_cast<T>(1.70158);
			t -= static_cast<T>(1);
			return static_cast<T>(1) + t * t * ((c1 + static_cast<T>(1)) * t + c1);
		}

		/**
		* @brief Ease-in-out back function
		* @tparam T Numeric type (float, double, etc.)
		* @param t Normalized time [0,1]
		* @return Eased value
		*/
		template<typename T>
		T EaseInOutBack(T t) {
			const T c1 = static_cast<T>(1.70158);
			const T c2 = c1 * static_cast<T>(1.525);
			return t < static_cast<T>(0.5)
				? (std::pow(static_cast<T>(2) * t, 2) * ((c2 + static_cast<T>(1)) * static_cast<T>(2) * t - c2)) / static_cast<T>(2)
				: (std::pow(static_cast<T>(2) * t - static_cast<T>(2), 2) * ((c2 + static_cast<T>(1)) * (t * static_cast<T>(2) - static_cast<T>(2)) + c2) + static_cast<T>(2)) / static_cast<T>(2);
		}

		/**
		* @brief Ease-in elastic function
		* @tparam T Numeric type (float, double, etc.)
		* @param t Normalized time [0,1]
		* @return Eased value
		*/
		template<typename T>
		T EaseInElastic(T t) {
			return t == static_cast<T>(0) || t == static_cast<T>(1) ? t : -std::pow(static_cast<T>(2), static_cast<T>(10) * t - static_cast<T>(10)) * std::sin((t * static_cast<T>(10) - static_cast<T>(10.75)) * ((static_cast<T>(2) * static_cast<T>(ALGORITHM_PI)) / static_cast<T>(3)));
		}

		/**
		* @brief Ease-out elastic function
		* @tparam T Numeric type (float, double, etc.)
		* @param t Normalized time [0,1]
		* @return Eased value
		*/
		template<typename T>
		T EaseOutElastic(T t) {
			return t == static_cast<T>(0) || t == static_cast<T>(1) ? t : std::pow(static_cast<T>(2), -static_cast<T>(10) * t) * std::sin((t * static_cast<T>(10) - static_cast<T>(0.75)) * ((static_cast<T>(2) * static_cast<T>(ALGORITHM_PI)) / static_cast<T>(3))) + static_cast<T>(1);
		}

		/**
		* @brief Ease-in-out elastic function
		* @tparam T Numeric type (float, double, etc.)
		* @param t Normalized time [0,1]
		* @return Eased value
		*/
		template<typename T>
		T EaseInOutElastic(T t) {
			if (t == static_cast<T>(0) || t == static_cast<T>(1)) {
				return t;
			}
			t = t * static_cast<T>(2);
			if (t < static_cast<T>(1)) {
				return -static_cast<T>(0.5) * std::pow(static_cast<T>(2), static_cast<T>(10) * t - static_cast<T>(10)) * std::sin((t * static_cast<T>(10) - static_cast<T>(10.75)) * ((static_cast<T>(2) * static_cast<T>(ALGORITHM_PI)) / static_cast<T>(3)));
			}
			return std::pow(static_cast<T>(2), -static_cast<T>(10) * (t - static_cast<T>(1))) * std::sin((t * static_cast<T>(10) - static_cast<T>(0.75)) * ((static_cast<T>(2) * static_cast<T>(ALGORITHM_PI)) / static_cast<T>(3))) * static_cast<T>(0.5) + static_cast<T>(1);
		}

		/**
		* @brief Ease-out bounce function
		* @tparam T Numeric type (float, double, etc.)
		* @param t Normalized time [0,1]
		* @return Eased value
		*/
		template<typename T>
		T EaseOutBounce(T t) {
			const T n1 = static_cast<T>(7.5625);
			const T d1 = static_cast<T>(2.75);

			if (t < static_cast<T>(1) / d1) {
				return n1 * t * t;
			}
			else if (t < static_cast<T>(2) / d1) {
				t -= static_cast<T>(1.5) / d1;
				return n1 * t * t + static_cast<T>(0.75);
			}
			else if (t < static_cast<T>(2.5) / d1) {
				t -= static_cast<T>(2.25) / d1;
				return n1 * t * t + static_cast<T>(0.9375);
			}
			else {
				t -= static_cast<T>(2.625) / d1;
				return n1 * t * t + static_cast<T>(0.984375);
			}
		}

		/**
		* @brief Ease-in bounce function
		* @tparam T Numeric type (float, double, etc.)
		* @param t Normalized time [0,1]
		* @return Eased value
		*/
		template<typename T>
		T EaseInBounce(T t) {
			return static_cast<T>(1) - EaseOutBounce(static_cast<T>(1) - t);
		}

		/**
		* @brief Ease-in-out bounce function
		* @tparam T Numeric type (float, double, etc.)
		* @param t Normalized time [0,1]
		* @return Eased value
		*/
		template<typename T>
		T EaseInOutBounce(T t) {
			return t < static_cast<T>(0.5)
				? (static_cast<T>(1) - EaseOutBounce(static_cast<T>(1) - static_cast<T>(2) * t)) / static_cast<T>(2)
				: (static_cast<T>(1) + EaseOutBounce(static_cast<T>(2) * t - static_cast<T>(1))) / static_cast<T>(2);
		}

	}

}