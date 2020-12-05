namespace hnrt.HttpRelay.Model
{
    internal class ResponseDelay
    {
        /// <summary>
        /// Duration in milliseconds.
        /// </summary>
        public int Duration { get; set; }

        /// <summary>
        /// If this is set, a response will be delayed only when the URI matches with this. 
        /// </summary>
        public string Uri { get; set; }

        public bool IsValid => Duration > 0;

        public ResponseDelay()
        {
            Duration = 0;
            Uri = null;
        }

        public ResponseDelay Clone()
        {
            return new ResponseDelay()
            {
                Duration = Duration,
                Uri = Uri
            };
        }
    }
}
