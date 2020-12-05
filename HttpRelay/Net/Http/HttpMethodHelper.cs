using System;

namespace hnrt.HttpRelay.Net.Http
{
    internal static class HttpMethodHelper
    {
        public static HttpMethod Parse(string value)
        {
            foreach (HttpMethod m in Enum.GetValues(typeof(HttpMethod)))
            {
                var name = Enum.GetName(typeof(HttpMethod), m);
                if (name == value)
                {
                    return m;
                }
            }
            return HttpMethod.Unknown;
        }

        public static string GetString(HttpMethod value)
        {
            return Enum.GetName(typeof(HttpMethod), value);
        }
    }
}
