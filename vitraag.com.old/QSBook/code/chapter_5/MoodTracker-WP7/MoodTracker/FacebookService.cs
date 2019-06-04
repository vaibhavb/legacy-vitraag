using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;

namespace MoodTracker
{
    public class FacebookService
    {
        private static string AppID = "250137598332699";

        private static string FacebookAuthUrl = @"https://graph.facebook.com/oauth/authorize?client_id={0}"
                                        + @"&redirect_uri=http://www.facebook.com/connect/login_success.html"
                                        + @"&type=user_agent"
                                        + @"&display=touch"
                                        + @"&scope=publish_stream";

        private static string graphDialogUrl = @"https://www.facebook.com/dialog/feed?"
                                        + @"app_id={0}"
                                        + @"&link=http://www.vmudi.com"
                                        + @"&picture=http://www.vmudi.com/images/vmudi/vmudi_{1}.png"
                                        + @"&name=Get your Mood on!"
                                        + @"&caption=How am I feeling now?"
                                        + @"&description=My Mood Buzz is {2}."
                                        + @"&message="
                                        + @"&redirect_uri=http://www.facebook.com/connect/login_success_postdone.html";
                                        /*+ @"&display=touch";*/

        public string AccessToken { get; set; }

        public string GetGraphUrl(string pic)
        {
            return string.Format(graphDialogUrl, AppID, 
                pic, PicToDesc(pic));
        }

        private string PicToDesc(string pic)
        {
            string [] desc = pic.Split('_');
            if (desc.Length == 3)
            {
                string d = string.Format(
                    "{0}, stress is {1} and wellbeing is {2}", desc[0],
                    desc[1], desc[2]);
                return d;
            }
            return "";
        }


        public string LoginUrl
        {
            get
            {
                return string.Format(FacebookAuthUrl, AppID);
            }
        }

        public FacebookService()
        {

        }

        /*
         * Goal of this methods is to do GET/POST on Facebook graph, and let the response be handled
        public void Graph(string node, Dictionary<string, string> params, Delegate handleResponse)
        {
             
            
        }
        */
    }
}
