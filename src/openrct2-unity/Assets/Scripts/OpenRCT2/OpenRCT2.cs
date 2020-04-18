using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace OpenRCT2.Unity
{
    public partial class OpenRCT2 : MonoBehaviour
    {
        // OpenRCT2 takes the executing directory by default; which is where the Unity Editor is installed here...
        const string rootpath = @"D:\Projects\Visual Studio\OpenRCT2-Unity";
        const string datapath = rootpath + @"\bin\data";
        const string parkpath = rootpath + @"\src\openrct2-unity\parks";


        void Awake()
        {
            Print("Start OpenRCT2...");

            StartGame(datapath);
            LoadPark($@"{parkpath}\My test park with burgers.sv6");

            string parkname = GetParkName();
            Print($"Name: {parkname} (pointer: {parkname})");


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

            for (int x = 8; x <= 10; x++)
            {
                for (int y = 2; y <= 4; y++)
                {
                    int amount = GetMapElementsAt(x, y, buffer, buffer.Length);

                    Print($"[ {x}, {y} ] items = {amount}");

                    for (int i = 0; i < amount; i++)
                    {
                        buffer[i].DumpToConsole();
                    }
                }
            }
            */

            Print("OpenRCT2 started.");
        }


        void OnDestroy()
        {
            StopGame();
            Print("OpenRCT2 has shutdown.");
        }
    }
}
