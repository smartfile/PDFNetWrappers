//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

#include <PDF/PDFNet.h>
#include <PDF/PDFDoc.h>
#include <PDF/ElementReader.h>
#include <PDF/Image.h>

//-----------------------------------------------------------------------------------
// This sample illustrates one approach to PDF image extraction 
// using PDFNet.
// 
// Note: Besides direct image export, you can also convert PDF images 
// to GDI+ Bitmap, or extract uncompressed/compressed image data directly 
// using element.GetImageData() (e.g. as illustrated in ElementReaderAdv 
// sample project).
//-----------------------------------------------------------------------------------

#include <iostream>
#include <iomanip>

using namespace std;

using namespace pdftron;
using namespace Common;
using namespace SDF;
using namespace PDF;

// Relative paths to folders containing test files.
string input_path =  "../../TestFiles/";
string output_path = "../../TestFiles/Output/";

int image_counter = 0;

void ImageExtract(ElementReader& reader) 
{
	// Set the precision for printing doubles on cout to 3 decimal places.
	ios iostate(NULL);
	iostate.copyfmt(cout);
	cout << fixed << showpoint << setprecision(3);

	Element element; 
	while ((element = reader.Next()) != 0)
	{
		switch (element.GetType()) 
		{
		case Element::e_image: 
		case Element::e_inline_image: 
			{
				cout << "--> Image: " << ++image_counter << endl;
				cout << "    Width: " << element.GetImageWidth() << endl;
				cout << "    Height: " << element.GetImageHeight() << endl;
				cout << "    BPC: " << element.GetBitsPerComponent() << endl;

				Common::Matrix2D ctm = element.GetCTM();
				double x2=1, y2=1;
				ctm.Mult(x2, y2);
				printf("    Coords: x1=%.2f, y1=%.2f, x2=%.2f, y2=%.2f\n", ctm.m_h, ctm.m_v, x2, y2);

				if (element.GetType() == Element::e_image) 
				{
					Image image(element.GetXObject());

					char fname[256];
					sprintf(fname, "image_extract1_%d", image_counter);

					string path(output_path + fname);
					image.Export(path.c_str());

					//string path(output_path + fname + ".tif");
					//image.ExportAsTiff(path.c_str());

					//string path(output_path + fname + ".png");
					//image.ExportAsPng(path.c_str());
				}
			}
			break;
		case Element::e_form:		// Process form XObjects
			reader.FormBegin(); 
            ImageExtract(reader);
			reader.End(); 
			break; 
		}
	}

	// Reset cout's state.
	cout.copyfmt(iostate);
}

int main(int argc, char *argv[])
{
	int ret = 0;

	// Initialize PDFNet
	PDFNet::Initialize();

	// Example 1: 
	// Extract images by traversing the display list for 
	// every page. With this approach it is possible to obtain 
	// image positioning information and DPI.
	try  
	{	 
		PDFDoc doc((input_path + "newsletter.pdf").c_str());
		doc.InitSecurityHandler();
		ElementReader reader;
		//  Read every page
		for (PageIterator itr=doc.GetPageIterator(); itr.HasNext(); itr.Next()) 
		{				
			reader.Begin(itr.Current());
			ImageExtract(reader);
			reader.End();
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

	cout << "----------------------------------------------------------------" << endl;

	// Example 2: 
	// Extract images by scanning the low-level document.
	try  
	{	 
		PDFDoc doc((input_path + "newsletter.pdf").c_str());

		doc.InitSecurityHandler();
		image_counter = 0;

		SDFDoc& cos_doc=doc.GetSDFDoc();
		int num_objs = cos_doc.XRefSize();
		for(int i=1; i<num_objs; ++i) 
		{
			Obj obj = cos_doc.GetObj(i);
			if(obj && !obj.IsFree() && obj.IsStream()) 
			{
				// Process only images
				DictIterator itr = obj.Find("Type");
				if(!itr.HasNext() || strcmp(itr.Value().GetName(), "XObject"))
					continue;

				itr = obj.Find("Subtype");
				if(!itr.HasNext() || strcmp(itr.Value().GetName(), "Image"))
					continue;
				
				PDF::Image image(obj);
				cout << "--> Image: " << ++image_counter << endl;
				cout << "    Width: " << image.GetImageWidth() << endl;
				cout << "    Height: " << image.GetImageHeight() << endl;
				cout << "    BPC: " << image.GetBitsPerComponent() << endl;

				char fname[256];
				sprintf(fname, "image_extract2_%d", image_counter);
				string path(output_path + fname);
				image.Export(path.c_str());

				//string path(output_path + fname + ".tif");
				//image.ExportAsTiff(path.c_str());

				//string path(output_path + fname + ".png");
				//image.ExportAsPng(path.c_str());
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

