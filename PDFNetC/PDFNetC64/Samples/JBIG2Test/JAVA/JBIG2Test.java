//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

import com.pdftron.filters.Filter;
import com.pdftron.filters.FilterReader;
import com.pdftron.pdf.*;
import com.pdftron.sdf.DictIterator;
import com.pdftron.sdf.Obj;
import com.pdftron.sdf.ObjSet;
import com.pdftron.sdf.SDFDoc;

// This sample project illustrates how to recompress bi-tonal images in an 
// existing PDF document using JBIG2 compression. The sample is not intended 
// to be a generic PDF optimization tool.
//
// You can download the entire document using the following link:
//   http://www.pdftron.com/net/samplecode/data/US061222892.pdf
public class JBIG2Test {

    public static void main(String[] args) {
        PDFNet.initialize();

        String input_path = "../../TestFiles/";
        String output_path = "../../TestFiles/Output/";

        try {
            PDFDoc pdf_doc = new PDFDoc(input_path + "US061222892-a.pdf");
            pdf_doc.initSecurityHandler();

            SDFDoc cos_doc = pdf_doc.getSDFDoc();
            int num_objs = (int) cos_doc.xRefSize();
            for (int i = 1; i < num_objs; ++i) {
                Obj obj = cos_doc.getObj(i);
                if (obj != null && !obj.isFree() && obj.isStream()) {
                    // Process only images
                    DictIterator itr = obj.find("Subtype");
                    if (!itr.hasNext() || !itr.value().getName().equals("Image"))
                        continue;

                    Image input_image = new Image(obj);
                    // Process only gray-scale images
                    if (input_image.getComponentNum() != 1)
                        continue;
                    int bpc = input_image.getBitsPerComponent();
                    if (bpc != 1)    // Recompress only 1 BPC images
                        continue;

                    // Skip images that are already compressed using JBIG2
                    itr = obj.find("Filter");
                    if (itr.hasNext() && itr.value().isName() &&
                            !itr.value().getName().equals("JBIG2Decode")) continue;

                    Filter filter = obj.getDecodedStream();
                    FilterReader reader = new FilterReader(filter);

                    ObjSet hint_set = new ObjSet();
                    Obj hint = hint_set.createArray(); // A hint to image encoder to use JBIG2 compression
                    hint.pushBackName("JBIG2");
                    hint.pushBackName("Lossless");

                    Image new_image = Image.create(cos_doc, reader,
                            input_image.getImageWidth(),
                            input_image.getImageHeight(), 1, ColorSpace.createDeviceGray(), hint);

                    Obj new_img_obj = new_image.getSDFObj();
                    itr = obj.find("Decode");
                    if (itr.hasNext())
                        new_img_obj.put("Decode", itr.value());
                    itr = obj.find("ImageMask");
                    if (itr.hasNext())
                        new_img_obj.put("ImageMask", itr.value());
                    itr = obj.find("Mask");
                    if (itr.hasNext())
                        new_img_obj.put("Mask", itr.value());

                    cos_doc.swap(i, new_img_obj.getObjNum());
                }
            }

            pdf_doc.save(output_path + "US061222892_JBIG2.pdf", SDFDoc.SaveMode.REMOVE_UNUSED, null);
            // output PDF pdf_doc
            pdf_doc.close();
        } catch (Exception e) {
            e.printStackTrace();
        }

        PDFNet.terminate();
    }

}
