//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

#include <PDF/PDFNet.h>
#include <PDF/PDFDoc.h>
#include <PDF/PDFDraw.h>
#include <Filters/MappedFile.h>
#include <Filters/FilterWriter.h>
#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <SDF/ObjSet.h>

using namespace std;
using namespace pdftron;
using namespace PDF;
using namespace Filters;

//---------------------------------------------------------------------------------------
// The following sample illustrates how to convert PDF documents to various raster image 
// formats (such as PNG, JPEG, BMP, TIFF, etc), as well as how to convert a PDF page to 
// GDI+ Bitmap for further manipulation and/or display in WinForms applications.
//---------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	try 
	{
		// The first step in every application using PDFNet is to initialize the 
		// library and set the path to common PDF resources. The library is usually 
		// initialized only once, but calling Initialize() multiple times is also fine.
		PDFNet::Initialize();

		// Optional: Set ICC color profiles to fine tune color conversion 
		// for PDF 'device' color spaces...

		// PDFNet::SetResourcesPath("../../../resources");
		// PDFNet::SetColorManagement(PDFNet::e_lcms);
		// PDFNet::SetDefaultDeviceCMYKProfile("D:/Misc/ICC/USWebCoatedSWOP.icc");
		// PDFNet::SetDefaultDeviceRGBProfile("AdobeRGB1998.icc"); // will search in PDFNet resource folder.

		// ----------------------------------------------------
		// Optional: Set predefined font mappings to override default font 
		// substitution for documents with missing fonts...

		// PDFNet::AddFontSubst("StoneSans-Semibold", "C:/WINDOWS/Fonts/comic.ttf");
		// PDFNet::AddFontSubst("StoneSans", "comic.ttf");  // search for 'comic.ttf' in PDFNet resource folder.
		// PDFNet::AddFontSubst(PDFNet::e_Identity, "C:/WINDOWS/Fonts/arialuni.ttf");
		// PDFNet::AddFontSubst(PDFNet::e_Japan1, "C:/Program Files/Adobe/Acrobat 7.0/Resource/CIDFont/KozMinProVI-Regular.otf");
		// PDFNet::AddFontSubst(PDFNet::e_Japan2, "c:/myfonts/KozMinProVI-Regular.otf");
		// PDFNet::AddFontSubst(PDFNet::e_Korea1, "AdobeMyungjoStd-Medium.otf");
		// PDFNet::AddFontSubst(PDFNet::e_CNS1, "AdobeSongStd-Light.otf");
		// PDFNet::AddFontSubst(PDFNet::e_GB1, "AdobeMingStd-Light.otf");

		// Relative path to the folder containing test files.
		string input_path =  "../../TestFiles/";
		string output_path = "../../TestFiles/Output/";

		PDFDraw draw;  // PDFDraw class is used to rasterize PDF pages.

		//--------------------------------------------------------------------------------
		// Example 1) Convert the first page to PNG and TIFF at 92 DPI. 
		// A three step tutorial to convert PDF page to an image.
		try 
		{
			// A) Open the PDF document.
			PDFDoc doc((input_path + "tiger.pdf").c_str());

			// Initialize the security handler, in case the PDF is encrypted.
			doc.InitSecurityHandler();  

			// B) The output resolution is set to 92 DPI.
			draw.SetDPI(92);

			// C) Rasterize the first page in the document and save the result as PNG.
			draw.Export(doc.GetPageIterator().Current(), (output_path + "tiger_92dpi.png").c_str());

			cout << "Example 1: tiger_92dpi.png" << endl;

			// Export the same page as TIFF
			draw.Export(doc.GetPageIterator().Current(), (output_path + "tiger_92dpi.tif").c_str(), "TIFF");
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
		// Example 2) Convert the all pages in a given document to JPEG at 72 DPI.
		cout << "Example 2:" << endl;
		SDF::ObjSet hint_set; //  A collection of rendering 'hits'.
		try 
		{
			PDFDoc doc((input_path + "newsletter.pdf").c_str());
			// Initialize the security handler, in case the PDF is encrypted.
			doc.InitSecurityHandler();  

			draw.SetDPI(72); // Set the output resolution is to 72 DPI.

			// Use optional encoder parameter to specify JPEG quality.
			SDF::Obj encoder_param=hint_set.CreateDict();
			encoder_param.PutNumber("Quality", 80);

			// Traverse all pages in the document.
			for (PageIterator itr=doc.GetPageIterator(); itr.HasNext(); itr.Next()) {
				ostringstream sstm;
				sstm << output_path << "newsletter" << itr.Current().GetIndex() << ".jpg";
				string path = sstm.str();
				cout << "newsletter" << itr.Current().GetIndex() << ".jpg" << endl;

				draw.Export(itr.Current(), path.c_str(), "JPEG", encoder_param);
			}

			cout << "Done." << endl;
		}
		catch(Common::Exception& e)	
		{
			cout << e << endl;
		}
		catch(...) 
		{
			cout << "Unknown Exception" << endl;
		}


		// Examples 3-6
		try  
		{				
			// Common code for remaining samples.
			PDFDoc tiger_doc((input_path + "tiger.pdf").c_str());
			// Initialize the security handler, in case the PDF is encrypted.
			tiger_doc.InitSecurityHandler();  
			Page page = tiger_doc.GetPage(1);

			//--------------------------------------------------------------------------------
			// Example 3) Convert the first page to raw bitmap. Also, rotate the 
			// page 90 degrees and save the result as RAW.
			draw.SetDPI(100); // Set the output resolution is to 100 DPI.
			draw.SetRotate(Page::e_90);  // Rotate all pages 90 degrees clockwise.

			int width = 0, height = 0, stride = 0;
			double dpi = 0.0;
			const UChar* buf = draw.GetBitmap(page, width, height, stride, dpi, PDFDraw::e_rgb);

			
			// Save the raw RGB data to disk.
			ofstream outfile((output_path + "tiger_100dpi_rot90.raw").c_str(), ofstream::binary);
			outfile.write((char*)buf, height * stride);
			outfile.close();
			

			cout << "Example 3: tiger_100dpi_rot90.raw" << endl;
			draw.SetRotate(Page::e_0);  // Disable image rotation for remaining samples.

			//--------------------------------------------------------------------------------
			// Example 4) Convert PDF page to a fixed image size. Also illustrates some 
			// other features in PDFDraw class such as rotation, image stretching, exporting 
			// to grayscale, or monochrome.

			// Initialize render 'gray_hint' parameter, that is used to control the 
			// rendering process. In this case we tell the rasterizer to export the image as 
			// 1 Bit Per Component (BPC) image.
			SDF::Obj mono_hint=hint_set.CreateDict();  
			mono_hint.PutNumber("BPC", 1);

			// SetImageSize can be used instead of SetDPI() to adjust page  scaling 
			// dynamically so that given image fits into a buffer of given dimensions.
			draw.SetImageSize(1000, 1000);		// Set the output image to be 1000 wide and 1000 pixels tall
			draw.Export(page, (output_path + "tiger_1000x1000.png").c_str(), "PNG", mono_hint);
			cout << "Example 4: tiger_1000x1000.png" << endl;

			draw.SetImageSize(200, 400);	    // Set the output image to be 200 wide and 300 pixels tall
			draw.SetRotate(Page::e_180);  // Rotate all pages 90 degrees clockwise.

			// 'gray_hint' tells the rasterizer to export the image as grayscale.
			SDF::Obj gray_hint=hint_set.CreateDict();  
			gray_hint.PutName("ColorSpace", "Gray");

			draw.Export(page, (output_path + "tiger_200x400_rot180.png").c_str(), "PNG", gray_hint);
			cout << "Example 4: tiger_200x400_rot180.png" << endl;

			draw.SetImageSize(400, 200, false);  // The third parameter sets 'preserve-aspect-ratio' to false.
			draw.SetRotate(Page::e_0);    // Disable image rotation.
			draw.Export(page, (output_path + "tiger_400x200_stretch.jpg").c_str(), "JPEG");
			cout << "Example 4: tiger_400x200_stretch.jpg" << endl;

			//--------------------------------------------------------------------------------
			// Example 5) Zoom into a specific region of the page and rasterize the 
			// area at 200 DPI and as a thumbnail (i.e. a 50x50 pixel image).
			Rect zoom_rect(216, 522, 330, 600);
			page.SetCropBox(zoom_rect);	// Set the page crop box.

			// Select the crop region to be used for drawing.
			draw.SetPageBox(Page::e_crop); 
			draw.SetDPI(900);  // Set the output image resolution to 900 DPI.
			draw.Export(page, (output_path + "tiger_zoom_900dpi.png").c_str(), "PNG");
			cout << "Example 5: tiger_zoom_900dpi.png" << endl;

			// -------------------------------------------------------------------------------
			// Example 6)
			draw.SetImageSize(50, 50);	   // Set the thumbnail to be 50x50 pixel image.
			draw.Export(page, (output_path + "tiger_zoom_50x50.png").c_str(), "PNG");
			cout << "Example 6: tiger_zoom_50x50.png" << endl;
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
		// Example 7) Convert the first PDF page to CMYK TIFF at 92 DPI.
		// A three step tutorial to convert PDF page to an image
		try 
		{
			pdftron::SDF::Obj cmyk_hint = hint_set.CreateDict();
			cmyk_hint.PutName("ColorSpace", "CMYK");
			// A) Open the PDF document.
			PDFDoc doc((input_path + "tiger.pdf").c_str());
			// Initialize the security handler, in case the PDF is encrypted.
			doc.InitSecurityHandler();  

			// B) The output resolution is set to 92 DPI.
			draw.SetDPI(92);

			// C) Rasterize the first page in the document and save the result as TIFF.
			Page pg = doc.GetPage(1);
			draw.Export(pg, output_path + "out1.tif", "TIFF", cmyk_hint);
			cout << "Example 7: out1.tif" << endl;
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
		// Example 8) PDFRasterizer can be used for more complex rendering tasks, such as 
		// strip by strip or tiled document rendering. In particular, it is useful for 
		// cases where you cannot simply modify the page crop box (interactive viewing,
		// parallel rendering).  This example shows how you can rasterize the south-west
		// quadrant of a page.
		try 
		{
			// A) Open the PDF document.
			PDFDoc doc((input_path + "tiger.pdf").c_str());
			// Initialize the security handler, in case the PDF is encrypted.
			doc.InitSecurityHandler();  

			// B) Get the page matrix 
			Page pg = doc.GetPage(1);
			Page::Box box = Page::e_crop;
			Common::Matrix2D mtx = pg.GetDefaultMatrix(true, box);
			// We want to render a quadrant, so use half of width and height
			const double pg_w = pg.GetPageWidth(box) / 2;
			const double pg_h = pg.GetPageHeight(box) / 2;

			// C) Scale matrix from PDF space to buffer space
			const double dpi = 96.0;
			const double scale = dpi / 72.0; // PDF space is 72 dpi
			const int buf_w = static_cast<int>(floor(scale * pg_w));
			const int buf_h = static_cast<int>(floor(scale * pg_h));
			const int bytes_per_pixel = 4; // BGRA buffer
			const int buf_size = buf_w * buf_h * bytes_per_pixel;
			mtx.Translate(0, -pg_h); // translate by '-pg_h' since we want south-west quadrant
			mtx = Common::Matrix2D(scale, 0, 0, scale, 0, 0) * mtx;

			// D) Rasterize page into memory buffer, according to our parameters
			std::vector<unsigned char> buf;
			PDFRasterizer rast;
			buf = rast.Rasterize(pg, buf_w, buf_h, buf_w * bytes_per_pixel, bytes_per_pixel, true, mtx);

			// buf now contains raw BGRA bitmap.
			cout << "Example 8: Successfully rasterized into memory buffer." << endl;
		}
		catch(Common::Exception& e)	
		{
			cout << e << endl;
		}
		catch (...) {
			cout << "Unknown Exception" << endl;
		}
		//--------------------------------------------------------------------------------
		// Example 9) Export raster content to PNG using different image smoothing settings. 
		try 
		{
			PDFDoc text_doc((input_path + "lorem_ipsum.pdf").c_str());
			text_doc.InitSecurityHandler();

			draw.SetImageSmoothing(false, false);
			string filename = "raster_text_no_smoothing.png";
			draw.Export(text_doc.GetPageIterator().Current(), (output_path + filename).c_str());
			cout << "Example 9 a): " << filename << ". Done." << endl;

			filename = "raster_text_smoothed.png";
			draw.SetImageSmoothing(true, false /*default quality bilinear resampling*/);
			draw.Export(text_doc.GetPageIterator().Current(), (output_path + filename).c_str());
			cout << "Example 9 b): " << filename << ". Done." << endl;

			filename = "raster_text_high_quality.png";
			draw.SetImageSmoothing(true, true /*high quality area resampling*/);
			draw.Export(text_doc.GetPageIterator().Current(), (output_path + filename).c_str());
			cout << "Example 9 c): " << filename << ". Done." << endl;
		}
		catch(Common::Exception& e)	
		{
			cout << e << endl;
		}
		catch (...) {
			cout << "Unknown Exception" << endl;
		}
		//--------------------------------------------------------------------------------
		// Example 10) Export separations directly, without conversion to an output colorspace
		try
		{
			PDFDoc separation_doc((input_path + "op_blend_test.pdf").c_str());
			separation_doc.InitSecurityHandler();
			pdftron::SDF::Obj separation_hint = hint_set.CreateDict();
			separation_hint.PutName("ColorSpace", "Separation");
			draw.SetDPI(96);
			draw.SetImageSmoothing(true, true);
			draw.SetOverprint(PDFRasterizer::e_op_on);

			string filename = "merged_separations.png";
			draw.Export(separation_doc.GetPageIterator().Current(), (output_path + filename).c_str(), "PNG");
			cout << "Example 10 a): " << filename <<". Done." << endl;

			filename = "separation";
			draw.Export(separation_doc.GetPageIterator().Current(), (output_path + filename).c_str(), "PNG", separation_hint);
			cout << "Example 10 b): " << filename <<"_[ink].png. Done." << endl;

			filename = "separation_NChannel.tif";
			draw.Export(separation_doc.GetPageIterator().Current(), (output_path + filename).c_str(), "TIFF", separation_hint);
			cout << "Example 10 c): " << filename << ". Done." << endl;
		}
		catch(Common::Exception& e)	
		{
			cout << e << endl;
		}
		catch (...) {
			cout << "Unknown Exception" << endl;
		}
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
