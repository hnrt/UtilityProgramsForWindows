using System;
using System.IO;
using System.Net.Sockets;
using System.Text;
using hnrt.HttpRelay.View;

namespace hnrt.HttpRelay.Controller
{
    internal class CommandClient
    {
        public string Name { get; }

        public string Hostname { get; set; }

        public int Port { get; set; }

        private IView View { get; } = ViewFactory.CreateView();

        public CommandClient()
        {
            Name = "CommandClient";
            Hostname = CommandProcessor.DEFAULT_HOST;
            Port = CommandProcessor.DEFAULT_PORT;
        }

        public void Send(string request)
        {
            TcpClient tcp = null;
            NetworkStream stream = null;
            try
            {
                tcp = new TcpClient(Hostname, Port);
                stream = tcp.GetStream();
                var data = Encoding.UTF8.GetBytes(request);
                stream.Write(data, 0, data.Length);
                tcp.Client.Shutdown(SocketShutdown.Send);
                const int CHUNK_SIZE = 1024;
                var buffer = new byte[CHUNK_SIZE];
                int length = 0;
                while (true)
                {
                    int bytesRead = stream.Read(buffer, length, buffer.Length - length);
                    if (bytesRead > 0)
                    {
                        length += bytesRead;
                        if (length + 256 > buffer.Length)
                        {
                            Array.Resize<byte>(ref buffer, buffer.Length + CHUNK_SIZE);
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                var response = Encoding.UTF8.GetString(buffer, 0, length);
                View.PutLine("{0}", response);
            }
            catch (IOException ex)
            {
                // Socket was closed or error occurred.
                View.Put(ex);
            }
            catch (ObjectDisposedException ex)
            {
                // Stream was closed.
                View.Put(ex);
            }
            catch (Exception ex)
            {
                View.Put(ex);
            }
            stream?.Close();
            tcp?.Close();
        }
    }
}
