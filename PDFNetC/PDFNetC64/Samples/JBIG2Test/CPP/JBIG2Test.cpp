//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

#include <iostream>
#include <PDF/PDFNet.h>
#include <PDF/PDFDoc.h>
#include <PDF/ElementBuilder.h>
#include <Filters/FilterReader.h>
#include <SDF/ObjSet.h>

using namespace std;
using namespace pdftron;
using namespace Filters;
using namespace PDF;
using namespace SDF;
using namespace Common;

// This sample project illustrates how to recompress bi-tonal images in an 
// existing PDF document using JBIG2 compression. The sample is not intended 
// to be a generic PDF optimization tool.
//
// You can download the entire document using the following link:
//   http://www.pdftron.com/net/samplecode/data/US061222892.pdf
//
int main(int argc, char *argv[]) 
{
	PDFNet::Initialize();
	
	try 
	{
		PDFDoc pdf_doc("../../TestFiles/US061222892-a.pdf");
		pdf_doc.InitSecurityHandler();

		SDFDoc& cos_doc = pdf_doc.GetSDFDoc();
		int num_objs = cos_doc.XRefSize();
		for(int i=1; i<num_objs; ++i) 
		{
			Obj obj = cos_doc.GetObj(i);
			if(obj && !obj.IsFree() && obj.IsStream()) 
			{
				// Process only images
				DictIterator itr = obj.Find("Subtype");
				if(!itr.HasNext() || strcmp(itr.Value().GetName(), "Image"))
					continue;
				
				Image input_image(obj);
				// Process only gray-scale images
				if(input_image.GetComponentNum() != 1)
					continue;
				int bpc = input_image.GetBitsPerComponent();
				if(bpc != 1)	// Recompress only 1 BPC images
					continue;

				// Skip images that are already compressed using JBIG2
				itr = obj.Find("Filter");
				if (itr.HasNext() && itr.Value().IsName() && 
					!strcmp(itr.Value().GetName(), "JBIG2Decode")) continue; 

				Filter filter=obj.GetDecodedStream();
				FilterReader reader(filter);


				ObjSet hint_set; 	// A hint to image encoder to use JBIG2 compression
				Obj hint=hint_set.CreateArray();
			
				hint.PushBackName("JBIG2");
				hint.PushBackName("Lossless");

				Image new_image = Image::Create(cos_doc, reader, 
					input_image.GetImageWidth(), 
					input_image.GetImageHeight(), 1, ColorSpace::CreateDeviceGray(), hint);

				Obj new_img_obj = new_image.GetSDFObj();
				itr = obj.Find("Decode");
				if(itr.HasNext())
					new_img_obj.Put("Decode", itr.Value());
				itr = obj.Find("ImageMask");
				if (itr.HasNext())
					new_img_obj.Put("ImageMask", itr.Value());
				itr = obj.Find("Mask");
				if (itr.HasNext())
					new_img_obj.Put("Mask", itr.Value());

				cos_doc.Swap(i, new_img_obj.GetObjNum());
			}
		}

		pdf_doc.Save("../../TestFiles/Output/US061222892_JBIG2.pdf", SDFDoc::e_remove_unused, 0);
	}
	catch(Common::Exception& e)
	{
		cout << e << endl;
		cout << "Please make sure that the pathname to the test file is correct." << endl;
	}
	catch(...)
	{
		cout << "Unknown Exception" << endl;
	}

	PDFNet::Terminate();
	return 0;
}
