using System;
using System.Collections.Generic;
using System.Text;
using Photon;

namespace PhotonTest
{
    class Program
    {
        static void Main(string[] args)
        {
            Process proc = new Process();

            try
            {
                proc.Init("10701", false);
            }
            catch(PhotonException e)
            {
                Console.WriteLine(e.ToString());
                Console.WriteLine("Init");
                Environment.Exit(e.ErrorCode());
            }

            int rc = 0;
            Console.WriteLine("Init" + rc);

            Session sess = new Session();

            ObjStatus status = new ObjStatus();

            sess.GetStatus("/test", ref status);
        }
    }
}
