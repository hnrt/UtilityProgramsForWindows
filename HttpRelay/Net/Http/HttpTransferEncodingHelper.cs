using System.Collections.Generic;

namespace hnrt.HttpRelay.Net.Http
{
    internal static class HttpTransferEncodingHelper
    {
        private static readonly Dictionary<string, HttpTransferEncoding> dict = new Dictionary<string, HttpTransferEncoding>()
        {
            { "chunked", HttpTransferEncoding.CHUNKED },
            { "identity", HttpTransferEncoding.IDENTITY },
            { "gzip", HttpTransferEncoding.GZIP },
            { "compress", HttpTransferEncoding.COMPRESS },
            { "deflate", HttpTransferEncoding.DEFLATE }
        };

        public static HttpTransferEncoding FromString(string s)
        {
            if (dict.TryGetValue(s.ToLowerInvariant(), out var value))
            {
                return value;
            }
            else
            {
                return HttpTransferEncoding.Other;
            }
        }
    }
}
