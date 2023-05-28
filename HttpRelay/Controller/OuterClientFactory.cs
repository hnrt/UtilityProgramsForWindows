using hnrt.HttpRelay.Data;
using hnrt.HttpRelay.Model;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace hnrt.HttpRelay.Controller
{
    internal class OuterClientFactory
    {
        public static IOuterClient CreateOuterClient(int serialNumber, ConnectionTarget target)
        {
            if (target.Type == ConnectionType.Encrypted)
            {
                return new SecureOuterClient(serialNumber, target);
            }
            else
            {
                return new NonSecureOuterClient(serialNumber, target);
            }
        }
    }
}
