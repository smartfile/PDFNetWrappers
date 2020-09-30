//
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
//

using System;
using pdftron;
using pdftron.SDF;
using pdftron.PDF;
using pdftron.PDF.PDFA;

//-----------------------------------------------------------------------------------
// The sample illustrates how to use PDF/A related API-s.
//-----------------------------------------------------------------------------------
namespace PDFATestCS
{
	class Class1
	{
		private static pdftron.PDFNetLoader pdfNetLoader = pdftron.PDFNetLoader.Instance();
		static Class1() {}
		
		// Relative path to the folder containing test files.
		static string input_path =  "../../TestFiles/";
		static string output_path = "../../TestFiles/Output/";

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main(string[] args)
		{
			PDFNet.Initialize();
			PDFNet.SetColorManagement(PDFNet.CMSType.e_lcms);  // Required for PDFA validation.

			//-----------------------------------------------------------
			// Example 1: PDF/A Validation
			//-----------------------------------------------------------
			try
			{
				string filename = "newsletter.pdf";
				using (PDFACompliance pdf_a = new PDFACompliance(false, input_path+filename, null, PDFACompliance.Conformance.e_Level2B, null, 10, false))
				{
					PrintResults(pdf_a, filename);
				}
			}
			catch (pdftron.Common.PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}

			//-----------------------------------------------------------
			// Example 2: PDF/A Conversion
			//-----------------------------------------------------------
			try
			{
				string filename = "fish.pdf";
				using (PDFACompliance pdf_a = new PDFACompliance(true, input_path+filename, null, PDFACompliance.Conformance.e_Level2B, null, 10, false))
				{
					filename = "pdfa.pdf";
					pdf_a.SaveAs(output_path + filename, false);
				}

				// Re-validate the document after the conversion...
                filename = "pdfa.pdf";
				using (PDFACompliance pdf_a = new PDFACompliance(false, output_path + filename, null, PDFACompliance.Conformance.e_Level2B, null, 10, false))
				{
					PrintResults(pdf_a, filename);				
				}
			}
			catch (pdftron.Common.PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}

            Console.WriteLine("PDFACompliance test completed.");

        }

		static void PrintResults(PDFACompliance pdf_a, String filename) 
		{
			int err_cnt = pdf_a.GetErrorCount();
			if (err_cnt == 0) 
			{
				Console.WriteLine("{0}: OK.", filename);
			}
			else 
			{
				Console.WriteLine("{0} is NOT a valid PDFA.", filename);
				for (int i=0; i<err_cnt; ++i) 
				{
					PDFACompliance.ErrorCode c = pdf_a.GetError(i);
					Console.WriteLine(" - e_PDFA {0}: {1}.", 
						(int)c, PDFACompliance.GetPDFAErrorMessage(c));

					if (true) 
					{
						int num_refs = pdf_a.GetRefObjCount(c);
						if (num_refs > 0)  
						{
							Console.Write("   Objects: ");
							for (int j=0; j<num_refs; ) 
							{
								Console.Write("{0}", pdf_a.GetRefObj(c, j));
								if (++j!=num_refs) Console.Write(", ");
							}
							Console.WriteLine();
						}
					}
				}
				Console.WriteLine();
			}
		}
	}
}
