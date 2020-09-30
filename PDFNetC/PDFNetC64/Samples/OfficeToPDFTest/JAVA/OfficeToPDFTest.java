//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

import com.pdftron.common.PDFNetException;
import com.pdftron.pdf.Convert;
import com.pdftron.pdf.DocumentConversion;
import com.pdftron.pdf.PDFDoc;
import com.pdftron.pdf.PDFNet;
import com.pdftron.pdf.OfficeToPDFOptions;
import com.pdftron.sdf.SDFDoc;

//---------------------------------------------------------------------------------------
// The following sample illustrates how to use the PDF.Convert utility class to convert 
// MS Office files to PDF
//
// This conversion is performed entirely within the PDFNet and has *no* external or
// system dependencies dependencies -- Conversion results will be the sam whether
// on Windows, Linux or Android.
//
// Please contact us if you have any questions. 
//---------------------------------------------------------------------------------------
public class OfficeToPDFTest {

    static String input_path = "../../TestFiles/";
    static String output_path = "../../TestFiles/Output/";

    public static void main(String[] args) {
        PDFNet.initialize();
        PDFNet.setResourcesPath("../../../Resources");

        // first the one-line conversion interface
        simpleDocxConvert("Fishermen.docx", "Fishermen.pdf");

        // then the more flexible line-by-line interface
        flexibleDocxConvert("the_rime_of_the_ancient_mariner.docx", "the_rime_of_the_ancient_mariner.pdf");
       
        // conversion of RTL content
        flexibleDocxConvert("factsheet_Arabic.docx", "factsheet_Arabic.pdf");

        PDFNet.terminate();
    }

    public static void simpleDocxConvert(String inputFilename, String outputFilename) {
        try {

            // perform the conversion with no optional parameters
            PDFDoc pdfdoc = new PDFDoc();
            Convert.officeToPdf(pdfdoc, input_path + inputFilename, null);

            // save the result
            pdfdoc.save(output_path + outputFilename, SDFDoc.SaveMode.INCREMENTAL, null);
            // output PDF pdfdoc

            // And we're done!
            System.out.println("Done conversion " + output_path + outputFilename);
        } catch (PDFNetException e) {
            System.out.println("Unable to convert MS Office document, error:");
            e.printStackTrace();
            System.out.println(e);
        }
    }

    public static void flexibleDocxConvert(String inputFilename, String outputFilename) {
        try {
            OfficeToPDFOptions options = new OfficeToPDFOptions();
            options.setSmartSubstitutionPluginPath(input_path);

            // create a conversion object -- this sets things up but does not yet
            // perform any conversion logic.
            // in a multithreaded environment, this object can be used to monitor
            // the conversion progress and potentially cancel it as well
            DocumentConversion conversion = Convert.streamingPdfConversion(
                    input_path + inputFilename, options);

            System.out.println(inputFilename + ": " + Math.round(conversion.getProgress() * 100.0)
                    + "% " + conversion.getProgressLabel());

            // actually perform the conversion
            while (conversion.getConversionStatus() == DocumentConversion.e_incomplete) {
                conversion.convertNextPage();
                System.out.println(inputFilename + ": " + Math.round(conversion.getProgress() * 100.0)
                        + "% " + conversion.getProgressLabel());
            }

            if (conversion.tryConvert() == DocumentConversion.e_success) {
                int num_warnings = conversion.getNumWarnings();

                // print information about the conversion
                for (int i = 0; i < num_warnings; ++i) {
                    System.out.println("Warning: " + conversion.getWarningString(i));
                }

                // save the result
                PDFDoc doc = conversion.getDoc();
                doc.save(output_path + outputFilename, SDFDoc.SaveMode.INCREMENTAL, null);
                // output PDF doc

                // done
                System.out.println("Done conversion " + output_path + outputFilename);
            } else {
                System.out.println("Encountered an error during conversion: " + conversion.getErrorString());
            }
        } catch (PDFNetException e) {
            System.out.println("Unable to convert MS Office document, error:");
            e.printStackTrace();
            System.out.println(e);
        }
    }

}
