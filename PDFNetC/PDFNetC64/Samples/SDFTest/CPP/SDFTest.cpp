//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

#include <PDF/PDFNet.h>
#include <SDF/SDFDoc.h>
#include <Filters/MappedFile.h>
#include <Filters/FilterReader.h>
#include <iostream>

using namespace std;

using namespace pdftron;
using namespace SDF;
using namespace Filters;

// This sample illustrates how to use basic SDF API (also known as Cos) to edit an 
// existing document.

int main(int argc, char *argv[])
{
	int ret = 0;
	PDFNet::Initialize();

	// Relative path to the folder containing test files.
	string input_path =  "../../TestFiles/";
	string output_path = "../../TestFiles/Output/";

	try
	{
		cout << "Opening the test file..." << endl;

		// Here we create a SDF/Cos document directly from PDF file. In case you have 
		// PDFDoc you can always access SDF/Cos document using PDFDoc.GetSDFDoc() method.
		SDFDoc doc((input_path + "fish.pdf").c_str());
		doc.InitSecurityHandler();

		cout << "Modifying info dictionary, adding custom properties, embedding a stream..." << endl;
		Obj trailer = doc.GetTrailer();			// Get the trailer

		// Now we will change PDF document information properties using SDF API

		// Get the Info dictionary. 
		DictIterator itr = trailer.Find("Info");	
		Obj info;
		if (itr.HasNext()) 
		{
			info = itr.Value();
			// Modify 'Producer' entry.
			info.PutString("Producer", "PDFTron PDFNet");

			// Read title entry (if it is present)
			itr = info.Find("Author"); 
			if (itr.HasNext()) 
			{
				UString oldstr;
				itr.Value().GetAsPDFText(oldstr);
				info.PutText("Author",oldstr+"- Modified");
			}
			else 
			{
				info.PutString("Author", "Me, myself, and I");
			}
		}
		else 
		{
			// Info dict is missing. 
			info = trailer.PutDict("Info");
			info.PutString("Producer", "PDFTron PDFNet");
			info.PutString("Title", "My document");
		}

		// Create a custom inline dictionary within Info dictionary
		Obj custom_dict = info.PutDict("My Direct Dict");
		custom_dict.PutNumber("My Number", 100);	 // Add some key/value pairs
		custom_dict.PutArray("My Array");

		// Create a custom indirect array within Info dictionary
		Obj custom_array = doc.CreateIndirectArray();	
		info.Put("My Indirect Array", custom_array);	// Add some entries
		
		// Create indirect link to root
		custom_array.PushBack(trailer.Get("Root").Value());

		// Embed a custom stream (file mystream.txt).
		MappedFile embed_file((input_path + "my_stream.txt"));
		FilterReader mystm(embed_file);
		custom_array.PushBack( doc.CreateIndirectStream(mystm) );

		// Save the changes.
		cout << "Saving modified test file..." << endl;
		doc.Save((output_path + "sdftest_out.pdf").c_str(), 0, 0, "%PDF-1.4");

		cout << "Test completed." << endl;
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
