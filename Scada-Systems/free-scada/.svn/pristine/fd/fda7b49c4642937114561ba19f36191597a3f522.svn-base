using System;
using System.Collections.ObjectModel;
using System.Collections.Generic;
using System.Collections.Specialized;

namespace ScadaShell.Model
{
    
    public interface IRemoveCollectionChangedSubscriber
    {  
        bool RemoveCollectionChangedSubscriber(object subscriber);
        bool RemoveCollectionChangedSubscribersAll();
    }

    public class MyObservableCollection<T> : ObservableCollection<T>,
       IRemoveCollectionChangedSubscriber
    {
        public override event NotifyCollectionChangedEventHandler CollectionChanged;

        public MyObservableCollection()
            : base()
        {
        }
        public MyObservableCollection(List<T> list)
            :base(list)
        {
        }
        #region IRemoveCollectionChangedSubscriber Members

        public bool RemoveCollectionChangedSubscriber(object subscriber)
        {
            NotifyCollectionChangedEventHandler _event = CollectionChanged;
            if (_event == null)  // No subscriber
                return false;

            // Go through handler
            foreach (NotifyCollectionChangedEventHandler handler in _event.GetInvocationList())
            {
                if (object.ReferenceEquals(handler.Target, subscriber))
                {
                    CollectionChanged -= handler;
                    return true;
                }
            }

            return false;
        }

        

        public bool RemoveCollectionChangedSubscribersAll()
        {
            NotifyCollectionChangedEventHandler _event = CollectionChanged;
            if (_event == null)  // No subscriber
                return false;

            // Go through handler
            foreach (NotifyCollectionChangedEventHandler handler in _event.GetInvocationList())
            {
                CollectionChanged -= handler;
            }

            return true;
        }

        #endregion
        protected override void OnCollectionChanged(NotifyCollectionChangedEventArgs e)
        {
            if (CollectionChanged != null)
                CollectionChanged(this, e);

        }

    }
}