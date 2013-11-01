using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using Windows.Networking;
using Windows.Networking.Sockets;
using Windows.Storage.Streams;

namespace Lights
{

    public class LightsClient
    {

        #region Communication Structs
        [StructLayout(LayoutKind.Sequential, Pack=1)]
        struct SettingsData
        {
            public byte Brightness;
            public byte Hue;
            public byte Saturation;
            public byte NightMode;
        }

        struct LightsCommand
        {
            public byte ServiceId;
            public byte[] Payload;
        }
        #endregion

        public enum CommandResult
        {
            Succeeded,
            Failed
        };

        private static const byte ReadSettingsServiceId = 1;
        private static const byte WriteSettingsServiceId = 2;
        private static const string _hostPort = "5001";
        private Windows.Networking.Sockets.StreamSocket _socket = null;
        private SettingsData _currentData;
        private SettingsData? _pendingData;

        public LightsClient()
        {
        }

        public event EventHandler ConnectionLost;

        public async Task<CommandResult> Connect(string hostString)
        {
            try {
                _socket = new Windows.Networking.Sockets.StreamSocket();
                var host = new HostName(hostString);
                await _socket.ConnectAsync(host, _hostPort);
                await ReadSettings();

            }
            catch (Exception e) {
                System.Diagnostics.Debug.WriteLine("Connection failed: " + e.Message);
                _socket = null;
                return CommandResult.Failed;
            }

            await System.Threading.Tasks.Task.Delay(2000);
            return CommandResult.Succeeded;
        }

        private async Task ReadSettings()
        {
            LightsCommand lc = new LightsCommand()
            {
                ServiceId = 1,
                Payload = null
            };
            byte[] commandAsBytes = CommandAsBytes(lc);

            try
            {
                DataWriter writer = new DataWriter(_socket.OutputStream);
                writer.WriteBytes(commandAsBytes);
                await writer.StoreAsync();

                DataReader reader = new DataReader(_socket.InputStream);
                
                await reader.LoadAsync(4);
                uint bytesToRead = reader.ReadUInt32();
                
                if (bytesToRead > 1024)
                {
                    System.Diagnostics.Debug.WriteLine("Very large size read requested. Closing connection.");
                }

            }
            catch (Exception e)
            {

            }


            // Convert LightsCommand into byte stream
            // Send byte stream
            // await the new response
            // fill a byte array
            // cast the right parts back

        }
        private async Task WriteSettings(SettingsData newSettings)
        {
        }

        private async Task ResetDevice()
        {
        }

        #region Communication Helpers
        struct Int32Converter
        {
            [FieldOffset(0)]
            public int Value;
            [FieldOffset(0)]
            public byte Byte1;
            [FieldOffset(1)]
            public byte Byte2;
            [FieldOffset(2)]
            public byte Byte3;
            [FieldOffset(3)]
            public byte Byte4;

            public Int32Converter(int value)
            {
                Byte1 = Byte2 = Byte3 = Byte4 = 0;
                Value = value;
            }

            public static implicit operator Int32(Int32Converter value)
            {
                return value.Value;
            }

            public static implicit operator Int32Converter(int value)
            {
                return new Int32Converter(value);
            }
        }
        private byte[] CommandAsBytes(LightsCommand lc)
        {
            const int lengthSpecifierSize = 4;
            const int serviceIdSize = 1;
            int payloadSize = lc.Payload == null ? 0 : lc.Payload.Length;
            int size = lengthSpecifierSize + serviceIdSize + payloadSize;

            byte[] retArray = new byte[size];
            Int32Converter serviceIdConverter = lc.ServiceId;
            retArray[0] = serviceIdConverter.Byte1;
            retArray[1] = serviceIdConverter.Byte2;
            retArray[2] = serviceIdConverter.Byte3;
            retArray[3] = serviceIdConverter.Byte4;

            retArray[4] = lc.ServiceId;

            if (lc.Payload != null)
            {
                Array.Copy(lc.Payload, 0, retArray, 5, payloadSize);
            }

            return retArray;
        }
        #endregion



        #region Public Properties
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
        #endregion
    }
}
