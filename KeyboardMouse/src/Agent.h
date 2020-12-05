#pragma once


#include <msctf.h>
#include <map>
#include "hnrt/ComLibrary.h"
#include "hnrt/SharedMemory.h"
#include "hnrt/RefPtr.h"
#include "AgentBlock.h"


namespace hnrt
{
    class Agent
        : public ComLibrary
        , public ITfCompartmentEventSink
        , public ITfLanguageProfileNotifySink
    {
    public:

        static UINT GetCommandMessage();
        static void MapSharedBlock(SharedMemory<AgentBlock>& sm, DWORD dw);

        Agent();
        Agent(const Agent&) = delete;
        ~Agent();
        void operator =(const Agent&) = delete;
        void Initialize();
        void Uninitialize();

        //IUnknown
        virtual HRESULT STDMETHODCALLTYPE QueryInterface(
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject);
        virtual ULONG STDMETHODCALLTYPE AddRef(void);
        virtual ULONG STDMETHODCALLTYPE Release(void);

        //ITfLanguageProfileNotifySink
        virtual HRESULT STDMETHODCALLTYPE OnLanguageChange(
            /* [in] */ LANGID langid,
            /* [out] */ __RPC__out BOOL* pfAccept);
        virtual HRESULT STDMETHODCALLTYPE OnLanguageChanged(void);

        //ITfCompartmentEventSink
        virtual HRESULT STDMETHODCALLTYPE OnChange(
            /* [in] */ __RPC__in REFGUID rguid);

        static LRESULT CALLBACK CallWndProcHook(
            _In_ int    nCode,
            _In_ WPARAM wParam,
            _In_ LPARAM lParam);

    private:

        bool OnCall(
            _In_ HWND   hwnd,
            _In_ UINT   uMsg,
            _In_ WPARAM wParam,
            _In_ LPARAM lParam);

        HRESULT GetLanguage();
        HRESULT SetLanguage(LANGID requested);
        HRESULT GetOpenClose();
        HRESULT SetOpenClose(LONG value);
        HRESULT GetInputMode();
        HRESULT SetInputMode(LONG value);
        HRESULT GetCompartment(const GUID& rguid, RefPtr<ITfCompartment>& pCompartment);
        HRESULT GetCompartmentLong(RefPtr<ITfCompartment>& pCompartment, LONG& value);
        HRESULT SetCompartmentLong(RefPtr<ITfCompartment>& pCompartment, LONG value);
        void UnadviseAllCompartmentEventSinks();

        ULONG m_ulRefCnt;
        UINT m_WM_AGENT_COMMAND;
        SharedMemory<AgentBlock> m_pAgentBlock;
        bool m_bInitialized;
        bool m_bOnCall;
        RefPtr<ITfInputProcessorProfiles> m_pInputProcessorProfiles;
        DWORD m_dwLanguageProfileNotifySinkCookie;
        RefPtr<ITfThreadMgr> m_pTfThreadMgr;
        TfClientId m_TfClientId;
        RefPtr<ITfCompartmentMgr> m_pTfCompartmentMgr;
        RefPtr<ITfCompartment> m_pTfCompartmentKeyboardOpenClose;
        RefPtr<ITfCompartment> m_pTfCompartmentInputModeConversion;
        std::map<RefPtr<ITfCompartment>, DWORD> m_CompartmentEventSinkCookieMap;
    };
}
