//
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
//
using System;
using pdftron;
using pdftron.Common;
using pdftron.Filters;
using pdftron.SDF;
using pdftron.PDF;

namespace PackageTestCS
{
	/// <summary>
	/// This sample illustrates how to create, extract, and manipulate PDF Portfolios
	/// (a.k.a. PDF Packages) using PDFNet SDK.
	/// </summary>
	class Class1
	{
		private static pdftron.PDFNetLoader pdfNetLoader = pdftron.PDFNetLoader.Instance();
		static Class1() {}
		
		// Relative path to the folder containing test files.
		const string input_path =  "../../TestFiles/";
		const string output_path = "../../TestFiles/Output/";

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main(string[] args)
		{
			PDFNet.Initialize();

			// Create a PDF Package.
			try
			{
				using (PDFDoc doc = new PDFDoc())
				{
					AddPackage(doc, input_path + "numbered.pdf", "My File 1");
					AddPackage(doc, input_path + "newsletter.pdf", "My Newsletter...");
					AddPackage(doc, input_path + "peppers.jpg", "An image");
					AddCovePage(doc);
					doc.Save(output_path + "package.pdf", SDFDoc.SaveOptions.e_linearized);
                    Console.WriteLine("Done.");
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}

			// Extract parts from a PDF Package.
			try
			{
				using (PDFDoc doc = new PDFDoc(output_path + "package.pdf"))
				{
					doc.InitSecurityHandler();

					pdftron.SDF.NameTree files = NameTree.Find(doc, "EmbeddedFiles");
					if(files.IsValid()) 
					{ 
						// Traverse the list of embedded files.
						NameTreeIterator i = files.GetIterator();
						for (int counter = 0; i.HasNext(); i.Next(), ++counter) 
						{
							string entry_name = i.Key().GetAsPDFText();
							Console.WriteLine("Part: {0}", entry_name);
							FileSpec file_spec = new FileSpec(i.Value());
							Filter stm = file_spec.GetFileData();
							if (stm!=null) 
							{
								string fname = output_path + "extract_" + counter.ToString() + System.IO.Path.GetExtension(entry_name);
								stm.WriteToFile(fname, false);
							}
						}
					}
				}

				Console.WriteLine("Done.");
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}
		}

		static void AddPackage(PDFDoc doc, string file, string desc) 
		{
			NameTree files = NameTree.Create(doc, "EmbeddedFiles");
			FileSpec fs = FileSpec.Create(doc, file, true);
			byte[] file1_name = System.Text.Encoding.UTF8.GetBytes(file);
			files.Put(file1_name, fs.GetSDFObj());
			fs.GetSDFObj().PutText("Desc", desc);

			Obj collection = doc.GetRoot().FindObj("Collection");
			if (collection == null) collection = doc.GetRoot().PutDict("Collection");

			// You could here manipulate any entry in the Collection dictionary. 
			// For example, the following line sets the tile mode for initial view mode
			// Please refer to section '2.3.5 Collections' in PDF Reference for details.
			collection.PutName("View", "T");
		}

		static void AddCovePage(PDFDoc doc) 
		{
			// Here we dynamically generate cover page (please see ElementBuilder 
			// sample for more extensive coverage of PDF creation API).
			Page page = doc.PageCreate(new Rect(0, 0, 200, 200));

			using (ElementBuilder b = new ElementBuilder())
			using (ElementWriter w = new ElementWriter())
			{
				w.Begin(page); 
				Font font = Font.Create(doc, Font.StandardType1Font.e_helvetica);
				w.WriteElement(b.CreateTextBegin(font, 12));
				Element e = b.CreateTextRun("My PDF Collection");
				e.SetTextMatrix(1, 0, 0, 1, 50, 96);
				e.GetGState().SetFillColorSpace(ColorSpace.CreateDeviceRGB());
				e.GetGState().SetFillColor(new ColorPt(1, 0, 0));
				w.WriteElement(e);
				w.WriteElement(b.CreateTextEnd());
				w.End();
				doc.PagePushBack(page);
			}

			// Alternatively we could import a PDF page from a template PDF document
			// (for an example please see PDFPage sample project).
			// ...
		}
	}
}
