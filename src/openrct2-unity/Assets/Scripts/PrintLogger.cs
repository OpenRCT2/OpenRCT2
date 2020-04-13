using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using UnityEngine;

public class PrintLogger : MonoBehaviour
{
	//[DllImport("Kernel32.dll", SetLastError = true)]
	//static extern int SetStdHandle(int device, IntPtr handle);


	DebugWriter debugWriter;


	void Awake()
	{
		debugWriter = new DebugWriter();
		Console.SetOut(debugWriter);

		Console.WriteLine("TESTEST");
	}


	void OnDestroy()
	{
		debugWriter?.Dispose();
		debugWriter = null;
	}


	class DebugWriter : TextWriter
	{
		public override Encoding Encoding
			=> Encoding.Unicode;

		FileStream fileStream;
		bool logAsError;

		public DebugWriter(bool logAsError = false)
		{
			this.logAsError = logAsError;

			fileStream = new FileStream("openrct2.log", FileMode.Create);
			//SetStdHandle(-11, fileStream.SafeFileHandle.DangerousGetHandle());
		}


		protected override void Dispose(bool disposing)
		{
			fileStream.Dispose();
			base.Dispose(disposing);
		}

		public override void Write(char value)
			=> Debug.LogAssertion($"Couldn't log to debug console: {value}");

		public override void Write(string value)
			=> Log(value);

		public override void WriteLine(string value)
			=> Log(value);


		void Log(object value)
		{
			if (logAsError)
				Debug.LogError(value);
			else
				Debug.Log(value);
		}
	}
}
