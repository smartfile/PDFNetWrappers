//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

import com.pdftron.common.PDFNetException;
import com.pdftron.pdf.*;
import com.pdftron.sdf.Obj;
import com.pdftron.sdf.ObjSet;
import com.pdftron.sdf.SDFDoc;

//---------------------------------------------------------------------------------------
// The following sample illustrates how to convert CAD documents to PDF format using
// the CAD2PDF class.
// 
// 'pdftron.PDF.CAD2PDF' is an optional PDFNet Add-On utility class that can be 
// used to convert CAD documents into PDF documents by using an external module (cad2pdf).
//
// cad2pdf modules can be downloaded from http://www.pdftron.com/pdfnet/downloads.html.
//---------------------------------------------------------------------------------------
public class CAD2PDFTest {
	
	public static boolean IsRVTFile(String input_file_name)
	{
		boolean rvt_input = false;
		
		if (input_file_name.length() > 2)
		{
			if (input_file_name.substring(input_file_name.length() - 3, input_file_name.length()) == "rvt")
			{
				rvt_input = true;
			}
		}
		return rvt_input;
	}

    public static void main(String[] args) {
		String input_file_name = "construction drawings color-28.05.18.dwg";
		String output_file_name = "construction drawings color-28.05.18.pdf";
		if (args.length != 0)
		{
			input_file_name = args[0];
			output_file_name = input_file_name + ".pdf";
		}
        PDFNet.initialize();
		try
		{
			PDFNet.addResourceSearchPath("../../../Lib/");
			if(!CADModule.isModuleAvailable())
			{
                System.out.println();
                System.out.println("Unable to run CAD2PDFTest: PDFTron SDK CAD module not available.");
                System.out.println("---------------------------------------------------------------");
                System.out.println("The CAD module is an optional add-on, available for download");
                System.out.println("at http://www.pdftron.com/. If you have already downloaded this");
                System.out.println("module, ensure that the SDK is able to find the required files");
                System.out.println("using the PDFNet::AddResourceSearchPath() function." );
                System.out.println();
			}
		} catch (PDFNetException e) {
            System.out.println("CAD module not available, error:");
            e.printStackTrace();
            System.out.println(e);
        }


        // Relative path to the folder containing test files.
        String input_path = "../../TestFiles/CAD/";
        String output_path = "../../TestFiles/Output/";
        String outputFile;
        boolean printerInstalled = false;

        System.out.println("-------------------------------------------------");
		
        // Convert an CAD document to pdf (Windows platform only)
        try {
			System.out.println("Converting DWG CAD document to PDF");
			
			
			PDFDoc doc = new PDFDoc();
			if (IsRVTFile(input_file_name))
			{
				CADConvertOptions opts = new CADConvertOptions();
				opts.setPageHeight(800);
				opts.setPageWidth(300);
				Convert.fromCAD(doc, input_path + input_file_name, opts);
			}
			else
			{
				Convert.fromCAD(doc, input_path + input_file_name, null);
			}
			outputFile = output_path + output_file_name;
			doc.save(outputFile, SDFDoc.SaveMode.LINEARIZED, null);
			// output PDF doc
			System.out.println("Result saved in " + outputFile);
			doc.close();
        } catch (PDFNetException e) {
            System.out.println("Unable to convert DWG document, error:");
            e.printStackTrace();
            System.out.println(e);
        }

        System.out.println("Done.");
        PDFNet.terminate();
    }
}
