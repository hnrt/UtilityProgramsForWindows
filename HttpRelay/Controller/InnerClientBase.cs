using System;
using System.Collections.Generic;
using System.IO;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using hnrt.HttpRelay.Data;
using hnrt.HttpRelay.Model;
using hnrt.HttpRelay.Net.Http;
using hnrt.HttpRelay.View;

namespace hnrt.HttpRelay.Controller
{
    internal abstract class InnerClientBase
    {
        protected static int serialNumber = 0;

        private int SerialNumber { get; }

        public string Name { get; }

        public string SrcName { get; }

        public string DstName { get; }

        public ConnectionTarget Destination { get; set; }

        public bool Rewrite { get; set; }

        public static HttpStatusLine StatusLine { get; set; }

        public static ResponseDelay ResponseDelay { get; set; }

        protected IView View { get; } = ViewFactory.CreateView();

        protected const int STANDBY = 0;
        protected const int OPEN_PENDING = 1;
        protected const int INPROGRESS = 2;
        protected const int CLOSE_PENDING = 3;
        protected const int CLOSED = 4;

        protected volatile int state = STANDBY;

        private TcpClient source;

        private ConnectionType sourceConnectionType;

        protected const int BUFFER_SIZE = 32768;

        protected byte[] buffer = new byte[BUFFER_SIZE];

        protected SemaphoreSlim writeSemaphore = new SemaphoreSlim(0, 1);

        private HttpMachine protocolMachine;

        private IOuterClient outerClient;

        private HttpHeaderHost hostHeader;

        protected InnerClientBase(string name, TcpClient source, ConnectionType sourceConnectionType)
        {
            SerialNumber = Interlocked.Increment(ref serialNumber);
            Name = string.Format("{0}/{1}", SerialNumber, name);
            SrcName = string.Format("{0}/SRC", SerialNumber);
            DstName = string.Format("{0}/DST", SerialNumber);
            this.source = source;
            this.source.ReceiveBufferSize = BUFFER_SIZE;
            this.source.SendBufferSize = BUFFER_SIZE;
            this.sourceConnectionType = sourceConnectionType;
        }

        protected bool SetState(int from, int to)
        {
            return Interlocked.CompareExchange(ref state, to, from) == from;
        }

        public virtual void Open()
        {
            if (Destination.Type == ConnectionType.Undefined)
            {
                throw new ArgumentException("Destination.Type is not yet set.");
            }
            if (string.IsNullOrEmpty(Destination.Hostname))
            {
                throw new ArgumentException("Destination.Hostname is null or empty.");
            }
            if (Destination.Port == 0)
            {
                throw new ArgumentException("Destination.Port is not yet set.");
            }

            protocolMachine = new HttpMachine()
            {
                OnWebsocketSend = OnWebsocketSend,
                OnWebsocketReceive = OnWebsocketReceive,
                OnConnectionOfOtherThanUpgrade = OnConnectionOfOtherThanUpgrade,
                OnUpgradeOfOtherThanWebsocket = OnUpgradeOfOtherThanWebsocket,
                OnError = OnError,
                OnCompletion = OnCompletion
            };
            protocolMachine.RequestMessage.OnReceiveHeader = OnRequestHeader;
            protocolMachine.RequestMessage.OnReceiveBody = OnRequestBody;
            protocolMachine.RequestMessage.OnReceiveChunk = OnRequestChunk;
            protocolMachine.RequestMessage.OnReceiveChunkTrailer = OnRequestChunkTrailer;
            protocolMachine.ResponseMessage.OnReceiveHeader = OnResponseHeader;
            protocolMachine.ResponseMessage.OnReceiveBody = OnResponseBody;
            protocolMachine.ResponseMessage.OnReceiveChunk = OnResponseChunk;
            protocolMachine.ResponseMessage.OnReceiveChunkTrailer = OnResponseChunkTrailer;
            protocolMachine.Start();

            hostHeader = new HttpHeaderHost(Destination.Hostname, Destination.Port, Destination.Type == ConnectionType.Encrypted);
        }

        public virtual void Close()
        {
            outerClient?.Close();
            source.Close();
            protocolMachine?.Stop();
            protocolMachine?.Dispose();
        }

        public abstract void BeginRead();

        public abstract int EndRead(IAsyncResult ar);

        public abstract void BeginWrite(byte[] data);

        public abstract void EndWrite(IAsyncResult ar);

        public abstract bool CanRead { get; }

        public abstract bool CanWrite { get; }

        public bool Connected => source.Client.Connected;

        protected void ReadCallback(IAsyncResult ar)
        {
            try
            {
                if (Connected)
                {
                    int bytesRead = EndRead(ar);
                    View.PutIncoming(SrcName, buffer, 0, bytesRead, protocolMachine.RequestMessage.Encoding);
                    protocolMachine.SendFeed(buffer, 0, bytesRead);
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
                // Network stream was closed.
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

        private void OnRequestHeader()
        {
            if (protocolMachine.RequestMessage.IsChunked)
            {
                byte[] hPart;
                if (Rewrite)
                {
                    var headers = new List<HttpHeader>();
                    headers.Add(hostHeader);
                    hPart = protocolMachine.RequestMessage.GetHeader(headers.ToArray());
                }
                else
                {
                    hPart = protocolMachine.RequestMessage.Header;
                }
                SendToDestination(hPart, protocolMachine.RequestMessage.Encoding);
            }
            else
            {
                // content-length header may be changed -- cannot write header at this moment
            }
        }

        private void OnRequestChunk(HttpChunk chunk)
        {
            var data = chunk.GetBytes(protocolMachine.RequestMessage.Encoding);
            SendToDestination(data, protocolMachine.RequestMessage.Encoding);
        }

        private void OnRequestChunkTrailer(HttpChunkTrailer trailer)
        {
            var data = trailer.GetBytes(protocolMachine.RequestMessage.Encoding);
            SendToDestination(data, protocolMachine.RequestMessage.Encoding);
        }

        private void OnRequestBody()
        {
            if (protocolMachine.RequestMessage.MessageBody != null)
            {
                byte[] hPart, bPart;
                if (Rewrite)
                {
                    ReplaceThisHostWithDstHost();
                    bPart = protocolMachine.RequestMessage.MessageBody.GetBytes(protocolMachine.RequestMessage.Encoding);
                    var headers = new List<HttpHeader>();
                    if (protocolMachine.RequestMessage.HeaderList.ContentLength >= 0)
                    {
                        headers.Add(new HttpHeaderContentLength(bPart.Length));
                    }
                    headers.Add(hostHeader);
                    hPart = protocolMachine.RequestMessage.GetHeader(headers.ToArray());
                }
                else
                {
                    hPart = protocolMachine.RequestMessage.Header;
                    bPart = protocolMachine.RequestMessage.MessageBody.GetBytes(protocolMachine.RequestMessage.Encoding);
                }
                SendToDestination(hPart, protocolMachine.RequestMessage.Encoding);
                SendToDestination(bPart, protocolMachine.RequestMessage.Encoding);
            }
            else if (Rewrite)
            {
                var hPart = protocolMachine.RequestMessage.GetHeader(hostHeader);
                SendToDestination(hPart, protocolMachine.RequestMessage.Encoding);
            }
            else
            {
                var hPart = protocolMachine.RequestMessage.Header;
                SendToDestination(hPart, protocolMachine.RequestMessage.Encoding);
            }
        }

        private void OnOuterClientReceive(byte[] buf, int offset, int count)
        {
            View.PutIncoming(DstName, buf, offset, count, protocolMachine.ResponseMessage.Encoding);
            protocolMachine.ReceiveFeed(buf, offset, count);
        }

        private void OnOuterClientClose()
        {
        }

        private void OnResponseHeader()
        {
            if (protocolMachine.ResponseMessage.StatusCode != 100)
            {
                if (ResponseDelay != null && ResponseDelay.IsValid)
                {
                    var uri = ResponseDelay.Uri;
                    if (uri == null || uri == protocolMachine.RequestMessage.RequestUri)
                    {
                        View.PutLine("{0}: Delay of {1} milliseconds started.", Name, ResponseDelay.Duration);
                        Thread.Sleep(ResponseDelay.Duration);
                        View.PutLine("{0}: Delay of {1} milliseconds ended.", Name, ResponseDelay.Duration);
                    }
                }
            }
            if (StatusLine != null && StatusLine.StatusCode > 0)
            {
                var rsp = new HttpResponseMessage();
                rsp.StatusLine.HttpVersion.Set(protocolMachine.ResponseMessage.StatusLine.HttpVersion);
                rsp.StatusLine.StatusCode = StatusLine.StatusCode;
                if (StatusLine.ReasonPhrase != null)
                {
                    rsp.StatusLine.ReasonPhrase = StatusLine.ReasonPhrase;
                }
                SendToSource(rsp.GetBytes(), rsp.Encoding);
            }
            else if (protocolMachine.ResponseMessage.IsChunked)
            {
                SendToSource(protocolMachine.ResponseMessage.Header, protocolMachine.ResponseMessage.Encoding);
            }
            else
            {
                // content-length header may be changed -- cannot write header at this moment
            }
        }

        private void OnResponseChunk(HttpChunk chunk)
        {
            var data = chunk.GetBytes(protocolMachine.ResponseMessage.Encoding);
            SendToSource(data, protocolMachine.ResponseMessage.Encoding);
        }

        private void OnResponseChunkTrailer(HttpChunkTrailer trailer)
        {
            var data = trailer.GetBytes(protocolMachine.ResponseMessage.Encoding);
            SendToSource(data, protocolMachine.ResponseMessage.Encoding);
            if (!CanRead || !CanWrite)
            {
                View.PutLine("{0}: OnResponseBody: CanRead={1} CanWrite={2}", Name, CanRead, CanWrite);
                Close();
            }
        }

        private void OnResponseBody()
        {
            if (StatusLine != null && StatusLine.StatusCode > 0)
            {
                // no message body is assumed
            }
            else
            {
                byte[] hPart, bPart;
                if (protocolMachine.ResponseMessage.MessageBody != null)
                {
                    if (Rewrite)
                    {
                        ReplaceDstHostWithThisHost();
                        bPart = protocolMachine.ResponseMessage.MessageBody.GetBytes(protocolMachine.ResponseMessage.Encoding);
                        var headers = new List<HttpHeader>();
                        if (protocolMachine.ResponseMessage.HeaderList.ContentLength >= 0)
                        {
                            headers.Add(new HttpHeaderContentLength(bPart.Length));
                        }
                        hPart = protocolMachine.ResponseMessage.GetHeader(headers.ToArray());
                    }
                    else
                    {
                        hPart = protocolMachine.ResponseMessage.Header;
                        bPart = protocolMachine.ResponseMessage.MessageBody.GetBytes(protocolMachine.ResponseMessage.Encoding);
                    }
                }
                else
                {
                    hPart = protocolMachine.ResponseMessage.Header;
                    bPart = null;
                }
                SendToSource(hPart, protocolMachine.ResponseMessage.Encoding);
                if (bPart != null)
                {
                    SendToSource(bPart, protocolMachine.ResponseMessage.Encoding);
                }
            }
            if (!CanRead || !CanWrite)
            {
                View.PutLine("{0}: OnResponseBody: CanRead={1} CanWrite={2}", Name, CanRead, CanWrite);
                Close();
            }
        }

        private void OnWebsocketSend(byte[] buf, int offset, int count)
        {
            SendToDestination(buf, offset, count, Encoding.UTF8);
        }

        private void OnWebsocketReceive(byte[] buf, int offset, int count)
        {
            var data = new byte[count];
            Buffer.BlockCopy(buf, offset, data, 0, count);
            SendToSource(data, Encoding.UTF8);
        }

        private void OnConnectionOfOtherThanUpgrade()
        {
            View.PutLine("{0}: Connection: Upgrade was expected.", Name);
        }

        private void OnUpgradeOfOtherThanWebsocket()
        {
            View.PutLine("{0}: Upgrade: Websocket was expected.", Name);
        }

        private void OnError(Exception ex)
        {
            View.Put(ex);
        }

        private void OnCompletion()
        {
            Close();
        }

        private void ReplaceThisHostWithDstHost()
        {
            string hostRequestHeaderValue = protocolMachine.RequestMessage.HeaderList.Host;
            if (hostRequestHeaderValue != null)
            {
                var thisListener = new ConnectionTarget();
                thisListener.Parse(hostRequestHeaderValue, sourceConnectionType);
                var x = thisListener.ToString();
                var y = Destination.ToString();
                var bx = protocolMachine.RequestMessage.Encoding.GetBytes(x);
                var by = protocolMachine.RequestMessage.Encoding.GetBytes(y);
                if (protocolMachine.RequestMessage.MessageBody.Contains(bx))
                {
                    View.PutLine("{0}: Rewrite: {1} ===> {2}", Name, x, y);
                    protocolMachine.RequestMessage.MessageBody.Replace(bx, by);
                }
                bx = protocolMachine.RequestMessage.Encoding.GetBytes(thisListener.Hostname);
                by = protocolMachine.RequestMessage.Encoding.GetBytes(Destination.Hostname);
                if (protocolMachine.RequestMessage.MessageBody.Contains(bx))
                {
                    View.PutLine("{0}: Rewrite: {1} ===> {2}", Name, thisListener.Hostname, Destination.Hostname);
                    protocolMachine.RequestMessage.MessageBody.Replace(bx, by);
                }
            }
        }

        private void ReplaceDstHostWithThisHost()
        {
            string hostRequestHeaderValue = protocolMachine.RequestMessage.HeaderList.Host;
            if (hostRequestHeaderValue != null)
            {
                var thisListener = new ConnectionTarget();
                thisListener.Parse(hostRequestHeaderValue, sourceConnectionType);
                var x = Destination.ToString();
                var y = thisListener.ToString();
                var bx = protocolMachine.ResponseMessage.Encoding.GetBytes(x);
                var by = protocolMachine.ResponseMessage.Encoding.GetBytes(y);
                if (protocolMachine.ResponseMessage.MessageBody.Contains(bx))
                {
                    View.PutLine("{0}: Rewrite: {1} ===> {2}", Name, x, y);
                    protocolMachine.ResponseMessage.MessageBody.Replace(bx, by);
                }
                bx = protocolMachine.ResponseMessage.Encoding.GetBytes(Destination.Hostname);
                by = protocolMachine.ResponseMessage.Encoding.GetBytes(thisListener.Hostname);
                if (protocolMachine.ResponseMessage.MessageBody.Contains(bx))
                {
                    View.PutLine("{0}: Rewrite: {1} ===> {2}", Name, Destination.Hostname, thisListener.Hostname);
                    protocolMachine.ResponseMessage.MessageBody.Replace(bx, by);
                }
            }
        }

        private void SendToDestination(byte[] data, Encoding encoding)
        {
            SendToDestination(data, 0, data.Length, encoding);
        }

        private void SendToDestination(byte[] data, int offset, int count, Encoding encoding)
        {
            try
            {
                if (outerClient == null || !outerClient.CanWrite)
                {
                    outerClient?.Close();
                    outerClient = CreateOuterClient(SerialNumber);
                    outerClient.OnReceive = OnOuterClientReceive;
                    outerClient.OnClose = OnOuterClientClose;
                    outerClient.Open();
                }
                View.PutOutgoing(DstName, data, offset, count, encoding);
                outerClient.BeginWrite(data, offset, count);
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

        private void SendToSource(byte[] data, Encoding encoding)
        {
            try
            {
                View.PutOutgoing(SrcName, data, encoding);
                BeginWrite(data);
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

        protected abstract IOuterClient CreateOuterClient(int serialNumber);
    }
}
