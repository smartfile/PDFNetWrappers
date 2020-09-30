//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

using System;
using pdftron;
using pdftron.Common;
using pdftron.Filters;
using pdftron.SDF;
using pdftron.PDF;

namespace ContentReplacerTestCS
{
	/// <summary>
	//-----------------------------------------------------------------------------------------
	// The sample code illustrates how to use the ContentReplacer class to make using 
	// 'template' pdf documents easier.
	//-----------------------------------------------------------------------------------------
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
			string input_path = "../../TestFiles/";
			string output_path = "../../TestFiles/Output/";


			// The following example illustrates how to replace an image in a certain region,
			// and how to change template text.
			try
			{
				using (PDFDoc doc = new PDFDoc(input_path + "BusinessCardTemplate.pdf"))
				using (ContentReplacer replacer = new ContentReplacer())
				{
					doc.InitSecurityHandler();

					// first, replace the image on the first page
					Page page = doc.GetPage(1);
					Image img = Image.Create(doc, input_path + "peppers.jpg");
					replacer.AddImage(page.GetMediaBox(), img.GetSDFObj());
					// next, replace the text place holders on the second page
					replacer.AddString("NAME", "John Smith");
					replacer.AddString("QUALIFICATIONS", "Philosophy Doctor"); 
					replacer.AddString("JOB_TITLE", "Software Developer"); 
					replacer.AddString("ADDRESS_LINE1", "#100 123 Software Rd"); 
					replacer.AddString("ADDRESS_LINE2", "Vancouver, BC"); 
					replacer.AddString("PHONE_OFFICE", "604-730-8989"); 
					replacer.AddString("PHONE_MOBILE", "604-765-4321"); 
					replacer.AddString("EMAIL", "info@pdftron.com"); 
					replacer.AddString("WEBSITE_URL", "http://www.pdftron.com"); 
					// finally, apply
					replacer.Process(page);

					doc.Save(output_path + "BusinessCard.pdf", 0);
					Console.WriteLine("Done. Result saved in BusinessCard.pdf");
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}


			// The following example illustrates how to replace text in a given region
			try
			{
				using (PDFDoc doc = new PDFDoc(input_path + "newsletter.pdf"))
				using (ContentReplacer replacer = new ContentReplacer())
				{
					doc.InitSecurityHandler();

					Page page = doc.GetPage(1);
					Rect target_region = page.GetMediaBox();
					string replacement_text = "hello hello hello hello hello hello hello hello hello hello";
					replacer.AddText(target_region, replacement_text);
					replacer.Process(page);

					doc.Save(output_path + "ContentReplaced.pdf", 0);
					Console.WriteLine("Done. Result saved in ContentReplaced.pdf");
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}

			Console.WriteLine("Done.");
		}
	}
}
