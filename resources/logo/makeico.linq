<Query Kind="Statements">
  <Namespace>System.Drawing</Namespace>
  <Namespace>System.Drawing.Imaging</Namespace>
</Query>

string inputDirectory = @"C:\Users\Ted\Documents\Programming\Projects\Hacking\OpenRCT2\resources\logo";
string outputPath = @"C:\Users\Ted\Documents\Programming\Projects\Hacking\OpenRCT2\resources\logo\icon.ico";
int numImages = 7;
using (FileStream fs = new FileStream(outputPath, FileMode.Create)) {
	BinaryWriter bw = new BinaryWriter(fs);
	bw.Write((short)0);
	bw.Write((short)1);
	bw.Write((short)numImages);
	
	int dataStartOffset = 6 + (numImages * 16);
	
	using (MemoryStream dataStream = new MemoryStream()) {
		int size = 256;
		for (int i = 0; i < numImages; i++) {
			bw.Write((byte)(size == 256 ? 0 : size));
			bw.Write((byte)(size == 256 ? 0 : size));
			bw.Write((byte)0);
			bw.Write((byte)0);
			bw.Write((short)0);
			bw.Write((short)32);
			
			int dataOffset = (int)dataStream.Position;
			int dataLength;
			
			string inputImagePath = Path.Combine(inputDirectory, "icon_x" + size + ".png");
			using (Image image = Image.FromFile(inputImagePath))
				image.Save(dataStream, ImageFormat.Png);
			
			dataLength = (int)dataStream.Position - dataOffset;
			dataOffset += dataStartOffset;
			
			bw.Write(dataLength);
			bw.Write(dataOffset);
			
			size /= 2;
		}
		bw.Write(dataStream.ToArray());
	}
}