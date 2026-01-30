#pragma once
#include <vector>

#include <CoreLib/Math/Vector2.h>

#include "Types/Types.h"
#include "UI/Types/UITypes.h"
#include "UI/Types/UIEvent.h"

namespace SDLCore::UI {

	class UICTXWrapper;
	class UINode;
	class FrameNode;
	class UIContext {
		friend class UICTXWrapper;
		friend class UINode;
	public:
		~UIContext();

		static UIContext* CreateContext();

		void CapturePressNode(uintptr_t id);
		void CaptureDragNode(uintptr_t id);
		void CaptureFocusNode(uintptr_t id);

		void ReleasePressNode(uintptr_t id);
		void ReleaseDragNode(uintptr_t id);
		void ReleaseFocusNode(uintptr_t id);

		bool HasPressNodeCaptured() const;
		bool HasDragNodeCaptured() const;
		bool HasFocusNodeCaptured() const;

		WindowID GetWindowID() const;
		float GetWindowScale() const;
		Vector2 GetWindowSize() const;

		Vector2 GetMousePos() const;
		Vector2 GetMouseDelta() const;
		bool GetLeftMouseDown() const;
		bool GetLeftMouseJustDown() const;
		bool GetLeftMouseJustUp() const;

		// should be used with HasPressNodeCaptured() to check if a node is Captured
		uintptr_t GetActiveCapturedPressNode() const;
		// should be used with HasDragNodeCaptured() to check if a node is Captured
		uintptr_t GetActiveCapturedDragNode() const;
		// should be used with HasFocusNodeCaptured() to check if a node is Captured
		uintptr_t GetActiveCapturedFocusNode() const;

		size_t GetNodeCount() const;

		void SetNodeState(UINode* node, UIState state);

	private:
		UIContext();
		WindowCallbackID m_windowResizeCBID;

		std::vector<uint16_t> m_lastChildPosition;/*< is the position of the current child inside of last parent node*/
		std::vector<UINode*> m_nodeCreationStack;/*< is for creating nodes. if a node is this stack, than those nodes are currently created*/
		std::vector<UINode*> m_lastNodeStack;
		std::vector<UINode*> m_relativeStack;

		std::shared_ptr<FrameNode> m_rootNode = nullptr;
		size_t m_currentNodeCount = 0;/* < changes while building nodeStack. Is used to update the stable node count */
		size_t m_nodeCount = 0;/* < is the stable node count. gets update with the last end call */
		WindowID m_windowID;
		float m_windowContentScale = 1.0f;
		Vector2 m_windowSize{ 0.0f, 0.0f };

		uint64_t m_updateInputFrame = 0;
		Vector2 m_mousePos{ 0.0f, 0.0f };
		Vector2 m_mouseDelta{ 0.0f, 0.0f };
		bool m_leftMouseDown = false;
		bool m_leftMouseJustDown = false;
		bool m_leftMouseJustUp = false;

		uintptr_t m_pressNodeID = 0;/*< Address can not be 0 */
		uintptr_t m_dragNodeID = 0;/*< Address can not be 0 */
		uintptr_t m_focusNodeID = 0;/*< Address can not be 0 */

		FrameNode* BeginFrame(uintptr_t id);
		UIEvent EndFrame();
		
		template<typename T, typename ...Args>
		T* AddNode(uintptr_t id, Args&&... args) {
			m_currentNodeCount++;
			if (!m_rootNode) {
				Log::Error("SDLCore::UI::UICTXWrapper::AddNode: Could not add not. no valid root element was found!");
				return nullptr;
			}

			UINode* parentNode = m_lastNodeStack.back();
			uint16_t pos = (m_lastChildPosition.empty()) ? 0 : m_lastChildPosition.back();
			UINode* currentNode = nullptr;

			m_lastChildPosition.back()++;
			if (parentNode->ContainsChildAtPos(pos, id, currentNode)) {
				// element with id exists at position. set it as last position
				// no stack increas cause add node has no end func
				CalculateClippingMask(currentNode);
				ProcessEvent(this, currentNode);
				currentNode->SetNodeActive();
				return reinterpret_cast<T*>(currentNode);
			}

			// remove pos and every entry after
			parentNode->RemoveChildrenFromIndex(pos);

#ifndef NDEBUG
			LogDuplicateIDIfAny(id, parentNode);
#endif

			// element does not exist. create element and create stack
			T* childNode = parentNode->AddChild<T>(static_cast<int>(pos), id, std::forward<Args>(args)...);
			CalculateClippingMask(reinterpret_cast<UINode*>(childNode));
			ProcessEvent(this, childNode);
			// no stack increas cause add node has no end func
			return childNode;
		}

		/*
		* @brief Traverses the UI node tree in depth-first (pre-order) order and applies a function to each node.
		*
		* This helper performs a recursive traversal starting from the given node.
		* The provided callable is invoked once for every node in the subtree,
		* including the root node itself.
		*
		* The traversal order is:
		*   1. Current node
		*   2. All children recursively (depth-first)
		*
		* The function is implemented as a template to avoid std::function overhead
		* and allow full inlining by the compiler.
		*
		* @tparam Func Callable type. Must be invocable as: void(UINode*)
		* @param node  Root node of the traversal. If null, the function returns immediately.
		* @param fn    Function to execute for each visited node.
		*
		* Example:
		*   ForEachNode(root, [](UINode* node) {
		*       node->ApplyStyle(ctx);
		*   });
		*/
		template<typename Func>
		void ForEachNode(UINode* node, Func&& fn) {
			if (!node)
				return;

			fn(node);

			for (const auto& child : node->GetChildren()) {
				ForEachNode(child.get(), fn);
			}
		}

		UINode* GetRootNode() const;
		const UINode* GetLastRelativeNode() const;
		void SetWindowParams(WindowID id);
		void RemoveWindowCB();

		// gets called on window resize
		void UpdateNodeStylesWindowResize();
		void UpdateInput();
		void ResolveNodeState(UIContext* ctx, UINode* node);
		UIEvent* ProcessEvent(UIContext* ctx, UINode* node);
		void RenderNodes(UIContext* ctx, UINode* rootNode);
		// gets called in begin, add element
		void CalculateClippingMask(UINode* node);
		Vector4 UIContext::GetWindowClip() const;
		Vector4 CreateClipRect(UINode* node) const;
		Vector4 IntersectAxis(const Vector4& parent, const Vector4& self, bool clipX, bool clipY) const;

		/**
		* @brief Checks whether an ID is unique among siblings and all parent layers.
		* @param idToCheck ID to validate
		* @param parent Parent node defining the current layer
		* @return true if the ID does not occur in any sibling or ancestor layer
		*/
		bool IsIDUnique(uintptr_t idToCheck, UINode* parent);
#ifndef NDEBUG
		void LogDuplicateIDIfAny(uintptr_t id, UINode* parent);
#endif
	};

}