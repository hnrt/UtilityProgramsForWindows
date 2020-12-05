using System;

namespace hnrt.HttpRelay.Controller
{
    internal interface IOuterClient
    {
        void Open();
        void Close();
        void BeginRead();
        int EndRead(IAsyncResult ar);
        void BeginWrite(byte[] buf, int offset, int count);
        void EndWrite(IAsyncResult ar);
        bool CanRead { get; }
        bool CanWrite { get; }

        Action<byte[], int, int> OnReceive { get; set; }
        Action OnClose { get; set; }
    }
}
