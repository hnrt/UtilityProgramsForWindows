using System;

namespace hnrt.HttpRelay.Controller
{
    internal class CommandProcessor
    {
        public static readonly string DEFAULT_HOST = "localhost";

        public const int DEFAULT_PORT = 59999;

        public static readonly string SHUTDOWN = "shutdown";

        public static readonly string STATUSCODE = "statuscode=";

        public static readonly string REASONPHRASE = "reasonphrase=";

        public static readonly string DELAY = "delay=";

        public static readonly string DELAYURI = "delayuri=";

        public Action OnShutdown { get; set; }

        public Action<int> OnForceStatusCode { get; set; }

        public Action<string> OnForceReasonPhrase { get; set; }

        public Action<int> OnDelay { get; set; }

        public Action<string> OnDelayUri { get; set; }

        public CommandProcessor()
        {
        }

        public string Dispatch(string s)
        {
            string rsp;
            if (s == SHUTDOWN)
            {
                OnShutdown?.Invoke();
                rsp = "Done.";
            }
            else if (s.StartsWith(STATUSCODE))
            {
                var sc = int.Parse(s.Substring(STATUSCODE.Length));
                OnForceStatusCode?.Invoke(sc);
                rsp = string.Format("Done. {0}{1}", STATUSCODE, sc);
            }
            else if (s.StartsWith(REASONPHRASE))
            {
                var rp = s.Substring(REASONPHRASE.Length);
                OnForceReasonPhrase?.Invoke(rp);
                rsp = string.Format("Done. {0}{1}", REASONPHRASE, rp);
            }
            else if (s.StartsWith(DELAY))
            {
                var ms = int.Parse(s.Substring(DELAY.Length));
                OnDelay?.Invoke(ms);
                rsp = string.Format("Done. {0}{1}", DELAY, ms);
            }
            else if (s.StartsWith(DELAYURI))
            {
                var uri = s.Substring(DELAYURI.Length);
                OnDelayUri?.Invoke(uri);
                rsp = string.Format("Done. {0}{1}", DELAYURI, uri);
            }
            else
            {
                rsp = "No such command.";
            }
            return rsp;
        }
    }
}
