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
using System.IO.IsolatedStorage;

namespace MoodTracker
{
    public partial class Page1 : BasePage 
    {
        /// <summary>
        /// DONE: Add a trial button to the application
        /// BUG: Remove network call from here and add it only if not Trial.
        /// </summary>
        public Page1()
        {
            InitializeComponent();
            textBlock1.Text = @"This application requires a connection to the Microsoft " +
                              @"HealthVault service to store emotional state. You will need " +
                              @"to sign up for a free HealthVault account and authorize this " +
                              @"application to access Emotional State in your HealthVault " +
                              @"record. Press Continue to begin the sign-up process," +
                              @"or press Trial button to try a limited set of readings without signing up for HealthVault.";
            button1.Click += new RoutedEventHandler(authenticateHealthVault);
            button2.Click += new RoutedEventHandler(button2_Click);
        }

        void button2_Click(object sender, RoutedEventArgs e)
        {
            TrialModeStorageProvider.Instance.TrialMode.Stage = TrialState.Trial;
            TrialModeStorageProvider.Instance.Save();

            Deployment.Current.Dispatcher.BeginInvoke(() =>
                {
                    NavigationService.Navigate(App.UserHomePage);
                });
        }


        private void PhoneApplicationPage_Loaded(object sender, RoutedEventArgs e)
        {
            App.HealthVaultService.LoadSettings(App.SettingsFilename);
            App.SetLiveTileSchedule();

            if (App.HealthVaultService.CurrentRecord != null || App.IsTrial)
            {
                Deployment.Current.Dispatcher.BeginInvoke(() =>
                {
                    NavigationService.Navigate(App.UserHomePage);
                });
            }          
        }

        void authenticateHealthVault(object sender, RoutedEventArgs e)
        {
            try
            {
                IsolatedStorageFile.GetUserStoreForApplication().DeleteFile(App.SettingsFilename);
                if (TrialModeStorageProvider.Instance.TrialStage == TrialState.Trial)
                {
                    TrialModeStorageProvider.Instance.TrialMode.Stage = TrialState.UpgradeData;
                    TrialModeStorageProvider.Instance.Save();
                }
            }
            catch (Exception) { };

            Deployment.Current.Dispatcher.BeginInvoke(() =>
            {
                NavigationService.Navigate(App.UserHomePage);
            });
        }
    }
}