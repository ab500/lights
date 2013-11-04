using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using Windows.Networking;
using Windows.Networking.Sockets;
using Windows.Storage.Streams;

namespace Lights
{

    public class LightsClient : INotifyPropertyChanged
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

        private const byte ReadSettingsServiceId = 1;
        private const byte WriteSettingsServiceId = 2;
        private const string _hostPort = "5001";
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

            byte[] responseAsBytes = null;
            byte[] commandAsBytes = CommandAsByteArray(lc);

            try
            {
                DataWriter writer = new DataWriter(_socket.OutputStream);
                DataReaderLoadOperation drlo = null;

                writer.WriteBytes(commandAsBytes);
                await writer.StoreAsync();

                DataReader reader = new DataReader(_socket.InputStream);
                drlo = reader.LoadAsync(4);
                await Task.WhenAny(new Task[] { drlo.AsTask(), Task.Delay(2000) });
                if (drlo.Status != Windows.Foundation.AsyncStatus.Completed)
                {
                    drlo.Cancel();
                    throw new Exception();
                }

                uint bytesToRead = reader.ReadUInt32();
                
                if (bytesToRead != Marshal.SizeOf<SettingsData>())
                {
                    System.Diagnostics.Debug.WriteLine("Incorrect byte size received.");
                    throw new Exception();
                }

                drlo = reader.LoadAsync(bytesToRead);
                await Task.WhenAny(new Task[] { drlo.AsTask(), Task.Delay(2000) });
                if (drlo.Status != Windows.Foundation.AsyncStatus.Completed)
                {
                    drlo.Cancel();
                    throw new Exception();
                }

                responseAsBytes = new byte[bytesToRead];
                reader.ReadBytes(responseAsBytes);
            }
            catch (Exception e)
            {
                System.Diagnostics.Debug.WriteLine("Read failed: " + e.Message);
                CloseConnection(true);
                return;
            }

            SettingsData sd = ByteArrayAsDataStruct<SettingsData>(responseAsBytes);
            _hue = (double)sd.Hue / 255;
            _brightness = (double)sd.Brightness / 255;
            _nightMode = sd.NightMode == 1;
            OnPropertyChanged("Hue");
            OnPropertyChanged("Brightness");
            OnPropertyChanged("NightMode");
        }
        private async Task WriteSettings(SettingsData newSettings)
        {
        }

        private async Task ResetDevice()
        {
        }

        #region Communication Helpers
        private void CloseConnection(bool fireErrorEvent)
        {
            System.Diagnostics.Debug.WriteLine("Closing connection due to error.");
            _socket.Dispose();
            _socket = null;
            if (fireErrorEvent)
            {
                var handler = ConnectionLost;
                if (handler != null)
                {
                    handler.Invoke(this, EventArgs.Empty);
                }
            }
        }

        [StructLayout(LayoutKind.Explicit)]
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
        private byte[] CommandAsByteArray(LightsCommand lc)
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

        private T ByteArrayAsDataStruct<T>(byte[] bytes) where T : struct
        {
            GCHandle handle = GCHandle.Alloc(bytes, GCHandleType.Pinned);
            T retStruct = Marshal.PtrToStructure<T>(handle.AddrOfPinnedObject());
            handle.Free();
            return retStruct;
        }
        #endregion

        public event PropertyChangedEventHandler PropertyChanged;        
        private void OnPropertyChanged([CallerMemberName]string caller = null)
        {
            var handler = PropertyChanged;
            if (handler != null)
            {
                handler(this, new PropertyChangedEventArgs(caller));
            }
        }

        #region Public Properties
        private bool _nightMode = false;
        private double _brightness = 0.0;
        private double _hue = 0.0;

        public bool NightMode
        {
            get { return _nightMode; }
            set { _nightMode = value; OnPropertyChanged(); }
        }

        public double Brightness
        {
            get { return _brightness; }
            set { _brightness = value; OnPropertyChanged(); }
        }

        public double Hue
        {
            get { return _hue; }
            set { _hue = value; OnPropertyChanged(); }
        }
        #endregion


    }
}
