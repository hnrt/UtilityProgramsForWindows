using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Threading.Tasks;
using hnrt.HttpRelay.View;
using hnrt.HttpRelay.Model;

namespace hnrt.HttpRelay.Controller
{
    internal abstract class ListenerBase : IListener
    {
        public string Name { get; }

        public int Port { get; set; }

        public ConnectionTarget Destination { get; set; }

        public bool Rewrite { get; set; } = true;

        private TcpListener tcpListener;

        private Task task;

        private const int STANDBY = 0;
        private const int START_PENDING = 1;
        private const int RUNNING = 2;
        private const int STOP_PENDING = 3;
        private const int STOPPED = 4;

        private int state = STANDBY;

        protected IView View { get; } = ViewFactory.CreateView();

        public ListenerBase(string name)
        {
            Name = name;
        }

        public void Start()
        {
            if (Interlocked.CompareExchange(ref state, START_PENDING, STANDBY) == STANDBY)
            {
                task = Task.Run(() => MainLoop());
            }
        }

        public void Stop()
        {
            if (Interlocked.CompareExchange(ref state, STOP_PENDING, RUNNING) == RUNNING)
            {
                tcpListener.Stop();
            }
        }

        public void Wait()
        {
            task.Wait();
            task.Dispose();
        }

        private void MainLoop()
        {
            View.PutLine("{0} MainLoop started. port={1}", Name, Port);
            try
            {
                InitMainLoop();
                tcpListener = new TcpListener(IPAddress.Any, Port);
                tcpListener.Start();
                Interlocked.CompareExchange(ref state, RUNNING, START_PENDING);
                while (state == RUNNING)
                {
                    TcpClient client = tcpListener.AcceptTcpClient();
                    StartSession(client);
                }
            }
            catch (Exception ex)
            {
                if (state == START_PENDING || state == RUNNING)
                {
                    View.Put(ex);
                }
            }
            Interlocked.Exchange(ref state, STOPPED);
            View.PutLine("{0} MainLoop finished.", Name);
        }

        protected abstract void InitMainLoop();

        protected abstract void StartSession(TcpClient client);
    }
}
