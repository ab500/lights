using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Lights
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        public MainPage()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.  The Parameter
        /// property is typically used to configure the page.</param>
        protected async override void OnNavigatedTo(NavigationEventArgs e)
        {
            var hostString = Windows.Storage.ApplicationData.Current.RoamingSettings.Values["HostString"];
            if (hostString != null)
            {
                txtHostString.Text = hostString as string;
                GoToState(State.Connecting);
                LightsClient.CommandResult cr = await ((App)App.Current).Client.Connect(hostString as string);
                if (cr == LightsClient.CommandResult.Succeeded)
                {
                    Frame.Navigate(typeof(HomePage));
                }
                else
                {
                    GoToState(State.InvalidHost);
                }
            }
        }

        private async void btnConnect_Click(object sender, RoutedEventArgs e)
        {
            if (String.IsNullOrEmpty(txtHostString.Text))
            {
                GoToState(State.InvalidHost);
            }
            else
            {
                GoToState(State.Connecting);
                LightsClient.CommandResult cr = await ((App)App.Current).Client.Connect(txtHostString.Text);
                if (cr == LightsClient.CommandResult.Succeeded)
                {
                    Windows.Storage.ApplicationData.Current.RoamingSettings.Values["HostString"] = txtHostString.Text;
                    Frame.Navigate(typeof(HomePage));
                }
                else
                {
                    GoToState(State.InvalidHost);
                }
            }
        }

        enum State
        {
            WaitingForInput,
            Connecting,
            InvalidHost
        }

        private void GoToState(State s)
        {
            switch (s)
            {
                case State.Connecting:
                    progressIndicator.IsActive = true;
                    txtHostString.IsEnabled = false;
                    btnConnect.IsEnabled = false;
                    lblError.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
                    break;

                case State.InvalidHost:
                    progressIndicator.IsActive = false;
                    txtHostString.IsEnabled = true;
                    btnConnect.IsEnabled = true;
                    lblError.Visibility = Windows.UI.Xaml.Visibility.Visible;
                    break;

                case State.WaitingForInput:
                    progressIndicator.IsActive = false;
                    txtHostString.IsEnabled = true;
                    btnConnect.IsEnabled = true;
                    lblError.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
                    break;
            }
        }
        
    }
}
