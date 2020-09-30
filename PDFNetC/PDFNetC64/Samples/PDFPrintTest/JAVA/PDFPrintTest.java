//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

import java.awt.Graphics;
import java.awt.image.BufferedImage;
import java.awt.print.PageFormat;
import java.awt.print.Printable;
import java.awt.print.PrinterException;
import java.awt.print.PrinterJob;

import javax.print.attribute.*;
import javax.print.attribute.standard.MediaPrintableArea;


import com.pdftron.common.PDFNetException;
import com.pdftron.pdf.*;

/// The following sample illustrates how to print PDF document using currently selected
/// default printer. 
/// 
/// The first example uses the new PDF::Print::StartPrintJob function to send a rasterization 
/// of the document with optimal compression to the printer.  If the OS is Windows 7, then the
/// XPS print path will be used to preserve vector quality.  For earlier Windows versions
/// the print path will be used.  On other operating systems this will be a no-op
/// 
/// The second example uses PDFDraw send unoptimized rasterized data via awt.print API. 
/// 
/// If you would like to rasterize page at high resolutions (e.g. more than 600 DPI), you 
/// should use PDFRasterizer or PDFNet vector output instead of PDFDraw. 

public class PDFPrintTest implements Printable {
    PDFDoc doc;
    PDFDraw draw;
    BufferedImage image = null;

    PDFPrintTest() {
        try {
            PDFNet.initialize();
            doc = new PDFDoc("../../TestFiles/tiger.pdf");
            doc.initSecurityHandler();

            //////////////////////////////////////////////////////////////////////////
            // Example 1: use the PDF.Print.startPrintJob interface
            // This is silent (no progress dialog) and blocks until print job is at spooler
            // The rasterized print job is compressed before sending to printer
            System.out.println("Printing the input file using PDF.Print.StartPrintJob...");

            // Print.startPrintJob can determine the default printer name for you, just pass an empty printer name

            // Setup printing options:
            PrinterMode printerMode = new PrinterMode();
            printerMode.setCollation(true);
            printerMode.setCopyCount(1);
            printerMode.setDPI(300); // regardless of ordering, an explicit DPI setting overrides the OutputQuality setting
            printerMode.setDuplexing(PrinterMode.e_Duplex_Auto);
            printerMode.setOutputColor(PrinterMode.e_OutputColor_Grayscale);
            printerMode.setOutputQuality(PrinterMode.e_OutputQuality_Medium);

            // printerMode.setPaperSize(PrinterMode.e_6_3_Quarters_Envelope);

            PageSet pagesToPrint = new PageSet(1, doc.getPageCount(), PageSet.e_all);

            // Print the document on the default printer, name the print job the name of the
            // file, print to the printer not a file, and use printer options:
            Print.startPrintJob(doc, "", "tiger.pdf", "", pagesToPrint, printerMode, null);
        } catch (PDFNetException e) {
            e.printStackTrace();
        }


        //////////////////////////////////////////////////////////////////////////
        // Example 2: Use Java.awt.print and PDFDraw rasterizer.
        System.out.println("Printing the input file using Java.awt.print API...");
        try {
            draw = new PDFDraw();
            draw.setDPI(200);

            PrinterJob job = PrinterJob.getPrinterJob();

            PageFormat pf = job.defaultPage();

            HashPrintRequestAttributeSet psettings = new HashPrintRequestAttributeSet();
            psettings.add(new MediaPrintableArea(0, 0,
                    (int) pf.getWidth(), (int) pf.getHeight(), MediaPrintableArea.MM));

            job.setPrintable(this);
            boolean ok = job.printDialog();
            if (ok) {
                try {
                    job.print(psettings);
                } catch (PrinterException ex) {
                    //The Print did not complete successfully
                    ex.printStackTrace();
                }
            }
            doc.close();
        } catch (PDFNetException e) {
            e.printStackTrace();
        }
        PDFNet.terminate();
    }

    public static void main(String[] args) {
        new PDFPrintTest();
    }

    public int print(Graphics g, PageFormat format, int page_num) throws PrinterException {
        try {
            if (page_num < 0 || page_num >= doc.getPageCount())
                return Printable.NO_SUCH_PAGE;

            draw.drawInRect(g, doc.getPage(page_num + 1), (int) (0), (int) (0),
                    (int) (format.getWidth()), (int) (format.getHeight()));

            return Printable.PAGE_EXISTS;
        } catch (PDFNetException e) {
            e.printStackTrace();
        }
        return Printable.NO_SUCH_PAGE;
    }
}
