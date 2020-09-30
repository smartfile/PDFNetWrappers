//
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
//

using pdftron;
using pdftron.Common;
using pdftron.Filters;
using pdftron.SDF;
using pdftron.PDF;
using pdftron.PDF.Annots;

namespace AnnotationTestCS
{
	/// <summary>
	/// Summary description for Class1.
	/// </summary>
	class Class1
	{
        private static pdftron.PDFNetLoader pdfNetLoader = pdftron.PDFNetLoader.Instance();
        static Class1() {}

        private static void AnnotationHighLevelAPI(PDFDoc doc)
        {
            // The following code snippet traverses all annotations in the document
            System.Console.WriteLine("Traversing all annotations in the document...");

            string uri;
            int page_num = 1;
            for (PageIterator itr = doc.GetPageIterator(); itr.HasNext(); itr.Next())
            {
                System.Console.WriteLine("Page " + page_num++ + ": ");

                Page page = itr.Current();
                int num_annots = page.GetNumAnnots();
                for (int i = 0; i < num_annots; ++i)
                {
                    Annot annot = page.GetAnnot(i);
                    if (!annot.IsValid()) continue;
                    System.Console.WriteLine("Annot Type: " + annot.GetSDFObj().Get("Subtype").Value().GetName());

                    Rect bbox = annot.GetRect();
                    System.Console.WriteLine("  Position: " + bbox.x1
                        + ", " + bbox.y1
                        + ", " + bbox.x2
                        + ", " + bbox.y2);

                    switch (annot.GetType())
                    {
                        case Annot.Type.e_Link:
                            {
                                Link lnk = new Link(annot);
                                Action action = lnk.GetAction();
                                if (!action.IsValid()) continue;
                                if (action.GetType() == Action.Type.e_GoTo)
                                {
                                    Destination dest = action.GetDest();
                                    if (!dest.IsValid())
                                    {
                                        System.Console.WriteLine("  Destination is not valid");
                                    }
                                    else
                                    {
                                        int pg_num = dest.GetPage().GetIndex();
                                        System.Console.WriteLine("  Links to: page number " + pg_num + " in this document");
                                    }
                                }
                                else if (action.GetType() == Action.Type.e_URI)
                                {
                                    uri = action.GetSDFObj().Get("URI").Value().GetAsPDFText();
                                    System.Console.WriteLine("  Links to: " + uri);
                                }
                                // ...
                            }
                            break;
                        case Annot.Type.e_Widget:
                            break;
                        case Annot.Type.e_FileAttachment:
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
            Link hyperlink = Link.Create(doc, new Rect(85, 570, 503, 524), Action.CreateURI(doc, "http://www.pdftron.com"));
            first_page.AnnotPushBack(hyperlink);

            // Create an intra-document link...
            Action goto_page_3 = Action.CreateGoto(Destination.CreateFitH(doc.GetPage(3), 0));
            Link link = Link.Create(doc, new Rect(85, 458, 503, 502), goto_page_3);
            link.SetColor(new ColorPt(0, 0, 1));

            // Add the new annotation to the first page
            first_page.AnnotPushBack(link);

            // Create a stamp annotation ...
            RubberStamp stamp = RubberStamp.Create(doc, new Rect(30, 30, 300, 200));
            stamp.SetIcon("Draft");
            first_page.AnnotPushBack(stamp);

            // Create a file attachment annotation (embed the 'peppers.jpg').
            FileAttachment file_attach = FileAttachment.Create(doc, new Rect(80, 280, 108, 320), (input_path + "peppers.jpg"));
            first_page.AnnotPushBack(file_attach);


            Ink ink = Ink.Create(doc, new Rect(110, 10, 300, 200));
            Point pt3 = new Point(110, 10);
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
            ink.SetColor(new ColorPt(0, 1, 1), 3);
            first_page.AnnotPushBack(ink);
        }

        static void AnnotationLowLevelAPI(PDFDoc doc)
        {
            Page page = doc.GetPage(1);

            Obj annots = page.GetAnnots();
            if (annots == null)
            {
                // If there are no annotations, create a new annotation 
                // array for the page.
                annots = doc.CreateIndirectArray();
                page.GetSDFObj().Put("Annots", annots);
            }

            // Create the Text annotation
            Obj text_annot = doc.CreateIndirectDict();
            text_annot.PutName("Subtype", "Text");
            text_annot.PutBool("Open", true);
            text_annot.PutString("Contents", "The quick brown fox ate the lazy mouse.");
            text_annot.PutRect("Rect", 266, 116, 430, 204);

            // Insert the annotation in the page annotation array
            annots.PushBack(text_annot);

            // Create a Link annotation
            Obj link1 = doc.CreateIndirectDict();
            link1.PutName("Subtype", "Link");
            Destination dest = Destination.CreateFit(doc.GetPage(2));
            link1.Put("Dest", dest.GetSDFObj());
            link1.PutRect("Rect", 85, 705, 503, 661);
            annots.PushBack(link1);

            // Create another Link annotation
            Obj link2 = doc.CreateIndirectDict();
            link2.PutName("Subtype", "Link");
            Destination dest2 = Destination.CreateFit(doc.GetPage(3));
            link2.Put("Dest", dest2.GetSDFObj());
            link2.PutRect("Rect", 85, 638, 503, 594);
            annots.PushBack(link2);

            // Note that PDFNet APi can be used to modify existing annotations. 
            // In the following example we will modify the second link annotation 
            // (link2) so that it points to the 10th page. We also use a different 
            // destination page fit type.

            link2.Put("Dest",
                Destination.CreateXYZ(doc.GetPage(10), 100, 792 - 70, 10).GetSDFObj());

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
        
        private static void CreateTestAnnots(PDFDoc doc) {

	        ElementWriter ew = new ElementWriter();
	        ElementBuilder eb = new ElementBuilder();
	        Element element;

	        Page first_page= doc.PageCreate(new Rect(0, 0, 600, 600));
	        doc.PagePushBack(first_page);
	        ew.Begin(first_page, ElementWriter.WriteMode.e_overlay, false );	// begin writing to this page
	        ew.End();  // save changes to the current page

	        //
	        // Test of a free text annotation.
	        //
	        {
                FreeText txtannot = FreeText.Create( doc, new Rect(10, 400, 160, 570)  );
		        txtannot.SetContents("\n\nSome swift brown fox snatched a gray hare out of the air by freezing it with an angry glare." +
							          "\n\nAha!\n\nAnd there was much rejoicing!" );
		        txtannot.SetBorderStyle( new Annot.BorderStyle( Annot.BorderStyle.Style.e_solid, 1, 10, 20 ) );
		        txtannot.SetQuaddingFormat(0);
		        first_page.AnnotPushBack(txtannot);
		        txtannot.RefreshAppearance();
	        }
	        {
		        FreeText txtannot = FreeText.Create( doc, new Rect(100, 100, 350, 500)  );
		        txtannot.SetContentRect( new Rect( 200, 200, 350, 500 ) );
		        txtannot.SetContents( "\n\nSome swift brown fox snatched a gray hare out of the air by freezing it with an angry glare." +
							          "\n\nAha!\n\nAnd there was much rejoicing!" );
		        txtannot.SetCalloutLinePoints( new Point(200,300), new Point(150,290), new Point(110,110) );
		        txtannot.SetBorderStyle(new Annot.BorderStyle(Annot.BorderStyle.Style.e_solid, 1, 10, 20 ) );
		        txtannot.SetEndingStyle(Line.EndingStyle.e_ClosedArrow );
		        txtannot.SetColor( new ColorPt( 0, 1, 0 ) );
		        txtannot.SetQuaddingFormat(1);
		        first_page.AnnotPushBack(txtannot);
		        txtannot.RefreshAppearance();
	        }
	        {
                FreeText txtannot = FreeText.Create( doc, new Rect(400, 10, 550, 400)  );
		        txtannot.SetContents( "\n\nSome swift brown fox snatched a gray hare out of the air by freezing it with an angry glare." +
							          "\n\nAha!\n\nAnd there was much rejoicing!" );
		        txtannot.SetBorderStyle( new Annot.BorderStyle( Annot.BorderStyle.Style.e_solid, 1, 10, 20 ) );
		        txtannot.SetColor( new ColorPt( 0, 0, 1 ) );
		        txtannot.SetOpacity( 0.2 );
		        txtannot.SetQuaddingFormat(2);
		        first_page.AnnotPushBack(txtannot);
		        txtannot.RefreshAppearance();
	        }

	        Page page= doc.PageCreate(new Rect(0, 0, 600, 600));
	        doc.PagePushBack(page);
	        ew.Begin(page, ElementWriter.WriteMode.e_overlay, false );	// begin writing to this page
	        eb.Reset();			// Reset the GState to default
	        ew.End();  // save changes to the current page

	        {
                //Create a Line annotation...
                Line line = Line.Create(doc, new Rect(250, 250, 400, 400));
		        line.SetStartPoint( new Point(350, 270 ) );
		        line.SetEndPoint( new Point(260,370) );
		        line.SetStartStyle(Line.EndingStyle.e_Square);
		        line.SetEndStyle(Line.EndingStyle.e_Circle);
		        line.SetColor(new ColorPt(.3, .5, 0), 3);
		        line.SetContents( "Dashed Captioned" );
		        line.SetShowCaption(true);
		        line.SetCaptionPosition(Line.CapPos.e_Top );
                double[] dash = new double[2];
                dash[0] = 2;
                dash[1] = 2.0;
		        line.SetBorderStyle( new Annot.BorderStyle( Annot.BorderStyle.Style.e_dashed, 2, 0, 0, dash ) );
		        line.RefreshAppearance();
		        page.AnnotPushBack(line);
	        }
	        {
		        Line line=Line.Create(doc, new Rect(347, 377, 600, 600));
		        line.SetStartPoint( new Point(385, 410 ) );
		        line.SetEndPoint(new Point(540,555) );
		        line.SetStartStyle(Line.EndingStyle.e_Circle);
		        line.SetEndStyle(Line.EndingStyle.e_OpenArrow);
		        line.SetColor(new ColorPt(1, 0, 0), 3);
		        line.SetInteriorColor(new ColorPt(0, 1, 0), 3);
		        line.SetContents( "Inline Caption" );
		        line.SetShowCaption(true);
		        line.SetCaptionPosition( Line.CapPos.e_Inline);
		        line.SetLeaderLineExtensionLength( -4 );
		        line.SetLeaderLineLength( -12 );
		        line.SetLeaderLineOffset( 2 );
		        line.RefreshAppearance();
		        page.AnnotPushBack(line);
	        }
	        {
		        Line line=Line.Create(doc, new Rect(10, 400, 200, 600));
		        line.SetStartPoint(new Point(25, 426 ) );
		        line.SetEndPoint(new Point(180,555) );
		        line.SetStartStyle(Line.EndingStyle.e_Circle);
		        line.SetEndStyle(Line.EndingStyle.e_Square);
		        line.SetColor(new ColorPt(0, 0, 1), 3);
		        line.SetInteriorColor(new ColorPt(1, 0, 0), 3);
		        line.SetContents("Offset Caption");
		        line.SetShowCaption(true);
		        line.SetCaptionPosition( Line.CapPos.e_Top );
		        line.SetTextHOffset( -60 );
		        line.SetTextVOffset( 10 );
		        line.RefreshAppearance();
		        page.AnnotPushBack(line);
	        }
	        {
		        Line line=Line.Create(doc, new Rect(200, 10, 400, 70));
		        line.SetStartPoint(new Point(220, 25 ) );
		        line.SetEndPoint(new Point(370,60) );
		        line.SetStartStyle(Line.EndingStyle.e_Butt);
		        line.SetEndStyle(Line.EndingStyle.e_OpenArrow);
		        line.SetColor(new ColorPt(0, 0, 1), 3);
		        line.SetContents("Regular Caption");
		        line.SetShowCaption(true);
		        line.SetCaptionPosition( Line.CapPos.e_Top );
		        line.RefreshAppearance();
		        page.AnnotPushBack(line);
	        }
	        {
		        Line line=Line.Create(doc, new Rect(200, 70, 400, 130));
		        line.SetStartPoint(new Point(220, 111 ) );
		        line.SetEndPoint(new Point(370,78) );
		        line.SetStartStyle(Line.EndingStyle.e_Circle);
		        line.SetEndStyle(Line.EndingStyle.e_Diamond);
		        line.SetContents("Circle to Diamond");
		        line.SetColor(new ColorPt(0, 0, 1), 3);
		        line.SetInteriorColor(new ColorPt(0, 1, 0), 3);
		        line.SetShowCaption(true);
		        line.SetCaptionPosition( Line.CapPos.e_Top );
		        line.RefreshAppearance();
		        page.AnnotPushBack(line);
	        }
	        {
		        Line line=Line.Create(doc, new Rect(10, 100, 160, 200));
		        line.SetStartPoint(new Point(15, 110 ) );
		        line.SetEndPoint(new Point(150, 190) );
		        line.SetStartStyle(Line.EndingStyle.e_Slash);
		        line.SetEndStyle(Line.EndingStyle.e_ClosedArrow);
		        line.SetContents("Slash to CArrow");
		        line.SetColor(new ColorPt(1, 0, 0), 3);
		        line.SetInteriorColor(new ColorPt(0, 1, 1), 3);
		        line.SetShowCaption(true);
		        line.SetCaptionPosition( Line.CapPos.e_Top );
		        line.RefreshAppearance();
		        page.AnnotPushBack(line);
	        }
	        {
		        Line line=Line.Create(doc, new Rect( 270, 270, 570, 433 ));
		        line.SetStartPoint(new Point(300, 400 ) );
		        line.SetEndPoint(new Point(550, 300) );
		        line.SetStartStyle(Line.EndingStyle.e_RClosedArrow);
		        line.SetEndStyle(Line.EndingStyle.e_ROpenArrow);
		        line.SetContents("ROpen & RClosed arrows");
		        line.SetColor(new ColorPt(0, 0, 1), 3);
		        line.SetInteriorColor(new ColorPt(0, 1, 0), 3);
		        line.SetShowCaption(true);
		        line.SetCaptionPosition( Line.CapPos.e_Top );
		        line.RefreshAppearance();
		        page.AnnotPushBack(line);
	        }
	        {
		        Line line=Line.Create(doc, new Rect( 195, 395, 205, 505 ));
		        line.SetStartPoint(new Point(200, 400 ) );
		        line.SetEndPoint(new Point(200, 500) );
		        line.RefreshAppearance();
		        page.AnnotPushBack(line);
	        }
	        {
		        Line line=Line.Create(doc, new Rect( 55, 299, 150, 301 ));
		        line.SetStartPoint(new Point(55, 300 ) );
		        line.SetEndPoint(new Point(155, 300) );
		        line.SetStartStyle(Line.EndingStyle.e_Circle);
		        line.SetEndStyle(Line.EndingStyle.e_Circle);
		        line.SetContents("Caption that's longer than its line.");
		        line.SetColor(new ColorPt(1, 0, 1), 3);
		        line.SetInteriorColor(new ColorPt(0, 1, 0), 3);
		        line.SetShowCaption(true);
		        line.SetCaptionPosition( Line.CapPos.e_Top );
		        line.RefreshAppearance();
		        page.AnnotPushBack(line);
	        }
	        {
		        Line line=Line.Create(doc, new Rect( 300, 200, 390, 234 ));
		        line.SetStartPoint(new Point(310, 210 ) );
		        line.SetEndPoint(new Point(380, 220) );
		        line.SetColor(new ColorPt(0, 0, 0), 3);
		        line.RefreshAppearance();
		        page.AnnotPushBack(line);
	        }

	        Page page3 = doc.PageCreate(new Rect(0, 0, 600, 600));
	        ew.Begin(page3);	// begin writing to the page
	        ew.End();  // save changes to the current page
	        doc.PagePushBack(page3);
	        {
                Circle circle = Circle.Create(doc, new Rect( 300, 300, 390, 350 ));
		        circle.SetColor(new ColorPt(0, 0, 0), 3);
		        circle.RefreshAppearance();
		        page3.AnnotPushBack(circle);
	        }
	        {
                Circle circle= Circle.Create(doc, new Rect( 100, 100, 200, 200 ));
		        circle.SetColor(new ColorPt(0, 1, 0), 3);
		        circle.SetInteriorColor(new ColorPt(0, 0, 1), 3);
		        double[] dash = new double[2];
                dash[0]=2;dash[1]=4;
		        circle.SetBorderStyle(new Annot.BorderStyle( Annot.BorderStyle.Style.e_dashed, 3, 0, 0, dash ) );
		        circle.SetPadding( new Rect(2,2,2,2) );
		        circle.RefreshAppearance();
		        page3.AnnotPushBack(circle);
	        }
	        {
                Square sq = Square.Create( doc, new Rect(10,200, 80, 300 ) );
		        sq.SetColor(new ColorPt(0, 0, 0), 3);
		        sq.RefreshAppearance();
		        page3.AnnotPushBack( sq );
	        }
	        {
                Square sq = Square.Create( doc, new Rect(500,200, 580, 300 ) );
		        sq.SetColor(new ColorPt(1, 0, 0), 3);
		        sq.SetInteriorColor(new ColorPt(0, 1, 1), 3);
		        double[] dash = new double[2];
                dash[0]=4;dash[1]=2;
		        sq.SetBorderStyle(new Annot.BorderStyle( Annot.BorderStyle.Style.e_dashed, 6, 0, 0, dash ) );
		        sq.SetPadding( new Rect(4,4,4,4) );
		        sq.RefreshAppearance();
		        page3.AnnotPushBack( sq );
	        }
	        {
                Polygon poly = Polygon.Create(doc, new Rect(5, 500, 125, 590));
		        poly.SetColor(new ColorPt(1, 0, 0), 3);
		        poly.SetInteriorColor(new ColorPt(1, 1, 0), 3);
		        poly.SetVertex(0, new Point(12,510) );
		        poly.SetVertex(1, new Point(100,510) );
		        poly.SetVertex(2, new Point(100,555) );
		        poly.SetVertex(3, new Point(35,544) );
		        poly.SetBorderStyle(new Annot.BorderStyle( Annot.BorderStyle.Style.e_solid, 4, 0, 0 ) );
		        poly.SetPadding( new Rect(4,4,4,4) );
		        poly.RefreshAppearance();
		        page3.AnnotPushBack( poly );
	        }
	        {
                PolyLine poly = PolyLine.Create(doc, new Rect(400, 10, 500, 90));
		        poly.SetColor(new ColorPt(1, 0, 0), 3);
		        poly.SetInteriorColor(new ColorPt(0, 1, 0), 3);
		        poly.SetVertex(0, new Point(405,20) );
		        poly.SetVertex(1, new Point(440,40) );
		        poly.SetVertex(2, new Point(410,60) );
		        poly.SetVertex(3, new Point(470,80) );
		        poly.SetBorderStyle( new Annot.BorderStyle( Annot.BorderStyle.Style.e_solid, 2, 0, 0 ) );
		        poly.SetPadding( new Rect(4,4,4,4) );
		        poly.SetStartStyle( Line.EndingStyle.e_RClosedArrow );
		        poly.SetEndStyle( Line.EndingStyle.e_ClosedArrow );
		        poly.RefreshAppearance();
		        page3.AnnotPushBack( poly );
	        }
	        {
                Link lk = Link.Create( doc, new Rect(5,5,55,24) );
		        //lk.SetColor( ColorPt(0,1,0), 3 );
		        lk.RefreshAppearance();
		        page3.AnnotPushBack( lk );
	        }


	        Page page4 = doc.PageCreate(new Rect(0, 0, 600, 600));
	        ew.Begin(page4);	// begin writing to the page
	        ew.End();  // save changes to the current page
	        doc.PagePushBack(page4);

	        {	
		        ew.Begin( page4 );
		        Font font = Font.Create(doc, Font.StandardType1Font.e_helvetica);
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
                Highlight hl = Highlight.Create( doc, new Rect(100,490,150,515) );
		        hl.SetColor(new ColorPt(0,1,0), 3 );
		        hl.RefreshAppearance();
		        page4.AnnotPushBack( hl );
	        }
	        {
                Squiggly sq = Squiggly.Create( doc, new Rect(100,450,250,600) );
		        //sq.SetColor( ColorPt(1,0,0), 3 );
		        sq.SetQuadPoint( 0, new QuadPoint(new Point( 122,455), new Point(240, 545), new Point(230, 595), new Point(101,500 ) ) );
		        sq.RefreshAppearance();
		        page4.AnnotPushBack( sq );
	        }
	        {
                Caret cr = Caret.Create( doc, new Rect(100,40,129,69) );
		        cr.SetColor( new ColorPt(0,0,1), 3 );
		        cr.SetSymbol( "P" );
		        cr.RefreshAppearance();
		        page4.AnnotPushBack( cr );
	        }


	        Page page5 = doc.PageCreate(new Rect(0, 0, 600, 600));
	        ew.Begin(page5);	// begin writing to the page
	        ew.End();  // save changes to the current page
	        doc.PagePushBack(page5);
	        Page page6 = doc.PageCreate(new Rect(0, 0, 600, 600));
	        ew.Begin(page6);	// begin writing to the page
	        ew.End();  // save changes to the current page
	        doc.PagePushBack(page6);

	        {
		        Text txt = Text.Create( doc, new Rect( 10, 20, 30, 40 ) );
		        txt.SetIcon( "UserIcon" );
		        txt.SetContents( "User defined icon, unrecognized by appearance generator" );
		        txt.SetColor(new ColorPt(0,1,0) );
		        txt.RefreshAppearance();
		        page6.AnnotPushBack( txt );
	        }
	        {
		        Ink ink = Ink.Create( doc, new Rect( 100, 400, 200, 550 ) );
		        ink.SetColor(new ColorPt(0,0,1) );
		        ink.SetPoint( 1, 3, new Point( 220, 505) );
		        ink.SetPoint( 1, 0, new Point( 100, 490) );
		        ink.SetPoint( 0, 1, new Point( 120, 410) );
		        ink.SetPoint( 0, 0, new Point( 100, 400) );
		        ink.SetPoint( 1, 2, new Point( 180, 490) );
		        ink.SetPoint( 1, 1, new Point( 140, 440) );		
		        ink.SetBorderStyle( new Annot.BorderStyle( Annot.BorderStyle.Style.e_solid, 3, 0, 0  ) );
		        ink.RefreshAppearance();
		        page6.AnnotPushBack( ink );
	        }


	        Page page7 = doc.PageCreate(new Rect(0, 0, 600, 600));
	        ew.Begin(page7);	// begin writing to the page
	        ew.End();  // save changes to the current page
	        doc.PagePushBack(page7);

	        {
                Sound snd = Sound.Create( doc, new Rect( 100, 500, 120, 520 ) );
		        snd.SetColor(  new ColorPt(1,1,0) );
		        snd.SetIcon(Sound.Icon.e_Speaker );
		        snd.RefreshAppearance();
		        page7.AnnotPushBack( snd );
	        }
	        {
                Sound snd = Sound.Create( doc, new Rect( 200, 500, 220, 520 ) );
		        snd.SetColor(new ColorPt(1,1,0) );
		        snd.SetIcon(Sound.Icon.e_Mic );
		        snd.RefreshAppearance();
		        page7.AnnotPushBack( snd );
	        }

            Page page8 = doc.PageCreate(new Rect(0, 0, 600, 600));
	        ew.Begin(page8);	// begin writing to the page
	        ew.End();  // save changes to the current page
	        doc.PagePushBack(page8);

	        for( int ipage =0; ipage < 2; ++ipage ) {
		        double px = 5, py = 520;
		        for (RubberStamp.Icon istamp = RubberStamp.Icon.e_Approved; 
			        istamp <= RubberStamp.Icon.e_Draft; 
			        istamp = (RubberStamp.Icon) (  (int)(istamp) + 1  ) ) {
                        RubberStamp stmp = RubberStamp.Create( doc, new Rect(1,1,100,100) );
                        stmp.SetIcon( istamp );
                        stmp.SetContents(stmp.GetIconName());
                        stmp.SetRect(new Rect(px, py, px+100, py+25 ) );
				        py -= 100;
				        if( py < 0 ) {
					        py = 520;
					        px += 200;
				        }
				        if( ipage == 0 ){
					        //page7.AnnotPushBack( st );
					        ;
						}
				        else {
					        page8.AnnotPushBack(stmp);
                            stmp.RefreshAppearance();
				        }
		        }
	        }
            RubberStamp st = RubberStamp.Create( doc, new Rect(400,5,550,45) );
	        st.SetIcon( "UserStamp" );
	        st.SetContents( "User defined stamp" );
	        page8.AnnotPushBack( st );
	        st.RefreshAppearance();



        }


		// Relative path to the folder containing test files.
		const string input_path =  "../../TestFiles/";
		const string output_path = "../../TestFiles/Output/";

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[System.STAThread]
		static void Main(string[] args)
		{
			PDFNet.Initialize();

			try
			{
				using (PDFDoc doc = new PDFDoc(input_path + "numbered.pdf"))
				{
					doc.InitSecurityHandler();

					// An example of using SDF/Cos API to add any type of annotations.
					AnnotationLowLevelAPI(doc);
					doc.Save(output_path + "annotation_test1.pdf", SDFDoc.SaveOptions.e_linearized);
					System.Console.WriteLine("Done. Results saved in annotation_test1.pdf");
					
					// An example of using the high-level PDFNet API to read existing annotations,
					// to edit existing annotations, and to create new annotation from scratch.
					AnnotationHighLevelAPI(doc);
					doc.Save(output_path + "annotation_test2.pdf", SDFDoc.SaveOptions.e_linearized);
					System.Console.WriteLine("Done. Results saved in annotation_test2.pdf");
				}

                // an example of creating various annotations in a brand new document
                using (PDFDoc doc1 = new PDFDoc())
                {
                    CreateTestAnnots(doc1);
                    doc1.Save(output_path + "new_annot_test_api.pdf", SDFDoc.SaveOptions.e_linearized);
                    System.Console.WriteLine("Saved new_annot_test_api.pdf");
                }
			}
			catch (PDFNetException e)
			{
				System.Console.WriteLine(e.Message);
			}

		}
	}
}
