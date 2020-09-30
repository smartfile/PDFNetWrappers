//
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
//

using System;
using pdftron;
using pdftron.SDF;
using pdftron.PDF;

//-----------------------------------------------------------------------------------
// The sample illustrates how to work with PDF page labels.
//
// PDF page labels can be used to describe a page. This is used to 
// allow for non-sequential page numbering or the addition of arbitrary 
// labels for a page (such as the inclusion of Roman numerals at the 
// beginning of a book). PDFNet PageLabel object can be used to specify 
// the numbering style to use (for example, upper- or lower-case Roman, 
// decimal, and so forth), the starting number for the first page,
// and an arbitrary prefix to be pre-appended to each number (for 
// example, "A-" to generate "A-1", "A-2", "A-3", and so forth.)
//-----------------------------------------------------------------------------------
namespace PageLabelsTestCS
{
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
			try
			{
				//-----------------------------------------------------------
				// Example 1: Add page labels to an existing or newly created PDF
				// document.
				//-----------------------------------------------------------
				{
					using (PDFDoc doc = new PDFDoc(input_path + "newsletter.pdf"))
					{
						doc.InitSecurityHandler();

						// Create a page labeling scheme that starts with the first page in 
						// the document (page 1) and is using uppercase roman numbering 
						// style. 
						doc.SetPageLabel(1, PageLabel.Create(doc, PageLabel.Style.e_roman_uppercase, "My Prefix ", 1));

						// Create a page labeling scheme that starts with the fourth page in 
						// the document and is using decimal arabic numbering style. 
						// Also the numeric portion of the first label should start with number 
						// 4 (otherwise the first label would be "My Prefix 1"). 
						PageLabel L2 = PageLabel.Create(doc, PageLabel.Style.e_decimal, "My Prefix ", 4);
						doc.SetPageLabel(4, L2);

						// Create a page labeling scheme that starts with the seventh page in 
						// the document and is using alphabetic numbering style. The numeric 
						// portion of the first label should start with number 1. 
						PageLabel L3 = PageLabel.Create(doc, PageLabel.Style.e_alphabetic_uppercase, "My Prefix ", 1);
						doc.SetPageLabel(7, L3);

						doc.Save(output_path + "newsletter_with_pagelabels.pdf", SDFDoc.SaveOptions.e_linearized);
						Console.WriteLine("Done. Result saved in newsletter_with_pagelabels.pdf..."); 
					}
				}

				//-----------------------------------------------------------
				// Example 2: Read page labels from an existing PDF document.
				//-----------------------------------------------------------
				{
					using (PDFDoc doc = new PDFDoc(output_path + "newsletter_with_pagelabels.pdf"))
					{
						doc.InitSecurityHandler();

						PageLabel label;
						int page_num = doc.GetPageCount();
						for (int i=1; i<=page_num; ++i) 
						{
							Console.Write("Page number: {0}", i);
							label = doc.GetPageLabel(i);
							if (label.IsValid()) {
								Console.WriteLine(" Label: {0}", label.GetLabelTitle(i)); 
							}
							else {
								Console.WriteLine(" No Label."); 
							}
						}
					}
				}

				//-----------------------------------------------------------
				// Example 3: Modify page labels from an existing PDF document.
				//-----------------------------------------------------------
				{
					using (PDFDoc doc = new PDFDoc(output_path + "newsletter_with_pagelabels.pdf"))
					{
						doc.InitSecurityHandler();

						// Remove the alphabetic labels from example 1.
						doc.RemovePageLabel(7); 

						// Replace the Prefix in the decimal lables (from example 1).
						PageLabel label = doc.GetPageLabel(4);
						if (label.IsValid()) {
							label.SetPrefix("A");
							label.SetStart(1);
						}

						// Add a new label
						PageLabel new_label = PageLabel.Create(doc, PageLabel.Style.e_decimal, "B", 1);
						doc.SetPageLabel(10, new_label);  // starting from page 10.

						doc.Save(output_path + "newsletter_with_pagelabels_modified.pdf", SDFDoc.SaveOptions.e_linearized);
						Console.WriteLine("Done. Result saved in newsletter_with_pagelabels_modified.pdf..."); 

						int page_num = doc.GetPageCount();
						for (int i=1; i<=page_num; ++i) 
						{
							Console.Write("Page number: {0}", i);
							label = doc.GetPageLabel(i);
							if (label.IsValid()) {
								Console.WriteLine(" Label: {0}", label.GetLabelTitle(i));
							}
							else {
								Console.WriteLine(" No Label."); 
							}
						}
					}
				}

				//-----------------------------------------------------------
				// Example 4: Delete all page labels in an existing PDF document.
				//-----------------------------------------------------------
				{
					using (PDFDoc doc = new PDFDoc(output_path + "newsletter_with_pagelabels.pdf"))
					{
						doc.GetRoot().Erase("PageLabels");
						// ...
					}
				}
			}
			catch (pdftron.Common.PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}

		}
	}
}
