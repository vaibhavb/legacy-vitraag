using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using Microsoft.Phone.Controls;

namespace MoodTracker
{
    /// <summary>
    /// FIXED: This page is giving trouble for handling back button in two scenarios
    //  1. Going back should be disabled after the auth application is complete
    /// 2. The page should restart the app work-flow if the application gets tombed.
    /// </summary>
    public partial class HostedBrowser : PhoneApplicationPage
    {
        public HostedBrowser()
        {
            InitializeComponent();
            c_webBrowser.IsScriptEnabled = true;

            Loaded += new RoutedEventHandler(HealthVaultWebPage_Loaded);
            c_webBrowser.Navigated += new EventHandler<System.Windows.Navigation.NavigationEventArgs>(c_webBrowser_Navigated);
            c_webBrowser.Navigating += new EventHandler<NavigatingEventArgs>(c_webBrowser_Navigating);
        }


        /// <summary>
        /// In case of a back button start over with GettingStarted page always.
        /// </summary>
        /// <param name="e"></param>
        protected override void OnBackKeyPress(System.ComponentModel.CancelEventArgs e)
        {
            Uri pageUri = App.GettingStartedPage;
            Deployment.Current.Dispatcher.BeginInvoke(() =>
                {
                    NavigationService.Navigate(App.UserHomePage);
                });
            e.Cancel = true;  //Cancels the default behavior.
        }

        void c_webBrowser_Navigating(object sender, NavigatingEventArgs e)
        {
            c_CurrentUrl.Text = e.Uri.AbsoluteUri;
        }

        void c_webBrowser_Navigated(object sender, System.Windows.Navigation.NavigationEventArgs e)
        {
            if (e.Uri.OriginalString.Contains("target=AppAuthSuccess"))
            {
                Uri pageUri = new Uri("/MyMood.xaml", UriKind.RelativeOrAbsolute);

                Deployment.Current.Dispatcher.BeginInvoke(() =>
                {
                    NavigationService.Navigate(pageUri);

                });
            }
        }

        void HealthVaultWebPage_Loaded(object sender, RoutedEventArgs e)
        {
            // Handle application activation case
            if (App.HealthVaultShellUrl == null)
            {
                Deployment.Current.Dispatcher.BeginInvoke(() =>
                {
                    NavigationService.Navigate(App.UserHomePage);
                });
            }
            else
            {
                c_webBrowser.Navigate(new Uri(App.HealthVaultShellUrl));
            }
        }
    }
}