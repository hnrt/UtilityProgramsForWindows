using System;
using System.Text;
using hnrt.HttpRelay.Model;

namespace hnrt.HttpRelay.Net.Http
{
    internal class HttpResponseMessage : HttpMessage
    {
        public static readonly string ERROR_MISSING_STATUSCODE = "Missing status code.";
        public static readonly string ERROR_MALFORMED_STATUSCODE = "Malformed status code.";

        public HttpStatusLine StatusLine { get; } = new HttpStatusLine();

        public override HttpMethod Method { get; set; }

        public override HttpVersion ProtocolVersion => StatusLine.HttpVersion;

        public int StatusCode => StatusLine.StatusCode;

        public string ReasonPhrase => StatusLine.ReasonPhrase;

        public HttpResponseMessage(ITextStore textStore)
            : base("response", textStore)
        {
        }

        public HttpResponseMessage()
            : this(new TextStore() { Encoding = Encoding.UTF8 })
        {
        }

        public void Parse(HttpMethod method)
        {
            do
            {
                Initialize(method);
                ParseStatusLine();
                while (ParseHeader())
                {
                    continue;
                }
                ParseCRLF(false);
                OnReceiveHeader?.Invoke();
                ParseMessageBody();
            }
            while (StatusCode == 100);
        }

        private void Initialize(HttpMethod method)
        {
            Method = method;
            StatusLine.Clear();
            HeaderList.Clear();
            MessageBody = null;
            c = TextStore.ReadChar();
        }

        private void ParseStatusLine()
        {
            ParseHttpVersion();
            ParseSP();
            ParseStatusCode();
            ParseSP();
            ParseReasonPhrase();
            ParseCRLF();
        }

        private void ParseStatusCode()
        {
            if (ParseNumber())
            {
                try
                {
                    StatusLine.StatusCode = int.Parse(sb.ToString());
                }
                catch (Exception ex)
                {
                    throw new Exception(ERROR_MALFORMED_STATUSCODE, ex);
                }
            }
            else
            {
                throw new Exception(ERROR_MISSING_STATUSCODE);
            }
        }

        private void ParseReasonPhrase()
        {
            sb.Clear();
            while (c != -1 && !HttpChar.IsControl(c))
            {
                sb.Append((char)c);
                c = TextStore.ReadChar();
            }
            StatusLine.ReasonPhrase = sb.ToString();
        }

        protected override bool CanHaveMessageBody()
        {
            if ((100 <= StatusCode && StatusCode <= 199)
                || StatusCode == 204 // No Content
                || StatusCode == 304 // Not Modified
                || Method == HttpMethod.HEAD
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
            sb.Append(StatusLine.ToString());
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
