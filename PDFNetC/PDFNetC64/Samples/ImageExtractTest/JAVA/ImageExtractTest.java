//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

import com.pdftron.common.Matrix2D;
import com.pdftron.common.PDFNetException;
import com.pdftron.pdf.*;
import com.pdftron.sdf.DictIterator;
import com.pdftron.sdf.Obj;
import com.pdftron.sdf.SDFDoc;

///-----------------------------------------------------------------------------------
/// This sample illustrates one approach to PDF image extraction 
/// using PDFNet.
/// 
/// Note: Besides direct image export, you can also convert PDF images 
/// to Java image, or extract uncompressed/compressed image data directly 
/// using element.GetImageData() (e.g. as illustrated in ElementReaderAdv 
/// sample project).
///-----------------------------------------------------------------------------------
public class ImageExtractTest {

    // Relative paths to folders containing test files.
    static String input_path = "../../TestFiles/";
    static String output_path = "../../TestFiles/Output/";

    static int image_counter = 0;

    static void ImageExtract(ElementReader reader) throws PDFNetException {
        Element element;
        while ((element = reader.next()) != null) {
            switch (element.getType()) {
                case Element.e_image:
                case Element.e_inline_image: {
                    System.out.println("--> Image: " + (++image_counter));
                    System.out.println("    Width: " + element.getImageWidth());
                    System.out.println("    Height: " + element.getImageHeight());
                    System.out.println("    BPC: " + element.getBitsPerComponent());

                    Matrix2D ctm = element.getCTM();
                    double x2 = 1, y2 = 1;
                    java.awt.geom.Point2D.Double p = ctm.multPoint(x2, y2);
                    System.out.println(String.format("    Coords: x1=%.2f, y1=%.2f, x2=%.2f, y2=%.2f", ctm.getH(), ctm.getV(), p.getX(), p.getY()));

                    if (element.getType() == Element.e_image) {
                        Image image = new Image(element.getXObject());

                        String fname = "image_extract1_" + image_counter;

                        String path = output_path + fname;
                        image.export(path);

                        //String path2 = output_path + fname + ".tif";
                        //image.exportAsTiff(path2);

                        //String path3 = output_path + fname + ".png";
                        //image.exportAsPng(path3);
                    }
                }
                break;
                case Element.e_form:        // Process form XObjects
                    reader.formBegin();
                    ImageExtract(reader);
                    reader.end();
                    break;
            }
        }
    }

    public static void main(String[] args) {
        // Initialize PDFNet
        PDFNet.initialize();

        // Example 1:
        // Extract images by traversing the display list for
        // every page. With this approach it is possible to obtain
        // image positioning information and DPI.
        try {
            PDFDoc doc = new PDFDoc((input_path + "newsletter.pdf"));
            doc.initSecurityHandler();
            ElementReader reader = new ElementReader();
            //  Read every page
            for (PageIterator itr = doc.getPageIterator(); itr.hasNext(); ) {
                reader.begin(itr.next());
                ImageExtract(reader);
                reader.end();
            }

            doc.close();
            System.out.println("Done.");
        } catch (Exception e) {
            e.printStackTrace();
        }


        System.out.println("----------------------------------------------------------------");

        // Example 2:
        // Extract images by scanning the low-level document.
        try {
            PDFDoc doc = new PDFDoc((input_path + "newsletter.pdf"));
            doc.initSecurityHandler();
            image_counter = 0;
            SDFDoc cos_doc = doc.getSDFDoc();
            long num_objs = cos_doc.xRefSize();
            for (int i = 1; i < num_objs; ++i) {
                Obj obj = cos_doc.getObj(i);
                if (obj != null && !obj.isFree() && obj.isStream()) {
                    // Process only images
                    DictIterator itr = obj.find("Type");
                    if (!itr.hasNext() || !itr.value().getName().equals("XObject"))
                        continue;

                    itr = obj.find("Subtype");
                    if (!itr.hasNext() || !itr.value().getName().equals("Image"))
                        continue;

                    Image image = new Image(obj);

                    System.out.println("--> Image: " + (++image_counter));
                    System.out.println("    Width: " + image.getImageWidth());
                    System.out.println("    Height: " + image.getImageHeight());
                    System.out.println("    BPC: " + image.getBitsPerComponent());

                    String fname = "image_extract2_" + image_counter;
                    String path = output_path + fname;
                    image.export(path);

                    //String path= output_path + fname + ".tif";
                    //image.exportAsTiff(path);

                    //String path = output_path + fname + ".png";
                    //image.exportAsPng(path);
                }
            }

            doc.close();
            System.out.println("Done.");
        } catch (Exception e) {
            e.printStackTrace();
        }

        PDFNet.terminate();
    }
}
