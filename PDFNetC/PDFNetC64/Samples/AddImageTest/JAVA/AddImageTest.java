//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

import java.io.File;
import java.io.IOException;

import javax.imageio.ImageIO;

import com.pdftron.common.Matrix2D;
import com.pdftron.common.PDFNetException;
import com.pdftron.pdf.*;
import com.pdftron.sdf.SDFDoc;
import com.pdftron.sdf.ObjSet;
import com.pdftron.sdf.Obj;

//-----------------------------------------------------------------------------------
// This sample illustrates how to embed various raster image formats
// (e.g. TIFF, JPEG, JPEG2000, JBIG2, GIF, PNG, BMP, etc.) in a PDF document.
//
// Note: On Windows platform this sample utilizes GDI+ and requires GDIPLUS.DLL to
// be present in the system path.
//-----------------------------------------------------------------------------------
public class AddImageTest {

	public static void main(String[] args) 
	{
		PDFNet.initialize();

		// Relative path to the folder containing test files.
		String input_path = "../../TestFiles/";
		String output_path = "../../TestFiles/Output/";

		try 
		{

			PDFDoc doc = new PDFDoc();
			
			ElementBuilder f = new ElementBuilder(); // Used to build new Element objects
			ElementWriter writer = new ElementWriter(); // Used to write Elements to the page
			
			Page page = doc.pageCreate(); // Start a new page
			writer.begin(page); // Begin writing to this page
			
			// ----------------------------------------------------------
			// Add JPEG image to the output file
			Image img = Image.create(doc.getSDFDoc(), input_path + "peppers.jpg");
			Element element = f.createImage(img, 50, 500, img.getImageWidth()/2, img.getImageHeight()/2);
			writer.writePlacedElement(element);

			// ----------------------------------------------------------
			// Add a PNG image to the output file
			img = Image.create(doc.getSDFDoc(), input_path + "butterfly.png");
			element = f.createImage(img, new Matrix2D(100, 0, 0, 100, 300, 500));
			writer.writePlacedElement(element);
			
			// ----------------------------------------------------------
			// Add a GIF image to the output file
			img = Image.create(doc.getSDFDoc(), input_path + "pdfnet.gif");
			element = f.createImage(img, new Matrix2D(img.getImageWidth(), 0, 0, img.getImageHeight(), 50, 350));
			writer.writePlacedElement(element);
			
			// ----------------------------------------------------------
			// Add a TIFF image to the output file
			img = Image.create(doc.getSDFDoc(), input_path + "grayscale.tif");
			element = f.createImage(img, new Matrix2D(img.getImageWidth(), 0, 0, img.getImageHeight(), 10, 50));
			writer.writePlacedElement(element);

			writer.end();           // Save the page
			doc.pagePushBack(page); // Add the page to the document page sequence
			
			// ----------------------------------------------------------
			// Embed a monochrome TIFF. Compress the image using lossy JBIG2 filter.

			page = doc.pageCreate(new Rect(0, 0, 612, 794));
			writer.begin(page); // begin writing to this page
			
			// Note: encoder hints can be used to select between different compression methods.
			// For example to instruct PDFNet to compress a monochrome image using JBIG2 compression.
			ObjSet hint_set = new ObjSet();
			Obj enc = hint_set.createArray();  // Initilaize encoder 'hint' parameter
			enc.pushBackName("JBIG2");
			enc.pushBackName("Lossy");

			img = Image.create(doc.getSDFDoc(), input_path + "multipage.tif");
			element = f.createImage(img, new Matrix2D(612, 0, 0, 794, 0, 0));
			writer.writePlacedElement(element);

			writer.end();           // Save the page
			doc.pagePushBack(page); // Add the page to the document page sequence

			// ----------------------------------------------------------
			// Add a JPEG2000 (JP2) image to the output file

			// Create a new page
			page = doc.pageCreate();
			writer.begin(page); // Begin writing to the page

			// Embed the image.
			img = Image.create(doc.getSDFDoc(), input_path + "palm.jp2");
			
			// Position the image on the page.
			element = f.createImage(img, new Matrix2D(img.getImageWidth(), 0, 0, img.getImageHeight(), 96, 80));
			writer.writePlacedElement(element);

			// Write 'JPEG2000 Sample' text string under the image.
			writer.writeElement(f.createTextBegin(Font.create(doc.getSDFDoc(), Font.e_times_roman), 32));
			element = f.createTextRun("JPEG2000 Sample");
			element.setTextMatrix(1, 0, 0, 1, 190, 30);
			writer.writeElement(element);
			writer.writeElement(f.createTextEnd());
			
			writer.end(); // Finish writing to the page
			doc.pagePushBack(page);

			// ----------------------------------------------------------
			// doc.Save((output_path + "addimage.pdf").c_str(), Doc.e_remove_unused, 0);
			doc.save((output_path + "addimage.pdf"), SDFDoc.SaveMode.LINEARIZED, null);
			doc.close();
			System.out.println("Done. Result saved in addimage.pdf...");
		}
		catch (PDFNetException e)
		{
			e.printStackTrace();
			System.out.println(e);
		}

		PDFNet.terminate();
	}
}
