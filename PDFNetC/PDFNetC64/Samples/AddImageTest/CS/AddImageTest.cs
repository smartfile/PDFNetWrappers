//
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
//

using System;
using pdftron;
using pdftron.Common;
using pdftron.Filters;
using pdftron.SDF;
using pdftron.PDF;

namespace AddImageTestCS
{
	class Class1
	{
        private static pdftron.PDFNetLoader pdfNetLoader = pdftron.PDFNetLoader.Instance();
        static Class1() {}

		/// <summary>
		//-----------------------------------------------------------------------------------
		// This sample illustrates how to embed various raster image formats 
		// (e.g. TIFF, JPEG, JPEG2000, JBIG2, GIF, PNG, BMP, etc.) in a PDF document.
		//-----------------------------------------------------------------------------------
		/// </summary>
		static void Main(string[] args)
		{
			PDFNet.Initialize();

			// Relative path to the folder containing test files.
			string input_path =  "../../TestFiles/";
			string output_path = "../../TestFiles/Output/";

			try
			{
				using (PDFDoc doc = new PDFDoc())
				using (ElementBuilder bld = new ElementBuilder())	// Used to build new Element objects
				using (ElementWriter writer = new ElementWriter())	// Used to write Elements to the page	
				{
					Page page = doc.PageCreate();	// Start a new page 
					writer.Begin(page);				// Begin writing to this page

					// ----------------------------------------------------------
					// Embed a JPEG image to the output document. 
					Image img = Image.Create(doc, input_path + "peppers.jpg");

					// You can also directly add any .NET Bitmap. The following commented-out code 
					// is equivalent to the above line:
					//    System.Drawing.Bitmap bmp;
                    //    System.Drawing.Bitmap bmp = new System.Drawing.Bitmap(input_path + "peppers.jpg");
                    //    Image img = Image.Create(doc, bmp);

                    Element element = bld.CreateImage(img, 50, 500, img.GetImageWidth() / 2, img.GetImageHeight() / 2);
					writer.WritePlacedElement(element);

					// ----------------------------------------------------------
					// Add a PNG image to the output file
					img = Image.Create(doc, input_path + "butterfly.png");
                    element = bld.CreateImage(img, new Matrix2D(100, 0, 0, 100, 300, 500));
                    writer.WritePlacedElement(element);
			
					// ----------------------------------------------------------
					// Add a GIF image to the output file
					img = Image.Create(doc, input_path + "pdfnet.gif");
					element = bld.CreateImage(img, new Matrix2D(img.GetImageWidth(), 0, 0, img.GetImageHeight(), 50, 350));
                    writer.WritePlacedElement(element);
			
					// ----------------------------------------------------------
					// Add a TIFF image to the output file
					img = Image.Create(doc, input_path + "grayscale.tif");
					element = bld.CreateImage(img, new Matrix2D(img.GetImageWidth(), 0, 0, img.GetImageHeight(), 10, 50));
					writer.WritePlacedElement(element);

                    writer.End();           // Save the page
                    doc.PagePushBack(page); // Add the page to the document page sequence

                    // ----------------------------------------------------------
                    // Add a BMP image to the output file
                    /*
                    bmp = new System.Drawing.Bitmap(input_path + "pdftron.bmp");
					img = Image.Create(doc, bmp);
					element = bld.CreateImage(img, new Matrix2D(bmp.Width, 0, 0, bmp.Height, 255, 700));
					writer.WritePlacedElement(element);
			
					writer.End();	// Finish writing to the page
					doc.PagePushBack(page);
                    */

                    // ----------------------------------------------------------
                    // Embed a monochrome TIFF. Compress the image using lossy JBIG2 filter.

                    page = doc.PageCreate(new pdftron.PDF.Rect(0, 0, 612, 794));
                    writer.Begin(page); // begin writing to this page

                    // Note: encoder hints can be used to select between different compression methods. 
                    // For example to instruct PDFNet to compress a monochrome image using JBIG2 compression.
                    ObjSet hint_set = new ObjSet();
                    Obj enc = hint_set.CreateArray();  // Initialize encoder 'hint' parameter 
                    enc.PushBackName("JBIG2");
                    enc.PushBackName("Lossy");

                    img = pdftron.PDF.Image.Create(doc, input_path + "multipage.tif", enc);
                    element = bld.CreateImage(img, new Matrix2D(612, 0, 0, 794, 0, 0));
                    writer.WritePlacedElement(element);

                    writer.End();           // Save the page
                    doc.PagePushBack(page); // Add the page to the document page sequence

                    // ----------------------------------------------------------
                    // Add a JPEG2000 (JP2) image to the output file

                    // Create a new page 
                    page = doc.PageCreate();
                    writer.Begin(page); // Begin writing to the page

                    // Embed the image.
                    img = pdftron.PDF.Image.Create(doc, input_path + "palm.jp2");

                    // Position the image on the page.
                    element = bld.CreateImage(img, new Matrix2D(img.GetImageWidth(), 0, 0, img.GetImageHeight(), 96, 80));
                    writer.WritePlacedElement(element);

                    // Write 'JPEG2000 Sample' text string under the image.
                    writer.WriteElement(bld.CreateTextBegin(pdftron.PDF.Font.Create(doc, pdftron.PDF.Font.StandardType1Font.e_times_roman), 32));
                    element = bld.CreateTextRun("JPEG2000 Sample");
                    element.SetTextMatrix(1, 0, 0, 1, 190, 30);
                    writer.WriteElement(element);
                    writer.WriteElement(bld.CreateTextEnd());

                    writer.End();   // Finish writing to the page
                    doc.PagePushBack(page);


                    doc.Save(output_path + "addimage.pdf", SDFDoc.SaveOptions.e_linearized);
					Console.WriteLine("Done. Result saved in addimage.pdf...");
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}

		}
	}
}
