#include <functional>

#include "Application.h"
#include "SDLCoreTime.h"
#include "UI/UINode.h"
#include "UI/Nodes/FrameNode.h"
#include "UI/Types/UIContext.h"

namespace SDLCore::UI {

    UIContext::UIContext() {
        size_t amount = 128;
        m_lastChildPosition.reserve(amount);
        m_nodeCreationStack.reserve(amount);
        m_lastNodeStack.reserve(amount);
    }

    UIContext::~UIContext() {
        RemoveWindowCB();
    }

	UIContext* UIContext::CreateContext() {
		return new UIContext();
	}

    void UIContext::CapturePressNode(uintptr_t id) {
        ReleasePressNode(m_pressNodeID);
        m_pressNodeID = id;
    }
    
    void UIContext::CaptureDragNode(uintptr_t id) {
        ReleaseDragNode(m_dragNodeID);
        m_dragNodeID = id;
    }

    void UIContext::CaptureFocusNode(uintptr_t id) {
        ReleaseDragNode(m_focusNodeID);
        m_focusNodeID = id;
    }

    void UIContext::ReleasePressNode(uintptr_t id) {
        if (m_pressNodeID != id)
            return;
        m_pressNodeID = 0;
    }

    void UIContext::ReleaseDragNode(uintptr_t id) {
        if (m_dragNodeID != id)
            return;
        m_dragNodeID = 0;
    }

    void UIContext::ReleaseFocusNode(uintptr_t id) {
        if (m_focusNodeID != id)
            return;
        m_focusNodeID = 0;
    }

    bool UIContext::HasPressNodeCaptured() const {
        return m_pressNodeID != 0;
    }

    bool UIContext::HasDragNodeCaptured() const {
        return m_dragNodeID != 0;
    }

    bool UIContext::HasFocusNodeCaptured() const {
        return m_focusNodeID != 0;
    }

    WindowID UIContext::GetWindowID() const {
        return m_windowID;
    }

    float UIContext::GetWindowScale() const {
        return m_windowContentScale;
    }

    Vector2 UIContext::GetWindowSize() const {
        return m_windowSize;
    }

    Vector2 UIContext::GetMousePos() const {
        return m_mousePos;
    }

    Vector2 UIContext::GetMouseDelta() const {
        return m_mouseDelta;
    }

    bool UIContext::GetLeftMouseDown() const {
        return m_leftMouseDown;
    }

    bool UIContext::GetLeftMouseJustDown() const {
        return m_leftMouseJustDown;
    }

    bool UIContext::GetLeftMouseJustUp() const {
        return m_leftMouseJustUp;
    }

    uintptr_t UIContext::GetActiveCapturedPressNode() const {
        return m_pressNodeID;
    }

    uintptr_t UIContext::GetActiveCapturedDragNode() const {
        return m_dragNodeID;
    }

    uintptr_t UIContext::GetActiveCapturedFocusNode() const {
        return m_focusNodeID;
    }

    FrameNode* UIContext::BeginFrame(uintptr_t id) {
        if (m_lastNodeStack.empty() && m_rootNode) {
            if (m_rootNode->GetID() != id) {
                // Creats new root node
                m_rootNode = nullptr;
            }
            else {
                // pushes root node on to stack
                m_lastNodeStack.push_back(reinterpret_cast<UINode*>(m_rootNode.get()));
                m_lastChildPosition.push_back(0);
                m_rootNode->SetNodeActive();
                return m_rootNode.get();
            }
        }
        
        if (!m_rootNode) {
            m_rootNode = std::make_shared<FrameNode>(-1, id);
            FrameNode* frame = m_rootNode.get();
            m_nodeCreationStack.push_back(reinterpret_cast<UINode*>(frame));
            m_lastNodeStack.push_back(reinterpret_cast<UINode*>(frame));
            m_lastChildPosition.push_back(0);
            return frame;
        }

        if (!m_nodeCreationStack.empty()) {
            UINode* parentNode = m_nodeCreationStack.back();
            if (!parentNode)
                return nullptr;

            int currentChildPos = (!m_lastChildPosition.empty()) ? 
                static_cast<int>(m_lastChildPosition.back()) : 0;

#ifndef NDEBUG
            LogDuplicateIDIfAny(id, parentNode);
#endif

            FrameNode* frame = parentNode->AddChild<FrameNode>(currentChildPos, id);
            m_nodeCreationStack.push_back(reinterpret_cast<UINode*>(frame));
            m_lastNodeStack.push_back(reinterpret_cast<UINode*>(frame));
            m_lastChildPosition.push_back(0);

            return frame;
        }
        else {
            UINode* parentNode = m_lastNodeStack.back();
            uint16_t pos = (m_lastChildPosition.empty()) ? 0 : m_lastChildPosition.back();
            UINode* currentNode = nullptr;
            if (parentNode->ContainsChildAtPos(pos, id, currentNode)) {
                // element with id exists at position. set it as last position
                m_lastNodeStack.push_back(currentNode);
                m_lastChildPosition.push_back(0);
                currentNode->SetNodeActive();
                return reinterpret_cast<FrameNode*>(currentNode);
            }
            else {
                // remove pos and every entry after
                parentNode->RemoveChildrenFromIndex(pos);

#ifndef NDEBUG
                LogDuplicateIDIfAny(id, parentNode);
#endif

                // element does not exist. create element and create stack
                FrameNode* frame = parentNode->AddChild<FrameNode>(static_cast<int>(pos), id);
                m_lastNodeStack.push_back(reinterpret_cast<UINode*>(frame));
                m_nodeCreationStack.push_back(reinterpret_cast<UINode*>(frame));
                m_lastChildPosition.push_back(0);
                return frame;
            }
        }

        return nullptr;
    }

    UIEvent UIContext::EndFrame() {
        if (!m_lastChildPosition.empty() && !m_lastNodeStack.empty()) {
            UINode* node = m_lastNodeStack.back();
            if (node && m_lastChildPosition.back() < node->GetChildren().size()) {
                node->RemoveChildrenFromIndex(m_lastChildPosition.back());
            }
            
            m_lastChildPosition.pop_back();
            if (!m_lastChildPosition.empty()) {
                m_lastChildPosition.back()++;
            }
        }

        if (!m_nodeCreationStack.empty()) {
            m_nodeCreationStack.pop_back();
        }

        if (!m_lastNodeStack.empty()) {
            UIEvent* uiEvent = ProcessEvent(this, m_lastNodeStack.back());
            m_lastNodeStack.pop_back();
            // last node poped
            if (m_lastNodeStack.empty()) {
                RenderNodes(this, m_rootNode.get());
            }

            return *uiEvent;
        }

#ifndef NDEBUG
        Log::Warn("SDLCore::UI::UIContext::EndFrame: EndFrame called without a fitting BeginFrame!");
#endif 
        return UIEvent{};
    }

    UINode* UIContext::GetRootNode() const {
        return m_rootNode.get();
    }

    void UIContext::SetWindowParams(WindowID id) {
        auto* app = Application::GetInstance();
        if (!app) {
            Log::Error("SDLCore::UI::SetWindowParams: Failed to resolve Window '{}': application instance not available", id);
            return;
        }

        Window* win = app->GetWindow(id);
        if (!win) {
            Log::Error("SDLCore::UI::SetWindowParams: Failed to initialize window parameters for '{}': window not found", id);
            return;
        }

        if (m_windowID != id) {
            // remove old window resize callback
            RemoveWindowCB();

            // sub to window resize callback
            m_windowResizeCBID = win->AddOnWindowResize([this](Window& win) {
                this->m_windowSize = win.GetSize();
                this->UpdateNodeStyles();
            });
        }

        m_windowContentScale = win->GetContentScale();
        m_windowSize = win->GetSize();

        m_windowID = id;
    }

    void UIContext::RemoveWindowCB() {
        if (m_windowResizeCBID.IsInvalid())
            return;

        auto* app = Application::GetInstance();
        if (!app) {
            Log::Error("SDLCore::UI::RemoveWindowCB: Failed to remove Callbacks for Window '{}': application instance not available", m_windowID);
            return;
        }

        if (Window* win = app->GetWindow(m_windowID)) {
            win->RemoveOnWindowResize(m_windowResizeCBID);
        }
    }

    void UIContext::UpdateNodeStyles() {
        std::function<void(UINode*)> updateRecursive;
        updateRecursive = [&](UINode* root) {
            root->ApplyStyle(this);

            for (const std::shared_ptr<UINode>& child : root->GetChildren()) {
                updateRecursive(child.get());
            }
        };

        updateRecursive(m_rootNode.get());
    }

    void UIContext::UpdateInput() {
        uint64_t frameCount = Time::GetFrameCount();
        if (m_updateInputFrame >= Time::GetFrameCount())
            return;
        m_updateInputFrame = frameCount;

        m_mousePos = Input::GetMousePosition();
        m_mouseDelta = Input::GetMouseDelta();

        m_leftMouseDown = Input::MousePressed(MouseButton::LEFT);
        m_leftMouseJustDown = Input::MouseJustPressed(MouseButton::LEFT);
        m_leftMouseJustUp = Input::MouseJustReleased(MouseButton::LEFT);
    }

    UIEvent* UIContext::ProcessEvent(UIContext* ctx, UINode* node) {
        static UIEvent dummy;

        // skip inactive nodes
        if (!node || !node->IsActive())
            return &dummy;

        // do not UI processing if cursor is locked
        auto* app = Application::GetInstance();
        if (app && app->IsCursorLocked())
            return &dummy;

        // skips this element if a child has a event
        const auto& children = node->GetChildren();
        for (const auto& child : children) {
            if (!child)
                continue;

            if (child->GetChildHasEvent() || (!child->IsInteractible() && child->IsMouseInNode())) {
                node->SetChildHasEvent(true);
                node->ResetState();// resets the node to normal
                return node->GetEventPtr();
            }

            if (UIEvent* childEvent = child->GetEventPtr()) {
                if (childEvent->IsHover()) {
                    node->SetChildHasEvent(true);
                    node->ResetState();// resets the node to normal
                    return node->GetEventPtr();
                }
            }
        }
        
        node->SetChildHasEvent(false);
        UIEvent* event = node->GetEventPtr();
        
        ctx->UpdateInput();
        node->ProcessEventInternal(ctx, event);

        return event;
    }

    void UIContext::RenderNodes(UIContext* ctx, UINode* rootNode) {
        std::function<void(UINode*)> renderRecursive; 
        renderRecursive = [&](UINode* root) {
            root->CalculateLayout(ctx);

            if (!root || !root->IsActive()) 
                return;

            root->RenderNode(ctx);
            for (const std::shared_ptr<UINode>& child : root->GetChildren()) {
                renderRecursive(child.get());
            }
        };

        renderRecursive(rootNode);
    }

    bool UIContext::IsIDUnique(uintptr_t idToCheck, UINode* parent) {
        if (!parent)
            return true;

        if (idToCheck == parent->GetID())
            return false;

        auto isUniqueInChilds = [&](UINode* pNode) -> bool {
            for (const auto& child : pNode->GetChildren()) {
                if (child->GetID() == idToCheck)
                    return false;
            }
            return true;
        };

        if (!isUniqueInChilds(parent))
            return false;

        return true;
    }

#ifndef NDEBUG
    void UIContext::LogDuplicateIDIfAny(uintptr_t id, UINode* parent) {
        if (!IsIDUnique(id, parent)) {
            Log::Warn(
                "SDLCore::UI::UIContext::BeginFrame: id '{}' is not unique on this layer, will lead to problems!",
                id
            );
        }
    }
#endif

}