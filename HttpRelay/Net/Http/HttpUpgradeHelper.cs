namespace hnrt.HttpRelay.Net.Http
{
    internal static class HttpUpgradeHelper
    {
        public static HttpUpgrade FromString(string s)
        {
            var x = s.ToLowerInvariant();
            if (x == "websocket")
            {
                return HttpUpgrade.WEBSOCKET;
            }
            else
            {
                return HttpUpgrade.Other;
            }
        }
    }
}
