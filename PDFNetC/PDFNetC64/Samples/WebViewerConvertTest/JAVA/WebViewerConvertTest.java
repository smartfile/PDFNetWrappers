//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

import com.pdftron.common.PDFNetException;
import com.pdftron.pdf.*;

//---------------------------------------------------------------------------------------
// The following sample illustrates how to convert PDF, XPS, image, MS Office, and 
// other image document formats to XOD format.
//
// Certain file formats such as PDF, generic XPS, EMF, and raster image formats can 
// be directly converted to XOD. Other formats such as MS Office 
// (Word, Excel, Publisher, Powerpoint, etc) can be directly converted via interop. 
// These types of conversions guarantee optimal output, while preserving important 
// information such as document metadata, intra document links and hyper-links, 
// bookmarks etc. 
//
// In case there is no direct conversion available, PDFNet can still convert from 
// any printable document to XOD using a virtual printer driver. To check 
// if a virtual printer is required use Convert::RequiresPrinter(filename). In this 
// case the installing application must be run as administrator. The manifest for this 
// sample specifies appropriate the UAC elevation. The administrator privileges are 
// not required for direct or interop conversions. 
//
// Please note that PDFNet Publisher (i.e. 'pdftron.PDF.Convert.ToXod') is an
// optionally licensable add-on to PDFNet Core SDK. For details, please see
// http://www.pdftron.com/webviewer/licensing.html.
//---------------------------------------------------------------------------------------

public class WebViewerConvertTest {
	public WebViewerConvertTest() {

	}

	private class TestFile {
		public String inputFile;
		public String outputFile;
		public boolean requiresWindowsPlatform;

		public TestFile(String inputFile, String outputFile,
				boolean requiresWindowsPlatform) {
			this.inputFile = inputFile;
			this.outputFile = outputFile;
			this.requiresWindowsPlatform = requiresWindowsPlatform;
		}
	}

	public static boolean printerInstalled;
	
	// Relative path to the folder containing test files.
	public static String input_path = "../../TestFiles/";
	public static String output_path = "../../TestFiles/Output/";	
	
	public TestFile[] testFiles = {
			new TestFile("simple-powerpoint_2007.pptx",
					"simple-powerpoint_2007.xod", true),
			new TestFile("simple-word_2007.docx", "simple-word_2007.xod", true),
			new TestFile("butterfly.png", "butterfly.xod", false),
			new TestFile("numbered.pdf", "numbered.xod", false),
			new TestFile("dice.jpg", "dice.xod", false),
			new TestFile("simple-xps.xps", "xps2pdf.xod", false) };

	public void bulkConvertRandomFilesToXod() {
		int err = 0;

		try {
			// See if the alternative printer is installed, the PDFNet printer
			// is installed, or if not try to install a printer
			if (ConvertPrinter.isInstalled("PDFTron PDFNet")) {
				ConvertPrinter.setPrinterName("PDFTron PDFNet");
				printerInstalled = true;
				System.out
						.println("PDFTron PDFNet Printer is already installed");
			} else if (ConvertPrinter.isInstalled()) {
				printerInstalled = true;
				System.out
						.println("PDFTron PDFNet Printer is already installed");
			} else {
				System.out
						.println("Installing printer (requires administrator and Windows platform)");
				// This will fail if not run as administrator. Harmless if
				// PDFNet
				// printer already installed
				ConvertPrinter.install();
				System.out.println("Installed printer "
						+ ConvertPrinter.getPrinterName());
				printerInstalled = true;
			}
		} catch (PDFNetException e) {
			System.out.println("Unable to install printer, error:");
			System.out.println(e);
		}
		for (int i=0; i<testFiles.length; ++i) {
			TestFile file=testFiles[i];
			try {
				if (Convert.requiresPrinter(file.inputFile)) {
					String osName = System.getProperty("os.name");
					if (osName.indexOf("Windows",0)==-1) {
						continue;
					}
					System.out.println("Using PDFNet printer to convert file "
							+ file.inputFile);
				}
				Convert.toXod(input_path + file.inputFile, output_path + file.outputFile);
				System.out.println("Converted file: " + file.inputFile
						+ " to :" + file.outputFile);

			} catch (PDFNetException e) {
				System.out.println("Unable to convert file: " + file.inputFile);
				System.out.println(e.toString());
				err = 1;
			}
		}
		if (err == 1) {
			System.out.println("ConvertFile failed");
		} else {
			System.out.println("ConvertFile succeeded");
		}

		// Uninstall the printer
		if (printerInstalled) {
			try {
				System.out
						.println("Uninstalling printer (requires administrator)");
				ConvertPrinter.uninstall();
				System.out.println("Uninstalled printer "
						+ ConvertPrinter.getPrinterName());
			} catch (PDFNetException e) {
				System.out.println("Unable to uninstall printer, error:");
				System.out.println(e);
			}
		}
	}

	public static void main(String[] args) {
		// The first step in every application using PDFNet is to initialize the
		// library. The library is usually initialized only once, but calling
		// Initialize() multiple times is also fine.
		PDFNet.initialize();

		
		String outputFile;
		printerInstalled = false;

		System.out.println("-------------------------------------------------");

		try {
			// Sample 1:
			// Directly convert from PDF to XOD.
			Convert.toXod(input_path + "newsletter.pdf", output_path
					+ "from_pdf.xod");

			// Sample 2:
			// Directly convert from generic XPS to XOD.
			Convert.toXod(input_path + "simple-xps.xps", output_path
					+ "from_xps.xod");

			// Sample 3:
			// Convert from MS Office (does not require printer driver for
			// Office 2007+)
			// and other document formats to XOD.
			WebViewerConvertTest test = new WebViewerConvertTest();
			test.bulkConvertRandomFilesToXod();

		} catch (PDFNetException e) {
			System.out
					.println("Unable to convert file document to XOD, error:");
			System.out.println(e);
		}

		PDFNet.terminate();
		System.out.println("Done.");
	}
}
