//
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
//

using System;
using pdftron;
using pdftron.Common;
using pdftron.Filters;
using pdftron.SDF;
using pdftron.PDF;
using pdftron.FDF;

namespace FDFTestCS
{
	/// <summary>
	/// PDFNet includes full support for FDF (Forms Data Format) and for merging/extracting
	/// forms data (FDF) with/from PDF. This sample illustrates basic FDF merge/extract functionality 
	/// available in PDFNet.
	/// </summary>
	class Class1
	{
		private static pdftron.PDFNetLoader pdfNetLoader = pdftron.PDFNetLoader.Instance();
		static Class1() {}
		
		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		static void Main(string[] args)
		{
			PDFNet.Initialize();

			// Relative path to the folder containing test files.
			string input_path =  "../../TestFiles/";
			string output_path = "../../TestFiles/Output/";

			// Example 1)
			// Iterate over all form fields in the document. Display all field names.
			try  
			{
				using (PDFDoc doc = new PDFDoc(input_path + "form1.pdf"))
				{
					doc.InitSecurityHandler();
					
					FieldIterator itr;
					for(itr=doc.GetFieldIterator(); itr.HasNext(); itr.Next())
					{
						Console.WriteLine("Field name: {0:s}", itr.Current().GetName());
						Console.WriteLine("Field partial name: {0:s}", itr.Current().GetPartialName());

						Console.Write("Field type: ");
						Field.Type type = itr.Current().GetType();
						switch(type)
						{
							case Field.Type.e_button: 
								Console.WriteLine("Button"); break;
							case Field.Type.e_text: 
								Console.WriteLine("Text"); break;
							case Field.Type.e_choice: 
								Console.WriteLine("Choice"); break;
							case Field.Type.e_signature: 
								Console.WriteLine("Signature"); break;
						}

						Console.WriteLine("------------------------------");
					}

					Console.WriteLine("Done.");
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}

			// Example 2) Import XFDF into FDF, then merge data from FDF into PDF
			try  
			{
				// XFDF to FDF
				// form fields
				Console.WriteLine("Import form field data from XFDF to FDF.");
				
				FDFDoc fdf_doc1 = new FDFDoc(FDFDoc.CreateFromXFDF(input_path + "form1_data.xfdf"));
				fdf_doc1.Save(output_path + "form1_data.fdf");
				
				// annotations
				Console.WriteLine("Import annotations from XFDF to FDF.");
				
				FDFDoc fdf_doc2 = new FDFDoc(FDFDoc.CreateFromXFDF(input_path + "form1_annots.xfdf"));
				fdf_doc2.Save(output_path + "form1_annots.fdf");
				
				// FDF to PDF
				// form fields
				Console.WriteLine("Merge form field data from FDF.");
				
				using (PDFDoc doc = new PDFDoc(input_path + "form1.pdf"))
				{
					doc.InitSecurityHandler();
					doc.FDFMerge(fdf_doc1);
					
					// Refreshing missing appearances is not required here, but is recommended to make them 
					// visible in PDF viewers with incomplete annotation viewing support. (such as Chrome)
					doc.RefreshAnnotAppearances();
					
					doc.Save(output_path + "form1_filled.pdf", SDFDoc.SaveOptions.e_linearized);
					
					// annotations
					Console.WriteLine("Merge annotations from FDF.");
					
					doc.FDFMerge(fdf_doc2);
					// Refreshing missing appearances is not required here, but is recommended to make them 
					// visible in PDF viewers with incomplete annotation viewing support. (such as Chrome)
					doc.RefreshAnnotAppearances();
					doc.Save(output_path + "form1_filled_with_annots.pdf", SDFDoc.SaveOptions.e_linearized);

					Console.WriteLine("Done.");
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}

			// Example 3) Extract data from PDF to FDF, then export FDF as XFDF
			try  
			{
				// PDF to FDF
				using (PDFDoc in_doc = new PDFDoc(output_path + "form1_filled_with_annots.pdf"))
				{
					in_doc.InitSecurityHandler();
					
					// form fields only
					Console.WriteLine("Extract form fields data to FDF.");
					
					FDFDoc doc_fields = in_doc.FDFExtract(PDFDoc.ExtractFlag.e_forms_only);
					doc_fields.SetPdfFileName("../form1_filled_with_annots.pdf");
					doc_fields.Save(output_path + "form1_filled_data.fdf");
					
					// annotations only
					Console.WriteLine("Extract annotations to FDF.");
					
					FDFDoc doc_annots = in_doc.FDFExtract(PDFDoc.ExtractFlag.e_annots_only);
					doc_annots.SetPdfFileName("../form1_filled_with_annots.pdf");
					doc_annots.Save(output_path + "form1_filled_annot.fdf");
					
					// both form fields and annotations
					Console.WriteLine("Extract both form fields and annotations to FDF.");
					
					FDFDoc doc_both = in_doc.FDFExtract(PDFDoc.ExtractFlag.e_both);
					doc_both.SetPdfFileName("../form1_filled_with_annots.pdf");
					doc_both.Save(output_path + "form1_filled_both.fdf");
					
					// FDF to XFDF
					// form fields
					Console.WriteLine("Export form field data from FDF to XFDF.");
					
					doc_fields.SaveAsXFDF(output_path + "form1_filled_data.xfdf");
					
					// annotations
					Console.WriteLine("Export annotations from FDF to XFDF.");
					
					doc_annots.SaveAsXFDF(output_path + "form1_filled_annot.xfdf");
					
					// both form fields and annotations
					Console.WriteLine("Export both form fields and annotations from FDF to XFDF.");
					
					doc_both.SaveAsXFDF(output_path + "form1_filled_both.xfdf");
					
					Console.WriteLine("Done.");
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}

            // Example 4) Merge/Extract XFDF into/from PDF
            try
            {
                // Merge XFDF from string
                PDFDoc in_doc = new PDFDoc(input_path + "numbered.pdf");
				{
					in_doc.InitSecurityHandler();

					Console.WriteLine("Merge XFDF string into PDF.");

					string str = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><xfdf xmlns=\"http://ns.adobe.com/xfdf\" xml:space=\"preserve\"><square subject=\"Rectangle\" page=\"0\" name=\"cf4d2e58-e9c5-2a58-5b4d-9b4b1a330e45\" title=\"user\" creationdate=\"D:20120827112326-07'00'\" date=\"D:20120827112326-07'00'\" rect=\"227.7814207650273,597.6174863387978,437.07103825136608,705.0491803278688\" color=\"#000000\" interior-color=\"#FFFF00\" flags=\"print\" width=\"1\"><popup flags=\"print,nozoom,norotate\" open=\"no\" page=\"0\" rect=\"0,792,0,792\" /></square></xfdf>";

					using (FDFDoc fdoc = new FDFDoc(FDFDoc.CreateFromXFDF(str)))
					{
						in_doc.FDFMerge(fdoc);
						in_doc.Save(output_path + "numbered_modified.pdf", SDFDoc.SaveOptions.e_linearized);
						Console.WriteLine("Merge complete.");
					}

					// Extract XFDF as string
					Console.WriteLine("Extract XFDF as a string.");
					FDFDoc fdoc_new = in_doc.FDFExtract(PDFDoc.ExtractFlag.e_both);
					string XFDF_str = fdoc_new.SaveAsXFDF();
					Console.WriteLine("Extracted XFDF: ");
					Console.WriteLine(XFDF_str);
					Console.WriteLine("Extract complete.");
				}
            }
            catch (PDFNetException e)
            {
                Console.WriteLine(e.Message);
            }

			// Example 5) Read FDF files directly
			try  
			{
				FDFDoc doc = new FDFDoc(output_path + "form1_filled_data.fdf");
				FDFFieldIterator itr = doc.GetFieldIterator();
				for(; itr.HasNext(); itr.Next()) 
				{
					Console.WriteLine("Field name: {0:s}", itr.Current().GetName());
					Console.WriteLine("Field partial name: {0:s}", itr.Current().GetPartialName());
					Console.WriteLine("------------------------------");
				}

				Console.WriteLine("Done.");
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}

			// Example 6) Direct generation of FDF.
			try  
			{
				FDFDoc doc = new FDFDoc();

				// Create new fields (i.e. key/value pairs).
				doc.FieldCreate("Company", (int)Field.Type.e_text, "PDFTron Systems");
				doc.FieldCreate("First Name", (int)Field.Type.e_text, "John");
				doc.FieldCreate("Last Name", (int)Field.Type.e_text, "Doe");
				// ...		

				// doc.SetPdfFileName("mydoc.pdf");
				doc.Save(output_path + "sample_output.fdf");
				Console.WriteLine("Done. Results saved in sample_output.fdf");
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}

		}
	}
}
