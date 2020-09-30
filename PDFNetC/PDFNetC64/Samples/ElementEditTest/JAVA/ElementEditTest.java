//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

import com.pdftron.pdf.*;
import com.pdftron.sdf.SDFDoc;
import com.pdftron.common.PDFNetException;

import java.util.*;

import com.pdftron.sdf.Obj;

//---------------------------------------------------------------------------------------
// The sample code shows how to edit the page display list and how to modify graphics state 
// attributes on existing Elements. In particular the sample program strips all images from 
// the page, changes path fill color to red, and changes text color to blue. 
//---------------------------------------------------------------------------------------
public class ElementEditTest {
    public static void processElements(ElementWriter writer, ElementReader reader, Set<Integer> visited)  throws PDFNetException {
        Element element;
		while ((element = reader.next()) != null) {
			switch (element.getType()) {
				case Element.e_image:
				case Element.e_inline_image:
					// remove all images by skipping them
					break;
				case Element.e_path: {
					// Set all paths to red color.
					GState gs = element.getGState();
					gs.setFillColorSpace(ColorSpace.createDeviceRGB());
					gs.setFillColor(new ColorPt(1, 0, 0));
					writer.writeElement(element);
				}
				break;
				case Element.e_text: {
					// Set all text to blue color.
					GState gs = element.getGState();
					gs.setFillColorSpace(ColorSpace.createDeviceRGB());
					gs.setFillColor(new ColorPt(0, 0, 1));
					writer.writeElement(element);
				}
				break;
				case Element.e_form: {
					writer.writeElement(element); // write Form XObject reference to current stream
					Obj form_obj = element.getXObject();
					if (!visited.contains((int) form_obj.getObjNum())) // if this XObject has not been processed
					{
						// recursively process the Form XObject
						visited.add((int) form_obj.getObjNum());
						ElementWriter new_writer = new ElementWriter();
						reader.formBegin();
						new_writer.begin(form_obj);

						reader.clearChangeList();
						new_writer.setDefaultGState(reader);  

						processElements(new_writer, reader, visited);
						new_writer.end();
						reader.end();
					}
				}
				break;
				default:
					writer.writeElement(element);
					break;
			}
		}
  
    }

    public static void main(String[] args) {

        PDFNet.initialize();

        // Relative path to the folder containing test files.
        String input_path = "../../TestFiles/";
        String output_path = "../../TestFiles/Output/";
        String input_filename = "newsletter.pdf";
        String output_filename = "newsletter_edited.pdf";

        try {
            System.out.println("Opening the input file...");
            PDFDoc doc = new PDFDoc((input_path + input_filename));
            doc.initSecurityHandler();

            ElementWriter writer = new ElementWriter();
            ElementReader reader = new ElementReader();
            Set<Integer> visited = new TreeSet<Integer>();

            PageIterator itr = doc.getPageIterator();
            while (itr.hasNext()) {
				try{
					Page page = itr.next();
					visited.add((int) page.getSDFObj().getObjNum());

					reader.begin(page);
					writer.begin(page, ElementWriter.e_replacement, false, true, page.getResourceDict());

					processElements(writer, reader, visited);
					writer.end();
					reader.end();
				} catch (Exception e) {
					e.printStackTrace();
				}
            }

            // Save modified document
            doc.save(output_path + output_filename, SDFDoc.SaveMode.REMOVE_UNUSED, null);
            // output PDF doc
            doc.close();
            System.out.println("Done. Result saved in " + output_filename + "...");
        } catch (Exception e) {
            e.printStackTrace();
        }

        PDFNet.terminate();
    }
}
