//
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
//

using System;
using pdftron;
using pdftron.Common;
using pdftron.Filters;
using pdftron.SDF;
using pdftron.PDF;
using pdftron.PDF.OCG;

/// <summary>
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
/// </summary>
namespace PDFLayersTestCS
{
	class Class1 
	{			
		private static pdftron.PDFNetLoader pdfNetLoader = pdftron.PDFNetLoader.Instance();
		static Class1() {}
		
		// Relative path to the folder containing test files.
		static string input_path =  "../../TestFiles/";
		static string output_path = "../../TestFiles/Output/";

		[STAThread]
		static void Main(string[] args)
		{
			PDFNet.Initialize();

			try
			{
				using (PDFDoc doc = new PDFDoc())
				using (ElementBuilder builder = new ElementBuilder()) // ElementBuilder is used to build new Element objects
				using (ElementWriter writer = new ElementWriter()) // ElementWriter is used to write Elements to the page
				{
					// Create three layers...
					Group image_layer = CreateLayer(doc, "Image Layer");
					Group text_layer = CreateLayer(doc, "Text Layer");
					Group vector_layer = CreateLayer(doc, "Vector Layer");

					// Start a new page ------------------------------------
					Page page = doc.PageCreate();

					writer.Begin(page);	// begin writing to this page

					// Add new content to the page and associate it with one of the layers.
					Element element = builder.CreateForm(CreateGroup1(doc, image_layer.GetSDFObj()));
					writer.WriteElement(element);

					element = builder.CreateForm(CreateGroup2(doc, vector_layer.GetSDFObj()));
					writer.WriteElement(element);

					// Add the text layer to the page...
					bool enableOCMD = false; // set to 'true' to enable 'ocmd' example.
					if (enableOCMD)  
					{
						// A bit more advanced example of how to create an OCMD text layer that 
						// is visible only if text, image and path layers are all 'ON'.
						// An example of how to set 'Visibility Policy' in OCMD.
						Obj ocgs = doc.CreateIndirectArray();
						ocgs.PushBack(image_layer.GetSDFObj());
						ocgs.PushBack(vector_layer.GetSDFObj());
						ocgs.PushBack(text_layer.GetSDFObj());
						OCMD text_ocmd = OCMD.Create(doc, ocgs, OCMD.VisibilityPolicyType.e_AllOn);
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
					prefs.SetPageMode(PDFDocViewPrefs.PageMode.e_UseOC);

					doc.Save(output_path + "pdf_layers.pdf", SDFDoc.SaveOptions.e_linearized);
					Console.WriteLine("Done.");
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}

			// The following is a code snippet shows how to selectively render 
			// and export PDF layers.
			try  
			{	 
				using (PDFDoc doc = new PDFDoc(output_path + "pdf_layers.pdf"))
				{
					doc.InitSecurityHandler();

					if (!doc.HasOC()) 
					{
						Console.WriteLine("The document does not contain 'Optional Content'");
					}
					else 
					{
						Config init_cfg = doc.GetOCGConfig();
						Context ctx = new Context(init_cfg);

						using (PDFDraw pdfdraw = new PDFDraw())
						{
							pdfdraw.SetImageSize(1000, 1000);
							pdfdraw.SetOCGContext(ctx); // Render the page using the given OCG context.

							Page page = doc.GetPage(1); // Get the first page in the document.
							pdfdraw.Export(page, output_path + "pdf_layers_default.png");

							// Disable drawing of content that is not optional (i.e. is not part of any layer).
							ctx.SetNonOCDrawing(false);

							// Now render each layer in the input document to a separate image.
							Obj ocgs = doc.GetOCGs(); // Get the array of all OCGs in the document.
							if (ocgs != null) 
							{
								int i, sz = ocgs.Size();
								for (i=0; i<sz; ++i) 
								{
									Group ocg = new Group(ocgs.GetAt(i));
									ctx.ResetStates(false);
									ctx.SetState(ocg, true);
									string fname = "pdf_layers_" + ocg.GetName() + ".png";
									Console.WriteLine(fname);
									pdfdraw.Export(page, output_path + fname);
								}
							}

							// Now draw content that is not part of any layer...
							ctx.SetNonOCDrawing(true);
							ctx.SetOCDrawMode(Context.OCDrawMode.e_NoOC);
							pdfdraw.Export(page, output_path + "pdf_layers_non_oc.png");

							Console.WriteLine("Done.");
						}
					}
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}


		}

		// A utility function used to add new Content Groups (Layers) to the document.
		static Group CreateLayer(PDFDoc doc, String layer_name)
		{
			Group grp = Group.Create(doc, layer_name);
			Config cfg = doc.GetOCGConfig();
			if (cfg == null) 
			{
				cfg = Config.Create(doc, true);
				cfg.SetName("Default");
			}

			// Add the new OCG to the list of layers that should appear in PDF viewer GUI.
			Obj layer_order_array = cfg.GetOrder();
			if (layer_order_array == null) 
			{
				layer_order_array = doc.CreateIndirectArray();
				cfg.SetOrder(layer_order_array);
			}
			layer_order_array.PushBack(grp.GetSDFObj());

			return grp;
		}

		// Creates some content (3 images) and associate them with the image layer
		static Obj CreateGroup1(PDFDoc doc, Obj layer)
		{
			using (ElementWriter writer = new ElementWriter())
			using (ElementBuilder builder = new ElementBuilder())
			{
				writer.Begin(doc);

				// Create an Image that can be reused in the document or on the same page.		
				Image img = Image.Create(doc.GetSDFDoc(), (input_path + "peppers.jpg"));

				Element element = builder.CreateImage(img, new Matrix2D(img.GetImageWidth()/2, -145, 20, img.GetImageHeight()/2, 200, 150));
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

				// As an example of further configuration, set the image layer to
				// be visible on screen, but not visible when printed...

				// The AS entry is an auto state array consisting of one or more usage application 
				// dictionaries that specify how conforming readers shall automatically set the 
				// state of optional content groups based on external factors.
				Obj cfg = doc.GetOCGConfig().GetSDFObj();
				Obj auto_state = cfg.FindObj("AS");
				if (auto_state == null) auto_state = cfg.PutArray("AS");
				Obj print_state = auto_state.PushBackDict();
				print_state.PutArray("Category").PushBackName("Print");
				print_state.PutName("Event", "Print");
				print_state.PutArray("OCGs").PushBack(layer);

				Obj layer_usage = layer.PutDict("Usage");

				Obj view_setting = layer_usage.PutDict("View");
				view_setting.PutName("ViewState", "ON");

				Obj print_setting = layer_usage.PutDict("Print");
				print_setting.PutName("PrintState", "OFF");

				return grp_obj;
			}
		}

		// Creates some content (a path in the shape of a heart) and associate it with the vector layer
		static Obj CreateGroup2(PDFDoc doc, Obj layer)
		{
			using (ElementWriter writer = new ElementWriter())
			using (ElementBuilder builder = new ElementBuilder())
			{
				writer.Begin(doc);

				// Create a path object in the shape of a heart.
				builder.PathBegin();		// start constructing the path
				builder.MoveTo(306, 396);
				builder.CurveTo(681, 771, 399.75, 864.75, 306, 771);
				builder.CurveTo(212.25, 864.75, -69, 771, 306, 396);
				builder.ClosePath();
				Element element = builder.PathEnd(); // the path geometry is now specified.

				// Set the path FILL color space and color.
				element.SetPathFill(true);
				GState gstate = element.GetGState();
				gstate.SetFillColorSpace(ColorSpace.CreateDeviceCMYK()); 
				gstate.SetFillColor(new ColorPt(1, 0, 0, 0));  // cyan

				// Set the path STROKE color space and color.
				element.SetPathStroke(true); 
				gstate.SetStrokeColorSpace(ColorSpace.CreateDeviceRGB()); 
				gstate.SetStrokeColor(new ColorPt(1, 0, 0));  // red
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
		}

		// Creates some text and associate it with the text layer
		static Obj CreateGroup3(PDFDoc doc, Obj layer)
		{
			using (ElementWriter writer = new ElementWriter())
			using (ElementBuilder builder = new ElementBuilder())
			{
				writer.Begin(doc);

				// Begin writing a block of text
				Element element = builder.CreateTextBegin(Font.Create(doc, Font.StandardType1Font.e_times_roman), 120);
				writer.WriteElement(element);

				element = builder.CreateTextRun("A text layer!");

				// Rotate text 45 degrees, than translate 180 pts horizontally and 100 pts vertically.
				Matrix2D transform = Matrix2D.RotationMatrix(-45 *  (3.1415/ 180.0));
				transform.Concat(1, 0, 0, 1, 180, 100);  
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
		}
	}
}
