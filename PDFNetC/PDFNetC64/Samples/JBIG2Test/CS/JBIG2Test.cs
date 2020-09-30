//
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
//

using System;
using System.Drawing;
using System.Collections;

using pdftron;
using pdftron.Common;
using pdftron.Filters;
using pdftron.SDF;
using pdftron.PDF;

namespace JBIG2TestCS
{
	/// <summary>
	/// This sample project illustrates how to recompress bi-tonal images in an 
	/// existing PDF document using JBIG2 compression. The sample is not intended 
	/// to be a generic PDF optimization tool.
	/// 
	/// You can download a sample scanned document using the following link:
	///   http://www.pdftron.com/net/samplecode/data/US061222892.pdf
	///
	/// Also a sample page compressed using CCITT Fax compression is located under 
	/// 'PDFNet/Samples/TestFiles' folder.
	/// </summary>
	public class Class1
	{

		private static pdftron.PDFNetLoader pdfNetLoader = pdftron.PDFNetLoader.Instance();
		static Class1() {}
		
		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		static void Main() 
		{
			// Initialize PDFNet before calling any other PDFNet function.
			PDFNet.Initialize();

            string input_path = "../../TestFiles/";
            string output_path = "../../TestFiles/Output/";
            string input_filename = "US061222892-a.pdf";
            
            PDFDoc pdf_doc = new PDFDoc(input_path + input_filename);
            pdf_doc.InitSecurityHandler();
			
			SDFDoc cos_doc = pdf_doc.GetSDFDoc();
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
					
					pdftron.PDF.Image input_image = new pdftron.PDF.Image(obj);
					pdftron.PDF.Image new_image = null;

					// Process only gray-scale images
					if (input_image.GetComponentNum() != 1) 
						continue; 
					
					int bpc = input_image.GetBitsPerComponent();
					if (bpc != 1) // Recompress 1 BPC images
						continue;
					
					// Skip images that are already compressed using JBIG2
					itr = obj.Find("Filter");
					if (itr.HasNext() && itr.Value().IsName() && 
						itr.Value().GetName() == "JBIG2Decode") 
						continue; 

					FilterReader reader = new FilterReader(obj.GetDecodedStream());
					
					ObjSet hint_set = new ObjSet();
					Obj hint = hint_set.CreateArray();
					hint.PushBackName("JBIG2");
					hint.PushBackName("Lossless");
					hint.PushBackName("Threshold");
					hint.PushBackNumber(0.4);
					hint.PushBackName("SharePages");
					hint.PushBackNumber(10000);
					
					new_image = pdftron.PDF.Image.Create(
						cos_doc, 
						reader, 							
						input_image.GetImageWidth(), 
						input_image.GetImageHeight(), 
						1, 
						ColorSpace.CreateDeviceGray(),
						hint  // A hint to image encoder to use JBIG2 compression
						);

					Obj new_img_obj = new_image.GetSDFObj();

					// Copy any important entries from the image dictionary
					itr = obj.Find("ImageMask");
					if (itr.HasNext()) new_img_obj.Put("ImageMask", itr.Value());

					itr = obj.Find("Mask");
					if (itr.HasNext()) new_img_obj.Put("Mask", itr.Value());

					cos_doc.Swap(i, new_image.GetSDFObj().GetObjNum());
				}
			}
			
			pdf_doc.Save(output_path + "US061222892_JBIG2.pdf", SDFDoc.SaveOptions.e_remove_unused);
			pdf_doc.Close();
		}
	}
}
