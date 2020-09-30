//
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
//

using System;
using pdftron;
using pdftron.Common;
using pdftron.Filters;
using pdftron.SDF;
using pdftron.PDF;

namespace ElementReaderTestCS
{
	/// <summary>
	/// Summary description for Class1.
	/// </summary>
	class Class1
	{
		private static pdftron.PDFNetLoader pdfNetLoader = pdftron.PDFNetLoader.Instance();
		static Class1() {}
		
		static void ProcessElements(ElementReader reader)
		{
			Element element;
			while ((element = reader.Next()) != null) 	// Read page contents
			{
				switch (element.GetType())
				{
               
					case Element.Type.e_path:				// Process path data...
						{
                            PathData data = element.GetPathData();
                            double[] points = data.points;
							break;
						}
					
                    case Element.Type.e_text: 				// Process text strings...
						{
                            String str = element.GetTextString();
                            Console.WriteLine(str);
                            break;
						}

					case Element.Type.e_form:				// Process form XObjects
						{
							Console.WriteLine("Process Element.Type.e_form");
							reader.FormBegin();
							ProcessElements(reader);
							reader.End();
							break;
						}
				}
			}
		}

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main(string[] args)
		{
            PDFNet.Initialize();

			// Relative path to the folder containing test files.
			string input_path = "../../TestFiles/";

			try
			{
				Console.WriteLine("-------------------------------------------------");
				Console.WriteLine("Sample 1 - Extract text data from all pages in the document.");

                // Open the test file
                Console.WriteLine("Opening the input pdf...");
                using (PDFDoc doc = new PDFDoc(input_path + "newsletter.pdf"))
				using (ElementReader page_reader = new ElementReader())
				{
					doc.InitSecurityHandler();

					PageIterator itr;
					for (itr = doc.GetPageIterator(); itr.HasNext(); itr.Next())		//  Read every page
					{
						page_reader.Begin(itr.Current());
						ProcessElements(page_reader);
						page_reader.End();
					}
					Console.WriteLine("Done.");
				}

            }
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}

		}
	}
}
