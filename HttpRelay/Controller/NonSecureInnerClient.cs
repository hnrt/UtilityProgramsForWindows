using System;
using System.Net.Sockets;
using hnrt.HttpRelay.Data;

namespace hnrt.HttpRelay.Controller
{
    internal class NonSecureInnerClient : InnerClientBase
    {
        private NetworkStream stream;

        public NonSecureInnerClient(TcpClient source)
            : base("NonSecureInnerClient", source, ConnectionType.Raw)
        {
            stream = source.GetStream();
        }

        public override void Open()
        {
            if (SetState(STANDBY, OPEN_PENDING))
            {
                try
                {
                    base.Open();
                    writeSemaphore.Release();
                    SetState(OPEN_PENDING, INPROGRESS);
                    View.PutLine("{0}: Connection established.", Name);
                    BeginRead();
                }
                catch (Exception ex)
                {
                    if (state == INPROGRESS || state == OPEN_PENDING)
                    {
                        View.Put(ex);
                    }
                    Close();
                }
            }
        }

        public override void Close()
        {
            if (SetState(INPROGRESS, CLOSE_PENDING))
            {
                View.PutLine("{0}: Connection being closed...", Name);
                stream.Flush();
                stream.Close();
                base.Close();
                SetState(CLOSE_PENDING, CLOSED);
                View.PutLine("{0}: Connection closed.", Name);
            }
        }

        public override void BeginRead()
        {
            stream.BeginRead(buffer, 0, BUFFER_SIZE, new AsyncCallback(ReadCallback), null);
        }

        public override int EndRead(IAsyncResult ar)
        {
            return stream.EndRead(ar);
        }

        public override void BeginWrite(byte[] data)
        {
            if (writeSemaphore.Wait(60000))
            {
                stream.BeginWrite(data, 0, data.Length, new AsyncCallback(WriteCallback), null);
            }
            else
            {
                View.PutLine("{0}: BeginWrite timed out.", Name);
            }
        }

        public override void EndWrite(IAsyncResult ar)
        {
            stream.EndWrite(ar);
            stream.Flush();
            writeSemaphore.Release();
        }

        protected override IOuterClient CreateOuterClient(int serialNumber)
        {
            return OuterClientFactory.CreateOuterClient(serialNumber, Destination);
        }

        public override bool CanRead => stream.CanRead;

        public override bool CanWrite => stream.CanWrite;
    }
}
