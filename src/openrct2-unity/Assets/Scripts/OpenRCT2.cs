using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace OpenRCT2.Unity
{
    public class OpenRCT2 : MonoBehaviour
    {
        const string PluginFile = "openrct2-dll";

        // OpenRCT2 takes the executing directory by default; which is where the Unity Editor is installed here...
        const string rootpath = @"D:\Projects\Visual Studio\OpenRCT2-Unity";
        const string datapath = rootpath + @"\bin\data";
        const string parkpath = rootpath + @"\src\openrct2-unity\parks";

        static readonly string[] types = { "Surface", "Path", "Track", "SmallScenery", "Entrance", "Wall", "LargeScenery", "Banner", "Corrupt" };


        [DllImport(PluginFile, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        static extern void StartGame([MarshalAs(UnmanagedType.LPStr)] string rootpath);

        [DllImport(PluginFile, CallingConvention = CallingConvention.Cdecl)]
        static extern void StopGame();

        [DllImport(PluginFile, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        static extern void LoadPark([MarshalAs(UnmanagedType.LPStr)] string path);

        [DllImport(PluginFile, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        static extern IntPtr GetParkName();





        [DllImport(PluginFile, CallingConvention = CallingConvention.Cdecl)]
        static extern void GetMapElementAt(int x, int y, ref TileElement element);


        [DllImport(PluginFile, CallingConvention = CallingConvention.Cdecl)]
        static extern int GetMapElementsAt(int x, int y, [In, Out] TileElement[] elements, int arraySize);



        void Start()
        {
            Console.WriteLine("(me) Start OpenRCT2...");

            StartGame(datapath);
            LoadPark($@"{parkpath}\My test park with burgers.sv6");

            IntPtr parkname = GetParkName();
            Console.WriteLine($"(me) Name: {Marshal.PtrToStringAnsi(parkname)} (pointer: {parkname})");


            //TileElement tile = new TileElement();
            //GetMapElementAt(9, 3, ref tile);
            //PrintTileElement(tile); 


            TileElement[] buffer = new TileElement[16];
            /*
            int amount = GetMapElementsAt(9, 3, buffer, buffer.Length);
            Console.WriteLine($"(me) [ 9, 3 ] items = {amount}");

            for (int i = 0; i < amount; i++)
            {
                PrintTileElement(buffer[i]);
            }
            */

            for (int x = 8; x <= 10; x++)
            {
                for (int y = 2; y <= 4; y++)
                {
                    int amount = GetMapElementsAt(x, y, buffer, buffer.Length);

                    Console.WriteLine($"(me) [ {x}, {y} ] items = {amount}");

                    for (int i = 0; i < amount; i++)
                    {
                        PrintTileElement(buffer[i]);
                    }
                }
            }


            /*
            tile_element_iterator iterator = GetTileIterator();
            for (int i = 0; i < 30; i++)
            {
                Debug.Log($"(me) [ {iterator.x}, {iterator.y} ] {iterator.element?.type.ToString() ?? "null"}");

                if (MoveToNextTile(iterator) == 0)
                {
                    Debug.Log($"(me) Finished at {i}");
                    break;
                }
            }
            */
            Console.WriteLine("(me) OpenRCT2 started.");
        }



        void PrintTileElement(TileElement tile)
        {
            int type = (tile.type >> 2);
            string typename = (type < types.Length) ? types[type] : $"(Unknown: {Convert.ToString(type, 2)})";

            Console.WriteLine($"(me) TileElement: (type: {typename})");
            Console.WriteLine("(me)   -> {0} \t(type)", tile.type);
            Console.WriteLine("(me)   -> {0} \t(flags)", tile.flags);
            Console.WriteLine("(me)   -> {0} \t(base_height)", tile.baseHeight);
            Console.WriteLine("(me)   -> {0} \t(clearance_height)", tile.clearanceHeight);
            Console.WriteLine("(me)   -> {0} \t(pad 0x1)", tile.slot0x1);
            Console.WriteLine("(me)   -> {0} \t(pad 0x2)", tile.slot0x2);
            Console.WriteLine("(me)   -> {0} \t(pad 0x3)", tile.slot0x3);
            Console.WriteLine("(me)   -> {0} \t(pad 0x4)", tile.slot0x4);
            Console.WriteLine("(me)   -> {0} \t(pad 0x5)", tile.slot0x5);
            Console.WriteLine("(me)   -> {0} \t(pad 0x6)", tile.slot0x6);
            Console.WriteLine("(me)   -> {0} \t(pad 0x7)", tile.slot0x7);
            Console.WriteLine("(me)   -> {0} \t(pad 0x8)", tile.slot0x8);
            Console.WriteLine("(me)   -> {0} \t(pad 0x9)", tile.slot0x9);
            Console.WriteLine("(me)   -> {0} \t(pad 0xA)", tile.slot0xA);
            Console.WriteLine("(me)   -> {0} \t(pad 0xB)", tile.slot0xB);
            Console.WriteLine("(me)   -> {0} \t(pad 0xC)", tile.slot0xC);
        }


        void OnDestroy()
        {
            StopGame();
        }
    }
}
