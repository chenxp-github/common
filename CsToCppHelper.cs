using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CsToCpp
{
    /// <summary>
    /// MapEntry
    /// </summary>
    class MapEntry
    {
        private Object mObject;

        public MapEntry()
        {
            this.clear();
        }

        public MapEntry(Object o)
        {
            this.set(o);
        }

        public void clear()
        {
            mObject = null;
        }

        public void set(Object o)
        {
            mObject = o;
        }

        public Object get()
        {
            return this.mObject;
        }

        
    }

    /// <summary>
    /// CsToCppObjectMap
    /// </summary>
    class CsToCppObjectMap
    {
        private MapEntry [] mMap;

        public CsToCppObjectMap(int capacity)
        {
            this.mMap = new MapEntry[capacity];
            for (int i = 0; i < this.mMap.Length; i++)
            {
                this.mMap[i] = new MapEntry(null);
            }
        }

        public int findSlot()
        {
            for (int i = 0; i < this.mMap.Length; i++)
            {
                if (this.mMap[i].get() == null)
                {
                    return i;
                }
            }
            return -1;
        }

        public int find(Object obj)
        {
            for (int i = 0; i < this.mMap.Length; i++)
            {
                if (this.mMap[i].get() == obj)
                {
                    return i;
                }
            }
            return -1;
        }

        public int put(Object obj)
        {
            int slot= this.findSlot();
            if (slot >= 0)
            {
                this.mMap[slot].set(obj);
                return slot;
            }
            return -1;
        }

        public Object get(int slot)
        {
            if (slot >= 0 && slot < this.mMap.Length)
            {
                return this.mMap[slot].get();
            }
            return null;
        }

        public bool del(int slot)
        {            
            if (slot >= 0 && slot < this.mMap.Length)
            {
                this.mMap[slot].clear();
                return true;
            }
            return false;
        }
    }
    /// <summary>
    /// CsToCppHelper
    /// </summary>
    class CsToCppHelper
    {
        static private CsToCppObjectMap mMap = new CsToCppObjectMap(1024);

        static public Object IntToObj(int i)
        {
            return mMap.get(i);
        }

        static public int ObjToInt(Object obj)
        {
            int slot = mMap.find(obj);
            if (slot >= 0) return slot;
            return mMap.put(obj);
        }

        static public int PutObj(Object obj)
        {
            return mMap.put(obj);
        }

        static public void DelObj(int slot)
        {
            mMap.del(slot);
        }
    }
}
