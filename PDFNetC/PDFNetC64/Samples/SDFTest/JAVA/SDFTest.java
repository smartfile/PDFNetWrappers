//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

import com.pdftron.pdf.*;
import com.pdftron.sdf.*;
import com.pdftron.filters.*;

// This sample illustrates how to use basic SDF API (also known as Cos) to edit an 
// existing document.
public class SDFTest {
    public static void main(String[] args) {
        PDFNet.initialize();

        // Relative path to the folder containing test files.
        String input_path = "../../TestFiles/";
        String output_path = "../../TestFiles/Output/";

        try {
            System.out.println("Opening the test file...");

            // Here we create a SDF/Cos document directly from PDF file. In case you have
            // PDFDoc you can always access SDF/Cos document using PDFDoc.GetSDFDoc() method.
            SDFDoc doc = new SDFDoc((input_path + "fish.pdf"));
            doc.initSecurityHandler();

            System.out.println("Modifying info dictionary, adding custom properties, embedding a stream...");
            Obj trailer = doc.getTrailer();            // Get the trailer

            // Now we will change PDF document information properties using SDF API

            // Get the Info dictionary.
            DictIterator itr = trailer.find("Info");
            Obj info;
            if (itr.hasNext()) {
                info = itr.value();
                // Modify 'Producer' entry.
                info.putString("Producer", "PDFTron PDFNet");

                // Read title entry (if it is present)
                itr = info.find("Author");
                if (itr.hasNext()) {
                    String oldstr = itr.value().getAsPDFText();

                    info.putText("Author", oldstr + "- Modified");
                } else {
                    info.putString("Author", "Me, myself, and I");
                }
            } else {
                // Info dict is missing.
                info = trailer.putDict("Info");
                info.putString("Producer", "PDFTron PDFNet");
                info.putString("Title", "My document");
            }

            // Create a custom inline dictionary within Info dictionary
            Obj custom_dict = info.putDict("My Direct Dict");
            custom_dict.putNumber("My Number", 100);     // Add some key/value pairs
            custom_dict.putArray("My Array");

            // Create a custom indirect array within Info dictionary
            Obj custom_array = doc.createIndirectArray();
            info.put("My Indirect Array", custom_array);    // Add some entries

            // Create indirect link to root
            custom_array.pushBack(trailer.get("Root").value());

            // Embed a custom stream (file mystream.txt).
            MappedFile embed_file = new MappedFile(input_path + "my_stream.txt");
            FilterReader mystm = new FilterReader(embed_file);
            custom_array.pushBack(doc.createIndirectStream(mystm));

            // Save the changes.
            System.out.println("Saving modified test file...");
            doc.save(output_path + "sdftest_out.pdf", SDFDoc.SaveMode.NO_FLAGS, null, "%PDF-1.4");
            // output PDF doc
            doc.close();

            System.out.println("Test completed.");
        } catch (Exception e) {
            System.out.println(e);
        }

        PDFNet.terminate();
    }
}
