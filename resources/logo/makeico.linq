<Query Kind="Statements">
  <Namespace>System.Drawing</Namespace>
  <Namespace>System.Drawing.Imaging</Namespace>
</Query>

string inputDirectory = @"resources\logo";
string outputPath = Path.Combine(inputDirectory, "icon.ico");
int[] imageSizes = new int[] { 256, 128, 96, 64, 48, 40, 32, 24, 16, 8, 4 };
using (FileStream fs = new FileStream(outputPath, FileMode.Create)) {
	BinaryWriter bw = new BinaryWriter(fs);
	bw.Write((short)0);
	bw.Write((short)1);
	bw.Write((short)imageSizes.Length);
	
	int dataStartOffset = 6 + (imageSizes.Length * 16);
	
	using (MemoryStream dataStream = new MemoryStream()) {
		foreach (int size in imageSizes) {
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
		}
		bw.Write(dataStream.ToArray());
	}
}