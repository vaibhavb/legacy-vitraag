using System;
using System.Net;
using System.Xml.Linq;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.Serialization;
using System.Xml;
using System.Diagnostics;
using System.IO;

using System.IO.IsolatedStorage;
using Microsoft.Phone.Info;
using System.Text;

namespace MoodTracker
{
    /// <summary>
    /// This singleton class provides local storage for the application to enable
    /// 1. Trial mode
    /// 
    /// The file based storage should provide
    /// 1. A bit for what mode we are running under
    /// 2. Number for max number of reading Max-Items
    /// 3. Store Emotional state values for Max-Items
    /// 4. Each emotional state can have -- 
    ///    a. Date Time
    ///    b. Mood
    ///    c. Stress
    ///    d. Wellbeing
    ///    e. Note
    /// </summary>
    public class TrialModeStorageProvider
    {
        private TrialMode _trialMode = null;
        //private IsolatedStorageSettings _store;
        private IsolatedStorageFile _store;

        public TrialMode TrialMode
        {
            get
            {
                if (_trialMode == null && 
                    IsolatedStorageFile.GetUserStoreForApplication().FileExists(_fileName))
                {
                    this.Load();
                }
                else if (_trialMode == null)
                {
                    _trialMode = new TrialMode();
                }
                return _trialMode;
            }
        }
        
        private string _fileName = "TrialMode.xml";

        private static readonly TrialModeStorageProvider instance = new TrialModeStorageProvider();

        private TrialModeStorageProvider()
        {
            //_store = IsolatedStorageSettings.ApplicationSettings;
            _store = IsolatedStorageFile.GetUserStoreForApplication();
        }

        public static TrialModeStorageProvider Instance
        {
            get
            {
                return instance;
            }
        }

        /// <summary>
        /// A bit crufty logic to determine if we are in trial mode
        /// </summary>
        public TrialState TrialStage
        {
            get
            {
                if (TrialModeStorageProvider.Instance.TrialMode == null)
                {
                    return TrialState.None;
                }
                return TrialModeStorageProvider.Instance.TrialMode.Stage;
            }
        }

        public void Save()
        {
            Debug.WriteLine("Creating saving the trial settings file.");
            IsolatedStorageFileStream writer = null;
            string s;
            using (writer = _store.OpenFile(_fileName, FileMode.OpenOrCreate))
            {
                DataContractSerializer ser =
                    new DataContractSerializer(typeof(TrialMode));
                s = ser.ToString();
                ser.WriteObject(writer, _trialMode);
            }
            Debug.WriteLine(String.Format("Wrote: {0}", s.ToString()));
        }

        public void Load()
        {
            Debug.WriteLine("Deserializing an instance of the object.");
            //if (_store.Contains(_fileName))
            //{
                
            //    using (MemoryStream ms =
            //        new MemoryStream(Encoding.UTF8.GetBytes( _store[_fileName].ToString() )))
            //    {
            //        DataContractSerializer ser = new DataContractSerializer(typeof(TrialMode));
            //        _trialMode = (TrialMode) ser.ReadObject(ms);
            //    }
            //}

            if (_store.FileExists(_fileName))
            {
                using (IsolatedStorageFileStream fs =
                    new IsolatedStorageFileStream(_fileName, FileMode.Open, _store))
                {
                    DataContractSerializer ser = new DataContractSerializer(typeof(TrialMode));
                    // Deserialize the data and read it from the instance.
                    _trialMode = (TrialMode)ser.ReadObject(fs);
                }
                Debug.WriteLine(String.Format("Trial: {0}", _trialMode.Stage));
            }          
        }

        public void DeleteStore()
        {
            try
            {
                //_store.Remove(_fileName);
                _store.DeleteFile(_fileName);
                _trialMode = null;
            }
            catch (IsolatedStorageException e)
            {
                Debug.WriteLine(e.Message);
            }
        }
    }

    [DataContract(Name = "TrialMode")]
    public class TrialMode
    {
        [DataMember]
        public TrialState Stage = TrialState.None;

        [DataMember]
        public List<EmotionalStateModel> emotionalStates = new List<EmotionalStateModel>();

        public TrialMode()
        { 
           
        }
    }

    [DataContract(Name="TrialState")]
    public enum TrialState
    {
        [EnumMember]
        None = 0,
        [EnumMember]
        Trial = 1,
        [EnumMember]
        UpgradeData = 2,
    }

}
