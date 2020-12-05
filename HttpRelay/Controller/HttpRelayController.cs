using System;
using System.Collections.Generic;
using System.Threading;
using hnrt.HttpRelay.Data;
using hnrt.HttpRelay.Model;
using hnrt.HttpRelay.Net.Http;
using hnrt.HttpRelay.View;

namespace hnrt.HttpRelay.Controller
{
    class HttpRelayController : IController
    {
        private static readonly string BAD_COMMAND_LINE_SYNTAX = "Bad command line syntax. Please try -help for usage.";
        private static readonly string BAD_TARGET = "The specified target is not valid.";
        private static readonly string NO_TARGET = "The target needs to be specified first.";
        private static readonly string BAD_NPORT = "The specified port is out of range.";
        private static readonly string BAD_SPORT = "The specified secure port is out of range.";
        private static readonly string CERTIFICATE_PATH_AND_PASSWORD = "Certificate path and password are both needed.";
        private static readonly string EXCLUSIVE_REQUESTS = "Unable to run listener and to send command at the same time.";

        private string Name { get; } = System.Reflection.Assembly.GetExecutingAssembly().GetName().Name;

        private IView View { get; set; } = ViewFactory.CreateView();

        private List<IListener> ListenerList { get; } = new List<IListener>();

        private ManualResetEvent TerminateEvent { get; } = new ManualResetEvent(false);

        private ConnectionTarget target = null;

        private int nonSecurePort = 0;

        private int securePort = 0;
        private string certificatePath = null;
        private string certificatePassword = null;

        private string commandTarget = CommandProcessor.DEFAULT_HOST;
        private int commandPort = CommandProcessor.DEFAULT_PORT;
        private string commandRequest = null;

        public HttpRelayController()
        {
        }

        public void Run(string[] args)
        {
            if (args.Length == 0)
            {
                Help();
                return;
            }

            var e = args.GetEnumerator();
            while (e.MoveNext())
            {
                var a = ((string)e.Current).ToLowerInvariant();
                if (a == "-help" || a == "-h" || a == "-?")
                {
                    Help();
                    return;
                }
                else if (a == "-output" || a == "-out")
                {
                    if (e.MoveNext())
                    {
                        ViewFactory.Builder.SetPath((string)e.Current);
                        View = ViewFactory.CreateView();
                    }
                    else
                    {
                        throw new Exception(BAD_COMMAND_LINE_SYNTAX);
                    }
                }
                else if (a == "-target")
                {
                    if (e.MoveNext())
                    {
                        target = new ConnectionTarget();
                        if (!target.Parse((string)e.Current))
                        {
                            throw new Exception(BAD_TARGET);
                        }
                    }
                    else
                    {
                        throw new Exception(BAD_COMMAND_LINE_SYNTAX);
                    }
                }
                else if (a == "-address" || a == "-addr")
                {
                    if (e.MoveNext())
                    {
                        if (target == null)
                        {
                            throw new Exception(NO_TARGET);
                        }
                        target.Address = (string)e.Current;
                    }
                    else
                    {
                        throw new Exception(BAD_COMMAND_LINE_SYNTAX);
                    }
                }
                else if (a == "-port")
                {
                    if (e.MoveNext())
                    {
                        nonSecurePort = int.Parse((string)e.Current);
                    }
                    else
                    {
                        throw new Exception(BAD_COMMAND_LINE_SYNTAX);
                    }
                    if (nonSecurePort <= 0 || 65536 <= nonSecurePort)
                    {
                        throw new Exception(BAD_NPORT);
                    }
                }
                else if (a == "-secureport" || a == "-secport")
                {
                    if (e.MoveNext())
                    {
                        securePort = int.Parse((string)e.Current);
                    }
                    else
                    {
                        throw new Exception(BAD_COMMAND_LINE_SYNTAX);
                    }
                    if (securePort <= 0 || 65536 <= securePort)
                    {
                        throw new Exception(BAD_SPORT);
                    }
                }
                else if (a == "-certpath")
                {
                    if (e.MoveNext())
                    {
                        certificatePath = (string)e.Current;
                    }
                    else
                    {
                        throw new Exception(BAD_COMMAND_LINE_SYNTAX);
                    }
                }
                else if (a == "-certpassword" || a == "-certpasswd" || a == "-certpass")
                {
                    if (e.MoveNext())
                    {
                        certificatePassword = (string)e.Current;
                    }
                    else
                    {
                        throw new Exception(BAD_COMMAND_LINE_SYNTAX);
                    }
                }
                else if (a == "-shutdown")
                {
                    commandRequest = "shutdown";
                }
                else if (a == "-commandtarget" || a == "-cmdtarget")
                {
                    if (e.MoveNext())
                    {
                        commandTarget = (string)e.Current;
                    }
                    else
                    {
                        throw new Exception(BAD_COMMAND_LINE_SYNTAX);
                    }
                    var ss = commandTarget.Split(new char[] { ':' });
                    if (ss.Length == 2)
                    {
                        commandTarget = ss[0];
                        commandPort = int.Parse(ss[1]);
                    }
                    else if (ss.Length != 1)
                    {
                        throw new Exception(BAD_COMMAND_LINE_SYNTAX);
                    }
                }
                else if (a == "-commandport" || a == "-cmdport")
                {
                    if (e.MoveNext())
                    {
                        commandPort = int.Parse((string)e.Current);
                    }
                    else
                    {
                        throw new Exception(BAD_COMMAND_LINE_SYNTAX);
                    }
                }
#if DEBUG
                else if (a == "-debug")
                {
                    System.Diagnostics.Debugger.Launch();
                }
#endif
                else
                {
                    throw new Exception(BAD_COMMAND_LINE_SYNTAX);
                }
            }

            if (target != null)
            {
                if (nonSecurePort == 0 && target.Type == ConnectionType.Raw)
                {
                    nonSecurePort = HttpConstants.HTTP_PORT;
                }
                if (nonSecurePort > 0)
                {
                    var nonSecureListener = new NonSecureListener()
                    {
                        Destination = target,
                        Port = nonSecurePort
                    };
                    ListenerList.Add(nonSecureListener);
                }

                if ((certificatePath != null && certificatePassword == null) || (certificatePath == null && certificatePassword != null))
                {
                    throw new Exception(CERTIFICATE_PATH_AND_PASSWORD);
                }
                if (securePort == 0 && (target.Type == ConnectionType.Encrypted || certificatePath != null))
                {
                    securePort = HttpConstants.HTTPS_PORT;
                }
                if (securePort > 0)
                {
                    if (certificatePath == null || certificatePassword == null)
                    {
                        throw new Exception(CERTIFICATE_PATH_AND_PASSWORD);
                    }
                    var secureListener = new SecureListener()
                    {
                        Destination = target,
                        Port = securePort,
                        ServerCertificatePath = certificatePath,
                        ServerCertificatePassword = certificatePassword
                    };
                    ListenerList.Add(secureListener);
                }
            }

            if (ListenerList.Count > 0)
            {
                if (commandRequest != null)
                {
                    throw new Exception(EXCLUSIVE_REQUESTS);
                }

                var commandListener = new CommandListener()
                {
                    Port = commandPort,
                    OnShutdown = ShutDown
                };
                ListenerList.Add(commandListener);

                foreach (var listener in ListenerList)
                {
                    listener.Start();
                }

                TerminateEvent.WaitOne();

                foreach (var listener in ListenerList)
                {
                    listener.Stop();
                }

                foreach (var listener in ListenerList)
                {
                    listener.Wait();
                }
            }
            else if (commandRequest != null)
            {
                var commandClient = new CommandClient()
                {
                    Hostname = commandTarget,
                    Port = commandPort
                };
                commandClient.Send(commandRequest);
            }
            else
            {
                throw new Exception(BAD_COMMAND_LINE_SYNTAX);
            }
        }

        public void ShutDown()
        {
            TerminateEvent.Set();
        }

        private void Help()
        {
            View.PutLine("Usage:");
            View.PutLine("  {0} -target http://hostname [-addr IPADDR] [-port PORT] [-cmdPort PORT] [-out PATH]", Name);
            View.PutLine("  {0} -target https://hostname [-addr IPADDR] [-secPort PORT] -certPath PATH -certPass PASSWORD [-cmdPort PORT] [-out PATH]", Name);
            View.PutLine("  {0} -shutdown [-cmdTarget HOSTNAME[:PORT]]", Name);
            View.PutLine("  {0} -help", Name);
        }
    }
}
