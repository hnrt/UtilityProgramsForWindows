using System;
using System.Text;
using hnrt.HttpRelay.Model;

namespace hnrt.HttpRelay.Net.Http
{
    internal class HttpRequestMessage : HttpMessage
    {
        public static readonly string ERROR_UNKNOWN_METHOD = "Unknown method.";
        public static readonly string ERROR_MISSING_REQUESTURI = "Missing request URI.";

        public HttpRequestLine RequestLine { get; } = new HttpRequestLine();

        public override HttpMethod Method
        {
            get
            {
                return RequestLine.Method;
            }
            set
            {
                RequestLine.Method = value;
            }
        }

        public override HttpVersion ProtocolVersion => RequestLine.HttpVersion;

        public string RequestUri => RequestLine.RequestUri;

        public HttpRequestMessage(ITextStore textStore)
            : base("request", textStore)
        {
        }

        public void Parse()
        {
            Initialize();
            ParseRequestLine();
            while (ParseHeader())
            {
                continue;
            }
            ParseCRLF(false);
            OnReceiveHeader?.Invoke();
            ParseMessageBody();
        }

        private void Initialize()
        {
            RequestLine.Clear();
            HeaderList.Clear();
            MessageBody = null;
            c = TextStore.ReadChar();
        }

        private void ParseRequestLine()
        {
            ParseMethod();
            ParseSP();
            ParseRequestUri();
            ParseSP();
            ParseHttpVersion();
            ParseCRLF();
        }

        private void ParseMethod()
        {
            if (ParseToken())
            {
                RequestLine.Method = HttpMethodHelper.Parse(sb.ToString());
                if (RequestLine.Method == HttpMethod.Unknown)
                {
                    throw new Exception(ERROR_UNKNOWN_METHOD);
                }
            }
            else
            {
                RequestLine.Method = c == -1 ? HttpMethod.None : HttpMethod.Unknown;
                throw new Exception(ERROR_MISSING_TOKEN);
            }
        }

        private void ParseRequestUri()
        {
            if (c == '*')
            {
                RequestLine.RequestUri = "*";
                c = TextStore.ReadChar();
            }
            else if (ParseString())
            {
                RequestLine.RequestUri = sb.ToString();
            }
            else
            {
                throw new Exception(ERROR_MISSING_REQUESTURI);
            }
        }

        protected override bool CanHaveMessageBody()
        {
            if (Method == HttpMethod.HEAD
                || Method == HttpMethod.GET
                )
            {
                return false;
            }
            else
            {
                return true;
            }
        }

        public override byte[] GetHeader(params HttpHeader[] rewrites)
        {
            sb.Clear();
            sb.Append(RequestLine.ToString());
            HeaderList.Format(sb, rewrites);
            sb.Append("\r\n");
            return Encoding.GetBytes(sb.ToString());
        }

        public override byte[] GetBytes(params HttpHeader[] rewrites)
        {
            var data = GetHeader(rewrites);
            if (MessageBody != null)
            {
                int headerLength = data.Length;
                var mb = MessageBody.GetBytes(Encoding);
                Array.Resize<byte>(ref data, headerLength + mb.Length);
                Buffer.BlockCopy(mb, 0, data, headerLength, mb.Length);
            }
            return data;
        }
    }
}
