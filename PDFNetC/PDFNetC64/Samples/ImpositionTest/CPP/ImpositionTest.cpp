//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

#include <PDF/PDFNet.h>
#include <PDF/PDFDoc.h>
#include <PDF/ElementBuilder.h>
#include <PDF/ElementWriter.h>
#include <PDF/ElementReader.h>
#include <iostream>

using namespace std;
using namespace pdftron;
using namespace Common;
using namespace SDF;
using namespace PDF;

//-----------------------------------------------------------------------------------
// The sample illustrates how multiple pages can be combined/imposed 
// using PDFNet. Page imposition can be used to arrange/order pages 
// prior to printing or to assemble a 'master' page from several 'source' 
// pages. Using PDFNet API it is possible to write applications that can 
// re-order the pages such that they will display in the correct order 
// when the hard copy pages are compiled and folded correctly. 
//-----------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	int ret = 0;
	PDFNet::Initialize();

	const char* resource_path = argc>3 ? argv[3] : "../../../resources";

	// Relative path to the folder containing test files.
	string input_path =  "../../TestFiles/newsletter.pdf";
	string output_path = "../../TestFiles/Output/newsletter_booklet.pdf";

	try
	{	 
		cout << "-------------------------------------------------" << endl;
		cout << "Opening the input pdf..." << endl;

		const char* filein = argc>1 ? argv[1] : input_path.c_str();
		const char* fileout = argc>2 ? argv[2] : output_path.c_str();

		PDFDoc in_doc(filein);
		in_doc.InitSecurityHandler();

		// Create a list of pages to import from one PDF document to another.
		vector<Page> import_pages; 
		for (PageIterator itr=in_doc.GetPageIterator(); itr.HasNext(); itr.Next())
			import_pages.push_back(itr.Current());

		PDFDoc new_doc;
		vector<Page> imported_pages = new_doc.ImportPages(import_pages);

		// Paper dimension for A3 format in points. Because one inch has 
		// 72 points, 11.69 inch 72 = 841.69 points
		Rect media_box(0, 0, 1190.88, 841.69); 
		double mid_point = media_box.Width()/2;

		ElementBuilder builder;
		ElementWriter  writer;
	
		for (size_t i=0; i<imported_pages.size(); ++i)
		{
			// Create a blank new A3 page and place on it two pages from the input document.
			Page new_page = new_doc.PageCreate(media_box);
			writer.Begin(new_page);

			// Place the first page
			Page src_page = imported_pages[i];
			Element element = builder.CreateForm(src_page);

			double sc_x = mid_point / src_page.GetPageWidth();
			double sc_y = media_box.Height() / src_page.GetPageHeight();
			double scale = sc_x < sc_y ? sc_x : sc_y; // min(sc_x, sc_y)
			element.GetGState().SetTransform(scale, 0, 0, scale, 0, 0);
			writer.WritePlacedElement(element);
			
			// Place the second page
			++i; 
			if (i<imported_pages.size())	{
				src_page = imported_pages[i];
				element = builder.CreateForm(src_page);
				sc_x = mid_point / src_page.GetPageWidth();
				sc_y = media_box.Height() / src_page.GetPageHeight();
				scale = sc_x < sc_y ? sc_x : sc_y; // min(sc_x, sc_y)
				element.GetGState().SetTransform(scale, 0, 0, scale, mid_point, 0);
				writer.WritePlacedElement(element);
			}

			writer.End();
			new_doc.PagePushBack(new_page);
		}		

		new_doc.Save(fileout, SDFDoc::e_linearized, 0);
		cout << "Done. Result saved in newsletter_booklet.pdf..." << endl;
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
