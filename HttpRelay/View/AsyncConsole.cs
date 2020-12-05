using System;
using System.Collections.Concurrent;
using System.Threading;
using System.Threading.Tasks;

namespace hnrt.HttpRelay.View
{
    internal class AsyncConsole
    {
        public event StartUpEventHandler OnStartUp;
        public event ShutDownEventHandler OnShutDown;
        public event PutLineEventHandler OnPutLine;
        public event ErrorEventHandler OnError;

        private ConcurrentQueue<string> Queue { get; } = new ConcurrentQueue<string>();

        private AutoResetEvent Enqueued { get; } = new AutoResetEvent(false);

        private Task task;

        private const int INITIAL_STATE = 0;
        private const int STARTUP_PENDING = 1;
        private const int RUNNING = 2;
        private const int SHUTDOWN_PENDING = 3;
        private const int SHUTDOWN = 4;
        private const int ERROR = 5;

        private volatile int state = INITIAL_STATE;

        public AsyncConsole()
        {
        }

        public void Open()
        {
            if (Interlocked.CompareExchange(ref state, STARTUP_PENDING, INITIAL_STATE) == INITIAL_STATE)
            {
                task = Task.Run(new Action(MainLoop));
            }
        }

        public void Close()
        {
            if (Interlocked.CompareExchange(ref state, SHUTDOWN_PENDING, RUNNING) == RUNNING)
            {
                Enqueued.Set();
                task?.Wait();
            }
        }

        public void Enqueue(string text)
        {
            if (state == RUNNING)
            {
                Queue.Enqueue(text);
                Enqueued.Set();
            }
            else
            {
                Console.WriteLine("{0}", text);
                OnPutLine?.Invoke(text);
            }
        }

        public void Enqueue(Exception e)
        {
            Console.Error.WriteLine("ERROR: {0}", e.Message);
            while ((e = e.InnerException) != null)
            {
                Console.Error.WriteLine("    {0}", e.Message);
            }
            OnError?.Invoke(e);
        }

        private void MainLoop()
        {
            try
            {
                Interlocked.CompareExchange(ref state, RUNNING, STARTUP_PENDING);
                OnStartUp?.Invoke();
                while (true)
                {
                    if (Queue.TryDequeue(out var text))
                    {
                        Console.WriteLine("{0}", text);
                        OnPutLine?.Invoke(text);
                    }
                    else if (state == RUNNING)
                    {
                        Enqueued.WaitOne();
                    }
                    else
                    {
                        break;
                    }
                }
                Interlocked.Exchange(ref state, SHUTDOWN);
                OnShutDown?.Invoke();
            }
            catch (Exception e)
            {
                Interlocked.Exchange(ref state, ERROR);
                Console.Error.WriteLine("ERROR: {0}", e.Message);
                while ((e = e.InnerException) != null)
                {
                    Console.Error.WriteLine("    {0}", e.Message);
                }
                OnError?.Invoke(e);
            }
        }
    }
}
