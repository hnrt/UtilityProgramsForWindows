using System;
using System.Text;

namespace hnrt.HttpRelay.View
{
    internal delegate void StartUpEventHandler();
    internal delegate void ShutDownEventHandler();
    internal delegate void PutLineEventHandler(string text);
    internal delegate void ErrorEventHandler(Exception e);

    internal interface IView
    {
        void Open();
        void Close();
        void Put(Exception e);
        void PutLine(string value);
        void PutLine(string format, object arg0);
        void PutLine(string format, object arg0, object arg1);
        void PutLine(string format, object arg0, object arg1, object arg2);
        void PutLine(string format, object arg0, object arg1, object arg2, object arg3);
        void PutLine(string format, object arg0, object arg1, object arg2, object arg3, object arg4);
        void PutIncoming(string from, byte[] data, int offset, int count, Encoding encoding);
        void PutOutgoing(string to, byte[] data, Encoding encoding);
        void PutOutgoing(string to, byte[] data, int offset, int count, Encoding encoding);

        event StartUpEventHandler OnStartUp;
        event ShutDownEventHandler OnShutDown;
        event PutLineEventHandler OnPutLine;
        event ErrorEventHandler OnError;
    }
}
