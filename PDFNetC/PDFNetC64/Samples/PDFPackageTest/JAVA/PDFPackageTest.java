//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

import com.pdftron.common.PDFNetException;
import com.pdftron.pdf.*;
import com.pdftron.filters.*;
import com.pdftron.sdf.*;


//-----------------------------------------------------------------------------------
// This sample illustrates how to create, extract, and manipulate PDF Portfolios
// (a.k.a. PDF Packages) using PDFNet SDK.
//-----------------------------------------------------------------------------------
public class PDFPackageTest {
    // Relative path to the folder containing test files.
    static String input_path = "../../TestFiles/";
    static String output_path = "../../TestFiles/Output/";

    public static void main(String[] args) {
        PDFNet.initialize();

        // Create a PDF Package.
        try {
            PDFDoc doc = new PDFDoc();
            addPackage(doc, input_path + "numbered.pdf", "My File 1");
            addPackage(doc, input_path + "newsletter.pdf", "My Newsletter...");
            addPackage(doc, input_path + "peppers.jpg", "An image");
            addCoverPage(doc);
            doc.save(output_path + "package.pdf", SDFDoc.SaveMode.LINEARIZED, null);
            System.out.println("Done.");			
            doc.close();
        } catch (Exception e) {
            e.printStackTrace();
        }

        // Extract parts from a PDF Package.
        try {
            PDFDoc doc = new PDFDoc(output_path + "package.pdf");
            doc.initSecurityHandler();

            com.pdftron.sdf.NameTree files = NameTree.find(doc.getSDFDoc(), "EmbeddedFiles");
            if (files.isValid()) {
                // Traverse the list of embedded files.
                NameTreeIterator i = files.getIterator();
                for (int counter = 0; i.hasNext(); i.next(), ++counter) {
                    String entry_name = i.key().getAsPDFText();
                    System.out.println("Part: " + entry_name);

                    FileSpec file_spec = new FileSpec(i.value());
                    Filter stm = file_spec.getFileData();
                    if (stm != null) {
                        String ext = "pdf";
                        if (entry_name.lastIndexOf('.') > 0) {
                            ext = entry_name.substring(entry_name.lastIndexOf('.')+1);
                        }
                        String fname = "extract_" + counter + "." + ext;
                        stm.writeToFile(output_path + fname, false);
                    }
                }
            }
            System.out.println("Done.");
            doc.close();
        } catch (Exception e) {
            e.printStackTrace();
        }

        PDFNet.terminate();
    }

    static void addPackage(PDFDoc doc, String file, String desc) throws PDFNetException {
        NameTree files = NameTree.create(doc.getSDFDoc(), "EmbeddedFiles");
        FileSpec fs = FileSpec.create(doc, file, true);
        files.put(file.getBytes(), fs.getSDFObj());
        fs.getSDFObj().putText("Desc", desc);

        Obj collection = doc.getRoot().findObj("Collection");
        if (collection == null) collection = doc.getRoot().putDict("Collection");

        // You could here manipulate any entry in the Collection dictionary.
        // For example, the following line sets the tile mode for initial view mode
        // Please refer to section '2.3.5 Collections' in PDF Reference for details.
        collection.putName("View", "T");
    }

    static void addCoverPage(PDFDoc doc) throws PDFNetException {
        // Here we dynamically generate cover page (please see ElementBuilder
        // sample for more extensive coverage of PDF creation API).
        Page page = doc.pageCreate(new Rect(0, 0, 200, 200));

        ElementBuilder b = new ElementBuilder();
        ElementWriter w = new ElementWriter();
        w.begin(page);
        Font font = Font.create(doc.getSDFDoc(), Font.e_helvetica);
        w.writeElement(b.createTextBegin(font, 12));
        Element e = b.createTextRun("My PDF Collection");
        e.setTextMatrix(1, 0, 0, 1, 50, 96);
        e.getGState().setFillColorSpace(ColorSpace.createDeviceRGB());
        e.getGState().setFillColor(new ColorPt(1, 0, 0));
        w.writeElement(e);
        w.writeElement(b.createTextEnd());
        w.end();
        doc.pagePushBack(page);

        // Alternatively we could import a PDF page from a template PDF document
        // (for an example please see PDFPage sample project).
        // ...
    }
}
