using System;
using System.Collections.Generic;

namespace hnrt.HttpRelay.Net.Http
{
    internal class HttpHeaderFactory
    {
        private static Dictionary<string, Func<string, HttpHeader>> dict;

        static HttpHeaderFactory()
        {
            dict = new Dictionary<string, Func<string, HttpHeader>>();

            dict.Add(HttpHeaderConnection.NAME.ToLowerInvariant(), (value) =>
            {
                return new HttpHeaderConnection(value);
            });

            dict.Add(HttpHeaderContentLength.NAME.ToLowerInvariant(), (value) =>
            {
                return new HttpHeaderContentLength(int.Parse(value));
            });

            dict.Add(HttpHeaderContentType.NAME.ToLowerInvariant(), (value) =>
            {
                return new HttpHeaderContentType(value);
            });

            dict.Add(HttpHeaderHost.NAME.ToLowerInvariant(), (value) =>
            {
                return new HttpHeaderHost(value);
            });

            dict.Add(HttpHeaderTransferEncoding.NAME.ToLowerInvariant(), (value) =>
            {
                return HttpHeaderTransferEncoding.Parse(value);
            });

            dict.Add(HttpHeaderUpgrade.NAME.ToLowerInvariant(), (value) =>
            {
                return new HttpHeaderUpgrade(value);
            });
        }

        public static HttpHeader CreateHeader(string key, string value)
        {
            if (dict.TryGetValue(key.ToLowerInvariant(), out Func<string, HttpHeader> func))
            {
                return func(value);
            }
            else
            {
                return new HttpHeader(key, value);
            }
        }
    }
}
