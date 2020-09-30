//
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
//

using System;
using System.Collections.Generic;

using pdftron;
using pdftron.Common;
using pdftron.Filters;
using pdftron.SDF;
using pdftron.PDF;

using XSet = System.Collections.Generic.List<int>;

namespace ElementEditTestCS
{
	/// <summary>
	/// The sample code shows how to edit the page display list and how to modify graphics state 
	/// attributes on existing Elements. In particular the sample program strips all images from 
	/// the page, changes path fill color to red, and changes text fill color to blue. 
	/// </summary>
	class Class1
	{
		private static pdftron.PDFNetLoader pdfNetLoader = pdftron.PDFNetLoader.Instance();
		static Class1() {}
		
		static void ProcessElements(ElementReader reader, ElementWriter writer, XSet visited)
		{
			Element element;
			while ((element = reader.Next()) != null) // Read page contents
			{
				switch (element.GetType())
				{
					case Element.Type.e_image:
					case Element.Type.e_inline_image:
							// remove all images by skipping them
							break;
					case Element.Type.e_path:
						{
							// Set all paths to red color.
							GState gs = element.GetGState();
							gs.SetFillColorSpace(ColorSpace.CreateDeviceRGB());
							gs.SetFillColor(new ColorPt(1, 0, 0));
                            writer.WriteElement(element);
							break;
						}
					case Element.Type.e_text:
						{
							// Set all text to blue color.
							GState gs = element.GetGState();
							gs.SetFillColorSpace(ColorSpace.CreateDeviceRGB());
							gs.SetFillColor(new ColorPt(0, 0, 1));
                            writer.WriteElement(element);
							break;
						}
					case Element.Type.e_form:
						{
							writer.WriteElement(element); // write Form XObject reference to current stream

                            Obj form_obj = element.GetXObject();
							if (!visited.Contains(form_obj.GetObjNum())) // if this XObject has not been processed
							{
								// recursively process the Form XObject
								visited.Add(form_obj.GetObjNum());
								ElementWriter new_writer = new ElementWriter();

								reader.FormBegin();
								new_writer.Begin(form_obj, true);

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
                        break;
				}
			}
		}

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main(string[] args)
		{
			PDFNet.Initialize();

			// Relative path to the folder containing test files.
			string input_path = "../../TestFiles/";
			string output_path = "../../TestFiles/Output/";
			string input_filename = "newsletter.pdf";
			string output_filename = "newsletter_edited.pdf";

			try
			{
				Console.WriteLine("Opening the input file...");
				using (PDFDoc doc = new PDFDoc(input_path + input_filename))
				{
					doc.InitSecurityHandler();

					ElementWriter writer = new ElementWriter();
					ElementReader reader = new ElementReader();
					XSet visited = new XSet();

					PageIterator itr = doc.GetPageIterator();

					while (itr.HasNext())
					{
						try
						{
							Page page = itr.Current();
							visited.Add(page.GetSDFObj().GetObjNum());

							reader.Begin(page);
							writer.Begin(page, ElementWriter.WriteMode.e_replacement, false, true, page.GetResourceDict());

                            ProcessElements(reader, writer, visited);
							writer.End();
							reader.End();

							itr.Next();
						}
						catch (PDFNetException e)
						{
							Console.WriteLine(e.Message);
						}
					}

					doc.Save(output_path + output_filename, SDFDoc.SaveOptions.e_remove_unused);
					Console.WriteLine("Done. Result saved in {0}...", output_filename);
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}

		}
	}
}
