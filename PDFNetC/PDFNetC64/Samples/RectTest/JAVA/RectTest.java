//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

import com.pdftron.pdf.*;
import com.pdftron.sdf.SDFDoc;

public class RectTest {

    public static void main(String[] args) {
        PDFNet.initialize();

        // Relative path to the folder containing test files.
        String input_path = "../../TestFiles/";
        String output_path = "../../TestFiles/Output/";

        try // Test  - Adjust the position of content within the page.
        {
            System.out.println("_______________________________________________");
            System.out.println("Opening the input pdf...");

            PDFDoc input_doc = new PDFDoc((input_path + "tiger.pdf"));
            input_doc.initSecurityHandler();

            PageIterator pg_itr1 = input_doc.getPageIterator();

            Rect media_box = pg_itr1.next().getMediaBox();

            media_box.setX1(media_box.getX1() - 200);    // translate the page 200 units (1 uint = 1/72 inch)
            media_box.setX2(media_box.getX2() - 200);

            media_box.update();

            input_doc.save(output_path + "tiger_shift.pdf", SDFDoc.SaveMode.NO_FLAGS, null);
            // output PDF input_doc
            input_doc.close();
            System.out.println("Done. Result saved in tiger_shift...");
        } catch (Exception e) {
            System.out.println(e);
        }

        PDFNet.terminate();
    }
}
