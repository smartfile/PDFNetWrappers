//
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
//

using System;
using System.IO;
using System.Collections;
using pdftron;
using pdftron.Common;
using pdftron.Filters;
using pdftron.SDF;
using pdftron.PDF;

namespace PDFPageTestCS
{
	/// <summary>
	/// Summary description for Class1.
	/// </summary>
	class Class1
	{
		private static pdftron.PDFNetLoader pdfNetLoader = pdftron.PDFNetLoader.Instance();
		static Class1() {}
		
		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main(string[] args)
		{
			PDFNet.Initialize();

			// Relative path to the folder containing test files.
			string input_path =  "../../TestFiles/";
			string output_path = "../../TestFiles/Output/";
			
			// Sample 1 - Split a PDF document into multiple pages
			try
			{
				Console.WriteLine("_______________________________________________");
				Console.WriteLine("Sample 1 - Split a PDF document into multiple pages...");
				Console.WriteLine("Opening the input pdf...");

				using (PDFDoc in_doc = new PDFDoc(input_path + "newsletter.pdf"))
				{
					in_doc.InitSecurityHandler();

					int page_num = in_doc.GetPageCount();
					for (int i = 1; i <= page_num; ++i) 
					{
						using (PDFDoc new_doc = new PDFDoc()) 
						{
							new_doc.InsertPages(0, in_doc, i, i, PDFDoc.InsertFlag.e_none);
							new_doc.Save(output_path + "newsletter_split_page_" + i + ".pdf", SDFDoc.SaveOptions.e_remove_unused);
							Console.WriteLine("Done. Result saved in newsletter_split_page_" + i + ".pdf");
						}
					}
				}
			}
			catch (Exception e)
			{
				Console.WriteLine("Exception caught:\n{0}", e);
			}

			// Sample 2 - Merge several PDF documents into one
			try
			{
				Console.WriteLine("_______________________________________________");
				Console.WriteLine("Sample 2 - Merge several PDF documents into one...");

				using (PDFDoc new_doc = new PDFDoc()) 
				{
					new_doc.InitSecurityHandler();
					int page_num = 15;
					for (int i = 1; i <= page_num; ++i) 
					{
						Console.WriteLine("Opening newsletter_split_page_" + i + ".pdf");
						using (PDFDoc in_doc = new PDFDoc(output_path + "newsletter_split_page_" + i + ".pdf")) 
						{
							new_doc.InsertPages(i, in_doc, 1, in_doc.GetPageCount(), PDFDoc.InsertFlag.e_none);
						}
					}
					new_doc.Save(output_path + "newsletter_merge_pages.pdf", SDFDoc.SaveOptions.e_remove_unused);
				}
				Console.WriteLine("Done. Result saved in newsletter_merge_pages.pdf");
			}
			catch (Exception e)
			{
				Console.WriteLine("Exception caught:\n{0}", e);
			}


			// Sample 3 - Delete every second page
			try	
			{
				Console.WriteLine("_______________________________________________");
				Console.WriteLine("Sample 3 - Delete every second page...");
				Console.WriteLine("Opening the input pdf...");

				using (PDFDoc in_doc = new PDFDoc(input_path +  "newsletter.pdf"))
				{
					in_doc.InitSecurityHandler();

					int page_num = in_doc.GetPageCount();
					PageIterator itr;
					while (page_num>=1)
					{
						itr = in_doc.GetPageIterator(page_num);
						in_doc.PageRemove(itr);
						page_num -= 2;
					}		

					in_doc.Save(output_path +  "newsletter_page_remove.pdf", 0);
				}
				Console.WriteLine("Done. Result saved in newsletter_page_remove.pdf...");
			}
			catch(Exception e)
			{
				Console.WriteLine("Exception caught:\n{0}", e);
			}

			// Sample 4 - Inserts a page from one document at different
			// locations within another document                       
			try   
			{	 
				Console.WriteLine("_______________________________________________");
				Console.WriteLine("Sample 4 - Insert a page at different locations...");
				Console.WriteLine("Opening the input pdf...");

				using (PDFDoc in1_doc = new PDFDoc(input_path +  "newsletter.pdf"))
				using (PDFDoc in2_doc = new PDFDoc(input_path + "fish.pdf"))
				{
					in1_doc.InitSecurityHandler();
					in2_doc.InitSecurityHandler();

					Page src_page = in2_doc.GetPage(1);
					PageIterator dst_page = in1_doc.GetPageIterator(1);
					int page_num = 1;
					while (dst_page.HasNext()) {
						if (page_num++ % 3 == 0) {
							in1_doc.PageInsert(dst_page, src_page);
						}
						dst_page.Next();
					}

					in1_doc.Save(output_path +  "newsletter_page_insert.pdf", 0);
					Console.WriteLine("Done. Result saved in newsletter_page_insert.pdf...");
				}
			}
			catch(Exception e)
			{
				Console.WriteLine("Exception caught:\n{0}", e);
			}

			// Sample 5 - Replicate pages within a single document
			try	
			{
				Console.WriteLine("_______________________________________________");
				Console.WriteLine("Sample 5 - Replicate pages within a single document...");
				Console.WriteLine("Opening the input pdf...");
				using (PDFDoc doc = new PDFDoc(input_path +  "newsletter.pdf")) 
				{
					doc.InitSecurityHandler();

					// Replicate the cover page three times (copy page #1 and place it before the 
					// seventh page in the document page sequence)
					Page cover = doc.GetPage(1);
                    PageIterator p7 = doc.GetPageIterator(7);
                    doc.PageInsert(p7, cover);
					doc.PageInsert(p7, cover);
					doc.PageInsert(p7, cover);

					// Replicate the cover page two more times by placing it before and after
					// existing pages.
					doc.PagePushFront(cover);
					doc.PagePushBack(cover);

					doc.Save(output_path +  "newsletter_page_clone.pdf", 0);
					Console.WriteLine("Done. Result saved in newsletter_page_clone.pdf...");
				}
			}
			catch(Exception e)
			{
				Console.WriteLine("Exception caught:\n{0}", e);
			}

			// Sample 6 - Use ImportPages() in order to copy multiple pages at once    
			// in order to preserve shared resources between pages (e.g. images, fonts,
			// colorspaces, etc.)
			try	
			{    
				Console.WriteLine("_______________________________________________");
				Console.WriteLine("Sample 6 - Preserving shared resources using ImportPages...");
				Console.WriteLine("Opening the input pdf...");
				using (PDFDoc in_doc = new PDFDoc(input_path +  "newsletter.pdf")) 
				{
					in_doc.InitSecurityHandler();
					using (PDFDoc new_doc = new PDFDoc()) 
					{
						ArrayList copy_pages = new ArrayList(); 
						for (PageIterator itr = in_doc.GetPageIterator(); itr.HasNext(); itr.Next()) {
							copy_pages.Add(itr.Current());
						}

						ArrayList imported_pages = new_doc.ImportPages(copy_pages);
						for (int i=0; i!=imported_pages.Count; ++i) {
							new_doc.PagePushFront((Page)imported_pages[i]); // Order pages in reverse order. 
							// Use PagePushBack() if you would like to preserve the same order.
						}
					
						new_doc.Save(output_path +  "newsletter_import_pages.pdf", 0);
						Console.WriteLine("Done. Result saved in newsletter_import_pages.pdf...");
						Console.WriteLine();
						Console.WriteLine("Note that the output file size is less than half the size"); 
						Console.WriteLine("of the file produced using individual page copy operations");
						Console.WriteLine("between two documents");
					}
				}
			}
			catch(Exception e)
			{
				Console.WriteLine("Exception caught:\n{0}", e);
			}
		}
	}
}
