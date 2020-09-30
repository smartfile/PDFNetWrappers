//
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
//

using System;
using System.Drawing;
using System.Drawing.Imaging;

using pdftron;
using pdftron.Common;
using pdftron.PDF;
using pdftron.SDF;
using pdftron.Filters;

namespace ImageExtractTestCS
{
	class Class1
	{
		/// <summary>
		///-----------------------------------------------------------------------------------
		/// This sample illustrates one approach to PDF image extraction 
		/// using PDFNet.
		/// 
		/// Note: Besides direct image export, you can also convert PDF images 
		/// to GDI+ Bitmap, or extract uncompressed/compressed image data directly 
		/// using element.GetImageData() (e.g. as illustrated in ElementReaderAdv 
		/// sample project).
		///-----------------------------------------------------------------------------------
		/// </summary>

		private static pdftron.PDFNetLoader pdfNetLoader = pdftron.PDFNetLoader.Instance();
		static Class1() {}

		static int image_counter = 0;

		// Relative path to the folder containing test files.
		static string input_path =  "../../TestFiles/";
		static string output_path = "../../TestFiles/Output/";

		static void ImageExtract(PDFDoc doc, ElementReader reader) 
		{
			Element element; 
			while ((element = reader.Next()) != null)
			{
				switch (element.GetType()) 
				{
					case Element.Type.e_image:
					case Element.Type.e_inline_image:
					{
						Console.WriteLine("--> Image: {0}", ++image_counter);
						Console.WriteLine("    Width: {0}", element.GetImageWidth());
						Console.WriteLine("    Height: {0}", element.GetImageHeight());
						Console.WriteLine("    BPC: {0}", element.GetBitsPerComponent());

						Matrix2D ctm = element.GetCTM();
						double x2=1, y2=1, y1=ctm.m_v;
						ctm.Mult(ref x2, ref y2);
                        // Write the coords to 3 decimal places.
						Console.WriteLine("    Coords: x1={0:N2}, y1={1:N2}, x2={2:N2}, y2={3:N2}", ctm.m_h, ctm.m_v, x2, y2);
						pdftron.PDF.Image image = null;
						if (element.GetType() == Element.Type.e_image) 
						{
							image = new pdftron.PDF.Image(element.GetXObject());

							string fname = output_path + "image_extract1_" + image_counter.ToString();
							image.Export(fname);  // or ExporAsPng() or ExporAsTiff() ...
						}
						break;
					}
					case Element.Type.e_form: // Process form XObjects
					{
						reader.FormBegin(); 
						ImageExtract(doc, reader);
						reader.End(); 
						break; 
					}
				}
			}
		}

		static void Main(string[] args)
		{
			PDFNet.Initialize();
			
			// Example 1: 
			// Extract images by traversing the display list for 
			// every page. With this approach it is possible to obtain 
			// image positioning information and DPI.
			try	
			{
				using (PDFDoc doc = new PDFDoc(input_path + "newsletter.pdf"))
				using (ElementReader reader = new ElementReader())
				{
					doc.InitSecurityHandler();
					PageIterator itr;
					for (itr=doc.GetPageIterator(); itr.HasNext(); itr.Next())	
					{				
						reader.Begin(itr.Current());
						ImageExtract(doc, reader);
						reader.End();
					}

					Console.WriteLine("Done.");
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}

			Console.WriteLine("----------------------------------------------------------------");

			// Example 2: 
			// Extract images by scanning the low-level document.
			try	
			{
				using (PDFDoc doc = new PDFDoc(input_path + "newsletter.pdf"))
				{
					doc.InitSecurityHandler();
					image_counter = 0;

					SDFDoc cos_doc = doc.GetSDFDoc();
					int num_objs = cos_doc.XRefSize();
					for (int i=1; i<num_objs; ++i)
					{
						Obj obj = cos_doc.GetObj(i);
						if (obj!=null && !obj.IsFree()&& obj.IsStream()) 
						{
							// Process only images
							DictIterator itr = obj.Find("Subtype");
							if (!itr.HasNext() || itr.Value().GetName() != "Image") 
								continue; 

							itr = obj.Find("Type");
							if (!itr.HasNext() || itr.Value().GetName() != "XObject") 
								continue;

							pdftron.PDF.Image image = new pdftron.PDF.Image(obj);

							Console.WriteLine("--> Image: {0}", ++image_counter);
							Console.WriteLine("    Width: {0}", image.GetImageWidth());
							Console.WriteLine("    Height: {0}", image.GetImageHeight());
							Console.WriteLine("    BPC: {0}", image.GetBitsPerComponent());

							string fname = output_path + "image_extract2_" + image_counter.ToString();
							image.Export(fname);  // or ExporAsPng() or ExporAsTiff() ...

							// Convert PDF bitmap to GDI+ Bitmap...
							//Bitmap bmp = image.GetBitmap();
							//bmp.Save(fname, ImageFormat.Png);
							//bmp.Dispose();

							// Instead of converting PDF images to a Bitmap, you can also extract 
							// uncompressed/compressed image data directly using element.GetImageData() 
							// as illustrated in ElementReaderAdv sample project.
						}
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
