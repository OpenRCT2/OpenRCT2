using System;
using System.Runtime.InteropServices;
using UnityEngine;

#pragma warning disable CA2101 // Specify marshaling for P/Invoke string arguments -> this is deliberate

namespace OpenRCT2.Unity
{
    public partial class OpenRCT2
    {
        const string PluginFile = "openrct2-dll";


        #region Game

        [DllImport(PluginFile, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        static extern void StartGame([MarshalAs(UnmanagedType.LPStr)] string rootpath);


        [DllImport(PluginFile, CallingConvention = CallingConvention.Cdecl)]
        static extern void StopGame();

        #endregion


        #region Parks

        /// <summary>
        /// Loads a park from the specified path.
        /// </summary>
        [DllImport(PluginFile, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        internal static extern void LoadPark([MarshalAs(UnmanagedType.LPStr)] string path);
            

        [DllImport(PluginFile, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        static extern IntPtr GetParkNamePtr();


        /// <summary>
        /// Returns the name of the currently loaded park.
        /// </summary>
        public static string GetParkName()
            => Marshal.PtrToStringAnsi(GetParkNamePtr());

        #endregion


        #region Map

        [DllImport(PluginFile, CallingConvention = CallingConvention.Cdecl)]
        internal static extern int GetMapSize();


        [DllImport(PluginFile, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void GetMapElementAt(int x, int y, ref TileElement element);


        [DllImport(PluginFile, CallingConvention = CallingConvention.Cdecl)]
        static extern int GetMapElementsAt(int x, int y, [In, Out] TileElement[] elements, int arraySize);


        /// <summary>
        /// Writes all elements to the given buffer at the specified position.
        /// </summary>
        public static int GetMapElementsAt(int x, int y, TileElement[] elements)
            => GetMapElementsAt(x, y, elements, elements.Length);

        #endregion


        #region Sprites

        [DllImport(PluginFile, CallingConvention = CallingConvention.Cdecl)]
        static extern int GetSpriteCount(SpriteType spriteType);


        [DllImport(PluginFile, CallingConvention = CallingConvention.Cdecl)]
        static extern int GetAllPeeps([In, Out] Peep[] elements, int arraySize);


        public static Peep[] GetAllPeeps()
        {
            int spriteCount = GetSpriteCount(SpriteType.Peep);
            Debug.Log($"Peeps found: {spriteCount}");

            Peep[] peeps = new Peep[spriteCount];

            GetAllPeeps(peeps, spriteCount);
            return peeps;
        }


        #endregion


        /// <summary>
        /// Adds '(me)' to the log and prints it out.
        /// </summary>
        static void Print(string text)
        {
            Debug.Log("(me) " + text);
        }
    }
}
