using System;
using System.IO;
using System.Net.Sockets;
using System.Threading;
using hnrt.HttpRelay.Model;
using hnrt.HttpRelay.View;

namespace hnrt.HttpRelay.Controller
{
    internal abstract class OuterClientBase : IOuterClient
    {
        public string Name { get; }

        public ConnectionTarget Target { get; }

        protected IView View { get; } = ViewFactory.CreateView();

        public const int STANDBY = 0;
        public const int OPEN_PENDING = 1;
        public const int INPROGRESS = 2;
        public const int CLOSE_PENDING = 3;
        public const int CLOSED = 4;

        protected volatile int state = STANDBY;

        public int State => state;

        protected TcpClient tcp;

        protected const int BUFFER_SIZE = 32768;

        protected byte[] buffer = new byte[BUFFER_SIZE];

        protected SemaphoreSlim writeSemaphore = new SemaphoreSlim(0, 1);

        public Action<byte[], int, int> OnReceive { get; set; }

        public Action OnClose { get; set; }

        public abstract bool CanRead { get; }

        public abstract bool CanWrite { get; }

        public bool Connected => tcp.Client.Connected;

        public OuterClientBase(string name, ConnectionTarget target)
        {
            Name = name;
            Target = target;
        }

        protected bool SetState(int from, int to)
        {
            return Interlocked.CompareExchange(ref state, to, from) == from;
        }

        public virtual void Open()
        {
            if (Target.Address != null)
            {
                View.PutLine("{0}: Connecting to {1}({3}):{2}", Name, Target.Hostname, Target.Port, Target.Address);
            }
            else
            {
                View.PutLine("{0}: Connecting to {1}:{2}", Name, Target.Hostname, Target.Port);
            }
            tcp = new TcpClient(Target.Address ?? Target.Hostname, Target.Port);
            tcp.ReceiveBufferSize = BUFFER_SIZE;
            tcp.SendBufferSize = BUFFER_SIZE;
        }

        public virtual void Close()
        {
            tcp?.Close();
        }

        public abstract void BeginRead();

        public abstract int EndRead(IAsyncResult ar);

        public abstract void BeginWrite(byte[] buf, int offset, int count);

        public abstract void EndWrite(IAsyncResult ar);

        protected void ReadCallback(IAsyncResult ar)
        {
            try
            {
                if (Connected)
                {
                    int bytesRead = EndRead(ar);
                    OnReceive?.Invoke(buffer, 0, bytesRead);
                    if (bytesRead > 0)
                    {
                        BeginRead();
                    }
                    else
                    {
                        View.PutLine("{0}: ReadCallback: Read zero byte.", Name);
                        Close();
                    }
                }
                else
                {
                    View.PutLine("{0}: ReadCallback: Disconnected.", Name);
                }
            }
            catch (IOException ex)
            {
                // Socket was closed or error occurred.
                if (state == INPROGRESS)
                {
                    View.Put(ex);
                }
                Close();
            }
            catch (ObjectDisposedException ex)
            {
                // Stream was closed.
                if (state == INPROGRESS)
                {
                    View.Put(ex);
                }
            }
        }

        protected void WriteCallback(IAsyncResult ar)
        {
            try
            {
                if (Connected)
                {
                    EndWrite(ar);
                }
                else
                {
                    View.PutLine("{0}: WriteCallback: Disconnected.", Name);
                }
            }
            catch (IOException ex)
            {
                // Socket was closed or error occurred.
                if (state == INPROGRESS)
                {
                    View.Put(ex);
                }
                Close();
            }
            catch (ObjectDisposedException ex)
            {
                // Network stream was closed.
                if (state == INPROGRESS)
                {
                    View.Put(ex);
                }
            }
        }
    }
}
