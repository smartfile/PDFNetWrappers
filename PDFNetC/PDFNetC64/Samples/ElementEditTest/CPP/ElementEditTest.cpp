//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

#include <PDF/PDFNet.h>
#include <PDF/PDFDoc.h>
#include <PDF/ElementWriter.h>
#include <PDF/ElementReader.h>
#include <SDF/Obj.h>
#include <set>
#include <iostream>

using namespace pdftron;
using namespace std;
using namespace SDF;
using namespace PDF;


//---------------------------------------------------------------------------------------
// The sample code shows how to edit the page display list and how to modify graphics state 
// attributes on existing Elements. In particular the sample program strips all images from 
// the page, changes path fill color to red, and changes text fill color to blue. 
//---------------------------------------------------------------------------------------

// XObjects are guaranteed to have unique object numbers 
typedef set<pdftron::UInt32> XObjSet;


static void ProcessElements(ElementReader& reader, ElementWriter& writer, XObjSet& visited) 
{
	Element element;
	while (element = reader.Next()) // Read page contents
	{
		switch (element.GetType())
		{
		case Element::e_image: 
		case Element::e_inline_image: 
			// remove all images by skipping them			
			break;
		case Element::e_path:
			{
				// Set all paths to red color.
				GState gs = element.GetGState();
				gs.SetFillColorSpace(ColorSpace::CreateDeviceRGB());
				ColorPt cp(1, 0, 0);
				gs.SetFillColor(cp);
				writer.WriteElement(element);
				break;
			}
		case Element::e_text:
			{
				// Set all text to blue color.
				GState gs = element.GetGState();
				gs.SetFillColorSpace(ColorSpace::CreateDeviceRGB());
				ColorPt cp(0, 0, 1);
				gs.SetFillColor(cp);
				writer.WriteElement(element);
				break;
			}
		case Element::e_form:
			{
				writer.WriteElement(element); // write Form XObject reference to current stream

				Obj form_obj = element.GetXObject();
				if (visited.find(form_obj.GetObjNum()) == visited.end()) // if this XObject has not been processed
				{
					// recursively process the Form XObject
					visited.insert(form_obj.GetObjNum());
					ElementWriter new_writer;
					reader.FormBegin();
					new_writer.Begin(form_obj);

					reader.ClearChangeList();
					new_writer.SetDefaultGState(reader); 

					ProcessElements(reader, new_writer, visited);
					new_writer.End();
					reader.End();
				}
				break; 
			}
		default:
			writer.WriteElement(element);
		}
	}
}

int main(int argc, char *argv[])
{
	int ret = 0;
	PDFNet::Initialize();

	// Relative path to the folder containing test files.
	string input_path =  "../../TestFiles/";
	string output_path = "../../TestFiles/Output/";
	string input_filename = "newsletter.pdf";
	string output_filename = "newsletter_edited.pdf";

	try 
	{
		cout << "Opening the input file..." << endl;
		PDFDoc doc(input_path + input_filename);
		doc.InitSecurityHandler();

		ElementWriter writer;
		ElementReader reader;
		XObjSet visited;
		
		// Process each page in the document
		for (PageIterator itr = doc.GetPageIterator();itr.HasNext();itr.Next())
		{
			try {
				Page page = itr.Current();
				visited.insert(page.GetSDFObj().GetObjNum());

				reader.Begin(page);
				writer.Begin(page, ElementWriter::e_replacement, false, true, page.GetResourceDict());
				ProcessElements(reader, writer, visited);
				writer.End();
				reader.End();
			}
			catch (Common::Exception& e)
			{
				cout << e << endl;
			}
		}

		// Save modified document
		doc.Save(output_path + output_filename, SDFDoc::e_remove_unused, 0);		
		cout << "Done. Result saved in " << output_filename <<"..." << endl;
	}
	catch(Common::Exception& e)
	{
		cout << e << endl;
		ret = 1;
	}
	catch(...)
	{
		cout << "Unknown Exception" << endl;
		ret = 1;
	}
	PDFNet::Terminate();
	return ret;
}
