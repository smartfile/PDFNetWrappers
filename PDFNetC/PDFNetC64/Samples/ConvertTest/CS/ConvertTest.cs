//
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
//

using System;
using System.Drawing;
using System.Drawing.Drawing2D;

using pdftron;
using pdftron.Common;
using pdftron.Filters;
using pdftron.SDF;
using pdftron.PDF;

namespace ConvertTestCS
{
	/// <summary>
	// The following sample illustrates how to use the PDF::Convert utility class to convert 
	// documents and files to PDF, XPS, SVG, or EMF.
	//
	// Certain file formats such as XPS, EMF, PDF, and raster image formats can be directly 
	// converted to PDF or XPS. Other formats are converted using a virtual driver. To check 
	// if ToPDF (or ToXPS) require that PDFNet printer is installed use Convert::RequiresPrinter(filename). 
	// The installing application must be run as administrator. The manifest for this sample 
	// specifies appropriate the UAC elevation.
	//
	// Note: the PDFNet printer is a virtual XPS printer supported on Vista SP1 and Windows 7, or .NET Framework
	// 3.x or higher. For older versions of .NET Framework running on Windows XP or Vista SP0 you need to install 
	// the XPS Essentials Pack (or equivalent redistributables). 
	//
	// Also note that conversion under ASP.NET can be tricky to configure. Please see the following document for advice: 
	// http://www.pdftron.com/pdfnet/faq_files/Converting_Documents_in_Windows_Service_or_ASP.NET_Application_using_PDFNet.pdf
	/// </summary>
	class Testfile
	{
		public string inputFile, outputFile;
		public Testfile(string inFile, string outFile)
		{
			inputFile = inFile;
			outputFile = outFile;
		}
	};

	class Class1
	{
		private static pdftron.PDFNetLoader pdfNetLoader = pdftron.PDFNetLoader.Instance();
		static Class1() {}

		// Relative path to the folder containing test files.
		const string inputPath = "../../TestFiles/";
		const string outputPath = "../../TestFiles/Output/";

		static Boolean ConvertSpecificFormats()
		{
            //////////////////////////////////////////////////////////////////////////
            Boolean err = false;
			try
			{
				using (PDFDoc pdfdoc = new PDFDoc())
				{
                    Console.WriteLine("Converting from XPS");

					pdftron.PDF.Convert.FromXps(pdfdoc, inputPath + "simple-xps.xps");
                    pdfdoc.Save(outputPath + "xps2pdf v2.pdf", SDFDoc.SaveOptions.e_remove_unused);
                    Console.WriteLine("Saved xps2pdf v2.pdf");
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
				err = true;
			}

            //////////////////////////////////////////////////////////////////////////
            try
            {
                using (PDFDoc pdfdoc = new PDFDoc())
                {
                    Console.WriteLine("Converting from EMF");
                    pdftron.PDF.Convert.FromEmf(pdfdoc, inputPath + "simple-emf.emf");
                    pdfdoc.Save(outputPath + "emf2pdf v2.pdf", SDFDoc.SaveOptions.e_remove_unused);
                    Console.WriteLine("Saved emf2pdf v2.pdf");
                }
            }
            catch (PDFNetException e)
            {
                Console.WriteLine(e.Message);
                err = true;
            }

            //////////////////////////////////////////////////////////////////////////
            try
            {
                using (PDFDoc pdfdoc = new PDFDoc())
                {
                    // Add a dictionary
                    ObjSet set = new ObjSet();
                    Obj options = set.CreateDict();

                    // Put options
                    options.PutNumber("FontSize", 15);
                    options.PutBool("UseSourceCodeFormatting", true);
                    options.PutNumber("PageWidth", 12);
                    options.PutNumber("PageHeight", 6);

                    // Convert from .txt file
                    Console.WriteLine("Converting from txt");
                    pdftron.PDF.Convert.FromText(pdfdoc, inputPath + "simple-text.txt", options);
                    pdfdoc.Save(outputPath + "simple-text.pdf", SDFDoc.SaveOptions.e_remove_unused);
                    Console.WriteLine("Saved simple-text.pdf");
                }
            }
            catch (PDFNetException e)
            {
                Console.WriteLine(e.Message);
                err = true;
            }

            //////////////////////////////////////////////////////////////////////////
            try
            {
                using (PDFDoc pdfdoc = new PDFDoc(inputPath + "newsletter.pdf"))
                {
                    // Convert PDF document to SVG
                    Console.WriteLine("Converting pdfdoc to SVG");
                    pdftron.PDF.Convert.ToSvg(pdfdoc, outputPath + "pdf2svg v2.svg");
                    Console.WriteLine("Saved pdf2svg v2.svg");
                }
            }
            catch (PDFNetException e)
            {
                Console.WriteLine(e.Message);
                err = true;
            }

            //////////////////////////////////////////////////////////////////////////
            try
            {
                // Convert PNG image to XPS
                Console.WriteLine("Converting PNG to XPS");
                pdftron.PDF.Convert.ToXps(inputPath + "butterfly.png", outputPath + "butterfly.xps");
                Console.WriteLine("Saved butterfly.xps");
            }
            catch (PDFNetException e)
            {
                Console.WriteLine(e.Message);
                err = true;
            }

            //////////////////////////////////////////////////////////////////////////
            try
            {
                // Convert MSWord document to XPS
                Console.WriteLine("Converting DOCX to XPS");
                pdftron.PDF.Convert.ToXps(inputPath + "simple-word_2007.docx", outputPath + "simple-word_2007.xps");
                Console.WriteLine("Saved simple-word_2007.xps");
            }
            catch (PDFNetException e)
            {
                Console.WriteLine(e.Message);
                err = true;
            }

            //////////////////////////////////////////////////////////////////////////
            try
            {
                // Convert PDF document to XPS
                Console.WriteLine("Converting PDF to XPS");
                pdftron.PDF.Convert.ToXps(inputPath + "newsletter.pdf", outputPath + "newsletter.xps");
                Console.WriteLine("Saved newsletter.xps");
            }
            catch (PDFNetException e)
            {
                Console.WriteLine(e.Message);
                err = true;
            }

            //////////////////////////////////////////////////////////////////////////
            try
            {
                // Convert PDF document to HTML
                Console.WriteLine("Converting PDF to HTML");
                pdftron.PDF.Convert.ToHtml(inputPath + "newsletter.pdf", outputPath + "newsletter");
                Console.WriteLine("Saved newsletter as HTML");
            }
            catch (PDFNetException e)
            {
                Console.WriteLine(e.Message);
                err = true;
            }

            //////////////////////////////////////////////////////////////////////////
            try
            {
                // Convert PDF document to EPUB
                Console.WriteLine("Converting PDF to EPUB");
                pdftron.PDF.Convert.ToEpub(inputPath + "newsletter.pdf", outputPath + "newsletter.epub");
                Console.WriteLine("Saved newsletter.epub");
            }
            catch (PDFNetException e)
            {
                Console.WriteLine(e.Message);
                err = true;
            }

            //////////////////////////////////////////////////////////////////////////
            try
            {
                // Convert PDF document to multipage TIFF
                Console.WriteLine("Converting PDF to multipage TIFF");
                pdftron.PDF.Convert.TiffOutputOptions tiff_options = new pdftron.PDF.Convert.TiffOutputOptions();
                tiff_options.SetDPI(200);
                tiff_options.SetDither(true);
                tiff_options.SetMono(true);
                pdftron.PDF.Convert.ToTiff(inputPath + "newsletter.pdf", outputPath + "newsletter.tiff", tiff_options);
                Console.WriteLine("Saved newsletter.tiff");
            }
            catch (PDFNetException e)
            {
                Console.WriteLine(e.Message);
                err = true;
            }

			return err;
		}

		static Boolean ConvertToPdfFromFile()
		{
			System.Collections.ArrayList testfiles = new System.Collections.ArrayList();
			testfiles.Add(new ConvertTestCS.Testfile("simple-powerpoint_2007.pptx", "pptx2pdf.pdf"));
			testfiles.Add(new ConvertTestCS.Testfile("simple-text.txt", "txt2pdf.pdf"));
			testfiles.Add(new ConvertTestCS.Testfile("simple-word_2007.docx", "docx2pdf.pdf"));
			testfiles.Add(new ConvertTestCS.Testfile("simple-rtf.rtf", "rtf2pdf.pdf"));
			testfiles.Add(new ConvertTestCS.Testfile("simple-excel_2007.xlsx", "xlsx2pdf.pdf"));
			testfiles.Add(new ConvertTestCS.Testfile("simple-publisher.pub", "pub2pdf.pdf"));
			testfiles.Add(new ConvertTestCS.Testfile("butterfly.png", "png2pdf.pdf"));
			testfiles.Add(new ConvertTestCS.Testfile("simple-emf.emf", "emf2pdf.pdf"));
			testfiles.Add(new ConvertTestCS.Testfile("simple-xps.xps", "xps2pdf.pdf"));
			// testfiles.Add(new ConvertTestCS.Testfile("simple-webpage.mht", "mht2pdf.pdf"));
			testfiles.Add(new ConvertTestCS.Testfile("simple-webpage.html", "html2pdf.pdf"));

			Boolean err = false;
			Boolean uninstallPrinterWhenDone = false;
			try{
				if (pdftron.PDF.Convert.Printer.IsInstalled("PDFTron Creator"))
				{
					pdftron.PDF.Convert.Printer.SetPrinterName("PDFTron Creator");
				}
				else if (!pdftron.PDF.Convert.Printer.IsInstalled())
				{
					try
					{
						Console.WriteLine("Installing printer (requires administrator)");
						pdftron.PDF.Convert.Printer.Install();
						Console.WriteLine("Installed printer " + pdftron.PDF.Convert.Printer.GetPrinterName());
						// the function ConvertToXpsFromFile may require the printer so leave it installed
						// uninstallPrinterWhenDone = true;
					}
					catch (PDFNetException e)
					{
						Console.WriteLine("ERROR: Unable to install printer.");
						Console.WriteLine(e.Message);
						err = true;
					}
					catch
					{
						Console.WriteLine("ERROR: Unable to install printer. Make sure that the package's bitness matches your operating system's bitness and that you are running with administrator privileges.");
					}
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine("ERROR: Unable to install printer.");
				Console.WriteLine(e.Message);
				err = true;
			}
			
			foreach (Testfile file in testfiles)
			{
				try
				{
					using (pdftron.PDF.PDFDoc pdfdoc = new PDFDoc())
					{

						if (pdftron.PDF.Convert.RequiresPrinter(inputPath + file.inputFile))
						{
							Console.WriteLine("Printing file: " + file.inputFile);
						}
						pdftron.PDF.Convert.ToPdf(pdfdoc, inputPath + file.inputFile);
						pdfdoc.Save(outputPath + file.outputFile, SDFDoc.SaveOptions.e_linearized);
						Console.WriteLine("Converted file: " + file.inputFile);
						Console.WriteLine("to: " + file.outputFile);
					}
				}
				catch (PDFNetException e)
				{
					Console.WriteLine("ERROR: on input file " + file.inputFile);
					Console.WriteLine(e.Message);
					err = true;
				}
			}

			if (uninstallPrinterWhenDone)
			{
				try
				{
					Console.WriteLine("Uninstalling printer (requires administrator)");
					pdftron.PDF.Convert.Printer.Uninstall();
					Console.WriteLine("Uninstalled printer " + pdftron.PDF.Convert.Printer.GetPrinterName());
				}
				catch (PDFNetException e)
				{
					Console.WriteLine(e.Message);
					err = true;
				}
			}
			return err;
		}

		static Boolean ConvertToXpsFromFile()
		{
			Boolean err = false;
			try
			{
				pdftron.PDF.Convert.ToXps(inputPath + "butterfly.png", outputPath + "butterfly.xps");
				Console.WriteLine("Wrote " + outputPath + "buttefly.xps");
				pdftron.PDF.Convert.ToXps(inputPath + "simple-word_2007.docx", outputPath + "simple-word_2007.xps");
				Console.WriteLine("Wrote " + outputPath + "simple-word_2007.xps");
				pdftron.PDF.Convert.ToXps(inputPath + "newsletter.pdf", outputPath + "newsletter.xps");
				Console.WriteLine("Wrote " + outputPath + "newsletter.xps");
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
				err = true;
			}
			return err;
		}

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main(string[] args)
		{
			PDFNet.Initialize();
			Boolean err = false;

			err = ConvertToPdfFromFile();
			if (err)
			{
				Console.WriteLine("ConvertFile failed");
			}
			else
			{
				Console.WriteLine("ConvertFile succeeded");
			}

			err = ConvertSpecificFormats();
			if (err)
			{
				Console.WriteLine("ConvertSpecificFormats failed");
			}
			else
			{
				Console.WriteLine("ConvertSpecificFormats succeeded");
			}

			err = ConvertToXpsFromFile();
			if (err)
			{
				Console.WriteLine("ConvertToXpsFromFile failed");
			}
			else
			{
				Console.WriteLine("ConvertToXpsFromFile succeeded");
			}

			Console.WriteLine("Done.");
		}
	}
}
