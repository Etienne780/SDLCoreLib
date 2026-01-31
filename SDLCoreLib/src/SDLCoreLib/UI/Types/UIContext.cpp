#include <functional>
#include <CoreLib/Profiler.h>

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

    size_t UIContext::GetNodeCount() const {
        return m_nodeCount;
    }

    void UIContext::SetNodeState(UINode* node, UIState state) {
        if(node)
            node->SetState(state);
    }

    FrameNode* UIContext::BeginFrame(uintptr_t id) {
        m_currentNodeCount++;

        if (m_lastNodeStack.empty() && m_rootNode) {
            return HandleExistingRootNode(id);
        }

        if (!m_rootNode) {
            return CreateRootNode(id);
        }

        if (!m_nodeCreationStack.empty()) {
            return CreateChildNodeInCreationMode(id);
        }

        return ReuseOrCreateChildNode(id);
    }

    FrameNode* UIContext::HandleExistingRootNode(uintptr_t id) {
        if (m_rootNode->GetID() != id) {
            m_rootNode = nullptr;
            return nullptr;
        }

        m_lastNodeStack.push_back(reinterpret_cast<UINode*>(m_rootNode.get()));
        m_lastChildPosition.push_back(0);
        m_rootNode->SetNodeActive();

        if (m_rootNode->IsStatePropagationEnabled()) {
            ResolveNodeState(this, m_rootNode.get());
        }

        CalculateClippingMask(m_lastNodeStack.back());
        return m_rootNode.get();
    }

    FrameNode* UIContext::CreateRootNode(uintptr_t id) {
        m_rootNode = std::make_shared<FrameNode>(-1, id);
        FrameNode* frame = m_rootNode.get();

        m_nodeCreationStack.push_back(reinterpret_cast<UINode*>(frame));
        m_lastNodeStack.push_back(reinterpret_cast<UINode*>(frame));
        m_lastChildPosition.push_back(0);
        CalculateClippingMask(m_lastNodeStack.back());

        return frame;
    }

    FrameNode* UIContext::CreateChildNodeInCreationMode(uintptr_t id) {
        UINode* parentNode = m_nodeCreationStack.back();
        if (!parentNode) {
            return nullptr;
        }

        int currentChildPos = (!m_lastChildPosition.empty()) ?
            static_cast<int>(m_lastChildPosition.back()) : 0;

#ifndef NDEBUG
        LogDuplicateIDIfAny(id, parentNode);
#endif

        FrameNode* frame = parentNode->AddChild<FrameNode>(currentChildPos, id);

        m_nodeCreationStack.push_back(reinterpret_cast<UINode*>(frame));
        m_lastNodeStack.push_back(reinterpret_cast<UINode*>(frame));
        m_lastChildPosition.push_back(0);
        CalculateClippingMask(m_lastNodeStack.back());

        return frame;
    }

    FrameNode* UIContext::ReuseOrCreateChildNode(uintptr_t id) {
        UINode* parentNode = m_lastNodeStack.back();
        uint16_t pos = m_lastChildPosition.empty() ? 0 : m_lastChildPosition.back();
        UINode* currentNode = nullptr;

        if (parentNode->ContainsChildAtPos(pos, id, currentNode)) {
            return ReuseExistingNode(currentNode);
        }

        return CreateNewChildNode(parentNode, pos, id);
    }

    FrameNode* UIContext::ReuseExistingNode(UINode* currentNode) {
        m_lastNodeStack.push_back(currentNode);
        m_lastChildPosition.push_back(0);
        currentNode->SetNodeActive();

        if (currentNode->IsStatePropagationEnabled()) {
            ResolveNodeState(this, currentNode);
        }

        if (currentNode->IsRelative()) {
            m_relativeStack.push_back(currentNode);
        }

        CalculateClippingMask(currentNode);
        return reinterpret_cast<FrameNode*>(currentNode);
    }

    FrameNode* UIContext::CreateNewChildNode(UINode* parentNode, uint16_t pos, uintptr_t id) {
        parentNode->RemoveChildrenFromIndex(pos);

#ifndef NDEBUG
        LogDuplicateIDIfAny(id, parentNode);
#endif

        FrameNode* frame = parentNode->AddChild<FrameNode>(static_cast<int>(pos), id);

        m_lastNodeStack.push_back(reinterpret_cast<UINode*>(frame));
        m_nodeCreationStack.push_back(reinterpret_cast<UINode*>(frame));
        m_lastChildPosition.push_back(0);
        CalculateClippingMask(m_lastNodeStack.back());

        return frame;
    }

    UIEvent UIContext::EndFrame() {
        if (!m_relativeStack.empty()) {
            if (m_relativeStack.back() == m_lastNodeStack.back()) {
                m_relativeStack.pop_back();
            }
        }

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
                m_nodeCount = m_currentNodeCount;
                m_currentNodeCount = 0;

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

    const UINode* UIContext::GetLastRelativeNode() const {
        return m_relativeStack.empty() ? m_rootNode.get() : m_relativeStack.back();
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
                this->UpdateNodeStylesWindowResize();
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

    void UIContext::UpdateNodeStylesWindowResize() {
        ForEachNode(m_rootNode.get(), [&](UINode* root) {
            root->ApplyStyle(this);
        });
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

        auto* app = Application::GetInstance();
        m_cursorLocked = (app) ? app->IsCursorLocked() : false;
    }

    void UIContext::ResolveNodeState(UIContext* ctx, UINode* node) {
        if (!ctx || !node)
            return;

        UIEvent* event = node->GetEventPtr();
        UIState prev = node->GetState();

        ctx->UpdateInput();
        node->ProcessEventInternal(ctx, event);

        node->SetResolvedState(node->GetState());
        node->SetState(prev);
    }

    UIEvent* UIContext::ProcessEvent(UIContext* ctx, UINode* node) {
        static UIEvent dummy;
        ctx->UpdateInput();

        // skip inactive nodes
        if (!node || !node->IsActive())
            return &dummy;

        // reset child-event state for this traversal
        node->SetChildHasEvent(false);

        // do not UI processing if cursor is locked
        if (m_cursorLocked)
            return &dummy;

        auto parent = node->GetParent();
        if (parent && parent->GetResolvedState() != UIState::NORMAL) {
            ctx->SetNodeState(node, parent->GetResolvedState());
            return node->GetEventPtr();
        }

        // skip this element if a non-transparent child has an event
        const auto& children = node->GetChildren();
        for (const auto& child : children) {
            if (!child || child->IsDisabled())
                continue;

            // child subtree already consumed an event (only if not hit-test transparent)
            if (child->GetChildHasEvent() && !child->HasHitTestTransparent()) {
                node->SetChildHasEvent(true);
                node->ResetState(); // reset this node to normal
                return node->GetEventPtr();
            }

            // mouse is over child and child blocks hit testing
            if (child->IsMouseInNode() && !child->HasHitTestTransparent()) {
                node->SetChildHasEvent(true);
                node->ResetState(); // reset this node to normal
                return node->GetEventPtr();
            }

            // explicit child event (hover etc.) that blocks parents
            UIEvent* childEvent = child->GetEventPtr();
            if (childEvent && childEvent->IsHover() && !child->HasHitTestTransparent()) {
                node->SetChildHasEvent(true);
                node->ResetState(); // reset this node to normal
                return node->GetEventPtr();
            }
        }

        // process this node
        UIEvent* event = node->GetEventPtr();
        node->ProcessEventInternal(ctx, event);
        node->SetResolvedState(UIState::NORMAL);

        return event;
    }

    void UIContext::RenderNodes(UIContext* ctx, UINode* rootNode) {
        Rect r = SDLCore::Render::GetClipRect();
        ForEachNode(rootNode, [&](UINode* root) {
            root->Update(ctx, Time::GetDeltaTimeMSF());

            if (!root)
                return;

            Vector4 clipRect = root->GetClippingRect();
            if (!root->IsActive() || clipRect.z == 0 || clipRect.w == 0)
                return;

            SDLCore::Render::SetClipRect(clipRect);
            root->RenderNode(ctx);
        });
        SDLCore::Render::SetClipRect(r);
    }

    void UIContext::CalculateClippingMask(UINode* node) {
        if (!node)
            return;

        UINode* parent = node->GetParent();
        Vector4 baseClip;

        if (!parent) {
            // Root node
            bool clipX = node->IsHorizontalOverflowHidden();
            bool clipY = node->IsVerticalOverflowHidden();

            if (clipX || clipY) {
                // Root clips to its own rect
                baseClip = CreateClipRect(node);
            }
            else {
                // Root allows overflow -> window bounds
                baseClip = GetWindowClip();
            }
        }
        else {
            // Non-root nodes always inherit parent's effective clip
            baseClip = parent->GetClippingRect();
        }

        Vector4 selfClip = CreateClipRect(node);

        bool clipX = node->IsHorizontalOverflowHidden();
        bool clipY = node->IsVerticalOverflowHidden();

        Vector4 effectiveClip = IntersectAxis(
            baseClip,
            selfClip,
            clipX,
            clipY
        );

        node->SetClippingRect(effectiveClip);
    }

    Vector4 UIContext::GetWindowClip() const {
        return Vector4{
            0,
            0,
            m_windowSize.x,
            m_windowSize.y
        };
    }

    Vector4 UIContext::CreateClipRect(UINode* node) const {
        if (!node)
            return Vector4::zero;

        Vector2 pos = node->GetVisiblePosition();
        Vector2 size = node->GetVisibleSize();

        return Vector4{ pos, size };
    }

    Vector4 UIContext::IntersectAxis(
        const Vector4& parent,
        const Vector4& self,
        bool clipX,
        bool clipY) const
    {
        Vector4 result = parent;

        if (clipX)
        {
            float x1 = std::max(parent.x, self.x);
            float x2 = std::min(parent.x + parent.z, self.x + self.z);

            if (x2 <= x1)
                return Vector4{ 0, 0, 0, 0 };

            result.x = x1;
            result.z = x2 - x1;
        }

        if (clipY)
        {
            float y1 = std::max(parent.y, self.y);
            float y2 = std::min(parent.y + parent.w, self.y + self.w);

            if (y2 <= y1)
                return Vector4{ 0, 0, 0, 0 };

            result.y = y1;
            result.w = y2 - y1;
        }

        return result;
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