//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

#include <SDF/NameTree.h>
#include <PDF/PDFNet.h>
#include <PDF/PDFDoc.h>
#include <PDF/PDFDraw.h>
#include <PDF/ElementBuilder.h>
#include <PDF/ElementWriter.h>
#include <Filters/MappedFile.h>

#include <iostream>

using namespace pdftron;
using namespace Common;
using namespace Filters;
using namespace SDF;
using namespace PDF;
using namespace std;

//-----------------------------------------------------------------------------------
/// This sample illustrates how to create, extract, and manipulate PDF Portfolios
/// (a.k.a. PDF Packages) using PDFNet SDK.
//-----------------------------------------------------------------------------------

// Relative path to the folder containing test files.
static const string input_path =  "../../TestFiles/";
static const string output_path = "../../TestFiles/Output/";

static void AddPackage(PDFDoc& doc, string file, const char* desc) 
{
	NameTree files = NameTree::Create(doc, "EmbeddedFiles");
	FileSpec fs = FileSpec::Create(doc, file.c_str(), true);
	files.Put((UChar*)file.c_str(), int(file.size()), fs.GetSDFObj());
	fs.SetDesc(desc);

	Obj collection = doc.GetRoot().FindObj("Collection");
	if (!collection) collection = doc.GetRoot().PutDict("Collection");

	// You could here manipulate any entry in the Collection dictionary. 
	// For example, the following line sets the tile mode for initial view mode
	// Please refer to section '2.3.5 Collections' in PDF Reference for details.
	collection.PutName("View", "T");
}

static void AddCoverPage(PDFDoc& doc) 
{
	// Here we dynamically generate cover page (please see ElementBuilder 
	// sample for more extensive coverage of PDF creation API).
	Page page = doc.PageCreate(Rect(0, 0, 200, 200));

	ElementBuilder b;
	ElementWriter w;
	w.Begin(page);
	Font font = Font::Create(doc, Font::e_helvetica);
	w.WriteElement(b.CreateTextBegin(font, 12));
	Element e = b.CreateTextRun("My PDF Collection");
	e.SetTextMatrix(1, 0, 0, 1, 50, 96);
	e.GetGState().SetFillColorSpace(ColorSpace::CreateDeviceRGB());
	e.GetGState().SetFillColor(ColorPt(1, 0, 0));
	w.WriteElement(e);
	w.WriteElement(b.CreateTextEnd());
	w.End();
	doc.PagePushBack(page);

	// Alternatively we could import a PDF page from a template PDF document
	// (for an example please see PDFPage sample project).
	// ...
}

int main(int argc, char *argv[])
{
	int ret = 0;
	PDFNet::Initialize();

	// Create a PDF Package.
	try
	{
		PDFDoc doc;
		AddPackage(doc, input_path + "numbered.pdf", "My File 1");
		AddPackage(doc, input_path + "newsletter.pdf", "My Newsletter...");
		AddPackage(doc, input_path + "peppers.jpg", "An image");
		AddCoverPage(doc);
		doc.Save((output_path + "package.pdf").c_str(), SDFDoc::e_linearized, 0);
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

	// Extract parts from a PDF Package.
	try  
	{	 
		PDFDoc doc((output_path + "package.pdf").c_str());
		doc.InitSecurityHandler();

		NameTree files = NameTree::Find(doc, "EmbeddedFiles");
		if(files.IsValid()) 
		{ 
			// Traverse the list of embedded files.
			NameTreeIterator i = files.GetIterator();
			for (int counter = 0; i.HasNext(); i.Next(), ++counter) 
			{
				UString entry_name;
				i.Key().GetAsPDFText(entry_name);
				cout << "Part: " << entry_name.ConvertToAscii() << endl;
				FileSpec file_spec(i.Value());
				Filter stm(file_spec.GetFileData());
				if (stm) 
				{
					string tmp = entry_name.ConvertToUtf8();
					string ext = tmp.find_last_of(".") != string::npos ? tmp.substr(tmp.find_last_of(".") + 1) : "pdf";
					char tmpbuf[1024];
					sprintf(tmpbuf, "%sextract_%d.%s", output_path.c_str(), counter, ext.c_str());
					stm.WriteToFile(UString(tmpbuf), false);
				}
			}
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

