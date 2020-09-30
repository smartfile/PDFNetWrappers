//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

#include <PDF/PDFNet.h>
#include <PDF/PDFDoc.h>
#include <PDF/ElementReader.h>
#include <PDF/Element.h>
#include <PDF/Font.h>
#include <Filters/FilterReader.h>
#include <PDF/Image/Image2RGB.h>

#include <iostream>
#include <assert.h>

using namespace std;

using namespace pdftron;
using namespace PDF;
using namespace SDF;
using namespace Common;
using namespace Filters; 

char m_buf[4000];

void ProcessElements(ElementReader& reader);

void ProcessPath(ElementReader& reader, Element path)
{
	if (path.IsClippingPath())
	{
		cout << "This is a clipping path" << endl;
	}

	PathData d = path.GetPathData();

	const UChar* opr = &d.GetOperators().front();
	const UChar *opr_itr = opr, *opr_end = opr + d.GetOperators().size();
	const double* data = &d.GetPoints().front();
	const double *data_itr = data, *data_end = data + d.GetPoints().size();

	double x1, y1, x2, y2, x3, y3;

	// Use path.GetCTM() if you are interested in CTM (current transformation matrix).

	cout << " Path Data Points := \"";
	for (; opr_itr<opr_end; ++opr_itr)
	{
		switch(*opr_itr)
		{
		case PathData::e_moveto:
			x1 = *data_itr; ++data_itr;
			y1 = *data_itr; ++data_itr;
			sprintf(m_buf, "M%.0f %.0f", x1, y1);
			cout << m_buf;
			break;
		case PathData::e_lineto:
			x1 = *data_itr; ++data_itr;
			y1 = *data_itr; ++data_itr;
			sprintf(m_buf, " L%.0f %.0f", x1, y1);
			cout << m_buf;
			break;
		case PathData::e_cubicto:
			x1 = *data_itr; ++data_itr;
			y1 = *data_itr; ++data_itr;
			x2 = *data_itr; ++data_itr;
			y2 = *data_itr; ++data_itr;
			x3 = *data_itr; ++data_itr;
			y3 = *data_itr; ++data_itr;
			sprintf(m_buf, " C%.0f %.0f %.0f %.0f %.0f %.0f", x1, y1, x2, y2, x3, y3);
			cout << m_buf;
			break;
		case PathData::e_rect:
			{
				x1 = *data_itr; ++data_itr;
				y1 = *data_itr; ++data_itr;
				double w = *data_itr; ++data_itr;
				double h = *data_itr; ++data_itr;
				x2 = x1 + w;
				y2 = y1;
				x3 = x2;
				y3 = y1 + h;
				double x4 = x1; 
				double y4 = y3;
				sprintf(m_buf, "M%.0f %.0f L%.0f %.0f L%.0f %.0f L%.0f %.0f Z", 
					x1, y1, x2, y2, x3, y3, x4, y4);
				cout << m_buf;
			}
			break;
		case PathData::e_closepath:
			cout << " Close Path" << endl;
			break;
		default: 
			assert(false);
			break;
		}	
	}

	cout << "\" ";

	GState gs = path.GetGState();

	// Set Path State 0 (stroke, fill, fill-rule) -----------------------------------
	if (path.IsStroked()) 
	{
		cout << "Stroke path" << endl; 

		if (gs.GetStrokeColorSpace().GetType() == ColorSpace::e_pattern)
		{
			cout << "Path has associated pattern" << endl; 
		}
		else
		{
			// Get stroke color (you can use PDFNet color conversion facilities)
			// ColorPt rgb;
			// gs.GetStrokeColorSpace().Convert2RGB(gs.GetStrokeColor(), rgb);
		}
	}
	else 
	{
		// Do not stroke path
	}

	if (path.IsFilled())
	{
		cout << "Fill path" << endl; 

		if (gs.GetFillColorSpace().GetType() == ColorSpace::e_pattern)
		{		
			cout << "Path has associated pattern" << endl; 
		}
		else
		{
			// ColorPt rgb;
			// gs.GetFillColorSpace().Convert2RGB(gs.GetFillColor(), rgb);
		}        
	}
	else 
	{
		// Do not fill path
	}

	// Process any changes in graphics state  ---------------------------------

	GSChangesIterator gs_itr = reader.GetChangesIterator();
	for (; gs_itr.HasNext(); gs_itr.Next()) 
	{
		switch(gs_itr.Current())
		{
		case GState::e_transform :
			// Get transform matrix for this element. Unlike path.GetCTM() 
			// that return full transformation matrix gs.GetTransform() return 
			// only the transformation matrix that was installed for this element.
			//
			// gs.GetTransform();
			break;
		case GState::e_line_width :
			// gs.GetLineWidth();
			break;
		case GState::e_line_cap :
			// gs.GetLineCap();
			break;
		case GState::e_line_join :
			// gs.GetLineJoin();
			break;
		case GState::e_flatness :	
			break;
		case GState::e_miter_limit :
			// gs.GetMiterLimit();
			break;
		case GState::e_dash_pattern :
			{
				// std::vector<double> dashes;
				// gs.GetDashes(dashes);
				// gs.GetPhase()
			}
			break;
		case GState::e_fill_color:
			{
				if ( gs.GetFillColorSpace().GetType() == ColorSpace::e_pattern &&
					gs.GetFillPattern().GetType() != PatternColor::e_shading )
				{	
					//process the pattern data
					reader.PatternBegin(true);
					ProcessElements(reader);
					reader.End();
				}
			}
			break;
		}
	}
	reader.ClearChangeList();
}

void ProcessText(ElementReader& page_reader) 
{
	// Begin text element
	cout << "Begin Text Block:" << endl;

	Element element; 
	while ((element = page_reader.Next()) != 0) 
	{
		switch (element.GetType())
		{
		case Element::e_text_end: 
			// Finish the text block
			cout << "End Text Block." << endl;
			return;

		case Element::e_text:
			{
				GState gs =  element.GetGState();

				ColorSpace cs_fill = gs.GetFillColorSpace();
				ColorPt fill = gs.GetFillColor();

				ColorPt out;
				cs_fill.Convert2RGB(fill, out);


				ColorSpace cs_stroke = gs.GetStrokeColorSpace();
				ColorPt stroke = gs.GetStrokeColor();

				Font font = gs.GetFont();

				cout << "Font Name: " << font.GetName() << endl;
				// font.IsFixedWidth();
				// font.IsSerif();
				// font.IsSymbolic();
				// font.IsItalic();
				// ... 

				// double font_size = gs.GetFontSize();
				// double word_spacing = gs.GetWordSpacing();
				// double char_spacing = gs.GetCharSpacing();
				// const UString* txt = element.GetTextString();

				if ( font.GetType() == Font::e_Type3 )
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
					Matrix2D text_mtx = element.GetTextMatrix();
					double x, y;
					unsigned int char_code;

					for (CharIterator itr = element.GetCharIterator(); itr.HasNext(); itr.Next()) 
					{
						cout << "Character code: ";
						char_code = itr.Current().char_code;
						if (char_code>=32 || char_code<=127)
						{ 
							// Print if in ASCII range...
							cout << char(char_code);
						}

						x = itr.Current().x;		// character positioning information
						y = itr.Current().y;

						// Use element.GetCTM() if you are interested in the CTM 
						// (current transformation matrix).
						Matrix2D ctm = element.GetCTM();

						// To get the exact character positioning information you need to 
						// concatenate current text matrix with CTM and then multiply 
						// relative positioning coordinates with the resulting matrix.
						Matrix2D mtx = ctm * text_mtx;
						mtx.Mult(x, y);

						// Get glyph path...
						//vector<UChar> oprs;
						//vector<double> glyph_data;
						//font.GetGlyphPath(char_code, oprs, glyph_data, false, 0);
					}
				}

				cout << endl;
			}
			break;
		}
	}
}

void ProcessImage(Element image)  
{
	bool image_mask = image.IsImageMask();
	bool interpolate = image.IsImageInterpolate();
	int width = image.GetImageWidth();
	int height = image.GetImageHeight();
	int out_data_sz = width * height * 3;

	cout << "Image:" 
		<< " width=\"" << width << "\""
		<< " height=\"" << height << "\"" << endl;

	// Matrix2D& mtx = image->GetCTM(); // image matrix (page positioning info)

	// You can use GetImageData to read the raw (decoded) image data
	//image->GetBitsPerComponent();	
	//image->GetImageData();	// get raw image data
	// .... or use Image2RGB filter that converts every image to RGB format,
	// This should save you time since you don't need to deal with color conversions, 
	// image up-sampling, decoding etc.

	Image2RGB img_conv(image);	// Extract and convert image to RGB 8-bpc format
	FilterReader reader(img_conv);

	// A buffer used to keep image data.
	std::vector<UChar> image_data_out; 
	image_data_out.resize(out_data_sz);

	reader.Read(&image_data_out.front(), out_data_sz);
	// &image_data_out.front() contains RGB image data.

	// Note that you don't need to read a whole image at a time. Alternatively
	// you can read a chuck at a time by repeatedly calling reader.Read(buf, buf_sz) 
	// until the function returns 0. 
}

void ProcessElements(ElementReader& reader) 
{
	Element element;
	while ((element = reader.Next()) != 0) 	// Read page contents
	{
		switch (element.GetType())
		{
		case Element::e_path:						// Process path data...
			{
				ProcessPath(reader, element);
			}
			break; 
		case Element::e_text_begin: 				// Process text block...
			{
				ProcessText(reader);
			}
			break;
		case Element::e_form:						// Process form XObjects
			{
				reader.FormBegin(); 
				ProcessElements(reader);
				reader.End();
			}
			break; 
		case Element::e_image:						// Process Images
			{
				ProcessImage(element);
			}	
			break; 
		}
	}
}

int main(int argc, char *argv[])
{
	int ret = 0;
	PDFNet::Initialize();

	// Relative path to the folder containing test files.
	string input_path =  "../../TestFiles/";
	// string output_path = "../../TestFiles/Output/";


	try	// Extract text data from all pages in the document
	{
		cout << "-------------------------------------------------" << endl;
		cout << "Extract page element information from all " << endl;
		cout << "pages in the document." << endl;

		PDFDoc doc((input_path + "newsletter.pdf").c_str());
		doc.InitSecurityHandler();

		int pgnum = doc.GetPageCount();
		PageIterator page_begin = doc.GetPageIterator();

		ElementReader page_reader;

		PageIterator itr;
		for (itr = page_begin; itr.HasNext(); itr.Next())		//  Read every page
		{				
			cout << "Page " << itr.Current().GetIndex() << "----------------------------------------" << endl;
			page_reader.Begin(itr.Current());
			ProcessElements(page_reader);
			page_reader.End();
		}

		cout << "Done." << endl;
	}
	catch(Exception& e)
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
