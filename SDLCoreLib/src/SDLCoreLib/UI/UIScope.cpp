#include <CoreLib/Log.h>
#include "UI/UIScope.h"

namespace SDLCore::UI {

    class UICTXWrapper {
    public:
        UIContext* ctx = nullptr;

        UICTXWrapper() = default;
        UICTXWrapper(UIContext* c) 
            : ctx(c) {
        }

        bool IsContextNull(const char* action) const {
            if (!ctx) {
                Log::Error("SDLCore::UI::UICTXWrapper::{}, UIContext is null!", action);
                return true;
            }
            return false;
        }

        FrameNode* BeginFrame(uintptr_t key) {
            if (IsContextNull("GetNode: Could not get node"))
                return;
            return ctx->BeginFrame(key);
        }

        void SetWindowParams(WindowID id) {
            if (IsContextNull("SetWindowParams: Could not Set window params"))
                return;
            ctx->SetWindowParams(id);
        }
    };

    static inline UICTXWrapper g_currentUIContext;
    static inline bool g_isCurrentUIContextDestroyed = false;
    
    namespace {
        FrameNode* InternalBeginFrame(uintptr_t key) {
            return g_currentUIContext.BeginFrame(key); // element with key or new element
        }
    }

    UIContext* CreateContext() {
        return UIContext::CreateContext();
    }

    UIContext* CreateContext(WindowID id) {
        UIContext* ctx = UIContext::CreateContext();
        SetContextWindow(ctx, id);
        return ctx;
    }

    void DestroyContext(UIContext* context) {
        if (!context)
            return;

        if (context == g_currentUIContext.ctx) {
            // Defer deletion until next BindContext
            g_isCurrentUIContextDestroyed = true;
            return;
        }

        delete context;
    }

    void BindContext(UIContext* context) {
        if (!context)
            return;

        // If the previous context was marked for deletion, destroy it now
        if (g_isCurrentUIContextDestroyed && g_currentUIContext.ctx) {
            delete g_currentUIContext.ctx;
            g_currentUIContext.ctx = nullptr;
            g_isCurrentUIContextDestroyed = false;
        }

        g_currentUIContext.ctx = context;
    }

    void RenderContext(UIContext* context) {
    }

    void SetContextWindow(UIContext* ctx, WindowID id) {
        if (!ctx) {
            Log::Error("SDLCore::UI::SetContextWindow: Cannot assign window '{}': context is null", id);
            return;
        }

        UICTXWrapper temp(ctx);
        temp.SetWindowParams(id);
    }

    UIContext* GetCurrentContext() {
        return g_currentUIContext.ctx;
    }

    UIEvent EndFrame() {
        // if (g_UIContext.nodeStack.empty())
        //    return UIEvent{};

        // FrameNode* node = static_cast<FrameNode*>(g_UIContext.nodeStack.back());
        // g_UIContext.nodeStack.pop_back();

        //return node ? node->GetEvent() : UIEvent{};
        return UIEvent{};
    }

    FrameScope::FrameScope() {
        // BeginFrame();
        // m_node = g_UIContext.nodeStack.back();
    }

    FrameScope::~FrameScope() {
        if (!m_manualEndDone)
            SafeEnd();
    }

    UIEvent FrameScope::EndGetEvent() {
        UIEvent evt = SafeEnd();

        if (m_nodeWasClosed)
            m_manualEndDone = true;
        else
            Log::Warn("SDLCore::UI::FrameScope::EndGetEvent: Ignored because this scope is not the active UI frame (node is not top of stack)");

        return evt;
    }

    UIEvent FrameScope::SafeEnd() {
        // if (g_UIContext.nodeStack.empty())
        //     return {};
        // 
        // if (g_UIContext.nodeStack.back() != m_node) {
        //     return {};
        // }
        // 
        // m_nodeWasClosed = true;
        return EndFrame();
    }

}