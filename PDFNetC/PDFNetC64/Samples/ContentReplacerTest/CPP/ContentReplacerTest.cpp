//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

#include <iostream>
#include <PDF/PDFNet.h>
#include <PDF/PDFDoc.h>
#include <PDF/Image.h>
#include <PDF/ContentReplacer.h>

using namespace std;
using namespace pdftron;
using namespace Common;
using namespace SDF;
using namespace PDF;

//-----------------------------------------------------------------------------------------
// The sample code illustrates how to use the ContentReplacer class to make using 
// 'template' pdf documents easier.
//-----------------------------------------------------------------------------------------
int main(int argc, char * argv[])
{
	int ret = 0;

	string input_path = "../../TestFiles/";
	string output_path = input_path + "Output/";

	// The first step in every application using PDFNet is to initialize the 
	// library and set the path to common PDF resources. The library is usually 
	// initialized only once, but calling Initialize() multiple times is also fine.
	PDFNet::Initialize();

	//--------------------------------------------------------------------------------
	// Example 1) Update a business card template with personalized info
	try
	{
		PDFDoc doc(input_path + "BusinessCardTemplate.pdf");
		doc.InitSecurityHandler();

		// first, replace the image on the first page
		ContentReplacer replacer;
		Page page = doc.GetPage(1);
		Image img = Image::Create(doc, input_path + "peppers.jpg");
		replacer.AddImage(page.GetMediaBox(), img.GetSDFObj());
		// next, replace the text place holders on the second page
		replacer.AddString("NAME", "John Smith");
		replacer.AddString("QUALIFICATIONS", "Philosophy Doctor"); 
		replacer.AddString("JOB_TITLE", "Software Developer"); 
		replacer.AddString("ADDRESS_LINE1", "#100 123 Software Rd"); 
		replacer.AddString("ADDRESS_LINE2", "Vancouver, BC"); 
		replacer.AddString("PHONE_OFFICE", "604-730-8989"); 
		replacer.AddString("PHONE_MOBILE", "604-765-4321"); 
		replacer.AddString("EMAIL", "info@pdftron.com"); 
		replacer.AddString("WEBSITE_URL", "http://www.pdftron.com"); 
		// finally, apply
		replacer.Process(page);
		
		doc.Save(output_path + "BusinessCard.pdf", SDFDoc::e_remove_unused, 0);
		cout << "Done. Result saved in BusinessCard.pdf" << endl;
	}
	catch (Common::Exception& e)
	{
		cout << e << endl;
		ret = 1;
	}
	catch (...)
	{
		cout << "Unknown Exception" << endl;
		ret = 1;
	}

	//--------------------------------------------------------------------------------
	// Example 2) Replace text in a region with new text
	try
	{
		PDFDoc doc(input_path + "newsletter.pdf");
		doc.InitSecurityHandler();

		ContentReplacer replacer;
		Page page = doc.GetPage(1);
		Rect target_region = page.GetMediaBox();
		UString replacement_text("hello hello hello hello hello hello hello hello hello hello");
		replacer.AddText(target_region, replacement_text);
		replacer.Process(page);

		doc.Save(output_path + "ContentReplaced.pdf", SDFDoc::e_remove_unused, 0);
		cout << "Done. Result saved in ContentReplaced.pdf" << endl;
	}
	catch (Common::Exception& e)
	{
		cout << e << endl;
		ret = 1;
	}
	catch (...)
	{
		cout << "Unknown Exception" << endl;
		ret = 1;
	}

	cout << "Done." << endl;

	PDFNet::Terminate();
	return ret;
}
