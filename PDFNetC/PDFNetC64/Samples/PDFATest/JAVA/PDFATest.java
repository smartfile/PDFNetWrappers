//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

import com.pdftron.common.PDFNetException;
import com.pdftron.pdf.*;
import com.pdftron.pdf.pdfa.*;

public class PDFATest {

    // Relative path to the folder containing test files.
    public static final String input_path = "../../TestFiles/";
    public static final String output_path = "../../TestFiles/Output/";

    public static void main(String[] args) {
      	try{ 
            PDFNet.initialize();
            PDFNet.setColorManagement(PDFNet.e_lcms); // Required for proper PDF/A validation and conversion.
		
            //-----------------------------------------------------------
            // Example 1: PDF/A Validation
            //-----------------------------------------------------------
		
			String filename = "newsletter.pdf";
            /* The max_ref_objs parameter to the PDFACompliance constructor controls the maximum number 
            of object numbers that are collected for particular error codes. The default value is 10 
            in order to prevent spam. If you need all the object numbers, pass 0 for max_ref_objs. */
            PDFACompliance pdf_a = new PDFACompliance(false, input_path + filename, null, PDFACompliance.e_Level2B, null, 10);
            printResults(pdf_a, filename);
            pdf_a.destroy();
        } catch (PDFNetException e) {
            System.out.println(e.getMessage());
        }
		
		
        
            //-----------------------------------------------------------
            // Example 2: PDF/A Conversion
            //-----------------------------------------------------------
        try {
			String filename = "fish.pdf";
            PDFACompliance pdf_a = new PDFACompliance(true, input_path + filename, null, PDFACompliance.e_Level2B, null, 10);
            filename = "pdfa.pdf";
            pdf_a.saveAs(output_path + filename, false);
            pdf_a.destroy();
            // output "pdf_a.pdf"

            // Re-validate the document after the conversion...
            pdf_a = new PDFACompliance(false, output_path + filename, null, PDFACompliance.e_Level2B, null, 10);
            printResults(pdf_a, filename);
            pdf_a.destroy();

            PDFNet.terminate();
        } catch (PDFNetException e) {
            System.out.println(e.getMessage());
        }

        System.out.println("PDFACompliance test completed.");
    }

    static void printResults(PDFACompliance pdf_a, String filename) {
        try {
            int err_cnt = pdf_a.getErrorCount();
            System.out.print(filename);
            if (err_cnt == 0) {
                System.out.print(": OK.\n");
            } else {
                System.out.println(" is NOT a valid PDFA.");
                for (int i = 0; i < err_cnt; ++i) {
                    int c = pdf_a.getError(i);
                    System.out.println(" - e_PDFA " + c + ": " + PDFACompliance.getPDFAErrorMessage(c) + ".");
                    if (true) {
                        int num_refs = pdf_a.getRefObjCount(c);
                        if (num_refs > 0) {
                            System.out.print("   Objects: ");
                            for (int j = 0; j < num_refs; ) {
                                System.out.print(String.valueOf(pdf_a.getRefObj(c, j)));
                                if (++j != num_refs) System.out.print(", ");
                            }
                            System.out.println();
                        }
                    }
                }
                System.out.println();
            }
        } catch (PDFNetException e) {
            System.out.println(e.getMessage());
        }
    }

}
