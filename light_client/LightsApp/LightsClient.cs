using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Lights
{
    public class LightsClient
    {
        public enum CommandResult
        {
            Succeeded,
            Failed
        };

        public async Task<CommandResult> Connect(string hostString)
        {
            // Try to connect to the IP address
            // Do a basic 'echo' command
            // If all that's successful save the connection, return Succeeded
            // else Fail and cleanup.
            await System.Threading.Tasks.Task.Delay(2000);
            return CommandResult.Succeeded;
        }
    }
}
