//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------
using System;
using System.IO;
using System.Collections;
using pdftron;
using pdftron.Common;
using pdftron.SDF;
using pdftron.PDF;

//-----------------------------------------------------------------------------------
// The sample illustrates how multiple pages can be combined/imposed 
// using PDFNet. Page imposition can be used to arrange/order pages 
// prior to printing or to assemble a 'master' page from several 'source' 
// pages. Using PDFNet API it is possible to write applications that can 
// re-order the pages such that they will display in the correct order 
// when the hard copy pages are compiled and folded correctly. 
//-----------------------------------------------------------------------------------

namespace ImpositionTestCS
{
	class Class1
	{
		private static pdftron.PDFNetLoader pdfNetLoader = pdftron.PDFNetLoader.Instance();
		static Class1() {}
		
		static void Main(string[] args)
		{
			PDFNet.Initialize();

			// Relative path to the folder containing test files.
			string input_path =  "../../TestFiles/";
			string output_path = "../../TestFiles/Output/";

			try	
			{    
				Console.WriteLine("-------------------------------------------------");
				Console.WriteLine("Opening the input pdf...");
				using (PDFDoc in_doc = new PDFDoc(input_path + "newsletter.pdf"))
				{
					in_doc.InitSecurityHandler();

					// Create a list of pages to import from one PDF document to another.
					ArrayList import_list = new ArrayList(); 
					for (PageIterator itr = in_doc.GetPageIterator(); itr.HasNext(); itr.Next()) 
						import_list.Add(itr.Current());

					using (PDFDoc new_doc = new PDFDoc()) //  Create a new document
					using (ElementBuilder builder = new ElementBuilder())
					using (ElementWriter  writer  = new ElementWriter())
					{
						ArrayList imported_pages = new_doc.ImportPages(import_list);

						// Paper dimension for A3 format in points. Because one inch has 
						// 72 points, 11.69 inch 72 = 841.69 points
						Rect media_box= new Rect(0, 0, 1190.88, 841.69); 
						double mid_point = media_box.Width()/2;

						for (int i=0; i<imported_pages.Count; ++i)
						{
							// Create a blank new A3 page and place on it two pages from the input document.
							Page new_page = new_doc.PageCreate(media_box);
							writer.Begin(new_page);

							// Place the first page
							Page src_page = (Page)imported_pages[i];
							Element element = builder.CreateForm(src_page);

							double sc_x = mid_point / src_page.GetPageWidth();
							double sc_y = media_box.Height() / src_page.GetPageHeight();
							double scale = Math.Min(sc_x, sc_y);
							element.GetGState().SetTransform(scale, 0, 0, scale, 0, 0);
							writer.WritePlacedElement(element);

							// Place the second page
							++i; 
							if (i<imported_pages.Count)	
							{
								src_page = (Page)imported_pages[i];
								element = builder.CreateForm(src_page);
								sc_x = mid_point / src_page.GetPageWidth();
								sc_y = media_box.Height() / src_page.GetPageHeight();
								scale = Math.Min(sc_x, sc_y);
								element.GetGState().SetTransform(scale, 0, 0, scale, mid_point, 0);
								writer.WritePlacedElement(element);
							}

							writer.End();
							new_doc.PagePushBack(new_page); 
						}
						new_doc.Save(output_path + "newsletter_booklet.pdf", SDFDoc.SaveOptions.e_linearized);
						Console.WriteLine("Done. Result saved in newsletter_booklet.pdf...");
					}
				}
			}
			catch (Exception e)
			{
				Console.WriteLine("Exception caught:\n{0}", e);
			}

		}
	}
}
	

