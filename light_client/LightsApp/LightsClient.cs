using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Lights
{

    public class LightsClient
    {
        [StructLayout(LayoutKind.Sequential, Pack=1)]
        struct SettingsData
        {
            public byte Brightness;
            public byte Hue;
            public byte Saturation;
            public byte NightMode;
        }

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

        private async Task<SettingsData> ReadSettings()
        {
            return new SettingsData()
            {
                Brightness = 200,
                Hue = 128,
                Saturation = 255,
                NightMode = 0
            };
        }

        private async Task ResetDevice()
        {
        }

        private async Task WriteSettings(SettingsData newSettings)
        {
        }

        public bool NightMode
        {
            get;
            set;
        }

        public double Brightness
        {
            get;
            set;
        }

        public double Hue
        {
            get;
            set;
        }
    }
}
