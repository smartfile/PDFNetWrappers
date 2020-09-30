//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------
#include <PDF/PDFNet.h>
#include <PDF/PDFDoc.h>
#include <PDF/OCRModule.h>
#include <PDF/OCROptions.h>
#include <SDF/Obj.h>
#include <iostream>

using namespace std;
using namespace pdftron;
using namespace PDF;
using namespace SDF;

//---------------------------------------------------------------------------------------
// The following sample illustrates how to use OCR module
//---------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	try 
	{
		// The first step in every application using PDFNet is to initialize the 
		// library and set the path to common PDF resources. The library is usually 
		// initialized only once, but calling Initialize() multiple times is also fine.
		PDFNet::Initialize();
		// The location of the OCR Module
		PDFNet::AddResourceSearchPath("../../../Lib/");
		if(!OCRModule::IsModuleAvailable())
		{
			cout << endl;
			cout << "Unable to run OCRTest: PDFTron SDK OCR module not available." << endl;
			cout << "---------------------------------------------------------------" << endl;
			cout << "The OCR module is an optional add-on, available for download" << endl;
			cout << "at http://www.pdftron.com/. If you have already downloaded this" << endl;
			cout << "module, ensure that the SDK is able to find the required files" << endl;
			cout << "using the PDFNet::AddResourceSearchPath() function." << endl << endl;
			return 0;
		}

		// Relative path to the folder containing test files.
		string input_path =  "../../TestFiles/OCR/";
		string output_path = "../../TestFiles/Output/";


		//--------------------------------------------------------------------------------
		// Example 1) Process image without specifying options, default language - English - is used
		try 
		{

			// A) Setup empty destination doc

			PDFDoc doc;

			// B) Run OCR on the .png without options

			OCRModule::ImageToPDF(doc, input_path + "psychomachia_excerpt.png", NULL);

			// C) check the result

			doc.Save(output_path + "psychomachia_excerpt.pdf", 0, 0);
			
			cout << "Example 1: psychomachia_excerpt.png" << endl;

		}
		catch(Common::Exception& e)	
		{
			cout << e << endl;
		}
		catch(...) 
		{
			cout << "Unknown Exception" << endl;
		}

		//--------------------------------------------------------------------------------
		// Example 2) Process document using multiple languages
		try
		{
			// A) Setup empty destination doc

			PDFDoc doc;

			// B) Setup options with multiple target languages, English will always be considered as secondary language

			OCROptions opts;
			opts.AddLang("rus");
			opts.AddLang("deu");

			// C) Run OCR on the .jpg with options

			OCRModule::ImageToPDF(doc, input_path + "multi_lang.jpg", &opts);

			// D) check the result

			doc.Save(output_path + "multi_lang.pdf", 0, 0);

			cout << "Example 2: multi_lang.jpg" << endl;

		}
		catch (Common::Exception& e)
		{
			cout << e << endl;
		}
		catch (...)
		{
			cout << "Unknown Exception" << endl;
		}

		//--------------------------------------------------------------------------------
		// Example 3) Process a .pdf specifying a language - German - and ignore zone comprising a sidebar image 
		try
		{
			// A) Open the .pdf document

			PDFDoc doc((input_path + "german_kids_song.pdf").c_str());

			// B) Setup options with a single language and an ignore zone

			OCROptions opts;
			opts.AddLang("deu");

			RectCollection ignore_zones;
			ignore_zones.AddRect(424, 163, 493, 730);
			opts.AddIgnoreZonesForPage(ignore_zones, 1);

			// C) Run OCR on the .pdf with options

			OCRModule::ProcessPDF(doc, &opts);

			// D) check the result

			PDFDoc doc_out(doc);
			doc_out.Save(output_path + "german_kids_song.pdf", 0, 0);

			cout << "Example 3: german_kids_song.pdf" << endl;
		}
		catch (Common::Exception& e)
		{
			cout << e << endl;
		}
		catch (...)
		{
			cout << "Unknown Exception" << endl;
		}

		//--------------------------------------------------------------------------------
		// Example 4) Process multipage tiff with text/ignore zones specified for each page, optionally provide English as the target language
		try
		{
			// A) Setup empty destination doc

			PDFDoc doc;

			// B) Setup options with a single language plus text/ignore zones

			OCROptions opts;
			opts.AddLang("eng");

			RectCollection ignore_zones;
			// ignore signature box in the first 2 pages
			ignore_zones.AddRect(1492, 56, 2236, 432);
			opts.AddIgnoreZonesForPage(ignore_zones, 1);
			opts.AddIgnoreZonesForPage(ignore_zones, 2);

			// can use a combination of ignore and text boxes to focus on the page area of interest,
			// as ignore boxes are applied first, we remove the arrows before selecting part of the diagram
			ignore_zones.Clear();
			ignore_zones.AddRect(992, 1276, 1368, 1372);
			opts.AddIgnoreZonesForPage(ignore_zones, 3);

			RectCollection text_zones;
			// we only have text zones selected in page 3

			// select horizontal BUFFER ZONE sign
			text_zones.AddRect(900, 2384, 1236, 2480);
			// select right vertical BUFFER ZONE sign
			text_zones.AddRect(1960, 1976, 2016, 2296);
			// select Lot No.
			text_zones.AddRect(696, 1028, 1196, 1128);

			// select part of the plan inside the BUFFER ZONE
			text_zones.AddRect(428, 1484, 1784, 2344);
			text_zones.AddRect(948, 1288, 1672, 1476);
			opts.AddTextZonesForPage(text_zones, 3);

			// C) Run OCR on the .tif with options

			OCRModule::ImageToPDF(doc, input_path + "bc_environment_protection.tif", &opts);

			// D) check the result

			doc.Save(output_path + "bc_environment_protection.pdf", 0, 0);

			cout << "Example 4: bc_environment_protection.tif" << endl;

		}
		catch (Common::Exception& e)
		{
			cout << e << endl;
		}
		catch (...)
		{
			cout << "Unknown Exception" << endl;
		}

		//--------------------------------------------------------------------------------
		// Example 5) Alternative workflow for extracting OCR result JSON, postprocessing (e.g., removing words not in the dictionary or filtering special
		// out special characters), and finally applying modified OCR JSON to the source PDF document 
		try
		{

			// A) Open the .pdf document

			PDFDoc doc((input_path + "zero_value_test_no_text.pdf").c_str());

			// B) Run OCR on the .pdf with default English language

			UString json = OCRModule::GetOCRJsonFromPDF(doc, NULL);

			// C) Post-processing step (whatever it might be)

			cout << "Have OCR result JSON, re-applying to PDF " << endl;

			// D) Apply potentially modified OCR JSON to the PDF

			OCRModule::ApplyOCRJsonToPDF(doc, json);

			// E) Check the result

			PDFDoc doc_out(doc);
			doc_out.Save(output_path + "zero_value_test_no_text.pdf", 0, 0);

			cout << "Example 5: extracting and applying OCR JSON from zero_value_test_no_text.pdf" << endl;

		}
		catch (Common::Exception& e)
		{
			cout << e << endl;
		}
		catch (...)
		{
			cout << "Unknown Exception" << endl;
		}

		//--------------------------------------------------------------------------------
		// Example 6) The postprocessing workflow has also an option of extracting OCR results in XML format, similar to the one used by TextExtractor
		try
		{

			// A) Setup empty destination doc

			PDFDoc doc;

			// B) Run OCR on the .tif with default English language, extracting OCR results in XML format. Note that
			// in the process we convert the source image into PDF. We reuse this PDF document later to add hidden text layer to it.

			UString xml = OCRModule::GetOCRXmlFromImage(doc, input_path + "physics.tif", NULL);

			// C) Post-processing step (whatever it might be)

			cout << "Have OCR result XML, re-applying to PDF" << endl;

			// D) Apply potentially modified OCR XML to the PDF

			OCRModule::ApplyOCRXmlToPDF(doc, xml);

			// E) Check the result

			PDFDoc doc_out(doc);
			doc_out.Save(output_path + "physics.pdf", 0, 0);

			cout << "Example 6: extracting and applying OCR XML from physics.tif" << endl;

		}
		catch (Common::Exception& e)
		{
			cout << e << endl;
		}
		catch (...)
		{
			cout << "Unknown Exception" << endl;
		}

		//--------------------------------------------------------------------------------
		// Example 7) Resolution can be manually set, when DPI missing from metadata or is wrong
		try
		{
			// A) Setup empty destination doc
			PDFDoc doc;

			// B) Setup options with a text zone
			OCROptions opts;
			RectCollection text_zones;
			text_zones.AddRect(140, 870, 310, 920);
			opts.AddTextZonesForPage(text_zones, 1);

			// C) Manually override DPI
			opts.AddDPI(100);

			// D) Run OCR on the .jpg with options
			OCRModule::ImageToPDF(doc, (input_path + "corrupted_dpi.jpg").c_str(), &opts);

			// E) Check the result
			doc.Save(output_path + "corrupted_dpi.pdf", 0, 0);

			cout << "Example 7: converting image with corrupted resolution metadata corrupted_dpi.jpg to pdf with searchable text" << endl;

		}
		catch (Common::Exception& e)
		{
			cout << e << endl;
		}
		catch (...)
		{
			cout << "Unknown Exception" << endl;
		}

		cout << "Done." << endl;

		PDFNet::Terminate();
	}
	catch(Common::Exception& e)	
	{
		cout << e << endl;
	}
	catch (...) {
		cout << "Unknown Exception" << endl;
	}

	return 0;	
}
