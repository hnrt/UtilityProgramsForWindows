using System;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace hnrt.HttpRelay.Controller
{
    internal class CommandListener : ListenerBase
    {
        public Action OnShutdown { get; set; }

        public CommandListener()
            : base("CommandListener")
        {
        }

        protected override void InitMainLoop()
        {
            // NOP
        }

        protected override void StartSession(TcpClient client)
        {
            var server = new CommandServer(client)
            {
                OnShutdown = OnShutdown
            };
            server.Open();
        }
    }
}
