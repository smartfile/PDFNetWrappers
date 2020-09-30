//
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
//

using System;
using System.IO;
using System.Text;
using pdftron;
using pdftron.Common;
using pdftron.SDF;
using pdftron.PDF;

namespace UnicodeWriteTestCS
{
	/// <summary>
	/// This example illustrates how to create Unicode text and how to embed composite fonts.
	/// </summary>
	class Class1
	{
		private static pdftron.PDFNetLoader pdfNetLoader = pdftron.PDFNetLoader.Instance();
		static Class1() {}
		
		// This example illustrates how to create Unicode text and how to embed composite fonts.
		// 
		// Note: This demo attempts to make use of 'arialuni.ttf' in the '/Samples/TestFiles' 
		// directory. Arial Unicode MS is about 24MB in size and used to come together with Windows and 
		// MS Office.
		// 
		// In case you don't have access to Arial Unicode MS you can use another wide coverage
		// font, like Google Noto, GNU UniFont, or cyberbit. Many of these are freely available,
		// and there is a list maintained at https://en.wikipedia.org/wiki/Unicode_font
		// 
		// If no specific font file can be loaded, the demo will fall back to system specific font
		// substitution routines, and the result will depend on which fonts are available.
		
		static void Main(string[] args)
		{
			PDFNet.Initialize();

			// Relative path to the folder containing test files.
			string input_path =  "../../TestFiles/";
			string output_path = "../../TestFiles/Output/";

			try	
			{
				using (PDFDoc doc = new PDFDoc())
				{
					using (ElementBuilder eb = new ElementBuilder())
					{
						using (ElementWriter writer = new ElementWriter())
						{
							// Start a new page ------------------------------------
							Page page = doc.PageCreate(new Rect(0, 0, 612, 794));

							writer.Begin(page);	// begin writing to this page

							Font fnt = null;
							try
							{
								// Full font embedding
								fnt = Font.Create(doc, "Arial", "");
							}
							catch (PDFNetException e)
							{
							}

							if (fnt == null) {
								try
								{
									fnt = Font.CreateCIDTrueTypeFont(doc, input_path + "ARIALUNI.TTF", true, true);
								}
								catch (PDFNetException e)
								{
								}
							}

							if (fnt == null)
							{
								try
								{
									fnt = Font.CreateCIDTrueTypeFont(doc, "C:/Windows/Fonts/ARIALUNI.TTF", true, true);
								}
								catch (PDFNetException e)
								{
								}
							}

							if (fnt == null)
							{
								Console.WriteLine("Note: using system font substitution for unshaped unicode text");
								fnt = Font.Create(doc, "Helvetica", "");
							}
							else
							{
								Console.WriteLine("Note: using Arial Unicode for unshaped unicode text");
							}


							Element element = eb.CreateTextBegin(fnt, 1);
							element.SetTextMatrix(10, 0, 0, 10, 50, 600);
							element.GetGState().SetLeading(2); // Set the spacing between lines
							writer.WriteElement(element);

							// Hello World!!!
							string hello = "Hello World!";
							writer.WriteElement(eb.CreateUnicodeTextRun(hello));
							writer.WriteElement(eb.CreateTextNewLine());

							// Latin
							char[] latin = {   
								'a', 'A', 'b', 'B', 'c', 'C', 'd', 'D', '\x45', '\x0046', '\x00C0', 
								'\x00C1', '\x00C2', '\x0143', '\x0144', '\x0145', '\x0152', '1', '2' // etc.
							};
							writer.WriteElement(eb.CreateUnicodeTextRun(new string(latin)));
							writer.WriteElement(eb.CreateTextNewLine());

							// Greek
							char[] greek = {   
								(char)0x039E, (char)0x039F, (char)0x03A0, (char)0x03A1, (char)0x03A3,
								(char)0x03A6, (char)0x03A8, (char)0x03A9  // etc.
							};
							writer.WriteElement(eb.CreateUnicodeTextRun(new string(greek)));
							writer.WriteElement(eb.CreateTextNewLine());

							// Cyrillic
							char[] cyrillic = {   
								(char)0x0409, (char)0x040A, (char)0x040B, (char)0x040C, (char)0x040E, (char)0x040F, (char)0x0410, (char)0x0411,
								(char)0x0412, (char)0x0413, (char)0x0414, (char)0x0415, (char)0x0416, (char)0x0417, (char)0x0418, (char)0x0419 // etc.
							};
							writer.WriteElement(eb.CreateUnicodeTextRun(new string(cyrillic)));
							writer.WriteElement(eb.CreateTextNewLine());

							// Hebrew
							char[] hebrew = {
								(char)0x05D0, (char)0x05D1, (char)0x05D3, (char)0x05D3, (char)0x05D4, (char)0x05D5, (char)0x05D6, (char)0x05D7, (char)0x05D8, 
								(char)0x05D9, (char)0x05DA, (char)0x05DB, (char)0x05DC, (char)0x05DD, (char)0x05DE, (char)0x05DF, (char)0x05E0, (char)0x05E1 // etc. 
							};
							writer.WriteElement(eb.CreateUnicodeTextRun(new string(hebrew)));
							writer.WriteElement(eb.CreateTextNewLine());

							// Arabic
							char[] arabic = {
								(char)0x0624, (char)0x0625, (char)0x0626, (char)0x0627, (char)0x0628, (char)0x0629, (char)0x062A, (char)0x062B, (char)0x062C, 
								(char)0x062D, (char)0x062E, (char)0x062F, (char)0x0630, (char)0x0631, (char)0x0632, (char)0x0633, (char)0x0634, (char)0x0635 // etc. 
							};
							writer.WriteElement(eb.CreateUnicodeTextRun(new string(arabic)));
							writer.WriteElement(eb.CreateTextNewLine());

							// Thai 
							char[] thai = {
								(char)0x0E01, (char)0x0E02, (char)0x0E03, (char)0x0E04, (char)0x0E05, (char)0x0E06, (char)0x0E07, (char)0x0E08, (char)0x0E09, 
								(char)0x0E0A, (char)0x0E0B, (char)0x0E0C, (char)0x0E0D, (char)0x0E0E, (char)0x0E0F, (char)0x0E10, (char)0x0E11, (char)0x0E12 // etc. 
							};
							writer.WriteElement(eb.CreateUnicodeTextRun(new string(thai)));
							writer.WriteElement(eb.CreateTextNewLine());

							// Hiragana - Japanese 
							char[] hiragana = {
								(char)0x3041, (char)0x3042, (char)0x3043, (char)0x3044, (char)0x3045, (char)0x3046, (char)0x3047, (char)0x3048, (char)0x3049, 
								(char)0x304A, (char)0x304B, (char)0x304C, (char)0x304D, (char)0x304E, (char)0x304F, (char)0x3051, (char)0x3051, (char)0x3052 // etc. 
							};
							writer.WriteElement(eb.CreateUnicodeTextRun(new string(hiragana)));
							writer.WriteElement(eb.CreateTextNewLine());

							// CJK Unified Ideographs
							char[] cjk_uni = {
								(char)0x5841, (char)0x5842, (char)0x5843, (char)0x5844, (char)0x5845, (char)0x5846, (char)0x5847, (char)0x5848, (char)0x5849, 
								(char)0x584A, (char)0x584B, (char)0x584C, (char)0x584D, (char)0x584E, (char)0x584F, (char)0x5850, (char)0x5851, (char)0x5852 // etc. 
							};
							writer.WriteElement(eb.CreateUnicodeTextRun(new string(cjk_uni)));
							writer.WriteElement(eb.CreateTextNewLine());

							// Finish the block of text
							writer.WriteElement(eb.CreateTextEnd());

							Console.WriteLine("Now using text shaping logic to place text");

							// Create a font in indexed encoding mode 
							// normally this would mean that we are required to provide glyph indices
							// directly to CreateUnicodeTextRun, but instead, we will use the GetShapedText
							// method to take care of this detail for us.
							Font indexedFont = Font.CreateCIDTrueTypeFont(doc, input_path + "NotoSans_with_hindi.ttf", true, true, Font.Encoding.e_Indices);
							element = eb.CreateTextBegin(indexedFont, 10.0);
							writer.WriteElement(element);

							double linePos = 350.0;
							double lineSpace = 20.0;

							// Transform unicode text into an abstract collection of glyph indices and positioning info 
							ShapedText shapedText = indexedFont.GetShapedText("Shaped Hindi Text:");

							// transform the shaped text info into a PDF element and write it to the page
							element = eb.CreateShapedTextRun(shapedText);
							element.SetTextMatrix(1.5, 0, 0, 1.5, 50, linePos);
							linePos -= lineSpace;
							writer.WriteElement(element);

							// read in unicode text lines from a file 

							string[] hindiTextLines = File.ReadAllLines(input_path + "hindi_sample_utf16le.txt", new UnicodeEncoding());

							Console.WriteLine("Read in " + hindiTextLines.Length + " lines of Unicode text from file");
							foreach (string textLine in hindiTextLines)
							{
								shapedText = indexedFont.GetShapedText(textLine);
								element = eb.CreateShapedTextRun(shapedText);
								element.SetTextMatrix(1.5, 0, 0, 1.5, 50, linePos);
								linePos -= lineSpace;
								writer.WriteElement(element);
								Console.WriteLine("Wrote shaped line to page");
							}

							// Finish the shaped block of text
							writer.WriteElement(eb.CreateTextEnd());


							writer.End();  // save changes to the current page
							doc.PagePushBack(page);
							doc.Save(output_path + "unicodewrite.pdf", SDFDoc.SaveOptions.e_remove_unused | SDFDoc.SaveOptions.e_hex_strings);
							Console.WriteLine("Done. Result saved in unicodewrite.pdf...");
						}
					}
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}

		}
	}
}
