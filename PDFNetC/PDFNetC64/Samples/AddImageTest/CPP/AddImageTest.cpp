//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

#include <PDF/PDFNet.h>
#include <PDF/PDFDoc.h>
#include <PDF/ElementBuilder.h>
#include <PDF/ElementWriter.h>
#include <PDF/ElementReader.h>
#include <PDF/Image.h>
#include <Filters/MappedFile.h>
#include <Filters/FilterReader.h>
#include <SDF/ObjSet.h>

#include <iostream>

using namespace std;

using namespace pdftron;
using namespace Common;
using namespace SDF;
using namespace PDF;

//-----------------------------------------------------------------------------------
// This sample illustrates how to embed various raster image formats
// (e.g. TIFF, JPEG, JPEG2000, JBIG2, GIF, PNG, BMP, etc.) in a PDF document.
//
// Note: On Windows platform this sample utilizes GDI+ and requires GDIPLUS.DLL to
// be present in the system path.
//-----------------------------------------------------------------------------------

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

		ElementBuilder f;		// Used to build new Element objects
		ElementWriter writer;	// Used to write Elements to the page	
		
		Page page = doc.PageCreate();	// Start a new page
		writer.Begin(page);		// Begin writing to this page
    
		// ----------------------------------------------------------
		// Add JPEG image to the output file
		PDF::Image img = PDF::Image::Create(doc, (input_path + "peppers.jpg").c_str());
		Element element = f.CreateImage(img, 50, 500, img.GetImageWidth()/2, img.GetImageHeight()/2);
		writer.WritePlacedElement(element);

		// ----------------------------------------------------------
		// Add a PNG image to the output file
		img = PDF::Image::Create(doc, (input_path + "butterfly.png").c_str());
		element = f.CreateImage(img, Matrix2D(100, 0, 0, 100, 300, 500));
		writer.WritePlacedElement(element);

		// ----------------------------------------------------------
		// Add a GIF image to the output file
		img = PDF::Image::Create(doc, (input_path + "pdfnet.gif").c_str());
		element = f.CreateImage(img, Matrix2D(img.GetImageWidth(), 0, 0, img.GetImageHeight(), 50, 350));
		writer.WritePlacedElement(element);	

		// ----------------------------------------------------------
		// Add a TIFF image to the output file

		img = PDF::Image::Create(doc, (input_path + "grayscale.tif").c_str());
		element = f.CreateImage(img, Matrix2D(img.GetImageWidth(), 0, 0, img.GetImageHeight(), 10, 50));
		writer.WritePlacedElement(element);

		writer.End();           // Save the page
		doc.PagePushBack(page); // Add the page to the document page sequence
        
		// ----------------------------------------------------------
		// Embed a monochrome TIFF. Compress the image using lossy JBIG2 filter.

		page = doc.PageCreate(PDF::Rect(0, 0, 612, 794));
		writer.Begin(page);	// begin writing to this page

		// Note: encoder hints can be used to select between different compression methods. 
		// For example to instruct PDFNet to compress a monochrome image using JBIG2 compression.
		ObjSet hint_set;
		Obj enc=hint_set.CreateArray();  // Initialize encoder 'hint' parameter 
		enc.PushBackName("JBIG2");
		enc.PushBackName("Lossy");

		img = PDF::Image::Create(doc, (input_path + "multipage.tif").c_str(), enc);
		element = f.CreateImage(img, Matrix2D(612, 0, 0, 794, 0, 0));
		writer.WritePlacedElement(element);

		writer.End();           // Save the page
		doc.PagePushBack(page); // Add the page to the document page sequence

		// ----------------------------------------------------------
		// Add a JPEG2000 (JP2) image to the output file

		// Create a new page 
		page = doc.PageCreate();
		writer.Begin(page);	// Begin writing to the page

		// Embed the image.
		img = Image::Create(doc, (input_path + "palm.jp2").c_str());

		// Position the image on the page.
		element = f.CreateImage(img, Matrix2D(img.GetImageWidth(), 0, 0, img.GetImageHeight(), 96, 80));
		writer.WritePlacedElement(element);

		// Write 'JPEG2000 Sample' text string under the image.
		writer.WriteElement(f.CreateTextBegin(Font::Create(doc, Font::e_times_roman), 32));
		element = f.CreateTextRun("JPEG2000 Sample");
		element.SetTextMatrix(1, 0, 0, 1, 190, 30);
		writer.WriteElement(element);
		writer.WriteElement(f.CreateTextEnd());
		
		writer.End();	// Finish writing to the page
		doc.PagePushBack(page);

		// ----------------------------------------------------------

		doc.Save((output_path + "addimage.pdf").c_str(), SDFDoc::e_linearized, 0);
		cout << "Done. Result saved in addimage.pdf..." << endl;
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

