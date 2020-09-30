//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

#include <PDF/PDFNet.h>
#include <PDF/PDFDoc.h>
#include <PDF/ElementBuilder.h>
#include <PDF/ElementReader.h>
#include <PDF/ElementWriter.h>
#include <PDF/Annot.h>
#include <SDF/Obj.h>
#include <iostream>

using namespace pdftron;
using namespace SDF;
using namespace PDF;
using namespace std;


std::string output_path = "../../TestFiles/Output/";
std::string input_path = "../../TestFiles/";

void AnnotationHighLevelAPI(PDFDoc& doc)
{
	// The following code snippet traverses all annotations in the document
	cout << "Traversing all annotations in the document..." << endl; 

	UString uri;
	int page_num=1;
	for (PageIterator itr = doc.GetPageIterator(); itr.HasNext(); itr.Next()) 
	{
		cout << "Page " << page_num++ << ": " << endl; 

		Page page = itr.Current();
		int num_annots = page.GetNumAnnots(); 
		for (int i=0; i<num_annots; ++i) 
		{
			Annot annot = page.GetAnnot(i);
			if (!annot.IsValid()) continue;
			cout << "Annot Type: " << annot.GetSDFObj().Get("Subtype").Value().GetName() << endl; 

			Rect bbox = annot.GetRect();
			cout << "  Position: " << bbox.x1 
				<< ", " << bbox.y1
				<< ", " << bbox.x2
				<< ", " << bbox.y2 << endl;

			switch (annot.GetType()) 
			{
			case Annot::e_Link: 
				{
					Annots::Link link(annot);
					Action action = link.GetAction();
					if (!action.IsValid()) continue;
					if (action.GetType() == Action::e_GoTo) 
					{
						Destination dest = action.GetDest();
						if (!dest.IsValid()) {
							cout << "  Destination is not valid" << endl;
						}
						else {
							int page_num = dest.GetPage().GetIndex();
							cout << "  Links to: page number " << page_num << " in this document" << endl;
						}
					}
					else if (action.GetType() == Action::e_URI) 
					{						
						action.GetSDFObj().Get("URI").Value().GetAsPDFText(uri);
						cout << "  Links to: " << uri << endl;
					}
					// ...
				}
				break; 
			case Annot::e_Widget:
				break; 
			case Annot::e_FileAttachment:
				break; 
				// ...
			default:
				break; 
			}
		}
	}

	// Use the high-level API to create new annotations.
	Page first_page = doc.GetPage(1);

	// Create a hyperlink...
	Annots::Link hyperlink = Annots::Link::Create(doc, Rect(85, 570, 503, 524), Action::CreateURI(doc, "http://www.pdftron.com"));
	first_page.AnnotPushBack(hyperlink);

	// Create an intra-document link...
	Action goto_page_3 = Action::CreateGoto(Destination::CreateFitH(doc.GetPage(3), 0));
	Annots::Link link = Annots::Link::Create(doc, Rect(85, 458, 503, 502), goto_page_3);
	link.SetColor(ColorPt(0, 0, 1));

	// Add the new annotation to the first page
	first_page.AnnotPushBack(link);

	// Create a stamp annotation ...
	Annots::RubberStamp stamp = Annots::RubberStamp::Create(doc, Rect(30, 30, 300, 200));
	stamp.SetIcon("Draft");
	first_page.AnnotPushBack(stamp);

	// Create a file attachment annotation (embed the 'peppers.jpg').
	Annots::FileAttachment file_attach = Annots::FileAttachment::Create(doc, Rect(80, 280, 108, 320), (input_path + "peppers.jpg").c_str());
	first_page.AnnotPushBack(file_attach);


	Annots::Ink ink = Annots::Ink::Create(doc, Rect(110, 10, 300, 200));
	Point pt3(110, 10);
	//pt3.x = 110; pt3.y = 10;
	ink.SetPoint(0, 0, pt3);
	pt3.x = 150; pt3.y = 50;
	ink.SetPoint(0, 1, pt3);
	pt3.x = 190; pt3.y = 60;
	ink.SetPoint(0, 2, pt3);
	pt3.x = 180; pt3.y = 90;
	ink.SetPoint(1, 0, pt3);
	pt3.x = 190; pt3.y = 95;
	ink.SetPoint(1, 1, pt3);
	pt3.x = 200; pt3.y = 100;
	ink.SetPoint(1, 2, pt3);
	pt3.x = 166; pt3.y = 86;
	ink.SetPoint(2, 0, pt3);
	pt3.x = 196; pt3.y = 96;
	ink.SetPoint(2, 1, pt3);
	pt3.x = 221; pt3.y = 121;
	ink.SetPoint(2, 2, pt3);
	pt3.x = 288; pt3.y = 188;
	ink.SetPoint(2, 3, pt3);
	ink.SetColor(ColorPt(0, 1, 1), 3);
	first_page.AnnotPushBack(ink);

}

void AnnotationLowLevelAPI(PDFDoc& doc)  
{
	Page page = (doc.GetPageIterator()).Current(); 

	Obj annots = page.GetAnnots();

	if (!annots) 
	{
		// If there are no annotations, create a new annotation 
		// array for the page.
		annots = doc.CreateIndirectArray();  
		page.GetSDFObj().Put("Annots", annots);
	}

	// Create a Text annotation
	Obj annot = doc.CreateIndirectDict();
	annot.PutName("Subtype", "Text");
	annot.PutBool("Open", true);
	annot.PutString("Contents", "The quick brown fox ate the lazy mouse.");
	annot.PutRect("Rect", 266, 116, 430, 204);

	// Insert the annotation in the page annotation array
	annots.PushBack(annot);	

	// Create a Link annotation
	Obj link1 = doc.CreateIndirectDict();
	link1.PutName("Subtype", "Link");
	Destination	dest = Destination::CreateFit(doc.GetPage(2));
	link1.Put("Dest", dest.GetSDFObj());
	link1.PutRect("Rect", 85, 705, 503, 661);
	annots.PushBack(link1);

	// Create another Link annotation
	Obj link2 = doc.CreateIndirectDict();
	link2.PutName("Subtype", "Link");
	Destination	dest2 = Destination::CreateFit(doc.GetPage(3));
	link2.Put("Dest", dest2.GetSDFObj());
	link2.PutRect("Rect", 85, 638, 503, 594);
	annots.PushBack(link2);

	// Note that PDFNet API can be used to modify existing annotations. 
	// In the following example we will modify the second link annotation 
	// (link2) so that it points to the 10th page. We also use a different 
	// destination page fit type.

	// link2 = annots.GetAt(annots.Size()-1);
	link2.Put("Dest", 
		Destination::CreateXYZ(doc.GetPage(10), 100, 792-70, 10).GetSDFObj());

	// Create a third link annotation with a hyperlink action (all other 
	// annotation types can be created in a similar way)
	Obj link3 = doc.CreateIndirectDict();
	link3.PutName("Subtype", "Link");
	link3.PutRect("Rect", 85, 570, 503, 524);

	// Create a URI action 
	Obj action = link3.PutDict("A");
	action.PutName("S", "URI");
	action.PutString("URI", "http://www.pdftron.com");

	annots.PushBack(link3);
}


void CreateTestAnnots(PDFDoc& doc) {
	using namespace pdftron;
	using namespace SDF;
	using namespace PDF;
	using namespace Annots;


	ElementWriter ew;
	ElementBuilder eb;
	Element element ;

	Page first_page= doc.PageCreate(Rect(0, 0, 600, 600));
	doc.PagePushBack(first_page);
	ew.Begin(first_page, ElementWriter::e_overlay, false );	// begin writing to this page
	ew.End();  // save changes to the current page

	//
	// Test of a free text annotation.
	//
	{
		Annots::FreeText txtannot = Annots::FreeText::Create( doc, Rect(10, 400, 160, 570)  );
		txtannot.SetContents( UString("\n\nSome swift brown fox snatched a gray hare out of the air by freezing it with an angry glare."
							  "\n\nAha!\n\nAnd there was much rejoicing!"	) );
		//std::vector<double> dash( 2, 2.0 );
		txtannot.SetBorderStyle( Annot::BorderStyle( Annot::BorderStyle::e_solid, 1, 10, 20 ), false);
		txtannot.SetQuaddingFormat(0);
		first_page.AnnotPushBack(txtannot);
		txtannot.RefreshAppearance();
	}
	{
		Annots::FreeText txtannot = Annots::FreeText::Create( doc, Rect(100, 100, 350, 500)  );
		txtannot.SetContentRect( Rect( 200, 200, 350, 500 ) );
		txtannot.SetContents( UString("\n\nSome swift brown fox snatched a gray hare out of the air by freezing it with an angry glare."
							  "\n\nAha!\n\nAnd there was much rejoicing!"	) );
		txtannot.SetCalloutLinePoints( Point(200,300), Point(150,290), Point(110,110) );
		//std::vector<double> dash( 2, 2.0 );
		txtannot.SetBorderStyle( Annot::BorderStyle( Annot::BorderStyle::e_solid, 1, 10, 20 ), false);
		txtannot.SetEndingStyle( Line::e_ClosedArrow );
		txtannot.SetColor( ColorPt( 0, 1, 0 ) );
		txtannot.SetQuaddingFormat(1);
		first_page.AnnotPushBack(txtannot);
		txtannot.RefreshAppearance();
	}
	{
		Annots::FreeText txtannot = Annots::FreeText::Create( doc, Rect(400, 10, 550, 400)  );
		txtannot.SetContents( UString("\n\nSome swift brown fox snatched a gray hare out of the air by freezing it with an angry glare."
							  "\n\nAha!\n\nAnd there was much rejoicing!"	) );
		txtannot.SetBorderStyle( Annot::BorderStyle( Annot::BorderStyle::e_solid, 1, 10, 20 ), false);
		txtannot.SetColor( ColorPt( 0, 0, 1 ) );
		txtannot.SetOpacity( 0.2 );
		txtannot.SetQuaddingFormat(2);
		first_page.AnnotPushBack(txtannot);
		txtannot.RefreshAppearance();
	}

	Page page= doc.PageCreate(Rect(0, 0, 600, 600));
	doc.PagePushBack(page);
	ew.Begin(page, ElementWriter::e_overlay, false );	// begin writing to this page
	eb.Reset();			// Reset the GState to default
	ew.End();  // save changes to the current page

	{
		//Create a Line annotation...
		Line line=Line::Create(doc, Rect(250, 250, 400, 400));
		line.SetStartPoint( Point(350, 270 ) );
		line.SetEndPoint( Point(260,370) );
		line.SetStartStyle(Line::e_Square);
		line.SetEndStyle(Line::e_Circle);
		line.SetColor(ColorPt(.3, .5, 0), 3);
		line.SetContents( UString("Dashed Captioned") );
		line.SetShowCaption(true);
		line.SetCaptionPosition( Line::e_Top );
		std::vector<double> dash( 2, 2.0 );
		line.SetBorderStyle( Annot::BorderStyle( Annot::BorderStyle::e_dashed, 2, 0, 0, dash ) );
		line.RefreshAppearance();
		page.AnnotPushBack(line);
	}
	{
		Line line=Line::Create(doc, Rect(347, 377, 600, 600));
		line.SetStartPoint( Point(385, 410 ) );
		line.SetEndPoint( Point(540,555) );
		line.SetStartStyle(Line::e_Circle);
		line.SetEndStyle(Line::e_OpenArrow);
		line.SetColor(ColorPt(1, 0, 0), 3);
		line.SetInteriorColor(ColorPt(0, 1, 0), 3);
		line.SetContents( UString("Inline Caption") );
		line.SetShowCaption(true);
		line.SetCaptionPosition( Line::e_Inline );
		line.SetLeaderLineExtensionLength( -4. );
		line.SetLeaderLineLength( -12. );
		line.SetLeaderLineOffset( 2. );
		line.RefreshAppearance();
		page.AnnotPushBack(line);
	}
	{
		Line line=Line::Create(doc, Rect(10, 400, 200, 600));
		line.SetStartPoint( Point(25, 426 ) );
		line.SetEndPoint( Point(180,555) );
		line.SetStartStyle(Line::e_Circle);
		line.SetEndStyle(Line::e_Square);
		line.SetColor(ColorPt(0, 0, 1), 3);
		line.SetInteriorColor(ColorPt(1, 0, 0), 3);
		line.SetContents( UString("Offset Caption") );
		line.SetShowCaption(true);
		line.SetCaptionPosition( Line::e_Top );
		line.SetTextHOffset( -60 );
		line.SetTextVOffset( 10 );
		line.RefreshAppearance();
		page.AnnotPushBack(line);
	}
	{
		Line line=Line::Create(doc, Rect(200, 10, 400, 70));
		line.SetStartPoint( Point(220, 25 ) );
		line.SetEndPoint( Point(370,60) );
		line.SetStartStyle(Line::e_Butt);
		line.SetEndStyle(Line::e_OpenArrow);
		line.SetColor(ColorPt(0, 0, 1), 3);
		line.SetContents( UString("Regular Caption") );
		line.SetShowCaption(true);
		line.SetCaptionPosition( Line::e_Top );
		line.RefreshAppearance();
		page.AnnotPushBack(line);
	}
	{
		Line line=Line::Create(doc, Rect(200, 70, 400, 130));
		line.SetStartPoint( Point(220, 111 ) );
		line.SetEndPoint( Point(370,78) );
		line.SetStartStyle(Line::e_Circle);
		line.SetEndStyle(Line::e_Diamond);
		line.SetContents( UString("Circle to Diamond") );
		line.SetColor(ColorPt(0, 0, 1), 3);
		line.SetInteriorColor(ColorPt(0, 1, 0), 3);
		line.SetShowCaption(true);
		line.SetCaptionPosition( Line::e_Top );
		line.RefreshAppearance();
		page.AnnotPushBack(line);
	}
	{
		Line line=Line::Create(doc, Rect(10, 100, 160, 200));
		line.SetStartPoint( Point(15, 110 ) );
		line.SetEndPoint( Point(150, 190) );
		line.SetStartStyle(Line::e_Slash);
		line.SetEndStyle(Line::e_ClosedArrow);
		line.SetContents( UString("Slash to CArrow") );
		line.SetColor(ColorPt(1, 0, 0), 3);
		line.SetInteriorColor(ColorPt(0, 1, 1), 3);
		line.SetShowCaption(true);
		line.SetCaptionPosition( Line::e_Top );
		line.RefreshAppearance();
		page.AnnotPushBack(line);
	}
	{
		Line line=Line::Create(doc, Rect( 270, 270, 570, 433 ));
		line.SetStartPoint( Point(300, 400 ) );
		line.SetEndPoint( Point(550, 300) );
		line.SetStartStyle(Line::e_RClosedArrow);
		line.SetEndStyle(Line::e_ROpenArrow);
		line.SetContents( UString("ROpen & RClosed arrows") );
		line.SetColor(ColorPt(0, 0, 1), 3);
		line.SetInteriorColor(ColorPt(0, 1, 0), 3);
		line.SetShowCaption(true);
		line.SetCaptionPosition( Line::e_Top );
		line.RefreshAppearance();
		page.AnnotPushBack(line);
	}
	{
		Line line=Line::Create(doc, Rect( 195, 395, 205, 505 ));
		line.SetStartPoint( Point(200, 400 ) );
		line.SetEndPoint( Point(200, 500) );
		line.RefreshAppearance();
		page.AnnotPushBack(line);
	}
	{
		Line line=Line::Create(doc, Rect( 55, 299, 150, 301 ));
		line.SetStartPoint( Point(55, 300 ) );
		line.SetEndPoint( Point(155, 300) );
		line.SetStartStyle(Line::e_Circle);
		line.SetEndStyle(Line::e_Circle);
		line.SetContents( UString("Caption that's longer than its line.") );
		line.SetColor(ColorPt(1, 0, 1), 3);
		line.SetInteriorColor(ColorPt(0, 1, 0), 3);
		line.SetShowCaption(true);
		line.SetCaptionPosition( Line::e_Top );
		line.RefreshAppearance();
		page.AnnotPushBack(line);
	}
	{
		Line line=Line::Create(doc, Rect( 300, 200, 390, 234 ));
		line.SetStartPoint( Point(310, 210 ) );
		line.SetEndPoint( Point(380, 220) );
		line.SetColor(ColorPt(0, 0, 0), 3);
		line.RefreshAppearance();
		page.AnnotPushBack(line);
	}

	Page page3 = doc.PageCreate(Rect(0, 0, 600, 600));
	ew.Begin(page3);	// begin writing to the page
	ew.End();  // save changes to the current page
	doc.PagePushBack(page3);
	{
		Circle circle=Circle::Create(doc, Rect( 300, 300, 390, 350 ));
		circle.SetColor(ColorPt(0, 0, 0), 3);
		circle.RefreshAppearance();
		page3.AnnotPushBack(circle);
	}
	{
		Circle circle=Circle::Create(doc, Rect( 100, 100, 200, 200 ));
		circle.SetColor(ColorPt(0, 1, 0), 3);
		circle.SetInteriorColor(ColorPt(0, 0, 1), 3);
		std::vector<double> dash( 2 ); dash[0]=2;dash[1]=4;
		circle.SetBorderStyle( Annot::BorderStyle( Annot::BorderStyle::e_dashed, 3, 0, 0, dash ) );
		circle.SetPadding( 2 );
		circle.RefreshAppearance();
		page3.AnnotPushBack(circle);
	}
	{
		Square sq = Square::Create( doc, Rect(10,200, 80, 300 ) );
		sq.SetColor(ColorPt(0, 0, 0), 3);
		sq.RefreshAppearance();
		page3.AnnotPushBack( sq );
	}
	{
		Square sq = Square::Create( doc, Rect(500,200, 580, 300 ) );
		sq.SetColor(ColorPt(1, 0, 0), 3);
		sq.SetInteriorColor(ColorPt(0, 1, 1), 3);
		std::vector<double> dash( 2 ); dash[0]=4;dash[1]=2;
		sq.SetBorderStyle( Annot::BorderStyle( Annot::BorderStyle::e_dashed, 6, 0, 0, dash ) );
		sq.SetPadding( 4 );
		sq.RefreshAppearance();
		page3.AnnotPushBack( sq );
	}
	{
		Polygon poly = Polygon::Create(doc, Rect(5, 500, 125, 590));
		poly.SetColor(ColorPt(1, 0, 0), 3);
		poly.SetInteriorColor(ColorPt(1, 1, 0), 3);
		poly.SetVertex(0, Point(12,510) );
		poly.SetVertex(1, Point(100,510) );
		poly.SetVertex(2, Point(100,555) );
		poly.SetVertex(3, Point(35,544) );
		poly.SetBorderStyle( Annot::BorderStyle( Annot::BorderStyle::e_solid, 4, 0, 0 ) );
		poly.SetPadding( 4 );
		poly.RefreshAppearance();
		page3.AnnotPushBack( poly );
	}
	{
		PolyLine poly = PolyLine::Create(doc, Rect(400, 10, 500, 90));
		poly.SetColor(ColorPt(1, 0, 0), 3);
		poly.SetInteriorColor(ColorPt(0, 1, 0), 3);
		poly.SetVertex(0, Point(405,20) );
		poly.SetVertex(1, Point(440,40) );
		poly.SetVertex(2, Point(410,60) );
		poly.SetVertex(3, Point(470,80) );
		poly.SetBorderStyle( Annot::BorderStyle( Annot::BorderStyle::e_solid, 2, 0, 0 ) );
		poly.SetPadding( 4 );
		poly.SetStartStyle( Line::e_RClosedArrow );
		poly.SetEndStyle( Line::e_ClosedArrow );
		poly.RefreshAppearance();
		page3.AnnotPushBack( poly );
	}
	{
		Link lk = Link::Create( doc, Rect(5,5,55,24) );
		//lk.SetColor( ColorPt(0,1,0), 3 );
		lk.RefreshAppearance();
		page3.AnnotPushBack( lk );
	}


	Page page4 = doc.PageCreate(Rect(0, 0, 600, 600));
	ew.Begin(page4);	// begin writing to the page
	ew.End();  // save changes to the current page
	doc.PagePushBack(page4);

	{	
		ew.Begin( page4 );
		Font font = Font::Create(doc, Font::e_helvetica);
		element = eb.CreateTextBegin( font, 16 );
		element.SetPathFill(true);
		ew.WriteElement(element);
		element = eb.CreateTextRun( "Some random text on the page", font, 16 );
		element.SetTextMatrix(1, 0, 0, 1, 100, 500 );
		ew.WriteElement(element);
		ew.WriteElement( eb.CreateTextEnd() );
		ew.End();
	}
	{
		Highlight hl = Highlight::Create( doc, Rect(100,490,150,515) );
		hl.SetColor( ColorPt(0,1,0), 3 );
		hl.RefreshAppearance();
		page4.AnnotPushBack( hl );
	}
	{
		Squiggly sq = Squiggly::Create( doc, Rect(100,450,250,600) );
		//sq.SetColor( ColorPt(1,0,0), 3 );
		sq.SetQuadPoint( 0, QuadPoint( Point( 122,455), Point(240, 545), Point(230, 595), Point(101,500 ) ) );
		sq.RefreshAppearance();
		page4.AnnotPushBack( sq );
	}
	{
		Caret cr = Caret::Create( doc, Rect(100,40,129,69) );
		cr.SetColor( ColorPt(0,0,1), 3 );
		cr.SetSymbol( "P" );
		cr.RefreshAppearance();
		page4.AnnotPushBack( cr );
	}


	Page page5 = doc.PageCreate(Rect(0, 0, 600, 600));
	ew.Begin(page5);	// begin writing to the page
	ew.End();  // save changes to the current page
	doc.PagePushBack(page5);
	FileSpec fs = FileSpec::Create( doc, (input_path + "butterfly.png").c_str(), false );
	Page page6 = doc.PageCreate(Rect(0, 0, 600, 600));
	ew.Begin(page6);	// begin writing to the page
	ew.End();  // save changes to the current page
	doc.PagePushBack(page6);

	{
		Text txt = Text::Create( doc, Rect( 10, 20, 30, 40 ) );
		txt.SetIcon( "UserIcon" );
		txt.SetContents( "User defined icon, unrecognized by appearance generator" );
		txt.SetColor( ColorPt(0,1,0) );
		txt.RefreshAppearance();
		page6.AnnotPushBack( txt );
	}
	{
		Ink ink = Ink::Create( doc, Rect( 100, 400, 200, 550 ) );
		ink.SetColor( ColorPt(0,0,1) );
		ink.SetPoint( 1, 3, Point( 220, 505) );
		ink.SetPoint( 1, 0, Point( 100, 490) );
		ink.SetPoint( 0, 1, Point( 120, 410) );
		ink.SetPoint( 0, 0, Point( 100, 400) );
		ink.SetPoint( 1, 2, Point( 180, 490) );
		ink.SetPoint( 1, 1, Point( 140, 440) );		
		ink.SetBorderStyle( Annot::BorderStyle( Annot::BorderStyle::e_solid, 3, 0, 0  ) );
		ink.RefreshAppearance();
		page6.AnnotPushBack( ink );
	}


	Page page7 = doc.PageCreate(Rect(0, 0, 600, 600));
	ew.Begin(page7);	// begin writing to the page
	ew.End();  // save changes to the current page
	doc.PagePushBack(page7);

	{
		Sound snd = Sound::Create( doc, Rect( 100, 500, 120, 520 ) );
		snd.SetColor(  ColorPt(1,1,0) );
		snd.SetIcon( Sound::e_Speaker );
		snd.RefreshAppearance();
		page7.AnnotPushBack( snd );
	}
	{
		Sound snd = Sound::Create( doc, Rect( 200, 500, 220, 520 ) );
		snd.SetColor(  ColorPt(1,1,0) );
		snd.SetIcon( Sound::e_Mic );
		snd.RefreshAppearance();
		page7.AnnotPushBack( snd );
	}




	Page page8 = doc.PageCreate(Rect(0, 0, 600, 600));
	ew.Begin(page8);	// begin writing to the page
	ew.End();  // save changes to the current page
	doc.PagePushBack(page8);

	for( int ipage =0; ipage < 2; ++ipage ) {
		double px = 5, py = 520;
		for( RubberStamp::Icon istamp = RubberStamp::e_Approved; 
			istamp <= RubberStamp::e_Draft; 
			istamp = static_cast<RubberStamp::Icon>(  static_cast<int>(istamp) + 1  ) ) {
				RubberStamp st = RubberStamp::Create( doc, Rect(1,1,100,100) );
				st.SetIcon( istamp );
				st.SetContents( UString( st.GetIconName() ) );
				st.SetRect( Rect(px, py, px+100, py+25 ) );
				py -= 100;
				if( py < 0 ) {
					py = 520;
					px += 200;
				}
				if( ipage == 0 )
					//page7.AnnotPushBack( st );
					;
				else {
					page8.AnnotPushBack( st );
					st.RefreshAppearance();
				}
		}
	}
	RubberStamp st = RubberStamp::Create( doc, Rect(400,5,550,45) );
	st.SetIcon( "UserStamp" );
	st.SetContents( "User defined stamp" );
	page8.AnnotPushBack( st );
	st.RefreshAppearance();



}




int main(int argc, char *argv[])
{
	int ret = 0;
	PDFNet::Initialize();

	std::string output_path = "../../TestFiles/Output/";

	try  
	{
		PDFDoc doc((input_path + "numbered.pdf").c_str());
		doc.InitSecurityHandler();

		// An example of using SDF/Cos API to add any type of annotations.
		AnnotationLowLevelAPI(doc);
		doc.Save((output_path + "annotation_test1.pdf").c_str(), SDFDoc::e_linearized, 0);
		cout << "Done. Results saved in annotation_test1.pdf" << endl;

		// An example of using the high-level PDFNet API to read existing annotations,
		// to edit existing annotations, and to create new annotation from scratch.
		AnnotationHighLevelAPI(doc);
		doc.Save((output_path + "annotation_test2.pdf").c_str(), SDFDoc::e_linearized, 0);
		cout << "Done. Results saved in annotation_test2.pdf" << endl;

		// an example of creating various annotations in a brand new document
		PDFDoc doc1;
		CreateTestAnnots( doc1 );
		doc1.Save(output_path + "new_annot_test_api.pdf", SDFDoc::e_linearized, 0);
		cout << "Saved new_annot_test_api.pdf" << std::endl;
	}
	catch(Common::Exception& e)
	{
		cout << e << std::endl;
		ret = 1;
	}
	catch(...)
	{
		cout << "Unknown Exception" << std::endl;
		ret = 1;
	}

	PDFNet::Terminate();
	return ret;
}

