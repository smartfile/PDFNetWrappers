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

namespace WebViewerConvertCS
{
	/// <summary>
	/// The following sample illustrates how to convert PDF, XPS, image, MS Office, and 
	/// other image document formats to XOD format.
	/// 
	/// Certain file formats such as PDF, generic XPS, EMF, and raster image formats can 
	/// be directly converted to XOD. Other formats such as MS Office 
	/// (Word, Excel, Publisher, Powerpoint, etc) can be directly converted via interop. 
	/// These types of conversions guarantee optimal output, while preserving important 
	/// information such as document metadata, intra document links and hyper-links, 
	/// bookmarks etc. 
	/// 
	/// In case there is no direct conversion available, PDFNet can still convert from 
	/// any printable document to XOD using a virtual printer driver. To check 
	/// if a virtual printer is required use Convert::RequiresPrinter(filename). In this 
	/// case the installing application must be run as administrator. The manifest for this 
	/// sample specifies appropriate the UAC elevation. The administrator privileges are 
	/// not required for direct or interop conversions. 
	/// 
	/// Please note that PDFNet Publisher (i.e. 'pdftron.PDF.Convert.ToXod') is an
	/// optionally licensable add-on to PDFNet Core SDK. For details, please see
	/// http://www.pdftron.com/webviewer/licensing.html 
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
		// Relative path to the folder containing test files.
		const string inputPath = "../../TestFiles/";
		const string outputPath = "../../TestFiles/Output/";

		static void BulkConvertRandomFilesToXod()
		{
			System.Collections.ArrayList testfiles = new System.Collections.ArrayList();
			testfiles.Add(new WebViewerConvertCS.Testfile("simple-powerpoint_2007.pptx", "simple-powerpoint_2007.xod"));
			testfiles.Add(new WebViewerConvertCS.Testfile("simple-word_2007.docx", "simple-word_2007.xod"));
			testfiles.Add(new WebViewerConvertCS.Testfile("butterfly.png", "butterfly.xod"));
			testfiles.Add(new WebViewerConvertCS.Testfile("numbered.pdf", "numbered.xod"));
			testfiles.Add(new WebViewerConvertCS.Testfile("dice.jpg", "dice.xod"));
			testfiles.Add(new WebViewerConvertCS.Testfile("simple-xps.xps", "simple-xps.xod"));

			foreach (Testfile file in testfiles)
			{
				try
				{
					if (pdftron.PDF.Convert.RequiresPrinter(inputPath + file.inputFile))
					{
						Console.WriteLine("Printing file: " + file.inputFile);
					}

					pdftron.PDF.Convert.ToXod(inputPath + file.inputFile, outputPath + file.outputFile);
					Console.WriteLine("Converted file: " + file.inputFile);
					Console.WriteLine("		to xod: " + file.outputFile);
				}
				catch (PDFNetException e)
				{
					Console.WriteLine("ERROR: on input file " + file.inputFile);
					Console.WriteLine(e.Message);
				}
			}
		}

        private static pdftron.PDFNetLoader pdfNetLoader = pdftron.PDFNetLoader.Instance();
		static Class1() {}

		/// <summary>
		/// </summary>
		[STAThread]
		static void Main(string[] args)
		{
			PDFNet.Initialize();

			// Sample 1:
			// Directly convert from PDF to XOD.
			pdftron.PDF.Convert.ToXod(inputPath + "newsletter.pdf", outputPath + "from_pdf.xod");

			// Sample 2:
			// Directly convert from generic XPS to XOD.
            pdftron.PDF.Convert.ToXod(inputPath + "simple-xps.xps", outputPath + "from_xps.xod");

			// Sample 3:
			// Convert from MS Office (does not require printer driver for Office 2007+)
			// and other document formats to XOD.
			BulkConvertRandomFilesToXod();

			Console.WriteLine("Done.");
		}
	}
}
