//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

using System;
using System.IO;

using pdftron;
using pdftron.Common;
using pdftron.SDF;
using pdftron.PDF;

namespace HTML2PDFTestCS
{
	//---------------------------------------------------------------------------------------
	// The following sample illustrates how to convert HTML pages to PDF format using
	// the HTML2PDF class.
	// 
	// 'pdftron.PDF.HTML2PDF' is an optional PDFNet Add-On utility class that can be 
	// used to convert HTML web pages into PDF documents by using an external module (html2pdf).
	//
	// html2pdf modules can be downloaded from http://www.pdftron.com/pdfnet/downloads.html.
	//
	// Users can convert HTML pages to PDF using the following operations:
	// - Simple one line static method to convert a single web page to PDF. 
	// - Convert HTML pages from URL or string, plus optional table of contents, in user defined order. 
	// - Optionally configure settings for proxy, images, java script, and more for each HTML page. 
	// - Optionally configure the PDF output, including page size, margins, orientation, and more. 
	// - Optionally add table of contents, including setting the depth and appearance.
	//---------------------------------------------------------------------------------------
	class HTML2PDFSample
	{
		private static pdftron.PDFNetLoader pdfNetLoader = pdftron.PDFNetLoader.Instance();
		static HTML2PDFSample() {}
		
		static void Main(string[] args)
		{
			string output_path = "../../TestFiles/Output/html2pdf_example";
			string host = "http://www.gutenberg.org/";
			string page0 = "wiki/Main_Page";
			string page1 = "catalog/";
			string page2 = "browse/recent/last1";
			string page3 = "wiki/Gutenberg:The_Sheet_Music_Project";

			// The first step in every application using PDFNet is to initialize the 
			// library and set the path to common PDF resources. The library is usually 
			// initialized only once, but calling Initialize() multiple times is also fine.
			PDFNet.Initialize();

			// For HTML2PDF we need to locate the html2pdf module. If placed with the 
			// PDFNet library, or in the current working directory, it will be loaded
			// automatically. Otherwise, it must be set manually using HTML2PDF.SetModulePath.
			HTML2PDF.SetModulePath("../../../Lib");

			//--------------------------------------------------------------------------------
			// Example 1) Simple conversion of a web page to a PDF doc. 

			try
			{
				using (PDFDoc doc = new PDFDoc())
				{

					// now convert a web page, sending generated PDF pages to doc
					if ( HTML2PDF.Convert(doc, host + page0) )
						doc.Save(output_path + "_01.pdf", SDFDoc.SaveOptions.e_linearized);
					else
						Console.WriteLine("Conversion failed.");
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}

			//--------------------------------------------------------------------------------
			// Example 2) Modify the settings of the generated PDF pages and attach to an
			// existing PDF document. 

			try
			{
				// open the existing PDF, and initialize the security handler
				using (PDFDoc doc = new PDFDoc("../../TestFiles/numbered.pdf"))
				{
					doc.InitSecurityHandler();

					// create the HTML2PDF converter object and modify the output of the PDF pages
					HTML2PDF converter = new HTML2PDF();
					converter.SetImageQuality(25);
					converter.SetPaperSize(PrinterMode.PaperSize.e_11x17);

					// insert the web page to convert
					converter.InsertFromURL(host + page0);

					// convert the web page, appending generated PDF pages to doc
					if ( converter.Convert(doc) )
						doc.Save(output_path + "_02.pdf", SDFDoc.SaveOptions.e_linearized);
					else
						Console.WriteLine("Conversion failed. HTTP Code: {0}\n{1}", converter.GetHTTPErrorCode(), converter.GetLog());
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}

			//--------------------------------------------------------------------------------
			// Example 3) Convert multiple web pages, adding a table of contents, and setting
			// the first page as a cover page, not to be included with the table of contents outline. 

			try
			{
				using (PDFDoc doc = new PDFDoc())
				{
					HTML2PDF converter = new HTML2PDF();

					// Add a cover page, which is excluded from the outline, and ignore any errors
					HTML2PDF.WebPageSettings cover = new HTML2PDF.WebPageSettings();
					cover.SetLoadErrorHandling(HTML2PDF.WebPageSettings.ErrorHandling.e_ignore);
					cover.SetIncludeInOutline(false);
					converter.InsertFromURL(host + page3, cover);

					// Add a table of contents settings (modifying the settings is optional)
					HTML2PDF.TOCSettings toc = new HTML2PDF.TOCSettings();
					toc.SetDottedLines(false);
					converter.InsertTOC(toc);

					// Now add the rest of the web pages, disabling external links and 
					// skipping any web pages that fail to load.
					//
					// Note that the order of insertion matters, so these will appear
					// after the cover and table of contents, in the order below.
					HTML2PDF.WebPageSettings settings = new HTML2PDF.WebPageSettings();
					settings.SetLoadErrorHandling(HTML2PDF.WebPageSettings.ErrorHandling.e_skip);
					settings.SetExternalLinks(false);
					converter.InsertFromURL(host + page0, settings);
					converter.InsertFromURL(host + page1, settings);
					converter.InsertFromURL(host + page2, settings);

					if ( converter.Convert(doc) )
						doc.Save(output_path + "_03.pdf", SDFDoc.SaveOptions.e_linearized);
					else
						Console.WriteLine("Conversion failed. HTTP Code: {0}\n{1}", converter.GetHTTPErrorCode(), converter.GetLog());
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}

			//--------------------------------------------------------------------------------
			// Example 4) Convert HTML string to PDF. 

			try
			{
				using (PDFDoc doc = new PDFDoc())
				{

					HTML2PDF converter = new HTML2PDF();
				
					// Our HTML data
					string html = "<html><body><h1>Heading</h1><p>Paragraph.</p></body></html>";
					
					// Add html data
					converter.InsertFromHtmlString(html);
					// Note, InsertFromHtmlString can be mixed with the other Insert methods.
					
					if ( converter.Convert(doc) )
						doc.Save(output_path + "_04.pdf", SDFDoc.SaveOptions.e_linearized);
					else
						Console.WriteLine("Conversion failed. HTTP Code: {0}\n{1}", converter.GetHTTPErrorCode(), converter.GetLog());
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}

		}
	}
}
