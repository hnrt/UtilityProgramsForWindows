using System;
using System.Net.Sockets;
using System.Security.Cryptography.X509Certificates;

namespace hnrt.HttpRelay.Controller
{
    class SecureListener : ListenerBase
    {
        public string ServerCertificatePath { get; set; }

        public string ServerCertificatePassword { get; set; }

        private X509Certificate2 serverCertificate;

        public SecureListener()
            : base("SecureListener")
        {
        }

        protected override void InitMainLoop()
        {
            if (string.IsNullOrEmpty(ServerCertificatePath))
            {
                throw new ArgumentException("ServerCertificatePath is null or empty.");
            }

            serverCertificate = new X509Certificate2(ServerCertificatePath, ServerCertificatePassword);

            View.PutLine("{0}: Subject={1}", Name, serverCertificate.Subject);
            View.PutLine("{0}: SerialNumber={1}", Name, serverCertificate.GetSerialNumberString());
        }

        protected override void StartSession(TcpClient client)
        {
            var innerClient = new SecureInnerClient(client)
            {
                ServerCertificate = serverCertificate,
                Destination = Destination,
                Rewrite = Rewrite
            };
            innerClient.Open();
        }
    }
}
