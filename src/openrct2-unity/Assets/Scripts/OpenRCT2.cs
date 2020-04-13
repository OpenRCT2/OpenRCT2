using System;
using System.Runtime.InteropServices;
using UnityEngine;

public class OpenRCT2 : MonoBehaviour
{
	const string PluginFile = "openrct2-dll";

    // OpenRCT2 takes the executing directory by default; which is where the Unity Editor is installed here...
    const string rootpath = @"D:\Projects\Visual Studio\OpenRCT2-Unity";
    const string datapath = rootpath + @"\data";
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


    [StructLayout(LayoutKind.Sequential)]
    class tile_element_iterator
    {
        public int x;
        public int y;
        public TileElement element;
    };


    // https://github.com/OpenRCT2/OpenRCT2/wiki/Maps
    [StructLayout(LayoutKind.Sequential)]
    struct TileElement
    {
        public byte type; // first 6 bits are type, last 2 bits are rotation
        public byte Flags;
        public byte base_height;
        public byte clearance_height;
    }

    [DllImport(PluginFile, CallingConvention = CallingConvention.Cdecl)]
    static extern TileElement GetMapElementAt(int x, int y);


    [DllImport(PluginFile, CallingConvention = CallingConvention.Cdecl)] 
    static extern int GetMapElementsAt(int x, int y, [In, Out] TileElement[] elements, int arraySize);

    [DllImport(PluginFile, CallingConvention = CallingConvention.Cdecl)]
    static extern tile_element_iterator GetTileIterator();

    [DllImport(PluginFile, CallingConvention = CallingConvention.Cdecl)]
    static extern int MoveToNextTile([In, Out] tile_element_iterator iterator);



    void Start()
    {
        Console.WriteLine("(me) Start OpenRCT2...");

        StartGame(datapath);        
        LoadPark($@"{parkpath}\My test park.sc6");

        IntPtr parkname = GetParkName();
        Console.WriteLine($"(me) Name: {Marshal.PtrToStringAnsi(parkname)} (pointer: {parkname})");
        
        TileElement[] buffer = new TileElement[16];


        for (int x = 5; x < 20; x++)
        { 
            for (int y = 5; y < 20; y++)
            {
                //TileElement element = GetMapElementAt(x, y);

                int amount = GetMapElementsAt(x, y, buffer, buffer.Length);

                Console.WriteLine($"(me) [ {x}, {y} ] items = {amount}");

                for (int i = 0; i < amount; i++)
                {
                    Console.WriteLine($"(me)   > {TileElementAsString(buffer[i])}");
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



    string TileElementAsString(TileElement tile)
    {
        int type = (tile.type >> 2);
        string typename = (type < types.Length) ? types[type] : $"(Unknown: {Convert.ToString(type, 2)})";

        string rotation = Convert.ToString(tile.type & 0b11, 2);


        return $"type: {typename}, rot: {rotation}, base: {tile.base_height}, top: {tile.clearance_height}";
    }


	void OnDestroy()
	{
		StopGame();
	}
}
