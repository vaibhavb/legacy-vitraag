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
using System.Windows.Navigation;
using System.Windows.Shapes;
using Microsoft.Phone.Controls;
using Microsoft.Phone.Shell;

using Microsoft.Health.Mobile;

namespace MoodTracker
{
    /// <summary>
    /// DONE: Add live tile support
    /// TODO: Add a setting button to choose between various HealthVault environments.
    /// </summary>
    public partial class App : Application
    {
        public static bool IsTrial
        {
            get
            {
                if (TrialModeStorageProvider.Instance.TrialStage == TrialState.Trial)
                {
                    return true;
                }
                return false;
            }
        }

        
        public static HealthVaultService HealthVaultService { get; set; }
        public static string HealthVaultShellUrl { get; set; }
        public static FacebookService FacebookService { get; set; }
        public static Uri GettingStartedPage = new Uri("/MainPage.xaml", UriKind.Relative);
        public static Uri UserHomePage = new Uri("/MyMood.xaml", UriKind.Relative);
        public static Uri HostedBrowserPage = new Uri("/HostedBrowser.xaml", UriKind.Relative);

        static string platformUrl = @"https://platform.healthvault-ppe.com/platform/wildcat.ashx";
        static string shellUrl = @"https://account.healthvault-ppe.com";
        static string masterAppId = "83bf507d-9186-407f-a6cd-b2d65f558690";
        //private static ShellTileSchedule _sampleTileSchedule;

        public static void SetLiveTileSchedule()
        {
            /* Disabling the live tile functionality for now */
            /*
            if (_sampleTileSchedule == null)
            {
                _sampleTileSchedule = new ShellTileSchedule();

                // Update will happen one time.
                _sampleTileSchedule.Recurrence = UpdateRecurrence.Onetime;

                // Start the update schedule now, but because updates are batched, the update may not happen for an hour or so.
                _sampleTileSchedule.StartTime = DateTime.Now;

                _sampleTileSchedule.RemoteImageUri = new Uri(@"http://www.vmudi.com/images/vmudi/vmudi_happy.png");
                _sampleTileSchedule.Start();
            }
             */
        }

        public static String SettingsFilename
        {
            get
            {
                // Stores information for HealthVault Connection
                return "Settings.xml";
            }
        }

        // Code to execute when the application is launching (eg, from Start)
        // This code will not execute when the application is reactivated
        private void Application_Launching(object sender, LaunchingEventArgs e)
        {
            HealthVaultService = new HealthVaultService(platformUrl, shellUrl, new Guid(masterAppId));
            FacebookService = new FacebookService();
        }

        // Code to execute when the application is activated (brought to foreground)
        // This code will not execute when the application is first launched
        private void Application_Activated(object sender, ActivatedEventArgs e)
        {
            TrialModeStorageProvider.Instance.Load();
            if (!App.IsTrial)
            {
                HealthVaultService = new HealthVaultService(platformUrl, shellUrl, new Guid(masterAppId));
                FacebookService = new FacebookService();
            }
            
        }

        /// <summary>
        /// Provides easy access to the root frame of the Phone Application.
        /// </summary>
        /// <returns>The root frame of the Phone Application.</returns>
        public PhoneApplicationFrame RootFrame { get; private set; }

        /// <summary>
        /// Constructor for the Application object.
        /// </summary>
        public App()
        {
            // Global handler for uncaught exceptions. 
            UnhandledException += Application_UnhandledException;

            // Show graphics profiling information while debugging.
            if (System.Diagnostics.Debugger.IsAttached)
            {
                // Display the current frame rate counters.
                Application.Current.Host.Settings.EnableFrameRateCounter = true;

                // Show the areas of the app that are being redrawn in each frame.
                //Application.Current.Host.Settings.EnableRedrawRegions = true;

                // Enable non-production analysis visualization mode, 
                // which shows areas of a page that are being GPU accelerated with a colored overlay.
                //Application.Current.Host.Settings.EnableCacheVisualization = true;
            }

            // Standard Silverlight initialization
            InitializeComponent();

            // Phone-specific initialization
            InitializePhoneApplication();
        }

        // Code to execute when the application is deactivated (sent to background)
        // This code will not execute when the application is closing
        private void Application_Deactivated(object sender, DeactivatedEventArgs e)
        {
            if (App.IsTrial)
            {
                TrialModeStorageProvider.Instance.Save();
            }
        }

        // Code to execute when the application is closing (eg, user hit Back)
        // This code will not execute when the application is deactivated
        private void Application_Closing(object sender, ClosingEventArgs e)
        {
            if (App.IsTrial)
            {
                TrialModeStorageProvider.Instance.Save();
            }

        }

        // Code to execute if a navigation fails
        private void RootFrame_NavigationFailed(object sender, NavigationFailedEventArgs e)
        {
            if (System.Diagnostics.Debugger.IsAttached)
            {
                // A navigation has failed; break into the debugger
                System.Diagnostics.Debugger.Break();
            }
        }

        private class QuitException : Exception { }

        public static void Quit()
        {
            throw new QuitException();
        }

        // Code to execute on Unhandled Exceptions
        private void Application_UnhandledException(object sender, ApplicationUnhandledExceptionEventArgs e)
        {
            if (e.ExceptionObject is QuitException)
                return;

            if (System.Diagnostics.Debugger.IsAttached)
            {
                // An unhandled exception has occurred; break into the debugger
                System.Diagnostics.Debugger.Break();
            }
        }

        #region Phone application initialization

        // Avoid double-initialization
        private bool phoneApplicationInitialized = false;

        // Do not add any additional code to this method
        private void InitializePhoneApplication()
        {
            if (phoneApplicationInitialized)
                return;

            // Create the frame but don't set it as RootVisual yet; this allows the splash
            // screen to remain active until the application is ready to render.
            RootFrame = new PhoneApplicationFrame();
            RootFrame.Navigated += CompleteInitializePhoneApplication;

            // Handle navigation failures
            RootFrame.NavigationFailed += RootFrame_NavigationFailed;

            // Ensure we don't initialize again
            phoneApplicationInitialized = true;
        }

        // Do not add any additional code to this method
        private void CompleteInitializePhoneApplication(object sender, NavigationEventArgs e)
        {
            // Set the root visual to allow the application to render
            if (RootVisual != RootFrame)
                RootVisual = RootFrame;

            // Remove this handler since it is no longer needed
            RootFrame.Navigated -= CompleteInitializePhoneApplication;
        }

        #endregion
    }










}