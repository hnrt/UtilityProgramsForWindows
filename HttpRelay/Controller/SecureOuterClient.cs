using System;
using System.Net.Security;
using System.Security.Authentication;
using hnrt.HttpRelay.Model;

namespace hnrt.HttpRelay.Controller
{
    class SecureOuterClient : OuterClientBase
    {
        private SslStream stream;

        public SecureOuterClient(int serialNumber, ConnectionTarget target)
            : base(string.Format("{0}/SecureOuterClient", serialNumber), target)
        {
        }

        public override void Open()
        {
            if (SetState(STANDBY, OPEN_PENDING))
            {
                try
                {
                    base.Open();
                    stream = new SslStream(tcp.GetStream(), false);
                    View.PutLine("{0}: SSL AuthenticateAsClient started.", Name);
                    stream.AuthenticateAsClient(Target.Hostname);
                    View.PutLine("{0}: SSL AuthenticateAsClient ended.", Name);
                    writeSemaphore.Release();
                    SetState(OPEN_PENDING, INPROGRESS);
                    View.PutLine("{0}: Connection established.", Name);
                    BeginRead();
                }
                catch (AuthenticationException ex)
                {
                    View.Put(ex);
                    Close();
                }
                catch (Exception ex)
                {
                    if (state == INPROGRESS)
                    {
                        View.Put(ex);
                    }
                    Close();
                }
            }
        }

        public override void Close()
        {
            if (SetState(INPROGRESS, CLOSE_PENDING) || SetState(OPEN_PENDING, CLOSE_PENDING))
            {
                View.PutLine("{0}: Connection being closed...", Name);
                stream?.Flush();
                stream?.Close();
                base.Close();
                SetState(CLOSE_PENDING, CLOSED);
                View.PutLine("{0}: Connection closed.", Name);
                OnClose?.Invoke();
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

        public override void BeginWrite(byte[] buf, int offset, int count)
        {
            byte[] data = new byte[count];
            Buffer.BlockCopy(buf, offset, data, 0, count);
            if (writeSemaphore.Wait(60000))
            {
                stream.BeginWrite(data, 0, count, new AsyncCallback(WriteCallback), data);
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

        public override bool CanRead => stream.CanRead;

        public override bool CanWrite => stream.CanWrite;
    }
}
