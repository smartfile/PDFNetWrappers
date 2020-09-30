//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

#include <PDF/PDFNet.h>
#include <PDF/PDFDoc.h>
#include <PDF/ElementReader.h>
#include <PDF/Element.h>
#include <iostream>

using namespace pdftron;
using namespace std;
using namespace PDF;

void ProcessElements(ElementReader& reader) 
{
	for (Element element=reader.Next(); element; element = reader.Next()) 	// Read page contents
	{
		switch (element.GetType())
		{
			case Element::e_path:				 // Process path data...
			{
					PathData data = element.GetPathData();
					const std::vector<unsigned char>& operators = data.GetOperators();
					const std::vector<double>& points = data.GetPoints();
			}
			break; 
			
			case Element::e_text: 				// Process text strings...
			{
				const UString data = element.GetTextString();
				cout << data << endl;
			}
			break;
			
			case Element::e_form:				// Process form XObjects
			{
					reader.FormBegin(); 
					ProcessElements(reader);
					reader.End(); 
			}
			break; 
		}
	}
}


int main(int argc, char *argv[])
{
	int ret = 0;
	PDFNet::Initialize();

	// Relative path to the folder containing test files.
	string input_path =  "../../TestFiles/";

	try	// Extract text data from all pages in the document
	{
		cout << "-------------------------------------------------" << endl;
		cout << "Sample 1 - Extract text data from all pages in the document." << endl;
		cout << "Opening the input pdf..." << endl;

		PDFDoc doc((input_path + "newsletter.pdf").c_str());
		doc.InitSecurityHandler();

		int pgnum = doc.GetPageCount();
		
		PageIterator itr;
		ElementReader page_reader;

		for (itr = doc.GetPageIterator(); itr.HasNext(); itr.Next())		//  Read every page
		{				
			page_reader.Begin(itr.Current());
			ProcessElements(page_reader);
			page_reader.End();
		}

		cout << "Done." << endl;
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
