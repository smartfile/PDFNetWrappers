//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

#include <PDF/PDFNet.h>
#include <PDF/PDFDoc.h>
#include <PDF/PageSet.h>
#include <PDF/Stamper.h>
#include <PDF/Image.h>
#include <iostream>

using namespace std;
using namespace pdftron;
using namespace Common;
using namespace SDF;
using namespace PDF;

//---------------------------------------------------------------------------------------
// The following sample shows how to add new content (or watermark) PDF pages
// using 'pdftron.PDF.Stamper' utility class. 
//
// Stamper can be used to PDF pages with text, images, or with other PDF content 
// in only a few lines of code. Although Stamper is very simple to use compared 
// to ElementBuilder/ElementWriter it is not as powerful or flexible. In case you 
// need full control over PDF creation use ElementBuilder/ElementWriter to add 
// new content to existing PDF pages as shown in the ElementBuilder sample project.
//---------------------------------------------------------------------------------------
int main(int argc, char * argv[])
{
	int ret = 0;
	std::string input_path = "../../TestFiles/";
	std::string output_path = "../../TestFiles/Output/";
	std::string input_filename = "newsletter";	

	PDFNet::Initialize();

	//--------------------------------------------------------------------------------
	// Example 1) Add text stamp to all pages, then remove text stamp from odd pages. 
	try
	{
		pdftron::PDF::PDFDoc doc((input_path + input_filename + ".pdf").c_str());
		doc.InitSecurityHandler();
		Stamper s(pdftron::PDF::Stamper::e_relative_scale, 0.5, 0.5);		

		s.SetAlignment(Stamper::e_horizontal_center, Stamper::e_vertical_center);
		ColorPt red(1, 0, 0); // set text color to red
		s.SetFontColor(red);
		s.StampText(doc, "If you are reading this\nthis is an even page", PageSet(1, doc.GetPageCount()));
		//delete all text stamps in even pages
		Stamper::DeleteStamps(doc, PageSet(1, doc.GetPageCount(), PageSet::e_odd));

		doc.Save(output_path + input_filename + ".ex1.pdf", SDFDoc::e_linearized, NULL);
	}
	catch (Common::Exception& e)
	{
		std::cout << e << endl;
		ret = 1;		
	}
	catch (...)
	{
		cout << "Unknown Exception" << endl;
		ret = 1;
	}

	//--------------------------------------------------------------------------------
	// Example 2) Add Image stamp to first 2 pages. 
	try
	{
		PDFDoc doc(input_path + input_filename + ".pdf");
		doc.InitSecurityHandler();

		Stamper s(Stamper::e_relative_scale, .05, .05);
		Image img = Image::Create(doc, input_path + "peppers.jpg");
		s.SetSize(Stamper::e_relative_scale, 0.5, 0.5);
		//set position of the image to the center, left of PDF pages
		s.SetAlignment(Stamper::e_horizontal_left, Stamper::e_vertical_center);
		ColorPt pt(0, 0, 0, 0);
		s.SetFontColor(pt);
		s.SetRotation(180);
		s.SetAsBackground(false);
		//only stamp first 2 pages
		PageSet ps(1, 2);
		s.StampImage(doc, img, ps);

		doc.Save(output_path + input_filename + ".ex2.pdf", SDFDoc::e_linearized, NULL);
	}
	catch (Common::Exception& e)
	{
		std::cout << e << endl;
		ret = 1;		
	}
	catch (...)
	{
		cout << "Unknown Exception" << endl;
		ret = 1;
	}

	//--------------------------------------------------------------------------------
	// Example 3) Add Page stamp to all pages. 
	try
	{
		PDFDoc doc(input_path + input_filename + ".pdf");
		doc.InitSecurityHandler();

		PDFDoc fish_doc(input_path + "fish.pdf");
		fish_doc.InitSecurityHandler();

		Stamper s(Stamper::e_relative_scale, 0.5, 0.5);
		Page src_page = fish_doc.GetPage(1);		 
		Rect page_one_crop = src_page.GetCropBox();
		// set size of the image to 10% of the original while keep the old aspect ratio
		s.SetSize(Stamper::e_absolute_size, page_one_crop.Width() * 0.1, -1);
		s.SetOpacity(0.4);
		s.SetRotation(-67);
		//put the image at the bottom right hand corner
		s.SetAlignment(Stamper::e_horizontal_right, Stamper::e_vertical_bottom);
		PageSet ps(1, doc.GetPageCount());
		s.StampPage(doc, src_page, ps);

		doc.Save(output_path + input_filename + ".ex3.pdf", SDFDoc::e_linearized, NULL);
	}
	catch (Common::Exception& e)
	{
		std::cout << e << endl;
		ret = 1;		
	}
	catch (...)
	{
		cout << "Unknown Exception" << endl;
		ret = 1;
	}

	//--------------------------------------------------------------------------------
	// Example 4) Add Image stamp to first 20 odd pages.
	try
	{
		PDFDoc doc(input_path + input_filename + ".pdf");
		doc.InitSecurityHandler();

		Stamper s(Stamper::e_absolute_size, 20, 20);
		s.SetOpacity(1);
		s.SetRotation(45);                
		s.SetAsBackground(true);
		s.SetPosition(30, 40);
		Image img = Image::Create(doc, input_path + "peppers.jpg");
		PageSet ps(1, 20, PageSet::e_odd);
		s.StampImage(doc, img, ps);

		doc.Save(output_path + input_filename + ".ex4.pdf", SDFDoc::e_linearized, NULL);
	}
	catch (Common::Exception& e)
	{
		std::cout << e << endl;
		ret = 1;		
	}
	catch (...)
	{
		cout << "Unknown Exception" << endl;
		ret = 1;
	}

	//--------------------------------------------------------------------------------
	// Example 5) Add text stamp to first 20 even pages
	try
	{
		PDFDoc doc(input_path + input_filename + ".pdf");
		doc.InitSecurityHandler();

		Stamper s(Stamper::e_relative_scale, .05, .05);
		s.SetPosition(0, 0);
		s.SetOpacity(0.7);
		s.SetRotation(90);
		s.SetSize(Stamper::e_font_size, 80, -1);
		s.SetTextAlignment(Stamper::e_align_center);
		PageSet ps(1, 20, PageSet::e_even);
		s.StampText(doc, "Goodbye\nMoon", ps);

		doc.Save(output_path + input_filename + ".ex5.pdf", SDFDoc::e_linearized, NULL);
	}
	catch (Common::Exception& e)
	{
		std::cout << e << endl;
		ret = 1;		
	}
	catch (...)
	{
		cout << "Unknown Exception" << endl;
		ret = 1;
	}

	//--------------------------------------------------------------------------------
	// Example 6) Add first page as stamp to all even pages
	try
	{
		PDFDoc doc(input_path + input_filename + ".pdf");
		doc.InitSecurityHandler();

		PDFDoc fish_doc(input_path + "fish.pdf");
		fish_doc.InitSecurityHandler();

		Stamper s(Stamper::e_relative_scale, .3, .3);
		s.SetOpacity(1);                
		s.SetRotation(270);
		s.SetAsBackground(true);
		s.SetPosition(0.5, 0.5, true);
		s.SetAlignment(Stamper::e_horizontal_left, Stamper::e_vertical_bottom);
		Page page_one = fish_doc.GetPage(1);
		PageSet ps(1, doc.GetPageCount(), PageSet::e_even);
		s.StampPage(doc, page_one, ps);

		doc.Save(output_path + input_filename + ".ex6.pdf", SDFDoc::e_linearized, NULL);
	}
	catch (Common::Exception& e)
	{
		std::cout << e << endl;
		ret = 1;		
	}
	catch (...)
	{
		cout << "Unknown Exception" << endl;
		ret = 1;
	}

	//--------------------------------------------------------------------------------
	// Example 7) Add image stamp at top right corner in every pages
	try
	{
		PDFDoc doc(input_path + input_filename + ".pdf");
		doc.InitSecurityHandler();

		Stamper s(Stamper::e_relative_scale, .1, .1);
		s.SetOpacity(0.8);
		s.SetRotation(135);
		s.SetAsBackground(false);
		s.ShowsOnPrint(false);
		s.SetAlignment(Stamper::e_horizontal_left, Stamper::e_vertical_top);
		s.SetPosition(10, 10);

		Image img = Image::Create(doc, input_path + "peppers.jpg");
		PageSet ps(1, doc.GetPageCount(), PageSet::e_all);
		s.StampImage(doc, img, ps);

		doc.Save(output_path + input_filename + ".ex7.pdf", SDFDoc::e_linearized, NULL);
	}
	catch (Common::Exception& e)
	{
		std::cout << e << endl;
		ret = 1;		
	}
	catch (...)
	{
		cout << "Unknown Exception" << endl;
		ret = 1;
	}

	//--------------------------------------------------------------------------------
	// Example 8) Add Text stamp to first 2 pages, and image stamp to first page.
	//          Because text stamp is set as background, the image is top of the text
	//          stamp. Text stamp on the first page is not visible.
	try
	{
		PDFDoc doc(input_path + input_filename + ".pdf");
		doc.InitSecurityHandler();

		Stamper s(Stamper::e_relative_scale, 0.07, -0.1);
		s.SetAlignment(Stamper::e_horizontal_right, Stamper::e_vertical_bottom);
		s.SetAlignment(Stamper::e_horizontal_center, Stamper::e_vertical_top);
		s.SetFont(Font::Create(doc, Font::e_courier, true));
		ColorPt red(1, 0, 0, 0);
		s.SetFontColor(red); //set color to red
		s.SetTextAlignment(Stamper::e_align_right);
		s.SetAsBackground(true); //set text stamp as background
		PageSet ps(1, 2);
		s.StampText(doc, "This is a title!", ps);

		Image img = Image::Create(doc, input_path + "peppers.jpg");
		s.SetAsBackground(false); // set image stamp as foreground
		PageSet first_page_ps(1);
		s.StampImage(doc, img, first_page_ps);

		doc.Save(output_path + input_filename + ".ex8.pdf", SDFDoc::e_linearized, NULL);
	}
	catch (Common::Exception& e)
	{
		std::cout << e << endl;
		ret = 1;		
	}
	catch (...)
	{
		cout << "Unknown Exception" << endl;
		ret = 1;
	}

	PDFNet::Terminate();
	return ret;
}
