﻿using System;
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

using Facebook;
using Microsoft.Phone.Net.NetworkInformation;
using Microsoft.Phone.Shell;
using Microsoft.Phone.Controls;

namespace MoodTracker
{
    public partial class FacebookLoginPage : PhoneApplicationPage
    {
        private const string AppId = "250137598332699";
        private const string AppSecret = "0a6b4177b9730023a1655e18585a8560";
        private string url;
        private string note;

        /// <summary>
        /// Extended permissions is a comma separated list of permissions to ask the user.
        /// </summary>
        /// <remarks>
        /// For extensive list of available extended permissions refer to 
        /// https://developers.facebook.com/docs/reference/api/permissions/
        /// </remarks>
        private const string ExtendedPermissions = "user_about_me,publish_stream,offline_access";

        private readonly Uri _loginUrl;

        public FacebookLoginPage()
        {
            // NOTE: make sure to enable scripting for the web browser control.
            // <phone:WebBrowser x:Name="webBrowser1" IsScriptEnabled="True" />
            if (!NetworkInterface.GetIsNetworkAvailable())
            {
                MessageBoxResult result = MessageBox.Show("No Network Available",
                    "Hit OK to Quit application", MessageBoxButton.OK);
                if (result == MessageBoxResult.OK)
                {
                    App.Quit();
                }
            }

            // Make sure to set the app id.
            var oauthClient = new FacebookOAuthClient { AppId = AppId };

            var loginParameters = new Dictionary<string, object>();

            // The requested response: an access token (token), an authorization code (code), or both (code token).
            // note: there is a bug in wpf browser control which ignores the fragment part (#) of the url
            // so we cannot get the access token. To fix this, set response_type to code as code is set in
            // the querystring.
            loginParameters["response_type"] = "code";

            // add the 'scope' parameter only if we have extendedPermissions.
            if (!string.IsNullOrEmpty(ExtendedPermissions))
            {
                // A comma-delimited list of permissions
                loginParameters["scope"] = ExtendedPermissions;
            }

            // when the Form is loaded navigate to the login url.
            _loginUrl = oauthClient.GetLoginUrl(loginParameters);

            InitializeComponent();
        }

        private void PhoneApplicationPage_Loaded(object sender, RoutedEventArgs e)
        {
            url = NavigationContext.QueryString["p"];
            note = NavigationContext.QueryString["n"];
            c_webBrowser.Navigate(_loginUrl);
        }

        void c_webBrowser_Navigating(object sender, NavigatingEventArgs e)
        {
            c_CurrentUrl.Text = e.Uri.AbsoluteUri;
        }

        private void c_webBrowser_Navigated(object sender, System.Windows.Navigation.NavigationEventArgs e)
        {
            c_CurrentUrl.Text = e.Uri.AbsoluteUri;
            // whenever the browser navigates to a new url, try parsing the url
            // the url may be the result of OAuth 2.0 authentication.
            FacebookOAuthResult oauthResult;

            if (FacebookOAuthResult.TryParse(e.Uri, out oauthResult))
            {
                // The url is the result of OAuth 2.0 authentication.
                if (oauthResult.IsSuccess)
                {
                    var oauthClient = new FacebookOAuthClient { AppId = AppId, AppSecret = AppSecret };

                    // we got the code here
                    var code = oauthResult.Code;
                    oauthClient.ExchangeCodeForAccessTokenCompleted +=
                        (o, args) =>
                        {
                            // make sure to check that no error has occurred.
                            if (args.Error != null)
                            {
                                // make sure to access ui stuffs on the correct thread.
                                Dispatcher.BeginInvoke(
                                    () =>
                                    {
                                        MessageBox.Show(args.Error.Message);
                                        NavigationService.Navigate(new Uri("/MainPage.xaml", UriKind.Relative));
                                    });
                            }
                            else
                            {
                                var result = (IDictionary<string, object>)args.GetResultData();
                                var accessToken = (string)result["access_token"];

                                // make sure to access ui stuffs on the correct thread.
                                Dispatcher.BeginInvoke(() => NavigationService.Navigate(
                                    new Uri(string.Format("/FacebookInfoPage.xaml?access_token={0}&n={1}&p={2}",                                                                                     
                                            accessToken,
                                            note,
                                            url
                                            ), 
                                        UriKind.Relative)));
                            }
                        };

                    oauthClient.ExchangeCodeForAccessTokenAsync(code);
                }
                else
                {
                    // the user clicked don't allow or some other error occurred.
                    MessageBox.Show(oauthResult.ErrorDescription);
                }
            }
            else
            {
                // The url is NOT the result of OAuth 2.0 authentication.
            }
        }
    }

    //public partial class FacebookLoginPage : PhoneApplicationPage
    //{
    //    public FacebookLoginPage()
    //    {
    //        InitializeComponent();
    //        c_webBrowser.IsScriptEnabled = true;

    //        Loaded += new RoutedEventHandler(FacebookWebPage_Loaded);
    //        c_webBrowser.Navigated += new EventHandler<System.Windows.Navigation.NavigationEventArgs>(c_webBrowser_Navigated);
    //        c_webBrowser.Navigating += new EventHandler<NavigatingEventArgs>(c_webBrowser_Navigating);
    //    }
    
    //    void c_webBrowser_Navigating(object sender, NavigatingEventArgs e)
    //    {
    //        c_CurrentUrl.Text = e.Uri.AbsoluteUri;
    //    }

    //    void c_webBrowser_Navigated(object sender, System.Windows.Navigation.NavigationEventArgs e)
    //    {
    //        if (e.Uri.OriginalString.Contains("login_success.html"))
    //        {
    //            if (e.Uri.OriginalString.Contains("error"))
    //            {
    //                /*
    //                Deployment.Current.Dispatcher.BeginInvoke(() =>
    //                    {
    //                        textBlockError.Text = e.Uri.OriginalString;
    //                        textBlockError.TextWrapping = TextWrapping.Wrap;
    //                        textBlockError.Visibility = System.Windows.Visibility.Visible;
    //                        c_webBrowser.Visibility = System.Windows.Visibility.Collapsed;
    //                    }
    //                );*/
    //            }
    //            /* Get teh access token and do the wall post */
    //            else if (e.Uri.OriginalString.Contains("access_token"))
    //            {
    //                Dictionary<string, string> QueryParams = ParseQueryParams(e.Uri);
    //                Uri pageUri = new Uri(string.Format("/FacebookWallPost.xaml?access_token={0}&p={1}", 
    //                    QueryParams["#access_token"], NavigationContext.QueryString["p"])
    //                    , UriKind.RelativeOrAbsolute);

    //                Deployment.Current.Dispatcher.BeginInvoke(() =>
    //                {
    //                    NavigationService.Navigate(pageUri);
    //                });
    //            }

    //        }
    //    }

    //    void FacebookWebPage_Loaded(object sender, RoutedEventArgs e)
    //    {
    //        string url = App.FacebookService.LoginUrl;

    //        c_webBrowser.Navigate(new Uri(url));
    //    }

    //    Dictionary<string, string> ParseQueryParams(Uri uri)
    //    {
    //        Dictionary<string, string> queryParams = new Dictionary<string, string>();
    //        string query = uri.Fragment;
    //        foreach (string keyValue in query.Split('&'))
    //        {
    //            string[] pair = keyValue.Split('=');
    //            if ((pair != null) && (pair.Length == 2))
    //            {
    //                queryParams[pair[0]] = pair[1];
    //            }
    //        }

    //        return queryParams;
    //    }
    //}
}