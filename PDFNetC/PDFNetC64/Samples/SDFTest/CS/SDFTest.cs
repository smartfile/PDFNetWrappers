//
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
//

using System;
using pdftron;
using pdftron.Common;
using pdftron.Filters;
using pdftron.SDF;
using pdftron.PDF;

namespace SDFTestCS
{
	/// <summary>
	/// This sample illustrates how to use basic SDF API (also known as Cos) to edit an 
	/// existing document.
	/// </summary>
	class Class1
	{
		private static pdftron.PDFNetLoader pdfNetLoader = pdftron.PDFNetLoader.Instance();
		static Class1() {}
		
		static void Main(string[] args)
		{
			PDFNet.Initialize();

			// Relative path to the folder containing test files.
			string input_path =  "../../TestFiles/";
			string output_path = "../../TestFiles/Output/";


			//------------------------------------------------------------------
			Console.WriteLine("Opening the test file...");

			try
			{
				// Here we create a SDF/Cos document directly from PDF file. In case you have 
				// PDFDoc you can always access SDF/Cos document using PDFDoc.GetSDFDoc() method.
				using (SDFDoc doc = new SDFDoc(input_path + "fish.pdf"))
				{
					doc.InitSecurityHandler();				
                    
					Console.WriteLine("Modifying info dictionary, adding custom properties, embedding a stream...");

					Obj trailer = doc.GetTrailer();	// Get the trailer

					// Now we will change PDF document information properties using SDF API

					// Get the Info dictionary. 
					DictIterator itr = trailer.Find("Info");	
					Obj info;
					if (itr.HasNext()) 
					{
						info = itr.Value();
						// Modify 'Producer' entry.
						info.PutString("Producer", "PDFTron PDFNet");

						// Read title entry (if it is present)
						itr = info.Find("Author"); 
						if (itr.HasNext()) 
						{
							info.PutString("Author", itr.Value().GetAsPDFText() + "- Modified");
						}
						else 
						{
							info.PutString("Author", "Joe Doe");
						}
					}
					else 
					{
						// Info dict is missing. 
						info = trailer.PutDict("Info");
						info.PutString("Producer", "PDFTron PDFNet");
						info.PutString("Title", "My document");
					}

					// Create a custom inline dictionary within Info dictionary
					Obj custom_dict = info.PutDict("My Direct Dict");

					// Add some key/value pairs
					custom_dict.PutNumber("My Number", 100);

					Obj my_array = custom_dict.PutArray("My Array");

					// Create a custom indirect array within Info dictionary
					Obj custom_array = doc.CreateIndirectArray();	
					info.Put("My Indirect Array", custom_array);
					
					// Create indirect link to root
					custom_array.PushBack(trailer.Get("Root").Value());

					// Embed a custom stream (file my_stream.txt).
					MappedFile embed_file = new MappedFile(input_path + "my_stream.txt");
					FilterReader mystm = new FilterReader(embed_file);
					custom_array.PushBack(doc.CreateIndirectStream(mystm));

					// Save the changes.
					Console.WriteLine("Saving modified test file...");
					doc.Save(output_path + "sdftest_out.pdf", 0, "%PDF-1.4");
				}

				Console.WriteLine("Test completed.");
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}

		}
	}
}
