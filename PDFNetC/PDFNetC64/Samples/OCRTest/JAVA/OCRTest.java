//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

import com.pdftron.sdf.Obj;
import com.pdftron.sdf.ObjSet;
import com.pdftron.sdf.SDFDoc;
import com.pdftron.pdf.*;

import com.pdftron.common.PDFNetException;

//---------------------------------------------------------------------------------------
// The following sample illustrates how to use OCR module
//---------------------------------------------------------------------------------------
public class OCRTest {
	public static void main(String[] args) {
		try {
			// The first step in every application using PDFNet is to initialize the
			// library and set the path to common PDF resources. The library is usually
			// initialized only once, but calling Initialize() multiple times is also fine.
			PDFNet.initialize();
			PDFNet.addResourceSearchPath("../../../Lib/");
			if( !OCRModule.isModuleAvailable() )
			{
				System.out.println("");
				System.out.println("Unable to run OCRTest: PDFTron SDK OCR module not available.");
				System.out.println("---------------------------------------------------------------");
				System.out.println("The OCR module is an optional add-on, available for download");
				System.out.println("at http://www.pdftron.com/. If you have already downloaded this");
				System.out.println("module, ensure that the SDK is able to find the required files");
				System.out.println("using the PDFNet.addResourceSearchPath() function.");
				System.out.println("");
				return;
			}

			// Relative path to the folder containing test files.
			String input_path = "../../TestFiles/OCR/";
			String output_path = "../../TestFiles/Output/";

			//--------------------------------------------------------------------------------
			// Example 1) Process image without specifying options, default language - English - is used
			try 
			{

				// A) Setup empty destination doc

				PDFDoc doc = new PDFDoc();

				// B) Run OCR on the .png with options

				OCRModule.imageToPDF(doc, input_path + "psychomachia_excerpt.png", null);

				// C) check the result

				doc.save(output_path + "psychomachia_excerpt.pdf", SDFDoc.SaveMode.LINEARIZED, null);
											
				doc.close();

				System.out.println("Example 1: psychomachia_excerpt.png");
				
			} catch (Exception e) {
				e.printStackTrace();
			}

			//--------------------------------------------------------------------------------
			// Example 2) Process document using multiple languages
			try 
			{

				// A) Setup empty destination doc

				PDFDoc doc = new PDFDoc();

				// B) Setup options with multiple target languages, English will always be considered as secondary language

				OCROptions options = new OCROptions();

				options.addLang("rus");
				options.addLang("deu");

				// C) Run OCR on the .jpg with options

				OCRModule.imageToPDF(doc, input_path + "multi_lang.jpg", options);

				// D) check the result

				doc.save(output_path + "multi_lang.pdf", SDFDoc.SaveMode.LINEARIZED, null);
											
				doc.close();

				System.out.println("Example 2: multi_lang.jpg");
				
			} catch (Exception e) {
				e.printStackTrace();
			}

			//--------------------------------------------------------------------------------
			// Example 3) Process a .pdf specifying a language - German - and ignore zone comprising a sidebar image 
			try 
			{

				// A) Open the .pdf document

				PDFDoc doc = new PDFDoc(input_path + "german_kids_song.pdf");

				// B) Setup options with a single language and an ignore zone

				OCROptions options = new OCROptions();
				options.addLang("deu");

				RectCollection zones = new RectCollection();
				zones.addRect(424, 163, 493, 730);

				options.addIgnoreZonesForPage(zones, 1);

				// C) Run OCR on the .pdf with options

				OCRModule.processPDF(doc, options);
				
				// D) check the result

				doc.save(output_path + "german_kids_song.pdf", SDFDoc.SaveMode.LINEARIZED, null);
				
				doc.close();

				System.out.println("Example 3: german_kids_song.pdf");
				
			} catch (Exception e) {
				e.printStackTrace();
			}

			//--------------------------------------------------------------------------------
			// Example 4) Process multipage tiff with text/ignore zones specified for each page, optionally provide English as the target language

			try 
			{
				// A) Setup empty destination doc

				PDFDoc doc = new PDFDoc();

			   // B) Setup options with a single language plus text/ignore zones

				OCROptions options = new OCROptions();
				options.addLang("eng");

				RectCollection zones = new RectCollection();
				zones.addRect(1492, 56, 2236, 432);

				// ignore signature box in the first 2 pages
				options.addIgnoreZonesForPage(zones, 1);
				options.addIgnoreZonesForPage(zones, 2);

				// can use a combination of ignore and text boxes to focus on the page area of interest,
				// as ignore boxes are applied first, we remove the arrows before selecting part of the diagram
				zones.clear();
				zones.addRect(992, 1276, 1368, 1372);
				options.addIgnoreZonesForPage(zones, 3);

				// we only have text zones selected in page 3

				zones.clear();
				// select horizontal BUFFER ZONE sign
				zones.addRect(900, 2384, 1236, 2480);
				// select right vertical BUFFER ZONE sign
				zones.addRect(1960, 1976, 2016, 2296);
				// select Lot No.
				zones.addRect(696, 1028, 1196, 1128);

				// select part of the plan inside the BUFFER ZONE
				zones.addRect(428, 1484, 1784, 2344);
				zones.addRect(948, 1288, 1672, 1476);

				options.addTextZonesForPage(zones, 3);

				// C) Run OCR on the .tif with options

				OCRModule.imageToPDF(doc, input_path + "bc_environment_protection.tif", options);
				
				// D) check the result

				doc.save(output_path + "bc_environment_protection.pdf", SDFDoc.SaveMode.LINEARIZED, null);
				
				doc.close();

				System.out.println("Example 4: bc_environment_protection.tif");
				
			} catch (Exception e) {
				e.printStackTrace();
			}

			//--------------------------------------------------------------------------------
			// Example 5) Alternative workflow for extracting OCR result JSON, postprocessing (e.g., removing words not in the dictionary or filtering special
			// out special characters), and finally applying modified OCR JSON to the source PDF document 
			try 
			{

				// A) Open the .pdf document

				PDFDoc doc = new PDFDoc(input_path + "zero_value_test_no_text.pdf");

				// B) Run OCR on the .pdf with default English language

				String json = OCRModule.getOCRJsonFromPDF(doc, null);

				// C) Post-processing step (whatever it might be), but we just print json here

				System.out.println("Have OCR result JSON, re-applying to PDF");

				// D) Apply potentially modified OCR JSON to the PDF

				OCRModule.applyOCRJsonToPDF(doc, json);

				// E) Check the result

				doc.save(output_path + "zero_value_test_no_text.pdf", SDFDoc.SaveMode.LINEARIZED, null);

				doc.close();

				System.out.println("Example 5: extracting and applying OCR JSON from zero_value_test_no_text.pdf");
				
			} catch (Exception e) {
				e.printStackTrace();
			}

		//--------------------------------------------------------------------------------
		// Example 6) The postprocessing workflow has also an option of extracting OCR results in XML format, similar to the one used by TextExtractor
		try
		{

			// A) Setup empty destination doc

			PDFDoc doc = new PDFDoc();

			// B) Run OCR on the .tif with default English language, extracting OCR results in XML format. Note that
			// in the process we convert the source image into PDF. We reuse this PDF document later to add hidden text layer to it.

			String xml = OCRModule.getOCRXmlFromImage(doc, input_path + "physics.tif", null);

			// C) Post-processing step (whatever it might be), but we just print XML here

			System.out.println("Have OCR result XML, applying to PDF");

			// D) Apply potentially modified OCR XML to the PDF

			OCRModule.applyOCRXmlToPDF(doc, xml);

			// E) Check the result
		
			doc.save(output_path + "physics.pdf", SDFDoc.SaveMode.LINEARIZED, null);											
			doc.close();
			System.out.println("Example 6: extracting and applying OCR XML from physics.tif");

		}
		catch (Exception e) {
			e.printStackTrace();
		}

		//--------------------------------------------------------------------------------
		// Example 7) Resolution can be manually set, when DPI missing from metadata or is wrong

		try 
		{
			// A) Setup empty destination doc

			PDFDoc doc = new PDFDoc();

			// B) Setup options with a text zone

			OCROptions options = new OCROptions();

			RectCollection zones = new RectCollection();
			zones.addRect(140, 870, 310, 920);
			options.addTextZonesForPage(zones, 1);

			// C) Manually override DPI
			options.addDPI(100);

			// D) Run OCR on the .jpg with options
			OCRModule.imageToPDF(doc, input_path + "corrupted_dpi.jpg", options);
				
			// D) check the result

			doc.save(output_path + "corrupted_dpi.pdf", SDFDoc.SaveMode.LINEARIZED, null);
				
			doc.close();

			System.out.println("Example 7: converting image with corrupted resolution metadata corrupted_dpi.jpg to pdf with searchable text");
				
		} catch (Exception e) {
			e.printStackTrace();
		}

			// Calling Terminate when PDFNet is no longer in use is a good practice, but
			// is not required.
			PDFNet.terminate();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}