using System;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using hnrt.HttpRelay.Model;

namespace hnrt.HttpRelay.Net.Http
{
    internal class HttpMachine
    {
        public HttpRequestMessage RequestMessage { get; }

        public HttpResponseMessage ResponseMessage { get; }

        /// <summary>
        /// To be called when data are to be sent in Websocket mode.
        /// </summary>
        public Action<byte[], int, int> OnWebsocketSend { get; set; }

        /// <summary>
        /// To be called when data were received in Websocket mode.
        /// </summary>
        public Action<byte[], int, int> OnWebsocketReceive { get; set; }

        /// <summary>
        /// To be called when Connection header has an other value than Upgrade.
        /// </summary>
        public Action OnConnectionOfOtherThanUpgrade { get; set; }

        /// <summary>
        /// To be called when Upgrade header has an other value than Websocket.
        /// </summary>
        public Action OnUpgradeOfOtherThanWebsocket { get; set; }

        /// <summary>
        /// To be called when an exception was caught.
        /// </summary>
        public Action<Exception> OnError { get; set; }

        /// <summary>
        /// To be called when HTTP session was ended.
        /// </summary>
        public Action OnCompletion { get; set; }

        private ITextStore sndTextStore;

        private ITextStore rcvTextStore;

        private Task task;

        private const int STANDBY = 0;
        private const int HTTP_PENDING = 1;
        private const int HTTP = 2;
        private const int HTTP_PROCESSING = 3;
        private const int WEBSOCKET_PENDING = 4;
        private const int WEBSOCKET = 5;
        private const int WEBSOCKET_PROCESSING = 6;
        private const int CLOSE_PENDING = 7;
        private const int CLOSED = 8;

        private volatile int state = STANDBY;

        public HttpMachine()
        {
            sndTextStore = new TextStore()
            {
                Encoding = Encoding.UTF8
            };
            rcvTextStore = new TextStore()
            {
                Encoding = Encoding.UTF8
            };
            RequestMessage = new HttpRequestMessage(sndTextStore);
            ResponseMessage = new HttpResponseMessage(rcvTextStore);
        }

        public void Dispose()
        {
            sndTextStore.Dispose();
            rcvTextStore.Dispose();
        }

        public void Start()
        {
            if (SetState(STANDBY, HTTP_PENDING))
            {
                task = Task.Run(() => MainLoop());
            }
        }

        public void Stop()
        {
            while (state != STANDBY && state != CLOSED)
            {
                if (SetState(HTTP, CLOSE_PENDING)
                    || SetState(WEBSOCKET, CLOSE_PENDING))
                {
                    sndTextStore.Write(new byte[0], 0, 0);
                    rcvTextStore.Write(new byte[0], 0, 0);
                    task.Wait();
                    task.Dispose();
                    SetState(CLOSED);
                }
            }
        }

        public void SendFeed(byte[] buf, int offset, int count)
        {
            for (var x = state; STANDBY < x && x < CLOSE_PENDING; x = state)
            {
                if (SetState(HTTP, HTTP_PROCESSING))
                {
                    sndTextStore.Write(buf, offset, count);
                    SetState(HTTP_PROCESSING, HTTP);
                    return;
                }
                else if (SetState(WEBSOCKET, WEBSOCKET_PROCESSING))
                {
                    OnWebsocketSend?.Invoke(buf, offset, count);
                    SetState(WEBSOCKET_PROCESSING, WEBSOCKET);
                    return;
                }
            }
        }

        public void ReceiveFeed(byte[] buf, int offset, int count)
        {
            for (var x = state; STANDBY < x && x < CLOSE_PENDING; x = state)
            {
                if (SetState(HTTP, HTTP_PROCESSING))
                {
                    rcvTextStore.Write(buf, offset, count);
                    SetState(HTTP_PROCESSING, HTTP);
                    return;
                }
                else if (SetState(WEBSOCKET, WEBSOCKET_PROCESSING))
                {
                    OnWebsocketReceive?.Invoke(buf, offset, count);
                    SetState(WEBSOCKET_PROCESSING, WEBSOCKET);
                    return;
                }
            }
        }

        private void SetState(int to)
        {
            Interlocked.Exchange(ref state, to);
        }

        private bool SetState(int from, int to)
        {
            return Interlocked.CompareExchange(ref state, to, from) == from;
        }

        private void MainLoop()
        {
            try
            {
                SetState(HTTP_PENDING, HTTP);
                for (var x = state; x == HTTP || x == HTTP_PROCESSING; x = state)
                {
                    RequestMessage.Parse();
                    ResponseMessage.Parse(RequestMessage.Method);
                    if (ResponseMessage.StatusCode == 101 /* Switching Protocols */)
                    {
                        if (ResponseMessage.HeaderList.Connection == HttpConnection.UPGRADE)
                        {
                            if (ResponseMessage.HeaderList.Upgrade == HttpUpgrade.WEBSOCKET)
                            {
                                while (state < CLOSE_PENDING)
                                {
                                    if (SetState(HTTP, WEBSOCKET_PENDING))
                                    {
                                        var sndData = sndTextStore.ReadBytes();
                                        if (sndData.Length > 0)
                                        {
                                            OnWebsocketSend?.Invoke(sndData, 0, sndData.Length);
                                        }
                                        var rcvData = rcvTextStore.ReadBytes();
                                        if (rcvData.Length > 0)
                                        {
                                            OnWebsocketReceive?.Invoke(rcvData, 0, rcvData.Length);
                                        }
                                        SetState(WEBSOCKET_PENDING, WEBSOCKET);
                                        break;
                                    }
                                }
                            }
                            else
                            {
                                OnUpgradeOfOtherThanWebsocket?.Invoke();
                                break;
                            }
                        }
                        else
                        {
                            OnConnectionOfOtherThanUpgrade?.Invoke();
                            break;
                        }
                    }
                }
                for (var x = state; x == WEBSOCKET || x == WEBSOCKET_PROCESSING; x = state)
                {
                    Thread.Sleep(1);
                }
            }
            catch (Exception ex)
            {
                if (SetState(HTTP, HTTP) && RequestMessage.Method != HttpMethod.None)
                {
                    OnError?.Invoke(ex);
                }
            }
            OnCompletion?.Invoke();
        }
    }
}
