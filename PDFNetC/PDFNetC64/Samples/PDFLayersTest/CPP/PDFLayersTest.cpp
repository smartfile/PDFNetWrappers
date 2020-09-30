//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

#include <SDF/Obj.h>
#include <PDF/PDFNet.h>
#include <PDF/PDFDoc.h>
#include <PDF/PDFDraw.h>
#include <PDF/OCG/OCMD.h>
#include <PDF/ElementBuilder.h>
#include <PDF/ElementWriter.h>
#include <PDF/ElementReader.h>

#include <iostream>

using namespace pdftron;
using namespace Common;
using namespace SDF;
using namespace PDF;
using namespace std;

//-----------------------------------------------------------------------------------
// This sample demonstrates how to create layers in PDF.
// The sample also shows how to extract and render PDF layers in documents 
// that contain optional content groups (OCGs)
//
// With the introduction of PDF version 1.5 came the concept of Layers. 
// Layers, or as they are more formally known Optional Content Groups (OCGs),
// refer to sections of content in a PDF document that can be selectively 
// viewed or hidden by document authors or consumers. This capability is useful 
// in CAD drawings, layered artwork, maps, multi-language documents etc.
// 
// Notes: 
// ---------------------------------------
// - This sample is using CreateLayer() utility method to create new OCGs. 
//   CreateLayer() is relatively basic, however it can be extended to set 
//   other optional entries in the 'OCG' and 'OCProperties' dictionary. For 
//   a complete listing of possible entries in OC dictionary please refer to 
//   section 4.10 'Optional Content' in the PDF Reference Manual.
// - The sample is grouping all layer content into separate Form XObjects. 
//   Although using PDFNet is is also possible to specify Optional Content in 
//   Content Streams (Section 4.10.2 in PDF Reference), Optional Content in  
//   XObjects results in PDFs that are cleaner, less-error prone, and faster 
//   to process.
//-----------------------------------------------------------------------------------

Obj CreateGroup1(PDFDoc& doc, Obj layer);
Obj CreateGroup2(PDFDoc& doc, Obj layer);
Obj CreateGroup3(PDFDoc& doc, Obj layer);
OCG::Group CreateLayer(PDFDoc& doc, const char* layer_name);

// Relative path to the folder containing test files.
static const string input_path =  "../../TestFiles/";
static const string output_path = "../../TestFiles/Output/";

int main(int argc, char *argv[])
{
	int ret = 0;
	PDFNet::Initialize();

	try  
	{	 
		PDFDoc doc;

		// Create three layers...
		OCG::Group image_layer = CreateLayer(doc, "Image Layer");
		OCG::Group text_layer = CreateLayer(doc, "Text Layer");
		OCG::Group vector_layer = CreateLayer(doc, "Vector Layer");

		// Start a new page ------------------------------------
		Page page = doc.PageCreate();

		ElementBuilder builder;	// ElementBuilder is used to build new Element objects
		ElementWriter writer;	// ElementWriter is used to write Elements to the page	
		writer.Begin(page);		// Begin writing to the page

		// Add new content to the page and associate it with one of the layers.
		Element element = builder.CreateForm(CreateGroup1(doc, image_layer.GetSDFObj()));
		writer.WriteElement(element);

		element = builder.CreateForm(CreateGroup2(doc, vector_layer.GetSDFObj()));
		writer.WriteElement(element);

		// Add the text layer to the page...
		if (false)  // set to true to enable 'ocmd' example.
		{
			// A bit more advanced example of how to create an OCMD text layer that 
			// is visible only if text, image and path layers are all 'ON'.
			// An example of how to set 'Visibility Policy' in OCMD.
			Obj ocgs = doc.CreateIndirectArray();
			ocgs.PushBack(image_layer.GetSDFObj());
			ocgs.PushBack(vector_layer.GetSDFObj());
			ocgs.PushBack(text_layer.GetSDFObj());
			OCG::OCMD text_ocmd = OCG::OCMD::Create(doc, ocgs, OCG::OCMD::e_AllOn);
			element = builder.CreateForm(CreateGroup3(doc, text_ocmd.GetSDFObj()));
		}
		else {
			element = builder.CreateForm(CreateGroup3(doc, text_layer.GetSDFObj()));
		}
		writer.WriteElement(element);

		// Add some content to the page that does not belong to any layer...
		// In this case this is a rectangle representing the page border.
		element = builder.CreateRect(0, 0, page.GetPageWidth(), page.GetPageHeight());
		element.SetPathFill(false);
		element.SetPathStroke(true);
		element.GetGState().SetLineWidth(40);
		writer.WriteElement(element);

		writer.End();  // save changes to the current page
		doc.PagePushBack(page);

		// Set the default viewing preference to display 'Layer' tab.
		PDFDocViewPrefs prefs = doc.GetViewPrefs();
		prefs.SetPageMode(PDFDocViewPrefs::e_UseOC);

		doc.Save((output_path + "pdf_layers.pdf").c_str(), SDFDoc::e_linearized, 0);
		cout << "Done." << endl;
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

	// The following is a code snippet shows how to selectively render 
	// and export PDF layers.
	try  
	{	 
		PDFDoc doc((output_path + "pdf_layers.pdf").c_str());
		doc.InitSecurityHandler();

		if (!doc.HasOC()) {
			cout << "The document does not contain 'Optional Content'" << endl;
		}
		else {
			OCG::Config init_cfg = doc.GetOCGConfig();
			OCG::Context ctx(init_cfg);

			PDFDraw pdfdraw;
			pdfdraw.SetImageSize(1000, 1000);
			pdfdraw.SetOCGContext(&ctx); // Render the page using the given OCG context.

			Page page = doc.GetPage(1); // Get the first page in the document.
			pdfdraw.Export(page, (output_path + "pdf_layers_default.png").c_str());

			// Disable drawing of content that is not optional (i.e. is not part of any layer).
			ctx.SetNonOCDrawing(false);

			// Now render each layer in the input document to a separate image.
			Obj ocgs = doc.GetOCGs(); // Get the array of all OCGs in the document.
			if (ocgs != 0) {
				int i, sz = int(ocgs.Size());
				for (i=0; i<sz; ++i) {
					OCG::Group ocg(ocgs.GetAt(i));
					ctx.ResetStates(false);
					ctx.SetState(ocg, true);
					std::string fname("pdf_layers_");
					fname += ocg.GetName().ConvertToAscii();
					fname += ".png";
					cout << fname << endl;
					pdfdraw.Export(page, (output_path + fname).c_str());
				}
			}

			// Now draw content that is not part of any layer...
			ctx.SetNonOCDrawing(true);
			ctx.SetOCDrawMode(OCG::Context::e_NoOC);
			pdfdraw.Export(page, (output_path + "pdf_layers_non_oc.png").c_str());
		}

		cout << "Done." << endl;
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


// A utility function used to add new Content Groups (Layers) to the document.
OCG::Group CreateLayer(PDFDoc& doc, const char* layer_name)
{
	OCG::Group grp = OCG::Group::Create(doc, layer_name);
	OCG::Config cfg = doc.GetOCGConfig();
	if (!cfg.IsValid()) {
		cfg = OCG::Config::Create(doc, true);
		cfg.SetName("Default");
	}

	// Add the new OCG to the list of layers that should appear in PDF viewer GUI.
	Obj layer_order_array = cfg.GetOrder();
	if (!layer_order_array) {
        layer_order_array = doc.CreateIndirectArray();
		cfg.SetOrder(layer_order_array);
	}
	layer_order_array.PushBack(grp.GetSDFObj());

	return grp;
}

// Creates some content (3 images) and associate them with the image layer
Obj CreateGroup1(PDFDoc& doc, Obj layer) 
{
	ElementWriter writer;
	writer.Begin(doc);

	// Create an Image that can be reused in the document or on the same page.		
	Image img = Image::Create(doc, (input_path + "peppers.jpg").c_str());

	ElementBuilder builder;
	Element element = builder.CreateImage(img, Common::Matrix2D(img.GetImageWidth()/2, -145, 20, img.GetImageHeight()/2, 200, 150));
	writer.WritePlacedElement(element);

	GState gstate = element.GetGState();	// use the same image (just change its matrix)
	gstate.SetTransform(200, 0, 0, 300, 50, 450);
	writer.WritePlacedElement(element);

	// use the same image again (just change its matrix).
	writer.WritePlacedElement(builder.CreateImage(img, 300, 600, 200, -150));

	Obj grp_obj = writer.End();	

	// Indicate that this form (content group) belongs to the given layer (OCG).
	grp_obj.PutName("Subtype","Form");
	grp_obj.Put("OC", layer);	
	grp_obj.PutRect("BBox", 0, 0, 1000, 1000);  // Set the clip box for the content.

	return grp_obj;
}

// Creates some content (a path in the shape of a heart) and associate it with the vector layer
Obj CreateGroup2(PDFDoc& doc, Obj layer) 
{
	ElementWriter writer;
	writer.Begin(doc);

	// Create a path object in the shape of a heart.
	ElementBuilder builder;
	builder.PathBegin();		// start constructing the path
	builder.MoveTo(306, 396);
	builder.CurveTo(681, 771, 399.75, 864.75, 306, 771);
	builder.CurveTo(212.25, 864.75, -69, 771, 306, 396);
	builder.ClosePath();
	Element element = builder.PathEnd(); // the path geometry is now specified.

	// Set the path FILL color space and color.
	element.SetPathFill(true);
	GState gstate = element.GetGState();
	gstate.SetFillColorSpace(ColorSpace::CreateDeviceCMYK()); 
	gstate.SetFillColor(ColorPt(1, 0, 0, 0));  // cyan

	// Set the path STROKE color space and color.
	element.SetPathStroke(true); 
	gstate.SetStrokeColorSpace(ColorSpace::CreateDeviceRGB()); 
	gstate.SetStrokeColor(ColorPt(1, 0, 0));  // red
	gstate.SetLineWidth(20);

	gstate.SetTransform(0.5, 0, 0, 0.5, 280, 300);

	writer.WriteElement(element);

	Obj grp_obj = writer.End();	

	// Indicate that this form (content group) belongs to the given layer (OCG).
	grp_obj.PutName("Subtype","Form");
	grp_obj.Put("OC", layer);
	grp_obj.PutRect("BBox", 0, 0, 1000, 1000); 	// Set the clip box for the content.

	return grp_obj;
}

// Creates some text and associate it with the text layer
Obj CreateGroup3(PDFDoc& doc, Obj layer) 
{
	ElementWriter writer;
	writer.Begin(doc);

	// Create a path object in the shape of a heart.
	ElementBuilder builder;

	// Begin writing a block of text
	Element element = builder.CreateTextBegin(Font::Create(doc, Font::e_times_roman), 120);
	writer.WriteElement(element);

	element = builder.CreateTextRun("A text layer!");

	// Rotate text 45 degrees, than translate 180 pts horizontally and 100 pts vertically.
	Matrix2D transform = Matrix2D::RotationMatrix(-45 *  (3.1415/ 180.0));
	transform *= Matrix2D(1, 0, 0, 1, 180, 100);  
	element.SetTextMatrix(transform);

	writer.WriteElement(element);
	writer.WriteElement(builder.CreateTextEnd());

	Obj grp_obj = writer.End();	

	// Indicate that this form (content group) belongs to the given layer (OCG).
	grp_obj.PutName("Subtype","Form");
	grp_obj.Put("OC", layer);
	grp_obj.PutRect("BBox", 0, 0, 1000, 1000); 	// Set the clip box for the content.

	return grp_obj;
}

