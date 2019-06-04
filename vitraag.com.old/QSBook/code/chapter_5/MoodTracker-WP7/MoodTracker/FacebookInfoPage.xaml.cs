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
using System.Windows.Media.Imaging;

using Facebook;
using Microsoft.Phone.Controls;

namespace MoodTracker
{
    public partial class FacebookInfoPage : PhoneApplicationPage
    {
        private string _accessToken;
        private string _text;
        private string _url;

        private IDictionary<string, object> _me;

        public FacebookInfoPage()
        {
            InitializeComponent();
        }

        protected override void OnNavigatedTo(System.Windows.Navigation.NavigationEventArgs e)
        {
            base.OnNavigatedTo(e);
            _accessToken = NavigationContext.QueryString["access_token"];
            _url = GetUrl(NavigationContext.QueryString["p"]);
            _text = NavigationContext.QueryString["n"];
            txtMessage.Text = _text;
            LoadMoodPicture();
        }

        private void PhoneApplicationPage_Loaded(object sender, RoutedEventArgs e)
        {
            var fb = new FacebookClient(_accessToken);

            fb.GetCompleted += (o, args) =>
                                   {
                                       if (args.Error == null)
                                       {
                                           _me = (IDictionary<string, object>)args.GetResultData();

                                           Dispatcher.BeginInvoke(
                                               () =>
                                               {
                                                   LoadProfilePicture();
                                                   ProfileName.Text = "Hi " + _me["name"];
                                                   FirstName.Text = "First Name: " + _me["first_name"];
                                                   LastName.Text = "Last Name: " + _me["last_name"];
                                               });
                                       }
                                       else
                                       {
                                           Dispatcher.BeginInvoke(() => MessageBox.Show(args.Error.Message));
                                       }
                                   };

            // do a GetAsync me in order to get basic details of the user.
            fb.GetAsync("me");

        }


        private void LoadMoodPicture()
        {
            moodPic.Source = new BitmapImage(new Uri(_url));
        }

        private void LoadProfilePicture()
        {
            picProfile.Source = new BitmapImage(new Uri(string.Format("https://graph.facebook.com/{0}/picture", _me["id"])));
        }

        private void PostToWall_Click(object sender, RoutedEventArgs e)
        {
            if (string.IsNullOrEmpty(txtMessage.Text))
            {
                MessageBox.Show("Enter message.");
                return;
            }

            var fb = new FacebookClient(_accessToken);

            // make sure to add event handler for PostCompleted.
            fb.PostCompleted += (o, args) =>
            {
                // incase you support cancellation, make sure to check
                // e.Cancelled property first even before checking (e.Error!=null).
                if (args.Cancelled)
                {
                    // for this example, we can ignore as we don't allow this
                    // example to be cancelled.

                    // you can check e.Error for reasons behind the cancellation.
                    var cancellationError = args.Error;
                }
                else if (args.Error != null)
                {
                    // error occurred
                    Dispatcher.BeginInvoke(() =>
                                               {
                                                   MessageBox.Show(args.Error.Message);
                                               });
                }
                else
                {
                    // the request was completed successfully

                    // now we can either cast it to IDictionary<string, object> or IList<object>
                    var result = (IDictionary<string, object>)args.GetResultData();

                    // make sure to be on the right thread when working with ui.
                    Dispatcher.BeginInvoke(() =>
                                               {
                                                   MessageBox.Show("Message Posted successfully");
                                                   txtMessage.Text = string.Empty;
                                               });
                }
            };

            var parameters = new Dictionary<string, object>();
            parameters["message"] = txtMessage.Text;
            parameters["link"] = "http://www.vMudi.com";
            parameters["name"] = "vMudi Mood Tracker";
            parameters["description"] = "Mood tracking just got simple!";
            parameters["picture"] = _url;
            fb.PostAsync("me/feed", parameters);
        }


        private string GetUrl(string url)
        {
            return (string.Format("http://www.vmudi.com/images/vmudi/vmudi_{0}.png", url));
        }

        private void ApplicationBarMenuItem_Click(object sender, EventArgs e)
        {
            Uri pageUri = new Uri("/MyMood.xaml", UriKind.RelativeOrAbsolute);
            NavigationService.Navigate(pageUri);
        }

        private void ApplicationBarMenuItem_Click_1(object sender, EventArgs e)
        {
            Uri pageUri = new Uri("/MyHistory.xaml", UriKind.RelativeOrAbsolute);
            NavigationService.Navigate(pageUri);
        }

        private void ApplicationBarMenuItem_Click_2(object sender, EventArgs e)
        {
            Uri pageUri = new Uri("/MyMoodPlant.xaml", UriKind.RelativeOrAbsolute);
            NavigationService.Navigate(pageUri);
        }
    }
}