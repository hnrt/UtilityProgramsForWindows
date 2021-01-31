#pragma once


#include "hnrt/KeyboardMouseBridge.h"
#include "hnrt/UiAutomation.h"
#include "Configuration.h"
#include "InputManager.h"
#include "Target.h"


#define GHOST_COMPLETED 0
#define GHOST_PROCESSED 1
#define GHOST_ERROR_INVALID_VIRTUAL_KEY_NAME -1
#define GHOST_ERROR_ACTIVE_ACCESSIBILITY_UNAVAILABLE -2
#define GHOST_ERROR_ACTIVE_ACCESSIBILITY_LOCATE_PART -3
#define GHOST_ERROR_LOCATE_WINDOW -4


namespace hnrt
{
    class Ghost
    {
    public:

        Ghost();
        Ghost(const Ghost&) = delete;
        ~Ghost() = default;
        void operator =(const Ghost&) = delete;
        void Initialize(RefPtr<Configuration> pConfiguration);
        void Uninitialize();
        bool IsProcessing() const;
        bool Start(RefPtr<Target> pTarget);
        int Process();
        void End();

    private:

        RefPtr<Configuration> m_pConfiguration;
        RefPtr<InputManager> m_pInputManager;
        KeyboardMouseBridge m_KeyboardMouseBridge;
        RefPtr<Target> m_pTarget;
        ActionCollection::ConstIter m_ActionIter;
        HWND m_hwndTarget;
        DWORD m_PreviousKeyboardState;
        DWORD m_CurrentKeyboardState;
        RefPtr<UiAutomation> m_pAutomation;
    };
}
