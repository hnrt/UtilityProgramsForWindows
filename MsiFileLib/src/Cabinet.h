#pragma once


#include <Windows.h>
#include <fdi.h>
#include "hnrt/ICabinet.h"
#include "hnrt/String.h"


namespace hnrt
{
    enum class CabinetExtractionState
    {
        Idle,
        Pending,
        Started,
        Ended,
        OpenFailure
    };

    struct ICabinetStream
    {
        virtual bool DIAMONDAPI Open(LPSTR pszFile, int oflag, int pmode) = 0;
        virtual UINT DIAMONDAPI Read(void* pv, UINT cb) = 0;
        virtual UINT DIAMONDAPI Write(void* pv, UINT cb) = 0;
        virtual int DIAMONDAPI Close() = 0;
        virtual long DIAMONDAPI Seek(long dist, int seektype) = 0;
    };

    class CabinetFileStream
        : public ICabinetStream
    {
    public:

        CabinetFileStream(HANDLE = INVALID_HANDLE_VALUE);
        CabinetFileStream(const CabinetFileStream&) = delete;
        virtual ~CabinetFileStream();
        void operator =(const CabinetFileStream&) = delete;
        virtual bool DIAMONDAPI Open(LPSTR pszFile, int oflag, int pmode);
        virtual UINT DIAMONDAPI Read(void* pv, UINT cb);
        virtual UINT DIAMONDAPI Write(void* pv, UINT cb);
        virtual int DIAMONDAPI Close();
        virtual long DIAMONDAPI Seek(long dist, int seektype);

    private:

        HANDLE m_h;
    };

    class CabinetMemoryStream
        : public ICabinetStream
    {
    public:

        CabinetMemoryStream(const void*, size_t);
        CabinetMemoryStream(const CabinetMemoryStream&) = delete;
        virtual ~CabinetMemoryStream();
        void operator =(const CabinetMemoryStream&) = delete;
        virtual bool DIAMONDAPI Open(LPSTR pszFile, int oflag, int pmode);
        virtual UINT DIAMONDAPI Read(void* pv, UINT cb);
        virtual UINT DIAMONDAPI Write(void* pv, UINT cb);
        virtual int DIAMONDAPI Close();
        virtual long DIAMONDAPI Seek(long dist, int seektype);

    private:

        const BYTE* m_pStart;
        const BYTE* m_pEnd;
        const BYTE* m_pCurrent;
    };

    class Cabinet
        : public ICabinet
    {
    public:

        Cabinet();
        Cabinet(const Cabinet&) = delete;
        ~Cabinet();
        void operator =(const Cabinet&) = delete;
        virtual long AddRef();
        virtual long Release();
        virtual PCWSTR get_Path() const;
        virtual void set_Path(PCWSTR pszPath);
        virtual void SetData(const void* pData, size_t cbSize);
        virtual const BYTE* get_Ptr() const;
        virtual size_t get_Len() const;
        virtual void Extract(ICabinetExtractCallbacks& callbacks);

    private:

        static void* DIAMONDAPI Allocate(ULONG cb);
        static void DIAMONDAPI Free(void* pv);
        static INT_PTR DIAMONDAPI Open(LPSTR pszFile, int oflag, int pmode);
        static UINT DIAMONDAPI Read(INT_PTR hf, void* pv, UINT cb);
        static UINT DIAMONDAPI Write(INT_PTR hf, void* pv, UINT cb);
        static int DIAMONDAPI Close(INT_PTR hf);
        static long DIAMONDAPI Seek(INT_PTR hf, long dist, int seektype);
        static INT_PTR DIAMONDAPI Notify(FDINOTIFICATIONTYPE fdint, PFDINOTIFICATION pfdin);

        INT_PTR OnOpen(PCSTR);
        INT_PTR OnClose(PCSTR, INT_PTR);

        long m_refs;
        HFDI m_hfdi;
        ERF m_erf;
        const void* m_pData;
        size_t m_cbSize;
        String m_InputPath;
        ICabinetExtractCallbacks* m_pCallbacks;
        CabinetExtractionState m_State;
        String m_OutputPath;
    };
}
