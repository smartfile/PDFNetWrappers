//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

import java.lang.*;
import java.awt.*;

import com.pdftron.pdf.*;
import com.pdftron.sdf.SDFDoc;


// PDF Redactor is a separately licensable Add-on that offers options to remove 
// (not just covering or obscuring) content within a region of PDF. 
// With printed pages, redaction involves blacking-out or cutting-out areas of 
// the printed page. With electronic documents that use formats such as PDF, 
// redaction typically involves removing sensitive content within documents for 
// safe distribution to courts, patent and government institutions, the media, 
// customers, vendors or any other audience with restricted access to the content. 
//
// The redaction process in PDFNet consists of two steps:
// 
//  a) Content identification: A user applies redact annotations that specify the 
// pieces or regions of content that should be removed. The content for redaction 
// can be identified either interactively (e.g. using 'pdftron.PDF.PDFViewCtrl' 
// as shown in PDFView sample) or programmatically (e.g. using 'pdftron.PDF.TextSearch'
// or 'pdftron.PDF.TextExtractor'). Up until the next step is performed, the user 
// can see, move and redefine these annotations.
//  b) Content removal: Using 'pdftron.PDF.Redactor.Redact()' the user instructs 
// PDFNet to apply the redact regions, after which the content in the area specified 
// by the redact annotations is removed. The redaction function includes number of 
// options to control the style of the redaction overlay (including color, text, 
// font, border, transparency, etc.).
// 
// PDFTron Redactor makes sure that if a portion of an image, text, or vector graphics 
// is contained in a redaction region, that portion of the image or path data is 
// destroyed and is not simply hidden with clipping or image masks. PDFNet API can also 
// be used to review and remove metadata and other content that can exist in a PDF 
// document, including XML Forms Architecture (XFA) content and Extensible Metadata 
// Platform (XMP) content.
public class PDFRedactTest {

    public static void redact(String input, String output, Redactor.Redaction[] vec, Redactor.Appearance app) {
        try {
            PDFDoc doc = new PDFDoc(input);
            if (doc.initSecurityHandler()) {
                Redactor.redact(doc, vec, app, false, true);
                doc.save(output, SDFDoc.SaveMode.REMOVE_UNUSED, null);
                // output PDF doc
                doc.close();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void main(String[] args) {
        // Relative paths to folders containing test files.
        String input_path = "../../TestFiles/";
        String output_path = "../../TestFiles/Output/";

        PDFNet.initialize();

        try {
            Redactor.Redaction[] vec = new Redactor.Redaction[7];
            vec[0] = new Redactor.Redaction(1, new Rect(100, 100, 550, 600), false, "Top Secret");
            vec[1] = new Redactor.Redaction(2, new Rect(30, 30, 450, 450), true, "Negative Redaction");
            vec[2] = new Redactor.Redaction(2, new Rect(0, 0, 100, 100), false, "Positive");
            vec[3] = new Redactor.Redaction(2, new Rect(100, 100, 200, 200), false, "Positive");
            vec[4] = new Redactor.Redaction(2, new Rect(300, 300, 400, 400), false, "");
            vec[5] = new Redactor.Redaction(2, new Rect(500, 500, 600, 600), false, "");
			vec[6] = new Redactor.Redaction(3, new Rect(0, 0, 700, 20), false, "");
			
			Redactor.Appearance app = new Redactor.Appearance();
			app.redactionOverlay = true;
			app.border = false;
			app.showRedactedContentRegions = true;

            redact(input_path + "newsletter.pdf", output_path + "redacted.pdf", vec, app);

            System.out.println("Done...");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}