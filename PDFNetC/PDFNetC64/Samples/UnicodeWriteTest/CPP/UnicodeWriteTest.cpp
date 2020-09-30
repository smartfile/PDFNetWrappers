//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------
#include <PDF/PDFNet.h>
#include <PDF/PDFDoc.h>
#include <PDF/ElementBuilder.h>
#include <PDF/ElementWriter.h>
#include <PDF/ColorSpace.h>
#include <PDF/ShapedText.h>
#include <PDF/Rect.h>

#include <Filters/MappedFile.h>
#include <Filters/FilterReader.h>

#include <fstream>
#include <iostream>

using namespace std;

using namespace pdftron;
using namespace SDF;
using namespace PDF;

std::vector<UString> TextFileToStringList(const UString& file_path);

/**
 * This example illustrates how to create Unicode text and how to embed composite fonts.
 * 
 * Note: This demo attempts to make use of 'arialuni.ttf' in the '/Samples/TestFiles' 
 * directory. Arial Unicode MS is about 24MB in size and used to come together with Windows and 
 * MS Office.
 * 
 * In case you don't have access to Arial Unicode MS you can use another wide coverage
 * font, like Google Noto, GNU UniFont, or cyberbit. Many of these are freely available,
 * and there is a list maintained at https://en.wikipedia.org/wiki/Unicode_font
 * 
 * If no specific font file can be loaded, the demo will fall back to system specific font
 * substitution routines, and the result will depend on which fonts are available.
 * 
 */
int main(int argc, char *argv[])
{
	int ret = 0;
	PDFNet::Initialize();

	// Relative path to the folder containing test files.
	string input_path =  "../../TestFiles/";
	string output_path = "../../TestFiles/Output/";

	try  
	{	 
		PDFDoc doc;

		ElementBuilder eb;		
		ElementWriter writer;	

		// Start a new page ------------------------------------
		Page page = doc.PageCreate(Rect(0, 0, 612, 794));

		writer.Begin(page);	// begin writing to this page

		string font_program = input_path + "ARIALUNI.TTF";

		// RAII block for ifstream
		{
			std::ifstream ifs(font_program.c_str(), ios_base::in);
#if defined(_WIN32)
			if (!ifs.is_open()) {
				font_program = string("C:/Windows/Fonts/ARIALUNI.TTF");
				ifs.open(font_program.c_str(), ios_base::in);
			}
#endif
			if (!ifs.is_open()) {
				font_program.clear();
			}
		}

		Font fnt;
		if(font_program.size())
		{
			cout << "Note: using " << font_program << " for unshaped unicode text" << endl;
			// if we can find a specific wide-coverage font file, then use that directly
			fnt = Font::CreateCIDTrueTypeFont(doc, font_program.c_str(), true, true);
		}
		else
		{
			cout << "Note: using system font substitution for unshaped unicode text" << endl;
			// if we can't find a specific file, then use system font subsitution 
			// as a fallback, using "Helvetica" as a hint
			UString empty_temp;
			fnt = Font::Create(doc, "Helvetica", empty_temp);
		}
		
		Element element = eb.CreateTextBegin(fnt, 1);
		element.SetTextMatrix(10, 0, 0, 10, 50, 600);
		element.GetGState().SetLeading(2);		 // Set the spacing between lines
		writer.WriteElement(element);

		// Hello World!
		Unicode hello[] = { 'H','e','l','l','o',' ','W','o','r','l','d','!'};
		writer.WriteElement(eb.CreateUnicodeTextRun(hello, sizeof(hello)/sizeof(Unicode)));
		writer.WriteElement(eb.CreateTextNewLine());

		// Latin
		Unicode latin[] = {   
			'a', 'A', 'b', 'B', 'c', 'C', 'd', 'D', 0x45, 0x0046, 0x00C0, 
			0x00C1, 0x00C2, 0x0143, 0x0144, 0x0145, 0x0152, '1', '2' // etc.
		};
		writer.WriteElement(eb.CreateUnicodeTextRun(latin, sizeof(latin)/sizeof(Unicode)));
		writer.WriteElement(eb.CreateTextNewLine());

		// Greek
		Unicode greek[] = {   
			0x039E, 0x039F, 0x03A0, 0x03A1,0x03A3, 0x03A6, 0x03A8, 0x03A9  // etc.
		};
		writer.WriteElement(eb.CreateUnicodeTextRun(greek, sizeof(greek)/sizeof(Unicode)));
		writer.WriteElement(eb.CreateTextNewLine());

		// Cyrillic
		Unicode cyrillic[] = {   
			0x0409, 0x040A, 0x040B, 0x040C, 0x040E, 0x040F, 0x0410, 0x0411,
			0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417, 0x0418, 0x0419 // etc.
		};
		writer.WriteElement(eb.CreateUnicodeTextRun(cyrillic, sizeof(cyrillic)/sizeof(Unicode)));
		writer.WriteElement(eb.CreateTextNewLine());

		// Hebrew
		Unicode hebrew[] = {
			0x05D0, 0x05D1, 0x05D3, 0x05D3, 0x05D4, 0x05D5, 0x05D6, 0x05D7, 0x05D8, 
			0x05D9, 0x05DA, 0x05DB, 0x05DC, 0x05DD, 0x05DE, 0x05DF, 0x05E0, 0x05E1 // etc. 
		};
		writer.WriteElement(eb.CreateUnicodeTextRun(hebrew, sizeof(hebrew)/sizeof(Unicode)));
		writer.WriteElement(eb.CreateTextNewLine());

		// Arabic
		Unicode arabic[] = {
			0x0624, 0x0625, 0x0626, 0x0627, 0x0628, 0x0629, 0x062A, 0x062B, 0x062C, 
			0x062D, 0x062E, 0x062F, 0x0630, 0x0631, 0x0632, 0x0633, 0x0634, 0x0635 // etc. 
		};
		writer.WriteElement(eb.CreateUnicodeTextRun(arabic, sizeof(arabic)/sizeof(Unicode)));
		writer.WriteElement(eb.CreateTextNewLine());

		// Thai 
		Unicode thai[] = {
			0x0E01, 0x0E02, 0x0E03, 0x0E04, 0x0E05, 0x0E06, 0x0E07, 0x0E08, 0x0E09, 
			0x0E0A, 0x0E0B, 0x0E0C, 0x0E0D, 0x0E0E, 0x0E0F, 0x0E10, 0x0E11, 0x0E12 // etc. 
		};
		writer.WriteElement(eb.CreateUnicodeTextRun(thai, sizeof(thai)/sizeof(Unicode)));
		writer.WriteElement(eb.CreateTextNewLine());

		// Hiragana - Japanese 
		Unicode hiragana[] = {
			0x3041, 0x3042, 0x3043, 0x3044, 0x3045, 0x3046, 0x3047, 0x3048, 0x3049, 
			0x304A, 0x304B, 0x304C, 0x304D, 0x304E, 0x304F, 0x3051, 0x3051, 0x3052 // etc. 
		};
		writer.WriteElement(eb.CreateUnicodeTextRun(hiragana, sizeof(hiragana)/sizeof(Unicode)));
		writer.WriteElement(eb.CreateTextNewLine());

		// CJK Unified Ideographs
		Unicode cjk_uni[] = {
			0x5841, 0x5842, 0x5843, 0x5844, 0x5845, 0x5846, 0x5847, 0x5848, 0x5849, 
			0x584A, 0x584B, 0x584C, 0x584D, 0x584E, 0x584F, 0x5850, 0x5851, 0x5852 // etc. 
		};
		writer.WriteElement(eb.CreateUnicodeTextRun(cjk_uni, sizeof(cjk_uni)/sizeof(Unicode)));
		writer.WriteElement(eb.CreateTextNewLine());

		// Simplified Chinese
		Unicode chinese_simplified[] = {
			0x4e16, 0x754c, 0x60a8, 0x597d
		};

		writer.WriteElement(eb.CreateUnicodeTextRun(chinese_simplified, sizeof(chinese_simplified)/sizeof(Unicode)));
		writer.WriteElement(eb.CreateTextNewLine());

		// Finish the block of text
		writer.WriteElement(eb.CreateTextEnd());

		cout << "Now using text shaping logic to place text" << endl;

		// Create a font in indexed encoding mode 
		// normally this would mean that we are required to provide glyph indices
		// directly to CreateUnicodeTextRun, but instead, we will use the GetShapedText
		// method to take care of this detail for us.
		Font indexed_font = Font::CreateCIDTrueTypeFont(doc, input_path + "NotoSans_with_hindi.ttf", true, true, Font::e_Indices);
		element = eb.CreateTextBegin(indexed_font, 10);
		writer.WriteElement(element);

		double line_pos = 350.0;
		double line_space = 20.0;

		// Transform unicode text into an abstract collection of glyph indices and positioning info 
		ShapedText shaped_text = indexed_font.GetShapedText(UString("Shaped Hindi Text:"));

		// transform the shaped text info into a PDF element and write it to the page
		element = eb.CreateShapedTextRun(shaped_text);
		element.SetTextMatrix(1.5, 0, 0, 1.5, 50, line_pos);
		writer.WriteElement(element);

		// read in unicode text lines from a file 
		std::vector<UString> hindi_text = TextFileToStringList(input_path + "hindi_sample_utf16le.txt");

		cout << "Read in " << hindi_text.size() << " lines of Unicode text from file" << endl;
		for (size_t i = 0; i < hindi_text.size(); ++i)
		{
			shaped_text = indexed_font.GetShapedText(hindi_text[i]);
			element = eb.CreateShapedTextRun(shaped_text);
			element.SetTextMatrix(1.5, 0, 0, 1.5, 50, line_pos-line_space*(i+1));
			writer.WriteElement(element);
			cout << "Wrote shaped line to page" << endl;
		}
		
		// Finish the shaped block of text
		writer.WriteElement(eb.CreateTextEnd());

		writer.End();  // save changes to the current page
		doc.PagePushBack(page);

		doc.Save((output_path + "unicodewrite.pdf").c_str(), SDFDoc::e_remove_unused | SDFDoc::e_hex_strings , NULL);
		cout << "Done. Result saved in unicodewrite.pdf..." << endl;
	}
	catch(Common::Exception& e)
	{
		cout << e << endl;
		ret = 1;
	}
	catch(...)
	{
		cout << "Unknown Exception" << endl;
		ret = 1;
	}
	
	PDFNet::Terminate();
	return ret;
}

std::vector<UString> TextFileToStringList(const UString& file_path)
{
	Filters::MappedFile utf_text_filter(file_path);
	size_t file_size = utf_text_filter.FileSize();
	Filters::FilterReader utf_reader(utf_text_filter);
	std::vector<unsigned char> data = utf_reader.Read(file_size);
	data.push_back(0);
	data.push_back(0);
	std::vector<UString> ret;
	size_t line_start = 0;
	for (size_t i = 0; i+1 < data.size(); i +=2)
	{
		bool has_newline = false;
		size_t end_index = i;
		while(i+1 < data.size() && data[i+1] == 0 && (data[i] == '\n' || data[i] == '\r'))
		{
			i+=2;
			has_newline = true;
		}
		if(has_newline || (i + 2 >=  data.size() && line_start < data.size()))
		{
			ret.push_back(UString(reinterpret_cast<Unicode*>(&data[line_start]), (end_index - line_start)/2));
			line_start = i;
		}
	}
	return ret;
}
