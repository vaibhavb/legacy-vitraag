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
using Microsoft.Phone.Controls;
using System.IO.IsolatedStorage;

using Microsoft.Phone.Net.NetworkInformation;
using Microsoft.Phone.Controls.Primitives;
using System.Collections.Generic;

namespace MoodTracker
{
    /// <summary>
    /// The class contains all the common code used by other pages, around
    /// 1. Moving or jumping to different pages.
    /// 2. 
    /// </summary>
    public class BasePage : PhoneApplicationPage
    {
        public void NavigateToUserHome()
        {
            Uri pageUri = new Uri("/MyMood.xaml", UriKind.RelativeOrAbsolute);
            NavigationService.Navigate(pageUri);
        }

        public void SetErrorMesasge(string message)
        {
            Dispatcher.BeginInvoke(() =>
            {
                MessageBox.Show(message);
            });
        }

        public void CheckNetwork()
        {
            if (!App.IsTrial)
            {
                if (!NetworkInterface.GetIsNetworkAvailable())
                {
                    MessageBoxResult result = MessageBox.Show("No Network Available",
                        "Hit OK to Quit application", MessageBoxButton.OK);
                    if (result == MessageBoxResult.OK)
                    {
                        App.Quit();
                    }
                }
            }
        }

        private void NavigateToSignUp()
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

            Uri pageUri = new Uri("/MyMood.xaml", UriKind.RelativeOrAbsolute);

            Deployment.Current.Dispatcher.BeginInvoke(() =>
            {
                NavigationService.Navigate(pageUri);
            });
        }

        public void PromptToSignUp()
        {
            Dispatcher.BeginInvoke(() =>
            {
                MessageBoxResult result = MessageBox.Show("Associate record with HealthVault to access this feature",
                    "Feature not available in trial mode", MessageBoxButton.OKCancel);
                if (result == MessageBoxResult.Cancel)
                {
                    NavigateToUserHome();
                }
                if (result == MessageBoxResult.OK)
                {
                    NavigateToSignUp();
                }
            });
        }

        public class ValueData : IComparable<ValueData>
        {
            public ValueData(string name, string image, int id)
            {
                Name = name;
                Image = image;
                ID = id;
            }

            public string Name
            {
                get;
                set;
            }

            public string Image
            {
                get;
                set;
            }
            public int ID
            {
                get;
                set;
            }

            public int CompareTo(ValueData other)
            {
                return this.ID.CompareTo(other.ID);
            }
        }

        // abstract the reusable code in a base class
        // this will allow us to concentrate on the specifics when implementing deriving looping data source classes
        public abstract class LoopingDataSourceBase : ILoopingSelectorDataSource
        {
            private object selectedItem;

            #region ILoopingSelectorDataSource Members

            public abstract object GetNext(object relativeTo);

            public abstract object GetPrevious(object relativeTo);

            public object SelectedItem
            {
                get
                {
                    return this.selectedItem;
                }
                set
                {
                    // this will use the Equals method if it is overridden for the data source item class
                    if (!object.Equals(this.selectedItem, value))
                    {
                        // save the previously selected item so that we can use it 
                        // to construct the event arguments for the SelectionChanged event
                        object previousSelectedItem = this.selectedItem;
                        this.selectedItem = value;
                        // fire the SelectionChanged event
                        this.OnSelectionChanged(previousSelectedItem, this.selectedItem);
                    }
                }
            }

            public event EventHandler<SelectionChangedEventArgs> SelectionChanged;

            protected virtual void OnSelectionChanged(object oldSelectedItem, object newSelectedItem)
            {
                EventHandler<SelectionChangedEventArgs> handler = this.SelectionChanged;
                if (handler != null)
                {
                    handler(this, new SelectionChangedEventArgs(new object[] { oldSelectedItem }, new object[] { newSelectedItem }));
                }
            }

            #endregion
        }

        public class ListLoopingDataSource<T> : LoopingDataSourceBase
        {
            private LinkedList<T> linkedList;
            private List<LinkedListNode<T>> sortedList;
            private IComparer<T> comparer;
            private NodeComparer nodeComparer;

            public ListLoopingDataSource()
            {
            }

            public IEnumerable<T> Items
            {
                get
                {
                    return this.linkedList;
                }
                set
                {
                    this.SetItemCollection(value);
                }
            }

            private void SetItemCollection(IEnumerable<T> collection)
            {
                this.linkedList = new LinkedList<T>(collection);

                this.sortedList = new List<LinkedListNode<T>>(this.linkedList.Count);
                // initialize the linked list with items from the collections
                LinkedListNode<T> currentNode = this.linkedList.First;
                while (currentNode != null)
                {
                    this.sortedList.Add(currentNode);
                    currentNode = currentNode.Next;
                }

                IComparer<T> comparer = this.comparer;
                if (comparer == null)
                {
                    // if no comparer is set use the default one if available
                    if (typeof(IComparable<T>).IsAssignableFrom(typeof(T)))
                    {
                        comparer = Comparer<T>.Default;
                    }
                    else
                    {
                        throw new InvalidOperationException("There is no default comparer for this type of item. You must set one.");
                    }
                }

                this.nodeComparer = new NodeComparer(comparer);
                this.sortedList.Sort(this.nodeComparer);
            }

            public IComparer<T> Comparer
            {
                get
                {
                    return this.comparer;
                }
                set
                {
                    this.comparer = value;
                }
            }

            public override object GetNext(object relativeTo)
            {
                // find the index of the node using binary search in the sorted list
                int index = this.sortedList.BinarySearch(new LinkedListNode<T>((T)relativeTo), this.nodeComparer);
                if (index < 0)
                {
                    return default(T);
                }

                // get the actual node from the linked list using the index
                LinkedListNode<T> node = this.sortedList[index].Next;
                if (node == null)
                {
                    // if there is no next node get the first one
                    node = this.linkedList.First;
                }
                return node.Value;
            }

            public override object GetPrevious(object relativeTo)
            {
                int index = this.sortedList.BinarySearch(new LinkedListNode<T>((T)relativeTo), this.nodeComparer);
                if (index < 0)
                {
                    return default(T);
                }
                LinkedListNode<T> node = this.sortedList[index].Previous;
                if (node == null)
                {
                    // if there is no previous node get the last one
                    node = this.linkedList.Last;
                }
                return node.Value;
            }

            private class NodeComparer : IComparer<LinkedListNode<T>>
            {
                private IComparer<T> comparer;

                public NodeComparer(IComparer<T> comparer)
                {
                    this.comparer = comparer;
                }

                #region IComparer<LinkedListNode<T>> Members

                public int Compare(LinkedListNode<T> x, LinkedListNode<T> y)
                {
                    return this.comparer.Compare(x.Value, y.Value);
                }

                #endregion
            }

        }
    }
}
