using System;
using hnrt.HttpRelay.Data;
using hnrt.HttpRelay.Net.Http;

namespace hnrt.HttpRelay.Model
{
    internal class ConnectionTarget
    {
        private static readonly char[] COLON = { ':' };

        public ConnectionType Type { get; set; }

        public string Hostname { get; set; }

        public int Port { get; set; }

        public string Address { get; set; }

        public ConnectionTarget()
        {
            Type = ConnectionType.Undefined;
            Hostname = null;
            Port = 0;
            Address = null;
        }

        public bool Parse(string value)
        {
            var s = value.Trim();
            if (s.StartsWith(HttpConstants.HTTP_LEADER))
            {
                return Parse(s.Substring(HttpConstants.HTTP_LEADER.Length), ConnectionType.Raw);
            }
            else if (s.StartsWith(HttpConstants.HTTPS_LEADER))
            {
                return Parse(s.Substring(HttpConstants.HTTPS_LEADER.Length), ConnectionType.Encrypted);
            }
            else
            {
                Type = ConnectionType.Undefined;
                Hostname = null;
                Port = 0;
                return false;
            }
        }

        public bool Parse(string value, ConnectionType connectionType)
        {
            Type = connectionType;
            Hostname = null;
            Port = 0;
            var s = value.Trim();
            int end = s.IndexOf('/');
            if (end > -1)
            {
                s = s.Substring(0, end);
            }
            var ss = s.Split(COLON, 2);
            if (ss.Length == 0)
            {
                return false;
            }
            else if (ss.Length == 1)
            {
                Hostname = ss[0];
                Port = Type == ConnectionType.Encrypted ? HttpConstants.HTTPS_PORT : HttpConstants.HTTP_PORT;
                return true;
            }
            else //if (ss.Length == 2)
            {
                try
                {
                    Hostname = ss[0];
                    Port = int.Parse(ss[1]);
                    return true;
                }
                catch (FormatException)
                {
                    return false;
                }
            }
        }

        public override string ToString()
        {
            var leader = Type == ConnectionType.Encrypted ? HttpConstants.HTTPS_LEADER : HttpConstants.HTTP_LEADER;
            if (Port == (Type == ConnectionType.Encrypted ? HttpConstants.HTTPS_PORT : HttpConstants.HTTP_PORT))
            {
                return string.Format("{0}{1}", leader, Hostname);
            }
            else
            {
                return string.Format("{0}{1}:{2}", leader, Hostname, Port);
            }
        }
    }
}
