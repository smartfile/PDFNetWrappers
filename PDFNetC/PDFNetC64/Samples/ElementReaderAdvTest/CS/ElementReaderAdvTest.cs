//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------
// A sample project illustrating some extraction capabilities of ElementReader
// in more detail
//---------------------------------------------------------------------------------------

using System;
using pdftron;
using pdftron.Common;
using pdftron.Filters;
using pdftron.SDF;
using pdftron.PDF;

namespace ElementReaderAdvTestCS
{
	/// <summary>
	/// Summary description for Class1.
	/// </summary>
	class Class1
	{
		private static pdftron.PDFNetLoader pdfNetLoader = pdftron.PDFNetLoader.Instance();
		static Class1() {}
		
		// Relative path to the folder containing test files.
		static string input_path =  "../../TestFiles/";
		static string output_path = "../../TestFiles/Output/";

		static string m_buf;

		static public void ProcessPath(ElementReader reader, Element path)
		{
			if (path.IsClippingPath())
			{
				Console.WriteLine("This is a clipping path");
			}

            PathData pathData = path.GetPathData();
			double[] data = pathData.points;
			int data_sz = data.Length;

            byte[] opr = pathData.operators;
			int opr_sz = opr.Length;

			int opr_itr = 0, opr_end = opr_sz;
			int data_itr = 0, data_end = data_sz;
			double x1, y1, x2, y2, x3, y3;

			// Use path.GetCTM() if you are interested in CTM (current transformation matrix).

			Console.Write(" Path Data Points := \"");
			for ( ; opr_itr < opr_end; ++opr_itr)
			{
				switch((PathData.PathSegmentType)((int)opr[opr_itr]))
				{
                    case PathData.PathSegmentType.e_moveto:
						x1 = data[data_itr]; ++data_itr;
						y1 = data[data_itr]; ++data_itr;
						m_buf = string.Format("M{0:n0} {1:n0}", x1, y1);
						Console.Write(m_buf);
						break;
                    case PathData.PathSegmentType.e_lineto:
						x1 = data[data_itr]; ++data_itr;
						y1 = data[data_itr]; ++data_itr;
						m_buf = string.Format(" L{0:n0} {1:n0}", x1, y1);
						Console.Write(m_buf);
						break;
                    case PathData.PathSegmentType.e_cubicto:
						x1 = data[data_itr]; ++data_itr;
						y1 = data[data_itr]; ++data_itr;
						x2 = data[data_itr]; ++data_itr;
						y2 = data[data_itr]; ++data_itr;
						x3 = data[data_itr]; ++data_itr;
						y3 = data[data_itr]; ++data_itr;
						m_buf = string.Format(" C{0:n0} {1:n0} {2:n0} {3:n0} {4:n0} {5:n0}",
							new object[] {x1, y1, x2, y2, x3, y3});
						Console.Write(m_buf);
						break;
                    case PathData.PathSegmentType.e_rect:
					{
						x1 = data[data_itr]; ++data_itr;
						y1 = data[data_itr]; ++data_itr;
						double w = data[data_itr]; ++data_itr;
						double h = data[data_itr]; ++data_itr;
						x2 = x1 + w;
						y2 = y1;
						x3 = x2;
						y3 = y1 + h;
						double x4 = x1; 
						double y4 = y3;
						m_buf = string.Format("M{0:n0} {1:n0} L{2:n0} {3:n0} L{4:n0} {5:n0} L{6:n0} {7:n0} Z",
							new object[] {x1, y1, x2, y2, x3, y3, x4, y4});
						Console.Write(m_buf);
						break;
					}
                    case PathData.PathSegmentType.e_closepath:
						Console.WriteLine(" Close Path");
						break;
					default: 
						System.Diagnostics.Debug.Assert(false);
						break;
				}	
			}

			Console.Write("\" ");

			GState gs = path.GetGState();

			// Set Path State 0 (stroke, fill, fill-rule) -----------------------------------
			if (path.IsStroked()) 
			{
				Console.WriteLine("Stroke path"); 

				if (gs.GetStrokeColorSpace().GetType() == ColorSpace.Type.e_pattern)
				{
					Console.WriteLine("Path has associated pattern"); 
				}
				else
				{
					// Get stroke color (you can use PDFNet color conversion facilities)
					// ColorPt rgb = new ColorPt();
					// gs.GetStrokeColorSpace().Convert2RGB(gs.GetStrokeColor(), rgb);
				}
			}
			else 
			{
				// Do not stroke path
			}

			if (path.IsFilled())
			{
				Console.WriteLine("Fill path"); 

				if (gs.GetFillColorSpace().GetType() == ColorSpace.Type.e_pattern)
				{		
					Console.WriteLine("Path has associated pattern"); 
				}
				else
				{
					// ColorPt rgb = new ColorPt();
					// gs.GetFillColorSpace().Convert2RGB(gs.GetFillColor(), rgb);
				}        
			}
			else 
			{
				// Do not fill path
			}

			// Process any changes in graphics state  ---------------------------------

			GSChangesIterator gs_itr = reader.GetChangesIterator();
			for ( ; gs_itr.HasNext(); gs_itr.Next()) 
			{
				switch(gs_itr.Current())
				{
					case GState.GStateAttribute.e_transform :
						// Get transform matrix for this element. Unlike path.GetCTM() 
						// that return full transformation matrix gs.GetTransform() return 
						// only the transformation matrix that was installed for this element.
						//
						// gs.GetTransform();
						break;
					case GState.GStateAttribute.e_line_width :
						// gs.GetLineWidth();
						break;
					case GState.GStateAttribute.e_line_cap :
						// gs.GetLineCap();
						break;
					case GState.GStateAttribute.e_line_join :
						// gs.GetLineJoin();
						break;
					case GState.GStateAttribute.e_flatness :	
						break;
					case GState.GStateAttribute.e_miter_limit :
						// gs.GetMiterLimit();
						break;
					case GState.GStateAttribute.e_dash_pattern :
					{
						// double[] dashes;
						// gs.GetDashes(dashes);
						// gs.GetPhase()
						break;
					}
					case GState.GStateAttribute.e_fill_color:
					{
						if ( gs.GetFillColorSpace().GetType() == ColorSpace.Type.e_pattern &&
							 gs.GetFillPattern().GetType() != PatternColor.Type.e_shading)
						{	
							//process the pattern data
							reader.PatternBegin(true);
							ProcessElements(reader);
							reader.End();
						}
						break;
					}
				}
			}
			reader.ClearChangeList();
		}

		static public void ProcessText(ElementReader page_reader) 
		{
			// Begin text element
			Console.WriteLine("Begin Text Block:");

			Element element; 
			while ((element = page_reader.Next()) != null) 
			{
				switch (element.GetType())
				{
					case Element.Type.e_text_end: 
						// Finish the text block
						Console.WriteLine("End Text Block.");
						return;

					case Element.Type.e_text:
					{
						GState gs = element.GetGState();

						ColorSpace cs_fill = gs.GetFillColorSpace();
						ColorPt fill = gs.GetFillColor();

						ColorPt outc = new ColorPt();
						cs_fill.Convert2RGB(fill, outc);


						ColorSpace cs_stroke = gs.GetStrokeColorSpace();
						ColorPt stroke = gs.GetStrokeColor();

						Font font = gs.GetFont();

						Console.Write("Font Name: ");
						Console.WriteLine(font.GetName());
						// font.IsFixedWidth();
						// font.IsSerif();
						// font.IsSymbolic();
						// font.IsItalic();
						// ... 

						// double word_spacing = gs.GetWordSpacing();
						// double char_spacing = gs.GetCharSpacing();

						// Use element.GetCTM() if you are interested in the CTM 
						// (current transformation matrix).
						if (font.GetType() == Font.Type.e_Type3)
						{
							//type 3 font, process its data
							for (CharIterator itr = element.GetCharIterator(); itr.HasNext(); itr.Next()) 
							{
								page_reader.Type3FontBegin(itr.Current());
								ProcessElements(page_reader);
								page_reader.End();
							}
						}

						else
						{

							Matrix2D ctm = element.GetCTM();

							Matrix2D text_mtx = element.GetTextMatrix();

                            /*
                            Matrix2D mtx = ctm * text_mtx;
                            double font_sz_scale_factor = System.Math.Sqrt(mtx.m_b * mtx.m_b + mtx.m_d * mtx.m_d);
                            double font_size = gs.GetFontSize();
                            Console.Write(" Font Size: {0:f}", font_sz_scale_factor * font_size);

                            ColorPt font_color = gs.GetFillColor();
                            ColorSpace cs = gs.GetFillColorSpace();

                            ColorPt rgb = new ColorPt();
                            cs.Convert2RGB(font_color, rgb);
                            Color font_color_rgb = Color.FromArgb(255, (byte)(rgb.get_c(0)*255),
                            (byte)(rgb.get_c(1)*255), (byte)(rgb.get_c(2)*255));
                                

                            Console.WriteLine(" Font Color(RGB): red={0:d} green={1:d} blue={2:d}", 
							(byte)(rgb.Get(0)*255),
							(byte)(rgb.Get(1)*255),
							(byte)(rgb.Get(2)*255));
                            */

                            double x, y;
							int char_code; 
											
							for (CharIterator itr = element.GetCharIterator(); itr.HasNext(); itr.Next()) 
							{
								Console.Write("Character code: ");
								char_code = itr.Current().char_code;
                                if (char_code >= 32 || char_code <= 127)
                                { 
                                    // Print if in ASCII range...
                                    Console.Write((char)char_code);
                                }

								x = itr.Current().x;		// character positioning information
								y = itr.Current().y;

								// To get the exact character positioning information you need to 
								// concatenate current text matrix with CTM and then multiply 
								// relative positioning coordinates with the resulting matrix.
								//
								Matrix2D mtx2 = ctm * text_mtx;
								mtx2.Mult(ref x, ref y);
								// Console.WriteLine(" Position: x={0:f} y={1:f}", x, y);
							}
						}

						Console.WriteLine();
						break;
					}
				}
			}
		}

		static int image_counter = 0;

		static public void ProcessImage(Element image)  
		{
			bool image_mask = image.IsImageMask();
			bool interpolate = image.IsImageInterpolate();
			int width = image.GetImageWidth();
			int height = image.GetImageHeight();
			int out_data_sz = width * height * 3;

			Console.WriteLine("Image: width=\"{0:d}\" height=\"{1:d}\"", width, height);

			// Matrix2D mtx = image.GetCTM(); // image matrix (page positioning info)

// 			++image_counter;
// 			System.Drawing.Bitmap bmp = image.GetBitmap();
// 			bmp.Save(output_path + "reader_img_extract_" + image_counter.ToString() + ".png", System.Drawing.Imaging.ImageFormat.Png);
// 
			// Alternatively you can use GetImageData to read the raw (decoded) image data
			// image.GetBitsPerComponent();	
			// image.GetImageData();	// get raw image data
			// another approach is to use Image2RGB filter that converts every image to 
			// RGB format. This could save you time since you don't need to deal with color 
			// conversions, image up-sampling, decoding etc.
			// ----------------
			   Image2RGB img_conv = new Image2RGB(image);	// Extract and convert image to RGB 8-bpc format
			   FilterReader reader = new FilterReader(img_conv);			//   
			   byte[] image_data_out = new byte[out_data_sz];  // A buffer used to keep image data.
			   reader.Read(image_data_out);  // image_data_out contains RGB image data.
			// ----------------
			// Note that you don't need to read a whole image at a time. Alternatively
			// you can read a chuck at a time by repeatedly calling reader.Read(buf, buf_sz) 
			// until the function returns 0. 
		}

	static void ProcessElements(ElementReader reader) 
	{
		Element element;

		while ((element = reader.Next()) != null)  // Read page contents
		{
			switch (element.GetType())
			{
				case Element.Type.e_path:          // Process path data...
				{
					ProcessPath(reader, element);
					break; 
				}
				case Element.Type.e_text_begin:    // Process text strings...
				{
					ProcessText(reader);
					break;
				}
				case Element.Type.e_form:          // Process form XObjects
				{
					reader.FormBegin(); 
					ProcessElements(reader);
					reader.End(); 
					break; 
				}
				case Element.Type.e_image:         // Process Images
				{
					ProcessImage(element);
					break; 
				}	
			}
		}
	}

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main(string[] args)
		{
			try
			{
				PDFNet.Initialize();

				Console.WriteLine("-------------------------------------------------");
				Console.WriteLine("Extract page element information from all ");
				Console.WriteLine("pages in the document.");

				// Open the test file
				using (PDFDoc doc = new PDFDoc(input_path + "newsletter.pdf"))
				{
					doc.InitSecurityHandler();

					int pgnum = doc.GetPageCount();
					PageIterator itr;

					using (ElementReader page_reader = new ElementReader())
					{
						for (itr = doc.GetPageIterator(); itr.HasNext(); itr.Next())		//  Read every page
						{				
							Console.WriteLine("Page {0:d}----------------------------------------",
								itr.GetPageNumber());

							Rect crop_box = itr.Current().GetCropBox();
							crop_box.Normalize();

							// Console.WriteLine(" Page Rectangle: x={0:f} y={1:f} x2={2:f} y2={3:f}", crop_box.x1, crop_box.y1, crop_box.x2, crop_box.y2);
							// Console.WriteLine(" Page Size: width={0:f} height={1:f}", crop_box.Width(), crop_box.Height());

							page_reader.Begin(itr.Current());
							ProcessElements(page_reader);
							page_reader.End(); 
						}
					}

					Console.WriteLine("Done.");
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}
		}
	}
}
