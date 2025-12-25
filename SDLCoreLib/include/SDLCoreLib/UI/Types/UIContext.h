#pragma once
#include <vector>

#include <CoreLib/Math/Vector2.h>

#include "IDManager.h"
#include "Types/Types.h"
#include "UI/Types/UITypes.h"
#include "UI/Types/UIEvent.h"

namespace SDLCore::UI {

	class UICTXWrapper;
	class UINode;
	class FrameNode;
	class UIContext {
		friend class UICTXWrapper;
	public:
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

	private:
		UIContext();

		std::vector<uint16_t> m_lastChildPosition;/*< is the position of the current child inside of last node*/
		std::vector<UINode*> m_nodeCreationStack;/*< is for creating nodes. if a node is this stack, than those nodes are currently created*/
		std::vector<UINode*> m_lastNodeStack;
		std::shared_ptr<FrameNode> m_rootNode = nullptr;
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
			ProcessEvent(this, childNode);
			// no stack increas cause add node has no end func
			return childNode;
		}

		UINode* GetRootNode() const;
		void SetWindowParams(WindowID id);

		void UpdateInput();
		static UIEvent* ProcessEvent(UIContext* ctx, UINode* node);
		static void RenderNodes(UIContext* ctx, UINode* rootNode);

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