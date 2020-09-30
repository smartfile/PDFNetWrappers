//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

using System;
using System.IO;

using pdftron;
using pdftron.Common;
using pdftron.Filters;
using pdftron.SDF;
using pdftron.PDF;

namespace PDFNetSamples
{
	class StamperSample
	{
		private static pdftron.PDFNetLoader pdfNetLoader = pdftron.PDFNetLoader.Instance();
		static StamperSample() {}
		
		/// <summary>
		// The following sample shows how to add new content (or watermark) PDF pages
		// using 'pdftron.PDF.Stamper' utility class. 
		//
		// Stamper can be used to PDF pages with text, images, or with other PDF content 
		// in only a few lines of code. Although Stamper is very simple to use compared 
		// to ElementBuilder/ElementWriter it is not as powerful or flexible. In case you 
		// need full control over PDF creation use ElementBuilder/ElementWriter to add 
		// new content to existing PDF pages as shown in the ElementBuilder sample project.
		/// </summary>
		static void Main(string[] args)
		{
			PDFNet.Initialize();

			string input_path = "../../TestFiles/";
			string output_path = "../../TestFiles/Output/";
			string input_filename = "newsletter";

			//--------------------------------------------------------------------------------
			// Example 1) Add text stamp to all pages, then remove text stamp from odd pages. 
			try
			{
				using (PDFDoc doc = new PDFDoc(input_path + input_filename + ".pdf"))
				using (Stamper s = new Stamper(Stamper.SizeType.e_relative_scale, 0.5, 0.5))
				{
					doc.InitSecurityHandler();

					s.SetAlignment(Stamper.HorizontalAlignment.e_horizontal_center, Stamper.VerticalAlignment.e_vertical_center);
					s.SetFontColor(new ColorPt(1, 0, 0)); // set text color to red                
					s.StampText(doc, "If you are reading this\nthis is an even page", new PageSet(1, doc.GetPageCount()));
					//delete all text stamps in odd pages
					Stamper.DeleteStamps(doc, new PageSet(1, doc.GetPageCount(), PageSet.Filter.e_odd));

					doc.Save(output_path + input_filename + ".ex1.pdf", SDFDoc.SaveOptions.e_linearized);
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}

			//--------------------------------------------------------------------------------
			// Example 2) Add Image stamp to first 2 pages. 
			try
			{
				using (PDFDoc doc = new PDFDoc(input_path + input_filename + ".pdf"))
				using (Stamper s = new Stamper(Stamper.SizeType.e_relative_scale, .05, .05))
				{
					doc.InitSecurityHandler();

					Image img = Image.Create(doc, input_path + "peppers.jpg");
					s.SetSize(Stamper.SizeType.e_relative_scale, 0.5, 0.5);
					//set position of the image to the center, left of PDF pages
					s.SetAlignment(Stamper.HorizontalAlignment.e_horizontal_left, Stamper.VerticalAlignment.e_vertical_center);
					s.SetFontColor(new ColorPt(0, 0, 0, 0));
					s.SetRotation(180);
					s.SetAsBackground(false);
					//only stamp first 2 pages
					s.StampImage(doc, img, new PageSet(1, 2));

					doc.Save(output_path + input_filename + ".ex2.pdf", SDFDoc.SaveOptions.e_linearized);
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}

			//--------------------------------------------------------------------------------
			// Example 3) Add Page stamp to all pages. 
			try
			{
				using (PDFDoc doc = new PDFDoc(input_path + input_filename + ".pdf"))
				using (PDFDoc fish_doc = new PDFDoc(input_path + "fish.pdf"))
				using (Stamper s = new Stamper(Stamper.SizeType.e_relative_scale, .5, .5))
				{
					doc.InitSecurityHandler();

					fish_doc.InitSecurityHandler();

					Page src_page = fish_doc.GetPage(1);
					Rect page_one_crop = src_page.GetCropBox();
					// set size of the image to 10% of the original while keep the old aspect ratio
					s.SetSize(Stamper.SizeType.e_absolute_size, page_one_crop.Width() * 0.1, -1);
					s.SetOpacity(0.4);
					s.SetRotation(-67);
					//put the image at the bottom right hand corner
					s.SetAlignment(Stamper.HorizontalAlignment.e_horizontal_right, Stamper.VerticalAlignment.e_vertical_bottom);
					s.StampPage(doc, src_page, new PageSet(1, doc.GetPageCount()));

					doc.Save(output_path + input_filename + ".ex3.pdf", SDFDoc.SaveOptions.e_linearized);
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}

			//--------------------------------------------------------------------------------
			// Example 4) Add Image stamp to first 20 odd pages. 
			try
			{
				using (PDFDoc doc = new PDFDoc(input_path + input_filename + ".pdf"))
				using (Stamper s = new Stamper(Stamper.SizeType.e_absolute_size, 20, 20))
				{
					doc.InitSecurityHandler();

					s.SetOpacity(1);
					s.SetRotation(45);
					s.SetAsBackground(true);
					s.SetPosition(30, 40);
					Image img = Image.Create(doc, input_path + "peppers.jpg");
					s.StampImage(doc, img, new PageSet(1, 20, PageSet.Filter.e_odd));

					doc.Save(output_path + input_filename + ".ex4.pdf", SDFDoc.SaveOptions.e_linearized);
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}

			//--------------------------------------------------------------------------------
			// Example 5) Add text stamp to first 20 even pages
			try
			{
				using (PDFDoc doc = new PDFDoc(input_path + input_filename + ".pdf"))
				using (Stamper s = new Stamper(Stamper.SizeType.e_relative_scale, .05, .05))
				{
					doc.InitSecurityHandler();

					s.SetPosition(0, 0);
					s.SetOpacity(0.7);
					s.SetRotation(90);
					s.SetSize(Stamper.SizeType.e_font_size, 80, -1);
					s.SetTextAlignment(Stamper.TextAlignment.e_align_center);
					s.StampText(doc, "Goodbye\nMoon", new PageSet(1, 20, PageSet.Filter.e_even));

					doc.Save(output_path + input_filename + ".ex5.pdf", SDFDoc.SaveOptions.e_linearized);
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}

			//--------------------------------------------------------------------------------
			// Example 6) Add first page as stamp to all even pages
			try
			{
				using (PDFDoc doc = new PDFDoc(input_path + input_filename + ".pdf"))
				using (PDFDoc fish_doc = new PDFDoc(input_path + "fish.pdf"))
				using (Stamper s = new Stamper(Stamper.SizeType.e_relative_scale, .3, .3))
				{
					doc.InitSecurityHandler();

					fish_doc.InitSecurityHandler();

					s.SetOpacity(1);
					s.SetRotation(270);
					s.SetAsBackground(true);
					s.SetPosition(0.5, 0.5, true);
					s.SetAlignment(Stamper.HorizontalAlignment.e_horizontal_left, Stamper.VerticalAlignment.e_vertical_bottom);
					Page page_one = fish_doc.GetPage(1);
					s.StampPage(doc, page_one, new PageSet(1, doc.GetPageCount(), PageSet.Filter.e_even));

					doc.Save(output_path + input_filename + ".ex6.pdf", SDFDoc.SaveOptions.e_linearized);
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}

			//--------------------------------------------------------------------------------
			// Example 7) Add image stamp at top right corner in every pages
			try
			{
				using (PDFDoc doc = new PDFDoc(input_path + input_filename + ".pdf"))
				using (Stamper s = new Stamper(Stamper.SizeType.e_relative_scale, .1, .1))
				{
					doc.InitSecurityHandler();

					s.SetOpacity(0.8);
					s.SetRotation(135);
					s.SetAsBackground(false);
					s.ShowsOnPrint(false);
					s.SetAlignment(Stamper.HorizontalAlignment.e_horizontal_left, Stamper.VerticalAlignment.e_vertical_top);
					s.SetPosition(10, 10);

					Image img = Image.Create(doc, input_path + "peppers.jpg");
					s.StampImage(doc, img, new PageSet(1, doc.GetPageCount(), PageSet.Filter.e_all));

					doc.Save(output_path + input_filename + ".ex7.pdf", SDFDoc.SaveOptions.e_linearized);
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}

			//--------------------------------------------------------------------------------
			// Example 8) Add Text stamp to first 2 pages, and image stamp to first page.
			//          Because text stamp is set as background, the image is top of the text
			//          stamp. Text stamp on the first page is not visible.
			try
			{
				using (PDFDoc doc = new PDFDoc(input_path + input_filename + ".pdf"))
				using (Stamper s = new Stamper(Stamper.SizeType.e_relative_scale, 0.07, -0.1))
				{
					doc.InitSecurityHandler();

					s.SetAlignment(Stamper.HorizontalAlignment.e_horizontal_right, Stamper.VerticalAlignment.e_vertical_bottom);
					s.SetAlignment(Stamper.HorizontalAlignment.e_horizontal_center, Stamper.VerticalAlignment.e_vertical_top);
					s.SetFont(Font.Create(doc, Font.StandardType1Font.e_courier, true));
					s.SetFontColor(new ColorPt(1, 0, 0, 0)); //set color to red
					s.SetTextAlignment(Stamper.TextAlignment.e_align_right);
					s.SetAsBackground(true); //set text stamp as background
					s.StampText(doc, "This is a title!", new PageSet(1, 2));

					Image img = Image.Create(doc, input_path + "peppers.jpg");
					s.SetAsBackground(false); // set image stamp as foreground
					s.StampImage(doc, img, new PageSet(1));

					doc.Save(output_path + input_filename + ".ex8.pdf", SDFDoc.SaveOptions.e_linearized);
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}

		}
	}
}
