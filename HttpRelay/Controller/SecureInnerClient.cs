using System;
using System.Net.Security;
using System.Net.Sockets;
using System.Security.Authentication;
using System.Security.Cryptography.X509Certificates;
using hnrt.HttpRelay.Data;

namespace hnrt.HttpRelay.Controller
{
    class SecureInnerClient : InnerClientBase
    {
        private SslStream stream;

        public X509Certificate ServerCertificate { get; set; }

        public SecureInnerClient(TcpClient source)
            : base("SecureInnerClient", source, ConnectionType.Encrypted)
        {
            stream = new SslStream(source.GetStream(), false, new RemoteCertificateValidationCallback(OnCertificateValidation));
        }

        private bool OnCertificateValidation(object sender, X509Certificate certificate, X509Chain chain, SslPolicyErrors sslPolicyErrors)
        {
            View.PutLine("{0}: OnCertificateValidation called.", Name);
            return true;
        }

        public override void Open()
        {
            if (SetState(STANDBY, OPEN_PENDING))
            {
                try
                {
                    if (ServerCertificate == null)
                    {
                        throw new ArgumentException("Server certificate is not yet set.");
                    }
                    View.PutLine("{0}: SSL AuthenticateAsServer started.", Name);
                    stream.AuthenticateAsServer(ServerCertificate, false, SslProtocols.Tls12, false);
                    writeSemaphore.Release();
                    View.PutLine("{0}: SSL AuthenticateAsServer ended.", Name);
                    DisplaySecurityLevel();
                    DisplaySecurityServices();
                    DisplayCertificateInformation();
                    DisplayStreamProperties();
                    View.PutLine("{0}: Connection established.", Name);
                    base.Open();
                    SetState(OPEN_PENDING, INPROGRESS);
                    BeginRead();
                }
                catch (AuthenticationException ex)
                {
                    View.Put(ex);
                    Close();
                }
                catch (Exception ex)
                {
                    if (state == INPROGRESS || state == OPEN_PENDING)
                    {
                        View.Put(ex);
                    }
                    Close();
                }
            }
        }

        public override void Close()
        {
            if (SetState(INPROGRESS, CLOSE_PENDING) || SetState(OPEN_PENDING, CLOSE_PENDING))
            {
                View.PutLine("{0}: Connection being closed...", Name);
                stream.Flush();
                stream.Close();
                base.Close();
                SetState(CLOSE_PENDING, CLOSED);
                View.PutLine("{0}: Connection closed.", Name);
            }
        }

        public override void BeginRead()
        {
            stream.BeginRead(buffer, 0, BUFFER_SIZE, new AsyncCallback(ReadCallback), null);
        }

        public override int EndRead(IAsyncResult ar)
        {
            return stream.EndRead(ar);
        }

        public override void BeginWrite(byte[] data)
        {
            if (writeSemaphore.Wait(60000))
            {
                stream.BeginWrite(data, 0, data.Length, new AsyncCallback(WriteCallback), null);
            }
            else
            {
                View.PutLine("{0}: BeginWrite timed out.", Name);
            }
        }

        public override void EndWrite(IAsyncResult ar)
        {
            stream.EndWrite(ar);
            stream.Flush();
            writeSemaphore.Release();
        }

        protected override IOuterClient CreateOuterClient(int serialNumber)
        {
            return new SecureOuterClient(serialNumber, Destination);
        }

        public override bool CanRead => stream.CanRead;

        public override bool CanWrite => stream.CanWrite;

        private void DisplaySecurityLevel()
        {
            View.PutLine("{0}: Cipher: {1} strength {2}", Name, stream.CipherAlgorithm, stream.CipherStrength);
            View.PutLine("{0}: Hash: {1} strength {2}", Name, stream.HashAlgorithm, stream.HashStrength);
            View.PutLine("{0}: Key exchange: {1} strength {2}", Name, stream.KeyExchangeAlgorithm, stream.KeyExchangeStrength);
            View.PutLine("{0}: Protocol: {1}", Name, stream.SslProtocol);
        }

        private void DisplaySecurityServices()
        {
            View.PutLine("{0}: IsAuthenticated: {1}", Name, stream.IsAuthenticated);
            View.PutLine("{0}: IsServer: {1}", Name, stream.IsServer);
            View.PutLine("{0}: IsSigned: {1}", Name, stream.IsSigned);
            View.PutLine("{0}: IsEncrypted: {1}", Name, stream.IsEncrypted);
        }

        private void DisplayStreamProperties()
        {
            View.PutLine("{0}: CanRead: {1}", Name, stream.CanRead);
            View.PutLine("{0}: CanWrite: {1}", Name, stream.CanWrite);
            View.PutLine("{0}: CanTimeout: {1}", Name, stream.CanTimeout);
        }

        private void DisplayCertificateInformation()
        {
            View.PutLine("{0}: Certificate revocation list checked: {1}", Name, stream.CheckCertRevocationStatus);

            X509Certificate localCertificate = stream.LocalCertificate;
            if (localCertificate != null)
            {
                View.PutLine("{0}: Local cert was issued to {1} and is valid from {2} until {3}.", Name,
                    localCertificate.Subject,
                    localCertificate.GetEffectiveDateString(),
                    localCertificate.GetExpirationDateString());
            }
            else
            {
                View.PutLine("{0}: Local certificate is null.", Name);
            }

            // Display the properties of the client's certificate.
            X509Certificate remoteCertificate = stream.RemoteCertificate;
            if (remoteCertificate != null)
            {
                View.PutLine("{0}: Remote cert was issued to {1} and is valid from {2} until {3}.", Name,
                    remoteCertificate.Subject,
                    remoteCertificate.GetEffectiveDateString(),
                    remoteCertificate.GetExpirationDateString());
            }
            else
            {
                View.PutLine("{0}: Remote certificate is null.", Name);
            }
        }
    }
}
