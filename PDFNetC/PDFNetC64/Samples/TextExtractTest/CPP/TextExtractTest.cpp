//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

#include <PDF/PDFNet.h>
#include <PDF/PDFDoc.h>
#include <PDF/ElementReader.h>
#include <PDF/Element.h>
#include <PDF/Font.h>
#include <Filters/FilterReader.h>
#include <PDF/Image/Image2RGB.h>
#include <PDF/TextExtractor.h>

// This sample illustrates the basic text extraction capabilities of PDFNet.

#include <iostream>

using namespace std;

using namespace pdftron;
using namespace PDF;
using namespace SDF;
using namespace Common;
using namespace Filters; 

// A utility method used to dump all text content in the console window.
void DumpAllText(ElementReader& reader) 
{
	Element element; 
	while ((element = reader.Next()) != 0)
	{
		switch (element.GetType()) 
		{
		case Element::e_text_begin: 
			cout << "\n--> Text Block Begin\n";
			break;
		case Element::e_text_end:
			cout << "\n--> Text Block End\n";
			break;
		case Element::e_text:
			{
				Rect bbox;
				element.GetBBox(bbox);
				cout << "\n--> BBox: " << bbox.x1 << ", " 
									   << bbox.y1 << ", " 
									   << bbox.x2 << ", " 
									   << bbox.y2 << "\n";

				UString arr = element.GetTextString();
				cout << arr << "\n";
			}
			break;
		case Element::e_text_new_line:
			cout << "\n--> New Line\n";
			break;
		case Element::e_form:				// Process form XObjects
			reader.FormBegin(); 
            DumpAllText(reader);
			reader.End(); 
			break; 
		}
	}
}

// A helper method for ReadTextFromRect
void RectTextSearch(ElementReader& reader, const Rect& pos, UString& srch_str) 
{			
	Element element; 
	while (element = reader.Next())
	{
		switch (element.GetType()) 
		{
		case Element::e_text:
			{
				Rect bbox;
				element.GetBBox(bbox);
				if(bbox.IntersectRect(bbox, pos)) 
				{
					UString arr = element.GetTextString();
					srch_str += arr;
					srch_str += "\n"; // add a new line?
				}
				break;
			}
		case Element::e_text_new_line:
			{
				break;
			}
		case Element::e_form: // Process form XObjects
			{
				reader.FormBegin(); 
				RectTextSearch(reader, pos, srch_str);
				reader.End(); 
				break; 
			}
		}
	}
}

// A utility method used to extract all text content from
// a given selection rectangle. The rectangle coordinates are
// expressed in PDF user/page coordinate system.
UString ReadTextFromRect(Page& page, const Rect& pos, ElementReader& reader)
{
	UString srch_str;
	reader.Begin(page);
	RectTextSearch(reader, pos, srch_str);
	reader.End();
	return srch_str;
}


void PrintStyle(TextExtractor::Style& s)
{
	UInt8 rgb[3];
	char rgb_hex[24];

	s.GetColor(rgb);
	sprintf(rgb_hex, "%02X%02X%02X;", rgb[0], rgb[1], rgb[2]);
	cout << " style=\"font-family:" << s.GetFontName() << "; "	<< "font-size:" << s.GetFontSize() << ";" 
		 << (s.IsSerif() ? " sans-serif; " : " ") << "color:#" << rgb_hex << "\"";
}

int main(int argc, char *argv[])
{
	int ret = 0;
	PDFNet::Initialize();
	// Relative path to the folder containing test files.
	string input_path =  "../../TestFiles/newsletter.pdf";
	// string output_path = "../../TestFiles/Output/";



	
	const char* filein = argc>1 ? argv[1] : input_path.c_str();

	bool example1_basic = false;
	bool example2_xml = false;
	bool example3_wordlist = false;
	bool example4_advanced  = true;
	bool example5_low_level = false;

	// Sample code showing how to use high-level text extraction APIs.
	try
	{
		PDFDoc doc(filein);
		doc.InitSecurityHandler();

		Page page = doc.GetPage(1);
		if (!page){
			cout << "Page not found." << endl;
			return 1;
		}

		TextExtractor txt;
		txt.Begin(page); // Read the page.
		// Other options you may want to consider...
		// txt.Begin(*itr, 0, TextExtractor::e_no_dup_remove);
		// txt.Begin(*itr, 0, TextExtractor::e_remove_hidden_text);


		// Example 1. Get all text on the page in a single string.
		// Words will be separated with space or new line characters.
		if (example1_basic) 
		{
			// Get the word count.
			cout << "Word Count: " << txt.GetWordCount() << endl;

			UString text;
			txt.GetAsText(text);
			cout << "\n\n- GetAsText --------------------------\n" << text << endl;
			cout << "-----------------------------------------------------------" << endl;
		}

		// Example 2. Get XML logical structure for the page.
		if (example2_xml) 
		{
			UString text;
			txt.GetAsXML(text, TextExtractor::e_words_as_elements | TextExtractor::e_output_bbox | TextExtractor::e_output_style_info);
			cout << "\n\n- GetAsXML  --------------------------\n" << text << endl;
			cout << "-----------------------------------------------------------" << endl;
		}

		// Example 3. Extract words one by one.
		if (example3_wordlist) 
		{
			UString text;
			TextExtractor::Line line = txt.GetFirstLine();
			TextExtractor::Word word;
			for (; line.IsValid(); line=line.GetNextLine())	{
				for (word=line.GetFirstWord(); word.IsValid(); word=word.GetNextWord()) {
					text.Assign(word.GetString(), word.GetStringLen());
					cout << text << '\n';
				}
			}
			cout << "-----------------------------------------------------------" << endl;
		}

		// Example 4. A more advanced text extraction example. 
		// The output is XML structure containing paragraphs, lines, words, 
		// as well as style and positioning information.
		if (example4_advanced) 
		{
			const double *b;
			double q[8];
			int cur_flow_id=-1, cur_para_id=-1;

			UString uni_str;
			TextExtractor::Line line;
			TextExtractor::Word word;
			TextExtractor::Style s, line_style;

			cout << "<PDFText>\n";

			// For each line on the page...
			for (line=txt.GetFirstLine(); line.IsValid(); line=line.GetNextLine())
			{
				if ( line.GetNumWords() == 0 ) {
					continue;
				}

				if (cur_flow_id != line.GetFlowID()) {
					if (cur_flow_id != -1) {
						if (cur_para_id != -1) {
							cur_para_id = -1;
							cout << "</Para>\n";
						}
						cout << "</Flow>\n";
					}
					cur_flow_id = line.GetFlowID();
					cout << "<Flow id=\""<< cur_flow_id << "\">\n";
				}

				if (cur_para_id != line.GetParagraphID()) {
					if (cur_para_id != -1)
						cout << "</Para>\n";
					cur_para_id = line.GetParagraphID();
					cout << "<Para id=\""<< cur_para_id << "\">\n";
				}	
				
				b = line.GetBBox();
				line_style = line.GetStyle();
				printf("<Line box=\"%.2f, %.2f, %.2f, %.2f\"", b[0], b[1], b[2], b[3]);
				PrintStyle(line_style);
				cout << " cur_num=\"" << line.GetCurrentNum() << "\"";
				cout << ">\n";

				// For each word in the line...
				for (word=line.GetFirstWord(); word.IsValid(); word=word.GetNextWord())
				{
					// Output the bounding box for the word.
					word.GetBBox(q);
					printf("<Word box=\"%.2f, %.2f, %.2f, %.2f\"", q[0], q[1], q[2], q[3]);
					cout << " cur_num=\"" << word.GetCurrentNum() << "\"";
					int sz = word.GetStringLen();
					if (sz == 0) continue;

					// If the word style is different from the parent style, output the new style.
					s = word.GetStyle();
					if (s != line_style) {
						PrintStyle(s);
					}

					uni_str.Assign(word.GetString(), sz);
					cout << ">" << uni_str;
					cout << "</Word>\n";
				}
				cout << "</Line>\n";
			}

			if (cur_flow_id != -1) {
				if (cur_para_id != -1) {
					cur_para_id = -1;
					cout << "</Para>\n";
				}
				cout << "</Flow>\n";
			}
			cout << "</PDFText>\n";
		}
	}
	catch(Exception& e)
	{
		cout << e << endl;
		ret = 1;
	}
	catch(...)
	{
		cout << "Unknown Exception" << endl;
		ret = 1;
	}

	   	  
	if(example5_low_level)
	{
		try	
		{
			PDFDoc doc(filein);
			doc.InitSecurityHandler();

			// Example 1. Extract all text content from the document

			ElementReader reader;
			//  Read every page
			for (PageIterator itr=doc.GetPageIterator(); itr.HasNext(); itr.Next()) 
			{				
				reader.Begin(itr.Current());
				DumpAllText(reader);
				reader.End();
			}

			// Example 2. Extract text content based on the 
			// selection rectangle.
			cout << "\n----------------------------------------------------";
			cout << "\nExtract text based on the selection rectangle.";
			cout << "\n----------------------------------------------------\n";

			Page first_page = doc.GetPageIterator().Current();
			UString s1 = ReadTextFromRect(first_page, Rect(27, 392, 563, 534), reader);
			cout << "\nField 1: " << s1;

			s1 = ReadTextFromRect(first_page, Rect(28, 551, 106, 623), reader);
			cout << "\nField 2: " << s1;

			s1 = ReadTextFromRect(first_page, Rect(208, 550, 387, 621), reader);
			cout << "\nField 3: " << s1;

			// ... 
			cout << "Done." << endl;
		}
		catch(Exception& e)
		{
			cout << e << endl;
			ret = 1;
		}
		catch(...)
		{
			cout << "Unknown Exception" << endl;
			ret = 1;
		}
	}
	PDFNet::Terminate();
	return ret;
}

