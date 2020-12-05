using System.Collections.Generic;

namespace hnrt.HttpRelay.Net.Http
{
    internal class HttpHeaderTransferEncoding : HttpHeader
    {
        public static readonly string NAME = "Transfer-Encoding";

        private static readonly char[] SEP_SC = { ';' };
        private static readonly char[] SEP_EQ = { '=' };

        public HttpTransferEncoding TransferEncoding { get; private set; } = HttpTransferEncoding.Other;

        public List<(string Name, string Value)> Parameters { get; } = new List<(string Name, string Value)>();

        public bool IsChunked => TransferEncoding == HttpTransferEncoding.CHUNKED;

        private HttpHeaderTransferEncoding(string value)
            : base(NAME, value)
        {
        }

        public static HttpHeaderTransferEncoding Parse(string value)
        {
            var te = new HttpHeaderTransferEncoding(value);
            var ss = value.Trim().Split(SEP_SC);
            if (ss.Length > 0)
            {
                te.TransferEncoding = HttpTransferEncodingHelper.FromString(ss[0].TrimEnd());
                for (int i = 1; i < ss.Length; i++)
                {
                    var tt = ss[i].Trim().Split(SEP_EQ, 2);
                    if (tt.Length == 2)
                    {
                        te.Parameters.Add((tt[0].TrimEnd(), tt[1].TrimStart()));
                    }
                    else if (tt.Length == 1)
                    {
                        te.Parameters.Add((tt[0], null));
                    }
                }
            }
            return te;
        }
    }
}
