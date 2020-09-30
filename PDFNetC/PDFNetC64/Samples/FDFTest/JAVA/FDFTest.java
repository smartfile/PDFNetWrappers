//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

import com.pdftron.pdf.*;
import com.pdftron.sdf.SDFDoc;
import com.pdftron.fdf.*;


//---------------------------------------------------------------------------------------
// PDFNet includes a full support for FDF (Forms Data Format) and capability to merge/extract 
// forms data (FDF) with/from PDF. This sample illustrates basic FDF merge/extract functionality 
// available in PDFNet.
//---------------------------------------------------------------------------------------
public class FDFTest {
    public static void main(String[] args) {

        PDFNet.initialize();

        // Relative path to the folder containing test files.
        String input_path = "../../TestFiles/";
        String output_path = "../../TestFiles/Output/";

        // Example 1)
        // Iterate over all form fields in the document. Display all field names.
        try {
            PDFDoc doc = new PDFDoc((input_path + "form1.pdf"));
            doc.initSecurityHandler();

            for (FieldIterator itr = doc.getFieldIterator(); itr.hasNext(); ) {
                Field current = itr.next();
                System.out.println("Field name: " + current.getName());
                System.out.println("Field partial name: " + current.getPartialName());

                System.out.print("Field type: ");
                int type = current.getType();
                switch (type) {
                    case Field.e_button:
                        System.out.println("Button");
                        break;
                    case Field.e_text:
                        System.out.println("Text");
                        break;
                    case Field.e_choice:
                        System.out.println("Choice");
                        break;
                    case Field.e_signature:
                        System.out.println("Signature");
                        break;
                }

                System.out.println("------------------------------");
            }

            doc.close();
            System.out.println("Done.");
        } catch (Exception e) {
            e.printStackTrace();
        }

        // Example 2) Import XFDF into FDF, then merge data from FDF into PDF
        try {
            // XFDF to FDF
            // form fields
            System.out.println("Import form field data from XFDF to FDF.");

            FDFDoc fdf_doc1 = FDFDoc.createFromXFDF((input_path + "form1_data.xfdf"));
            fdf_doc1.save(output_path + "form1_data.fdf");
            // output FDF fdf_doc1

            // annotations
            System.out.println("Import annotations from XFDF to FDF.");

            FDFDoc fdf_doc2 = FDFDoc.createFromXFDF((input_path + "form1_annots.xfdf"));
            fdf_doc2.save(output_path + "form1_annots.fdf");
            // output FDF fdf_doc2

            // FDF to PDF
            // form fields
            System.out.println("Merge form field data from FDF.");

            PDFDoc doc = new PDFDoc((input_path + "form1.pdf"));
            doc.initSecurityHandler();
            doc.fdfMerge(fdf_doc1);

            // Refreshing missing appearances is not required here, but is recommended to make them 
            // visible in PDF viewers with incomplete annotation viewing support. (such as Chrome)
            doc.refreshAnnotAppearances();

            doc.save((output_path + "form1_filled.pdf"), SDFDoc.SaveMode.LINEARIZED, null);
            // output PDF doc

            // annotations
            System.out.println("Merge annotations from FDF.");

            doc.fdfMerge(fdf_doc2);
            // Refreshing missing appearances is not required here, but is recommended to make them 
            // visible in PDF viewers with incomplete annotation viewing support. (such as Chrome)
            doc.refreshAnnotAppearances();
            doc.save(output_path + "form1_filled_with_annots.pdf", SDFDoc.SaveMode.LINEARIZED, null);
            // output PDF doc
            doc.close();
            System.out.println("Done.");
        } catch (Exception e) {
            e.printStackTrace();
        }

        // Example 3) Extract data from PDF to FDF, then export FDF as XFDF
        try {
            // PDF to FDF
            PDFDoc in_doc = new PDFDoc((output_path + "form1_filled_with_annots.pdf"));
            in_doc.initSecurityHandler();

            // form fields only
            System.out.println("Extract form fields data to FDF.");

            FDFDoc doc_fields = in_doc.fdfExtract(PDFDoc.e_forms_only);
            doc_fields.setPDFFileName("../form1_filled_with_annots.pdf");
            doc_fields.save(output_path + "form1_filled_data.fdf");
            // output FDF doc_fields

            // annotations only
            System.out.println("Extract annotations to FDF.");

            FDFDoc doc_annots = in_doc.fdfExtract(PDFDoc.e_annots_only);
            doc_annots.setPDFFileName("../form1_filled_with_annots.pdf");
            doc_annots.save(output_path + "form1_filled_annot.fdf");
            // output FDF doc_annots

            // both form fields and annotations
            System.out.println("Extract both form fields and annotations to FDF.");

            FDFDoc doc_both = in_doc.fdfExtract(PDFDoc.e_both);
            doc_both.setPDFFileName("../form1_filled_with_annots.pdf");
            doc_both.save(output_path + "form1_filled_both.fdf");
            // output FDF doc_both

            // FDF to XFDF
            // form fields
            System.out.println("Export form field data from FDF to XFDF.");

            doc_fields.saveAsXFDF((output_path + "form1_filled_data.xfdf"));
            // output FDF doc_fields

            // annotations
            System.out.println("Export annotations from FDF to XFDF.");

            doc_annots.saveAsXFDF((output_path + "form1_filled_annot.xfdf"));
            // output FDF doc_annots

            // both form fields and annotations
            System.out.println("Export both form fields and annotations from FDF to XFDF.");

            doc_both.saveAsXFDF((output_path + "form1_filled_both.xfdf"));
            // output FDF doc_both

            in_doc.close();
            System.out.println("Done.");
        } catch (Exception e) {
            e.printStackTrace();
        }

        // Example 4) Merge/Extract XFDF into/from PDF
        try {
            // Merge XFDF from string
            PDFDoc in_doc = new PDFDoc((input_path + "numbered.pdf"));
            in_doc.initSecurityHandler();

            System.out.println("Merge XFDF string into PDF.");

            String str = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><xfdf xmlns=\"http://ns.adobe.com/xfdf\" xml:space=\"preserve\"><square subject=\"Rectangle\" page=\"0\" name=\"cf4d2e58-e9c5-2a58-5b4d-9b4b1a330e45\" title=\"user\" creationdate=\"D:20120827112326-07'00'\" date=\"D:20120827112326-07'00'\" rect=\"227.7814207650273,597.6174863387978,437.07103825136608,705.0491803278688\" color=\"#000000\" interior-color=\"#FFFF00\" flags=\"print\" width=\"1\"><popup flags=\"print,nozoom,norotate\" open=\"no\" page=\"0\" rect=\"0,792,0,792\" /></square></xfdf>";

            FDFDoc fdoc = FDFDoc.createFromXFDF(str);
            in_doc.fdfMerge(fdoc);
            in_doc.save(output_path + "numbered_modified.pdf", SDFDoc.SaveMode.LINEARIZED, null);
            // output PDF in_doc
            System.out.println("Merge complete.");

            // Extract XFDF as string
            System.out.println("Extract XFDF as a string.");

            FDFDoc fdoc_new = in_doc.fdfExtract(PDFDoc.e_both);
            String XFDF_str = fdoc_new.saveAsXFDF();
            System.out.println("Extracted XFDF: ");
            System.out.println(XFDF_str);
            in_doc.close();
            System.out.println("Extract complete.");
        } catch (Exception e) {
            e.printStackTrace();
        }

        // Example 5) Read FDF files directly
        try {
            FDFDoc doc = new FDFDoc((output_path + "form1_filled_data.fdf"));

            for (FDFFieldIterator itr = doc.getFieldIterator(); itr.hasNext(); ) {
                FDFField current = itr.next();
                System.out.println("Field name: " + current.getName());
                System.out.println("Field partial name: " + current.getPartialName());

                System.out.println("------------------------------");
            }
            doc.close();
            System.out.println("Done.");
        } catch (Exception e) {
            e.printStackTrace();
        }

        // Example 6) Direct generation of FDF.
        try {
            FDFDoc doc = new FDFDoc();
            // Create new fields (i.e. key/value pairs).
            doc.fieldCreate("Company", Field.e_text, "PDFTron Systems");
            doc.fieldCreate("First Name", Field.e_text, "John");
            doc.fieldCreate("Last Name", Field.e_text, "Doe");
            // ...

            // doc.setPdfFileName("mydoc.pdf");

            doc.save(output_path + "sample_output.fdf");
            // output FDF doc
            doc.close();
            System.out.println("Done. Results saved in sample_output.fdf");
        } catch (Exception e) {
            e.printStackTrace();
        }

        PDFNet.terminate();
    }
}
