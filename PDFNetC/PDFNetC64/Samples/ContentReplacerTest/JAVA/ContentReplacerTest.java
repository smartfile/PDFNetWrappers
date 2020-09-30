//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

import com.pdftron.pdf.*;
import com.pdftron.sdf.*;

public class ContentReplacerTest {

    public static void main(String[] args) {
        String input_path = "../../TestFiles/";
        String output_path = input_path + "Output/";

        // The first step in every application using PDFNet is to initialize the
        // library and set the path to common PDF resources. The library is usually
        // initialized only once, but calling Initialize() multiple times is also fine.
        PDFNet.initialize();

        //--------------------------------------------------------------------------------
        // Example 1) Update a business card template with personalized info

        try {
            PDFDoc doc = new PDFDoc(input_path + "BusinessCardTemplate.pdf");
            doc.initSecurityHandler();

            ContentReplacer replacer = new ContentReplacer();
            Page page = doc.getPage(1);
            // first, replace the image on the first page
            Image img = Image.create(doc, input_path + "peppers.jpg");
            replacer.addImage(page.getMediaBox(), img.getSDFObj());
            // next, replace the text place holders on the second page
            replacer.addString("NAME", "John Smith");
            replacer.addString("QUALIFICATIONS", "Philosophy Doctor");
            replacer.addString("JOB_TITLE", "Software Developer");
            replacer.addString("ADDRESS_LINE1", "#100 123 Software Rd");
            replacer.addString("ADDRESS_LINE2", "Vancouver, BC");
            replacer.addString("PHONE_OFFICE", "604-730-8989");
            replacer.addString("PHONE_MOBILE", "604-765-4321");
            replacer.addString("EMAIL", "info@pdftron.com");
            replacer.addString("WEBSITE_URL", "http://www.pdftron.com");
            // finally, apply
            replacer.process(page);

            doc.save(output_path + "BusinessCard.pdf", SDFDoc.SaveMode.REMOVE_UNUSED, null);
            // output PDF doc
            doc.close();
            System.out.println("Done. Result saved in BusinessCard.pdf");
        } catch (Exception e) {
            e.printStackTrace();
            return;
        }

        //--------------------------------------------------------------------------------
        // Example 2) Replace text in a region with new text

        try {
            PDFDoc doc = new PDFDoc(input_path + "newsletter.pdf");
            doc.initSecurityHandler();

            ContentReplacer replacer = new ContentReplacer();
            Page page = doc.getPage(1);
            Rect target_region = page.getMediaBox();
            String replacement_text = "hello hello hello hello hello hello hello hello hello hello";
            replacer.addText(target_region, replacement_text);
            replacer.process(page);

            doc.save(output_path + "ContentReplaced.pdf", SDFDoc.SaveMode.REMOVE_UNUSED, null);
            // output PDF doc
            doc.close();
            System.out.println("Done. Result saved in ContentReplaced.pdf");
        } catch (Exception e) {
            e.printStackTrace();
            return;
        }

        System.out.println("Done.");

        PDFNet.terminate();
    }
}
