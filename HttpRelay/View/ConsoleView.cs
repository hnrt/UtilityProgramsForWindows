using System;
using System.Text;

namespace hnrt.HttpRelay.View
{
    internal class ConsoleView : IView
    {
        private static readonly AsyncConsole console = new AsyncConsole();

        private static readonly char[] SEP_LF = { '\n' };

        private static readonly string DTFMT = "yyyy-MM-ddTHH:mm:ss.fff";

        private object incoming = new object();

        private object outgoing = new object();

        private StringBuilder ibuf = new StringBuilder();

        private StringBuilder obuf = new StringBuilder();

        public ConsoleView()
        {
        }

        public void Open()
        {
            console.Open();
        }

        public void Close()
        {
            console.Close();
        }

        public void Put(Exception e)
        {
            console.Enqueue(e);
        }

        public void PutLine(string value)
        {
            console.Enqueue(value);
        }

        public void PutLine(string format, object arg0)
        {
            console.Enqueue(string.Format(format, arg0));
        }

        public void PutLine(string format, object arg0, object arg1)
        {
            console.Enqueue(string.Format(format, arg0, arg1));
        }

        public void PutLine(string format, object arg0, object arg1, object arg2)
        {
            console.Enqueue(string.Format(format, arg0, arg1, arg2));
        }

        public void PutLine(string format, object arg0, object arg1, object arg2, object arg3)
        {
            console.Enqueue(string.Format(format, arg0, arg1, arg2, arg3));
        }

        public void PutLine(string format, object arg0, object arg1, object arg2, object arg3, object arg4)
        {
            console.Enqueue(string.Format(format, arg0, arg1, arg2, arg3, arg4));
        }

        public void PutIncoming(string from, byte[] data, int offset, int count, Encoding encoding)
        {
            lock(incoming)
            {
                ibuf.Length = 0;
                ibuf.AppendFormat("{0} INCOMING {1} bytes from {2}", DateTime.Now.ToString(DTFMT), count, from);
                var data1 = encoding.GetString(data, offset, count);
                var data2 = data1.Split(SEP_LF);
                for (int i = 0; i < data2.Length; i++)
                {
                    var s1 = data2[i];
                    var s2 = s1.Replace("\\", "\\\\").Replace("\r", "\\r");
                    if (i < data2.Length - 1)
                    {
                        ibuf.AppendLine();
                        ibuf.AppendFormat("{0}>> {1}\\n", from, s2);
                    }
                    else if (s2.Length > 0)
                    {
                        ibuf.AppendLine();
                        ibuf.AppendFormat("{0}>> {1}", from, s2);
                    }
                }
                console.Enqueue(ibuf.ToString());
            }
        }

        public void PutOutgoing(string to, byte[] data, Encoding encoding)
        {
            PutOutgoing(to, data, 0, data.Length, encoding);
        }

        public void PutOutgoing(string to, byte[] data, int offset, int count, Encoding encoding)
        {
            lock(outgoing)
            {
                obuf.Length = 0;
                obuf.AppendFormat("{0} OUTGOING {1} bytes to {2}", DateTime.Now.ToString(DTFMT), count, to);
                var data1 = (encoding ?? Encoding.UTF8).GetString(data, offset, count);
                var data2 = data1.Split(SEP_LF);
                for (int i = 0; i < data2.Length; i++)
                {
                    var s1 = data2[i];
                    var s2 = s1.Replace("\\", "\\\\").Replace("\r", "\\r");
                    if (i < data2.Length - 1)
                    {
                        obuf.AppendLine();
                        obuf.AppendFormat("{0}<< {1}\\n", to, s2);
                    }
                    else if (s2.Length > 0)
                    {
                        obuf.AppendLine();
                        obuf.AppendFormat("{0}<< {1}", to, s2);
                    }
                }
                console.Enqueue(obuf.ToString());
            }
        }

        public event StartUpEventHandler OnStartUp
        {
            add
            {
                console.OnStartUp += value;
            }
            remove
            {
                console.OnStartUp -= value;
            }
        }

        public event ShutDownEventHandler OnShutDown
        {
            add
            {
                console.OnShutDown += value;
            }
            remove
            {
                console.OnShutDown -= value;
            }
        }

        public event PutLineEventHandler OnPutLine
        {
            add
            {
                console.OnPutLine += value;
            }
            remove
            {
                console.OnPutLine -= value;
            }
        }

        public event ErrorEventHandler OnError
        {
            add
            {
                console.OnError += value;
            }
            remove
            {
                console.OnError -= value;
            }
        }
    }
}
