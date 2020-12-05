using System;
using System.IO;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using hnrt.HttpRelay.View;

namespace hnrt.HttpRelay.Controller
{
    internal class CommandServer
    {
        public string Name { get; }
        public Action OnShutdown { get; set; }

        private const int BUFFER_SIZE = 4096;

        private TcpClient client;

        private NetworkStream stream;

        private byte[] buffer = new byte[BUFFER_SIZE];

        private int length = 0;

        private volatile int enabled = 0;

        private CommandProcessor CommandProcessor { get; }

        private IView View { get; } = ViewFactory.CreateView();

        public CommandServer(TcpClient client)
        {
            Name = "CommandServer";
            CommandProcessor = new CommandProcessor()
            {
                OnShutdown = Shutdown,
                OnForceStatusCode = ForceStatusCode,
                OnForceReasonPhrase = ForceReasonPhrase,
                OnDelay = ForceDelay,
                OnDelayUri = ForceDelayUri
            };
            this.client = client;
        }

        public void Open()
        {
            if (Interlocked.CompareExchange(ref enabled, 1, 0) == 0)
            {
                stream = client.GetStream();
                stream.BeginRead(buffer, length, buffer.Length - length, new AsyncCallback(ReadCallback), null);
            }
        }

        public void Close()
        {
            if (Interlocked.CompareExchange(ref enabled, 0, 1) == 1)
            {
                stream?.Close();
                client.Close();
            }
        }

        private void ReadCallback(IAsyncResult ar)
        {
            try
            {
                int bytesRead = stream.EndRead(ar);
                if (bytesRead > 0)
                {
                    length += bytesRead;
                    if (length + 256 > buffer.Length)
                    {
                        Array.Resize<byte>(ref buffer, buffer.Length + BUFFER_SIZE);
                    }
                    stream.BeginRead(buffer, length, buffer.Length - length, new AsyncCallback(ReadCallback), null);
                }
                else
                {
                    Dispatch(Encoding.UTF8.GetString(buffer, 0, length));
                    Close();
                }
            }
            catch (IOException ex)
            {
                // Socket was closed or error occurred.
                View.Put(ex);
                Close();
            }
            catch (ObjectDisposedException ex)
            {
                // Network stream was closed.
                View.Put(ex);
                Close();
            }
            catch (Exception ex)
            {
                View.Put(ex);
                Close();
            }
        }

        private void Dispatch(string s)
        {
            var rsp = CommandProcessor.Dispatch(s);
            var data = Encoding.UTF8.GetBytes(rsp);
            stream.Write(data, 0, data.Length);
            client.Client.Shutdown(SocketShutdown.Send);
        }

        private void Shutdown()
        {
            View.PutLine("Command {0} received.", CommandProcessor.SHUTDOWN);
            OnShutdown?.Invoke();
        }

        private void ForceStatusCode(int value)
        {
            View.PutLine("Command {0}{1} received.", CommandProcessor.STATUSCODE, value);
            //TODO
        }

        private void ForceReasonPhrase(string value)
        {
            View.PutLine("Command {0}{1} received.", CommandProcessor.REASONPHRASE, value);
            //TODO
        }

        private void ForceDelay(int milliseconds)
        {
            View.PutLine("Command {0}{1} received.", CommandProcessor.DELAY, milliseconds);
            //TODO
        }

        private void ForceDelayUri(string uri)
        {
            View.PutLine("Command {0}{1} received.", CommandProcessor.DELAYURI, uri);
            //TODO
        }
    }
}
