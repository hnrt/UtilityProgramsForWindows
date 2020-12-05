namespace hnrt.HttpRelay.Net.Http
{
    internal class HttpConstants
    {
        public static readonly string HTTP_LEADER = "http://";
        public static readonly string HTTPS_LEADER = "https://";
        public const int HTTP_PORT = 80;
        public const int HTTPS_PORT = 443;

        public static int GetDefaultPort(bool isSecureConnection)
        {
            return isSecureConnection ? HTTPS_PORT : HTTP_PORT;
        }
    }
}
