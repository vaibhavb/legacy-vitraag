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
using System.Xml;
using Microsoft.Phone.Controls;

using Microsoft.Health.Mobile;
using Microsoft.Phone.Tasks;
using System.Windows.Navigation;
using System.Xml.Linq;
using System.Windows.Media.Imaging;
using System.IO;

using Microsoft.Phone.Net.NetworkInformation;
using Microsoft.Phone.Shell;
using Microsoft.Phone.Controls.Primitives;

namespace MoodTracker
{

    /// <summary>
    /// This is the main page which displays to the user if 
    /// they go through the HealthVault authentication and authorization.
    /// DONE: Update the UI to have logos for buttons.
    /// DONE: Remove the individual images for mood, stress and well-being activities.
    /// DONE: Add support for livetile for logged in user
    /// BUG:  The slider interval needs to be reduced
    /// 
    /// </summary>
    public partial class MainPage : BasePage
    {
        bool _addingRecord = false;
        List<string> _currentThingIds = new List<string>();

        // Constructor
        public MainPage()
        {
            InitializeComponent();
            
            CheckNetwork();
 
            Loaded += new RoutedEventHandler(MainPage_Loaded);

            PopulateUI();
           
        }

        private void PopulateUI()
        {
            List<ValueData> mood = new List<ValueData>();
            List<ValueData> stress = new List<ValueData>();
            List<ValueData> wellbeing = new List<ValueData>();
            for (int i = 0; i <= 5; i++)
            {
                mood.Add(new ValueData(Enum.GetName(typeof(Mood), i),
                    string.Format("Images/values/{0}_SM.png", Enum.GetName(typeof(Mood), i)),
                    i));
                stress.Add(new ValueData(Enum.GetName(typeof(Stress), i),
                    string.Format("Images/values/{0}_SM.png", Enum.GetName(typeof(Stress), i)),
                    i));
                wellbeing.Add(new ValueData(Enum.GetName(typeof(Wellbeing), i),
                    string.Format("Images/values/{0}_SM.png", Enum.GetName(typeof(Wellbeing), i)),
                    i));
            }
            moodSliderSelector.DataSource = new ListLoopingDataSource<ValueData>() { Items = mood, SelectedItem = mood[4] };
            stressSliderSelector.DataSource = new ListLoopingDataSource<ValueData>() { Items = stress, SelectedItem = stress[1] };
            wellbeingSliderSelector.DataSource = new ListLoopingDataSource<ValueData>() { Items = wellbeing, SelectedItem = wellbeing[3] };
            RefreshImage();
        }

        void MainPage_Loaded(object sender, RoutedEventArgs e)
        {
            if (!App.IsTrial)
            {
                App.HealthVaultService.LoadSettings(App.SettingsFilename);

                // HACK: Kludgy way of handling the upgrade state machine
                if (NavigationContext != null && NavigationContext.QueryString.ContainsKey("m"))
                {
                    string val;
                    NavigationContext.QueryString.TryGetValue("m", out val);
                    if (!String.IsNullOrEmpty(val))
                    {
                        MessageBoxResult result = MessageBox.Show(val,
                            "Hit OK to continue", MessageBoxButton.OK);
                    }
                }
                App.HealthVaultService.BeginAuthenticationCheck(AuthenticationCompleted,
                    DoShellAuthentication);
                SetProgressBarVisibility(true);
            }
            else
            {
                SetRecordName("TRIAL MODE");
            }
            //c_MoodSlider.Value = 4;
            //c_StressSlider.Value = 1;
            //c_WellbeingSlider.Value = 3;
            this.DataContext = this;
            
        }

        void DoShellAuthentication(object sender, HealthVaultResponseEventArgs e)
        {
            SetProgressBarVisibility(false);

            App.HealthVaultService.SaveSettings(App.SettingsFilename);

            string url;

            if (_addingRecord)
            {
                url = App.HealthVaultService.GetUserAuthorizationUrl();
            }
            else
            {
                url = App.HealthVaultService.GetApplicationCreationUrl();
            }

            App.HealthVaultShellUrl = url;

            // If we are  using hosted browser via the hosted browser page
            Uri pageUri = new Uri("/HostedBrowser.xaml", UriKind.RelativeOrAbsolute);

            Deployment.Current.Dispatcher.BeginInvoke(() =>
            {
                NavigationService.Navigate(pageUri);
            });

        }

        void AuthenticationCompleted(object sender, HealthVaultResponseEventArgs e)
        {
            SetProgressBarVisibility(false);

            if (e != null && e.ErrorText != null)
            {
                SetErrorMesasge(e.ErrorText);
                return;
            }

            if (App.HealthVaultService.CurrentRecord == null)
            {
                App.HealthVaultService.CurrentRecord = App.HealthVaultService.Records[0];
            }

            App.HealthVaultService.SaveSettings(App.SettingsFilename);
            if (App.HealthVaultService.CurrentRecord != null)
            {
                SetRecordName(App.HealthVaultService.CurrentRecord.RecordName);
                // We are only interested in the last item
                HealthVaultMethods.GetThings(EmotionalStateModel.TypeId, 1, null, null, GetThingsCompleted);
                SetProgressBarVisibility(true);
            }

            // Check to see if we should upload all the reading from Trial
            if (TrialModeStorageProvider.Instance.TrialStage == TrialState.UpgradeData)
            {
                if (TrialModeStorageProvider.Instance.TrialMode.emotionalStates.Count > 0)
                {
                    Dispatcher.BeginInvoke(() =>
                    {
                        MessageBoxResult result = MessageBox.Show(string.Format("You have {0} emotional state item(s).",
                            TrialModeStorageProvider.Instance.TrialMode.emotionalStates.Count),
                            "Upload local items to HealthVault", MessageBoxButton.OKCancel);
                        if (result == MessageBoxResult.Cancel)
                        {
                            CancelUpgrade();
                        }
                        if (result == MessageBoxResult.OK)
                        {
                            if (TrialModeStorageProvider.Instance.TrialMode.emotionalStates.Count > 0)
                            {
                                HealthVaultMethods.PutThings(TrialModeStorageProvider.Instance.TrialMode.emotionalStates,
                                    PutThingsDoUpgradeCompleted);
                                SetProgressBarVisibility(true);
                            }
                        }
                    });
                }
                else
                {
                    EnjoyHealthVault();
                }
            }
        }

        void PutThingsDoUpgradeCompleted(object sender, HealthVaultResponseEventArgs e)
        {
            SetProgressBarVisibility(false);
            if (e.ErrorText != null)
            {
                SetErrorMesasge(e.ErrorText);
            }
            else
            {
                SetUserToast("Reading(s) successfully moved to HealthVault, enjoy!");
                TrialModeStorageProvider.Instance.DeleteStore(); 
            } 
        }

        void EnjoyHealthVault()
        {
            SetUserToast("Successfully connected to HealthVault, enjoy!");
            TrialModeStorageProvider.Instance.DeleteStore(); 
        }

        void CancelUpgrade()
        {
            TrialModeStorageProvider.Instance.DeleteStore();
        }

        void SetRecordName(string recordName)
        {
            Dispatcher.BeginInvoke(() =>
            {
                c_RecordName.Text = recordName;
            });
        }

        void SetProgressBarVisibility(bool visible)
        {
            Dispatcher.BeginInvoke(() =>
            {
                c_progressBar.Visibility = visible ? Visibility.Visible : Visibility.Collapsed;
            });
        }

        void SetUserToast(string message)
        {
            SetErrorMesasge(message);
        }

        void GetThingsCompleted(object sender, HealthVaultResponseEventArgs e)
        {
            SetProgressBarVisibility(false);

            if (e.ErrorText == null)
            {
                XElement responseNode = XElement.Parse(e.ResponseXml);
                // using linq to get the latest reading of emotional state
                XElement latestEmotion = (from thingNode in responseNode.Descendants("thing")
                                          orderby Convert.ToDateTime(thingNode.Element("eff-date").Value) descending
                                          select thingNode).FirstOrDefault<XElement>();

                if (latestEmotion != null)
                {
                    EmotionalStateModel emotionalState =
                        new EmotionalStateModel();
                    emotionalState.Parse(latestEmotion);

                    Deployment.Current.Dispatcher.BeginInvoke(() =>
                    {
                        c_LastUpdated.Text = string.Format("Last Update - {0}", emotionalState.When.ToString("MMM dd, yyyy"));
                        
                        //c_MoodSlider.Value = (double)emotionalState.Mood;
                        //c_StressSlider.Value = (double)emotionalState.Stress;
                        //c_WellbeingSlider.Value = (double)emotionalState.Wellbeing;
                        this.DataContext = this;
                    });
                }
            }
        }

        // Save the readings to HealthVault
        private void button1_Click(object sender, RoutedEventArgs e)
        {
            EmotionalStateModel model = new EmotionalStateModel();
            model.Mood = (Mood)(moodSliderSelector.DataSource.SelectedItem as ValueData).ID;
            model.Stress = (Stress) (stressSliderSelector.DataSource.SelectedItem as ValueData).ID;
            model.Wellbeing = (Wellbeing) (wellbeingSliderSelector.DataSource.SelectedItem as ValueData).ID;
            model.Note = Txt_Note.Text;
            model.When = DateTime.Now;
            if (!App.IsTrial)
            {
                HealthVaultMethods.PutThings(model, PutThingsCompleted);
                SetProgressBarVisibility(true);
            }
            else
            {
                SetProgressBarVisibility(true);
                TrialModeStorageProvider.Instance.TrialMode.emotionalStates.Add(model);
                TrialModeStorageProvider.Instance.Save();
                SetProgressBarVisibility(false);
                SetUserToast("Mood successfully saved on the device!");
            }

        }

        void PutThingsCompleted(object sender, HealthVaultResponseEventArgs e)
        {
            SetProgressBarVisibility(false);
            if (e.ErrorText != null)
            {
                SetErrorMesasge(e.ErrorText);
            }
            else
            {
                SetUserToast("Mood successfully saved!");
            }
        }

        /*
        public string GetSliderValue(Type t, Slider slider)
        {
            return System.Enum.GetName(
                t, (int)slider.Value);
        }
        private void c_MoodSlider_ValueChanged(object sender, 
			System.Windows.RoutedPropertyChangedEventArgs<double> e)
        {
            Dispatcher.BeginInvoke(() =>
                {
                    string value = GetSliderValue(typeof(Mood), c_MoodSlider);
                    MoodSliderValue.Text = value;
                    //c_MoodSlider_Image.Source = new BitmapImage(new Uri(
                    //    string.Format("Images/values/{0}_SM.png", value.ToLower()), UriKind.Relative));
                    c_vmudi_mood.Source = new BitmapImage( new Uri(
                        string.Format("Images/vmudi/vmudi_{0}.png", value.ToLower()), UriKind.Relative));
                });
        }

        private void c_WellbeingSlider_ValueChanged(object sender, System.Windows.RoutedPropertyChangedEventArgs<double> e)
        {
            Dispatcher.BeginInvoke(() =>
            {
                string value = GetSliderValue(typeof(Wellbeing), c_WellbeingSlider);
                WellbeingSliderValue.Text = value;
                //c_WellbeingSlider_Image.Source = new BitmapImage(new Uri(
                //    string.Format("Images/values/{0}_SM.png", value.ToLower()), UriKind.Relative));
                c_vmudi_wellbeing.Source = new BitmapImage(new Uri(
                        string.Format("Images/vmudi/vmudi_{0}.png", value.ToLower()), UriKind.Relative));
            });
        }

        private void c_StressSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            Dispatcher.BeginInvoke(() =>
            {
                string value = GetSliderValue(typeof(Stress), c_StressSlider);
                StressSliderValue.Text = value;
                //c_StressSlider_Image.Source = new BitmapImage(new Uri(
                //    string.Format("Images/values/{0}_SM.png", value.ToLower()), UriKind.Relative));
                c_vmudi_stress.Source = new BitmapImage(new Uri(
                        string.Format("Images/vmudi/vmudi_{0}.png", value.ToLower()), UriKind.Relative));
            });
        }
        */
        private void Button_Click(object sender, RoutedEventArgs e)
        {
            Uri pageUri = new Uri("/MyHistory.xaml", UriKind.RelativeOrAbsolute);
            NavigationService.Navigate(pageUri);
        }

        private void Button_Click_Plant(object sender, RoutedEventArgs e)
        {
            Uri pageUri = new Uri("/MyMoodPlant.xaml", UriKind.RelativeOrAbsolute);
            NavigationService.Navigate(pageUri);
        }

        private string GetCurrentPicString()
        {
            return (string.Format("{0}_{1}_{2}",
                (moodSliderSelector.DataSource.SelectedItem as ValueData).Name,
                (stressSliderSelector.DataSource.SelectedItem as ValueData).Name,
                (wellbeingSliderSelector.DataSource.SelectedItem as ValueData).Name));
        }

        private string GetNote()
        {
            if (!string.IsNullOrEmpty(Txt_Note.Text))
            {
                return Txt_Note.Text;
            }
            else return "";
        }

        private void Button_Click_Facebook(object sender, RoutedEventArgs e)
        {
            Uri pageUri = new Uri(String.Format("/FacebookLoginPage.xaml?p={0}&n={1}", 
                GetCurrentPicString(),
                GetNote()),
                UriKind.RelativeOrAbsolute);
            
            Deployment.Current.Dispatcher.BeginInvoke(() =>
            {
                NavigationService.Navigate(pageUri);
            });
        }

        private void moodSliderSelector_ManipulationCompleted(object sender, ManipulationCompletedEventArgs e)
        {
            Dispatcher.BeginInvoke(() =>
            {
                string value = (moodSliderSelector.DataSource.SelectedItem as ValueData).Name;
                c_vmudi_mood.Source = new BitmapImage(new Uri(
                    string.Format("Images/vmudi/vmudi_{0}.png", value.ToLower()), UriKind.Relative));
                c_vmudi_mood.UpdateLayout();
            });
        }

        private void stressSliderSelector_ManipulationCompleted(object sender, ManipulationCompletedEventArgs e)
        {
            Dispatcher.BeginInvoke(() =>
            {
                string value = (stressSliderSelector.DataSource.SelectedItem as ValueData).Name;
                c_vmudi_stress.Source = new BitmapImage(new Uri(
                        string.Format("Images/vmudi/vmudi_{0}.png", value.ToLower()), UriKind.Relative));
            });
        }

        private void wellbeingSliderSelector_ManipulationCompleted(object sender, ManipulationCompletedEventArgs e)
        {
            Dispatcher.BeginInvoke(() =>
            {
                string value = (wellbeingSliderSelector.DataSource.SelectedItem as ValueData).Name;
                c_vmudi_wellbeing.Source = new BitmapImage(new Uri(
                        string.Format("Images/vmudi/vmudi_{0}.png", value.ToLower()), UriKind.Relative));
            });
        }

        private void RefreshImage()
        {
            Dispatcher.BeginInvoke(() =>
            {
                c_vmudi_mood.Source = new BitmapImage(new Uri(
                   string.Format("Images/vmudi/vmudi_{0}.png",
                   (moodSliderSelector.DataSource.SelectedItem as ValueData).Name), UriKind.Relative));
                c_vmudi_stress.Source = new BitmapImage(new Uri(
                       string.Format("Images/vmudi/vmudi_{0}.png", 
                       (stressSliderSelector.DataSource.SelectedItem as ValueData).Name.ToLower()), UriKind.Relative));
                c_vmudi_wellbeing.Source = new BitmapImage(new Uri(
                        string.Format("Images/vmudi/vmudi_{0}.png", 
                        (wellbeingSliderSelector.DataSource.SelectedItem as ValueData).Name.ToLower()), UriKind.Relative));
            });
        }


        #region handlebackbutton
        /// <summary>
        /// In case of a back button start over with GettingStarted page always.
        /// </summary>
        /// <param name="e"></param>
        protected override void OnBackKeyPress(System.ComponentModel.CancelEventArgs e)
        {
            App.Quit();
        }

        #endregion
    }
}