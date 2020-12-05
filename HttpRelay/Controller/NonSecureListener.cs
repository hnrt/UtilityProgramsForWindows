using System.Net.Sockets;

namespace hnrt.HttpRelay.Controller
{
    internal class NonSecureListener : ListenerBase
    {
        public NonSecureListener()
            : base("NonSecureListener")
        {
        }

        protected override void InitMainLoop()
        {
        }

        protected override void StartSession(TcpClient client)
        {
            var innerClient = new NonSecureInnerClient(client)
            {
                Destination = Destination,
                Rewrite = Rewrite
            };
            innerClient.Open();
        }
    }
}
