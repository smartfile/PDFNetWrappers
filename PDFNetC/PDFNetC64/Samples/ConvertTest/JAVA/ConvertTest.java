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
// The following sample illustrates how to use the PDF::Convert utility class to convert 
// documents and files to PDF, XPS, SVG, or EMF.
//
// Certain file formats such as XPS, EMF, PDF, and raster image formats can be directly 
// converted to PDF or XPS. Other formats are converted using a virtual driver. To check 
// if ToPDF (or ToXPS) require that PDFNet printer is installed use Convert::RequiresPrinter(filename). 
// The installing application must be run as administrator. The manifest for this sample 
// specifies appropriate the UAC elevation.
//
// Note: the PDFNet printer is a virtual XPS printer supported on Vista SP1 and Windows 7.
// For Windows XP SP2 or higher, or Vista SP0 you need to install the XPS Essentials Pack (or 
// equivalent redistributables). You can download the XPS Essentials Pack from:
//		http://www.microsoft.com/downloads/details.aspx?FamilyId=B8DCFFDD-E3A5-44CC-8021-7649FD37FFEE&displaylang=en
// Windows XP Sp2 will also need the Microsoft Core XML Services (MSXML) 6.0:
// 		http://www.microsoft.com/downloads/details.aspx?familyid=993C0BCF-3BCF-4009-BE21-27E85E1857B1&displaylang=en
//
// Note: Convert.fromEmf and Convert.toEmf will only work on Windows and require GDI+.
//
// Please contact us if you have any questions.	
//---------------------------------------------------------------------------------------
public class ConvertTest {

    public static void main(String[] args) {
        boolean uninstallPrinterWhenDone = false; // change this to test the uninstallation functions
        PDFNet.initialize();

        // Relative path to the folder containing test files.
        String input_path = "../../TestFiles/";
        String output_path = "../../TestFiles/Output/";
        String outputFile;
        boolean printerInstalled = false;

        System.out.println("-------------------------------------------------");

        try {
            // See if the alternative printer is installed, the PDFNet printer
            // is installed, or if not try to install a printer
            if (ConvertPrinter.isInstalled("PDFTron PDFNet")) {
                ConvertPrinter.setPrinterName("PDFTron PDFNet");
                printerInstalled = true;
                System.out.println("PDFTron PDFNet Printer is already installed");
            } else if (ConvertPrinter.isInstalled()) {
                printerInstalled = true;
                System.out.println("PDFTron PDFNet Printer is already installed");
            } else {
                System.out.println("Installing printer (requires administrator and Windows platform)");
                ConvertPrinter.install();
                System.out.println("Installed printer " + ConvertPrinter.getPrinterName());
                printerInstalled = true;
            }
        } catch (PDFNetException e) {
            System.out.println("Unable to install printer, error:");
            System.out.println(e);
        }

        // Convert an MS Word document to pdf (Windows platform only, requires MS Word
        // and PDFNet printer installed
        try {
            if (Convert.requiresPrinter(input_path + "simple-word_2007.docx")
                    && printerInstalled) {
                System.out.println("Converting MS Word document to PDF");
                System.out.println("Using the PDFTron PDFNet printer");
                PDFDoc doc = new PDFDoc();
                Convert.toPdf(doc, input_path + "simple-word_2007.docx");
                outputFile = output_path + "docx2pdf_Java.pdf";
                doc.save(outputFile, SDFDoc.SaveMode.LINEARIZED, null);
                // output PDF doc
                System.out.println("Result saved in " + outputFile);
                doc.close();
            } else {
                System.out.println("Unable to convert MS Word document to PDF because printer is not installed");
            }
        } catch (PDFNetException e) {
            System.out.println("Unable to convert MS Word document, error:");
            e.printStackTrace();
            System.out.println(e);
        }

        // Uninstall the printer
        if (printerInstalled && uninstallPrinterWhenDone) {
            try {
                System.out.println("Uninstalling printer (requires administrator)");
                ConvertPrinter.uninstall();
                System.out.println("Uninstalled printer " + ConvertPrinter.getPrinterName());
            } catch (PDFNetException e) {
                System.out.println("Unable to uninstall printer, error:");
                System.out.println(e);
            }
        }

        try {
            System.out.println("Converting Text to PDF with options");
            ObjSet objset = new ObjSet();
            Obj options = objset.createDict();
            options.putNumber("FontSize", 15);
            options.putBool("UseSourceCodeFormatting", true);
            options.putNumber("PageWidth", 12);
            options.putNumber("PageHeight", 6);
            PDFDoc doc = new PDFDoc();
            outputFile = output_path + "simple-text.pdf";
            Convert.fromText(doc, input_path + "simple-text.txt", options);
            doc.save(outputFile, SDFDoc.SaveMode.LINEARIZED, null);
            // output PDF doc
            System.out.println("Result saved in " + outputFile);
            doc.close();
        } catch (PDFNetException e) {
            System.out.println("Unable to convert Plain Text document to PDF, error:");
            System.out.println(e);
        }

        // Convert the XPS document to PDF
        try {
            System.out.println("Converting XPS document to PDF");
            PDFDoc doc = new PDFDoc();
            Convert.fromXps(doc, input_path + "simple-xps.xps");
            outputFile = output_path + "xps2pdf_Java.pdf";
            doc.save(outputFile, SDFDoc.SaveMode.LINEARIZED, null);
            // output PDF doc
            System.out.println("Result saved in " + outputFile);
            doc.close();
        } catch (PDFNetException e) {
            System.out.println("Unable to convert XPS document to PDF, error:");
            System.out.println(e);
        }

        // Convert the PDF document to EMF (Windows platform only)
        try {
            System.out.println("Converting PDF document to EMF");
            PDFDoc doc = new PDFDoc();
            Convert.fromXps(doc, input_path + "simple-xps.xps");
            outputFile = output_path + "pdf2emf_Java.emf";
            Convert.toEmf(doc, outputFile);
            System.out.println("Result saved in " + outputFile);
            doc.close();
        } catch (PDFNetException e) {
            System.out.println("Unable to convert PDF document to EMF, error:");
            System.out.println(e);
        }

        // Convert the PDF document to SVG
        try {
            System.out.println("Converting XPS document to SVG");
            PDFDoc doc = new PDFDoc(input_path + "tiger.pdf");
            Convert.fromXps(doc, input_path + "simple-xps.xps");
            System.out.println("Converting PDF document to SVG");
            outputFile = output_path + "pdf2svg_Java.svg";
            Convert.toSvg(doc, outputFile);
            System.out.println("Result saved in " + outputFile);
            doc.close();
        } catch (PDFNetException e) {
            System.out.println("Unable to convert PDF document to SVG, error:");
            System.out.println(e);
        }

        // Convert the PDF document to XPS
        try {
            System.out.println("Converting PDF document to XPS");
            PDFDoc doc = new PDFDoc(input_path + "newsletter.pdf");
            outputFile = output_path + "pdf2xps_Java.xps";
            Convert.toXps(doc, outputFile);
            System.out.println("Result saved in " + outputFile);
            doc.close();
        } catch (PDFNetException e) {
            System.out.println("Unable to convert PDF document to SVG, error:");
            System.out.println(e);
        }

        // Convert an image to PDF using internal converter
        try {
            System.out.println("Converting PNG image to PDF");
            PDFDoc doc = new PDFDoc();
            Convert.toPdf(doc, input_path + "butterfly.png");
            outputFile = output_path + "png2pdf_Java.pdf";
            doc.save(outputFile, SDFDoc.SaveMode.LINEARIZED, null);
            // output PDF doc
            System.out.println("Result saved in " + outputFile);
            doc.close();
        } catch (PDFNetException e) {
            System.out.println("Unable to convert PNG image to XPS, error:");
            System.out.println(e);
        }

        // Convert an image to XPS using internal converter
        try {
            System.out.println("Converting PNG image to XPS");
            outputFile = output_path + "buttefly_Java.xps";
            Convert.toXps(input_path + "butterfly.png", outputFile);
            System.out.println("Result saved in " + outputFile);
        } catch (PDFNetException e) {
            System.out.println("Unable to convert PNG image to XPS, error:");
            System.out.println(e);
        }

        // Convert an MSWord document to XPS using printer
        try {
            System.out.println("Converting DOCX to XPS");
            outputFile = output_path + "simple-word_2007_Java.xps";
            Convert.toXps(input_path + "simple-word_2007.docx", outputFile);
            System.out.println("Result saved in " + outputFile);
        } catch (PDFNetException e) {
            System.out.println("Unable to convert MSWord document to XPS, error:");
            System.out.println(e);
        }

        // Convert a PDF document directly to XPS
        try {
            System.out.println("Converting PDF to XPS");
            outputFile = output_path + "newsletter.xps";
            Convert.toXps(input_path + "newsletter.pdf", outputFile);
            System.out.println("Result saved in " + outputFile);
        } catch (PDFNetException e) {
            System.out.println("Unable to convert PDF document to XPS, error:");
            System.out.println(e);
        }

        // Convert a PDF document to HTML
        try {
            System.out.println("Converting PDF to HTML");
            outputFile = output_path + "newsletter";
            Convert.toHtml(input_path + "newsletter.pdf", outputFile);
            System.out.println("Result saved in " + outputFile);
        } catch (PDFNetException e) {
            System.out.println("Unable to convert PDF document to HTML, error:");
            System.out.println(e);
        }

        // Convert a PDF document to EPUB
        try {
            System.out.println("Converting PDF to EPUB");
            outputFile = output_path + "newsletter.epub";
            Convert.toEpub(input_path + "newsletter.pdf", outputFile);
            System.out.println("Result saved in " + outputFile);
        } catch (PDFNetException e) {
            System.out.println("Unable to convert PDF document to EPUB, error:");
            System.out.println(e);
        }

        // Convert a PDF document to multipage TIFF
        try {
            System.out.println("Converting PDF to multipage TIFF");
            outputFile = output_path + "newsletter.tiff";
            Convert.TiffOutputOptions tiff_options = new Convert.TiffOutputOptions();
            tiff_options.setDPI(200);
            tiff_options.setMono(true);
            tiff_options.setDither(true);
            Convert.toTiff(input_path + "newsletter.pdf", outputFile, tiff_options);
            System.out.println("Result saved in " + outputFile);
        } catch (PDFNetException e) {
            System.out.println("Unable to convert PDF document to TIFF, error:");
            System.out.println(e);
        }

        System.out.println("Done.");
        PDFNet.terminate();
    }
}
