namespace hnrt.HttpRelay.Controller
{
    internal class ControllerFactory
    {
        public static IController CreateController()
        {
            return new HttpRelayController();
        }
    }
}
