//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

import com.pdftron.common.PDFNetException;
import com.pdftron.pdf.*;
import com.pdftron.pdf.Annot.BorderStyle;
import com.pdftron.pdf.annots.*;
import com.pdftron.sdf.SDFDoc;
import com.pdftron.sdf.Obj;

import java.text.DecimalFormat;


public class AnnotationTest {
    // Relative path to the folder containing test files.
    public static final String input_path = "../../TestFiles/";

    public static final DecimalFormat format = new DecimalFormat("0.#");

    static void AnnotationHighLevelAPI(PDFDoc doc) throws PDFNetException {
        // The following code snippet traverses all annotations in the document
        System.out.println("Traversing all annotations in the document...");

        int page_num = 1;
        for (PageIterator itr = doc.getPageIterator(); itr.hasNext(); ) {
            System.out.println("Page " + (page_num++) + ": ");

            Page page = itr.next();
            int num_annots = page.getNumAnnots();
            for (int i = 0; i < num_annots; ++i) {
                Annot annot = page.getAnnot(i);
                if (annot.isValid() == false) continue;
                System.out.println("Annot Type: " + annot.getSDFObj().get("Subtype").value().getName());

                double[] bbox = annot.getRect().get();
                System.out.println("  Position: " + format.format(bbox[0])
                        + ", " + format.format(bbox[1])
                        + ", " + format.format(bbox[2])
                        + ", " + format.format(bbox[3]));

                switch (annot.getType()) {
                    case Annot.e_Link: {
                        com.pdftron.pdf.annots.Link link = new com.pdftron.pdf.annots.Link(annot);
                        Action action = link.getAction();
                        if (action.isValid() == false) continue;
                        if (action.getType() == Action.e_GoTo) {
                            Destination dest = action.getDest();
                            if (dest.isValid() == false) {
                                System.out.println("  Destination is not valid.");
                            } else {
                                int page_link = dest.getPage().getIndex();
                                System.out.println("  Links to: page number " + page_link + " in this document");
                            }
                        } else if (action.getType() == Action.e_URI) {
                            String uri = action.getSDFObj().get("URI").value().getAsPDFText();
                            System.out.println("  Links to: " + uri);
                        }
                        // ...
                    }
                    break;
                    case Annot.e_Widget:
                        break;
                    case Annot.e_FileAttachment:
                        break;
                    // ...
                    default:
                        break;
                }
            }
        }

        // Use the high-level API to create new annotations.
        Page first_page = doc.getPage(1);

        // Create a hyperlink...
        com.pdftron.pdf.annots.Link hyperlink = com.pdftron.pdf.annots.Link.create(doc, new Rect(85, 570, 503, 524), Action.createURI(doc, "http://www.pdftron.com"));
        first_page.annotPushBack(hyperlink);

        // Create an intra-document link...
        Action goto_page_3 = Action.createGoto(Destination.createFitH(doc.getPage(3), 0));
        com.pdftron.pdf.annots.Link link = com.pdftron.pdf.annots.Link.create(doc.getSDFDoc(),
                new Rect(85, 458, 503, 502),
                goto_page_3);
        link.setColor(new ColorPt(0, 0, 1), 3);

        // Add the new annotation to the first page
        first_page.annotPushBack(link);

        // Create a stamp annotation ...
        com.pdftron.pdf.annots.RubberStamp stamp = com.pdftron.pdf.annots.RubberStamp.create(doc, new Rect(30, 30, 300, 200));
        stamp.setIcon("Draft");
        first_page.annotPushBack(stamp);

        // Create a file attachment annotation (embed the 'peppers.jpg').
        com.pdftron.pdf.annots.FileAttachment file_attach = com.pdftron.pdf.annots.FileAttachment.create(doc, new Rect(80, 280, 108, 320), (input_path + "peppers.jpg"));
        first_page.annotPushBack(file_attach);


        com.pdftron.pdf.annots.Ink ink = com.pdftron.pdf.annots.Ink.create(doc, new Rect(110, 10, 300, 200));
        Point pt3 = new Point(110, 10);
        //pt3.x = 110; pt3.y = 10;
        ink.setPoint(0, 0, pt3);
        pt3.x = 150;
        pt3.y = 50;
        ink.setPoint(0, 1, pt3);
        pt3.x = 190;
        pt3.y = 60;
        ink.setPoint(0, 2, pt3);
        pt3.x = 180;
        pt3.y = 90;
        ink.setPoint(1, 0, pt3);
        pt3.x = 190;
        pt3.y = 95;
        ink.setPoint(1, 1, pt3);
        pt3.x = 200;
        pt3.y = 100;
        ink.setPoint(1, 2, pt3);
        pt3.x = 166;
        pt3.y = 86;
        ink.setPoint(2, 0, pt3);
        pt3.x = 196;
        pt3.y = 96;
        ink.setPoint(2, 1, pt3);
        pt3.x = 221;
        pt3.y = 121;
        ink.setPoint(2, 2, pt3);
        pt3.x = 288;
        pt3.y = 188;
        ink.setPoint(2, 3, pt3);
        ink.setColor(new ColorPt(0, 1, 1), 3);
        first_page.annotPushBack(ink);

    }

    static void AnnotationLowLevelAPI(PDFDoc doc) throws PDFNetException {
        Page page = doc.getPageIterator().next();

        Obj annots = page.getAnnots();

        if (annots == null) {
            // If there are no annotations, create a new annotation
            // array for the page.
            annots = doc.createIndirectArray();
            page.getSDFObj().put("Annots", annots);
        }

        // Create a Text annotation
        Obj annot = doc.createIndirectDict();
        annot.putName("Subtype", "Text");
        annot.putBool("Open", true);
        annot.putString("Contents", "The quick brown fox ate the lazy mouse.");
        annot.putRect("Rect", 266, 116, 430, 204);

        // Insert the annotation in the page annotation array
        annots.pushBack(annot);

        // Create a Link annotation
        Obj link1 = doc.createIndirectDict();
        link1.putName("Subtype", "Link");
        Destination dest = Destination.createFit(doc.getPage(2));
        link1.put("Dest", dest.getSDFObj());
        link1.putRect("Rect", 85, 705, 503, 661);
        annots.pushBack(link1);

        // Create another Link annotation
        Obj link2 = doc.createIndirectDict();
        link2.putName("Subtype", "Link");
        Destination dest2 = Destination.createFit(doc.getPage(3));
        link2.put("Dest", dest2.getSDFObj());
        link2.putRect("Rect", 85, 638, 503, 594);
        annots.pushBack(link2);

        // Note that PDFNet APi can be used to modify existing annotations.
        // In the following example we will modify the second link annotation
        // (link2) so that it points to the 10th page. We also use a different
        // destination page fit type.

        // link2 = annots.GetAt(annots.Size()-1);
        link2.put("Dest",
                Destination.createXYZ(doc.getPage(10), 100, 792 - 70, 10).getSDFObj());

        // Create a third link annotation with a hyperlink action (all other
        // annotation types can be created in a similar way)
        Obj link3 = doc.createIndirectDict();
        link3.putName("Subtype", "Link");
        link3.putRect("Rect", 85, 570, 503, 524);

        // Create a URI action
        Obj action = link3.putDict("A");
        action.putName("S", "URI");
        action.putString("URI", "http://www.pdftron.com");

        annots.pushBack(link3);
    }


    static void CreateTestAnnots(PDFDoc doc) throws PDFNetException {
        ElementWriter ew = new ElementWriter();
        ElementBuilder eb = new ElementBuilder();
        Element element;

        Page first_page = doc.pageCreate(new Rect(0, 0, 600, 600));
        doc.pagePushBack(first_page);
        ew.begin(first_page, ElementWriter.e_overlay, false);    // begin writing to this page
        ew.end();  // save changes to the current page

        //
        // Test of a free text annotation.
        //
        {
            FreeText txtannot = FreeText.create(doc, new Rect(10, 400, 160, 570));
            txtannot.setContents("\n\nSome swift brown fox snatched a gray hare out of the air by freezing it with an angry glare." +
                    "\n\nAha!\n\nAnd there was much rejoicing!");
            txtannot.setBorderStyle(new Annot.BorderStyle(Annot.BorderStyle.e_solid, 1, 10, 20));
            txtannot.setQuaddingFormat(0);
            first_page.annotPushBack(txtannot);
            txtannot.refreshAppearance();
        }
        {
            FreeText txtannot = FreeText.create(doc, new Rect(100, 100, 350, 500));
            txtannot.setContentRect(new Rect(200, 200, 350, 500));
            txtannot.setContents("\n\nSome swift brown fox snatched a gray hare out of the air by freezing it with an angry glare." +
                    "\n\nAha!\n\nAnd there was much rejoicing!");
            txtannot.setCalloutLinePoints(new Point(200, 300), new Point(150, 290), new Point(110, 110));
            txtannot.setBorderStyle(new Annot.BorderStyle(Annot.BorderStyle.e_solid, 1, 10, 20));
            txtannot.setEndingStyle(Line.e_ClosedArrow);
            txtannot.setColor(new ColorPt(0, 1, 0));
            txtannot.setQuaddingFormat(1);
            first_page.annotPushBack(txtannot);
            txtannot.refreshAppearance();
        }
        {
            FreeText txtannot = FreeText.create(doc, new Rect(400, 10, 550, 400));
            txtannot.setContents("\n\nSome swift brown fox snatched a gray hare out of the air by freezing it with an angry glare." +
                    "\n\nAha!\n\nAnd there was much rejoicing!");
            txtannot.setBorderStyle(new Annot.BorderStyle(Annot.BorderStyle.e_solid, 1, 10, 20));
            txtannot.setColor(new ColorPt(0, 0, 1));
            txtannot.setOpacity(0.2);
            txtannot.setQuaddingFormat(2);
            first_page.annotPushBack(txtannot);
            txtannot.refreshAppearance();
        }

        Page page = doc.pageCreate(new Rect(0, 0, 600, 600));
        doc.pagePushBack(page);
        ew.begin(page, ElementWriter.e_overlay, false);    // begin writing to this page
        eb.reset();            // Reset the GState to default
        ew.end();  // save changes to the current page

        {
            //Create a Line annotation...
            Line line = Line.create(doc, new Rect(250, 250, 400, 400));
            line.setStartPoint(new Point(350, 270));
            line.setEndPoint(new Point(260, 370));
            line.setStartStyle(Line.e_Square);
            line.setEndStyle(Line.e_Circle);
            line.setColor(new ColorPt(.3, .5, 0), 3);
            line.setContents("Dashed Captioned");
            line.setShowCaption(true);
            line.setCaptionPosition(Line.e_Top);
            double[] dash = {2, 2.0};
            line.setBorderStyle(new Annot.BorderStyle(Annot.BorderStyle.e_dashed, 2, 0, 0, dash));
            line.refreshAppearance();
            page.annotPushBack(line);
        }
        {
            Line line = Line.create(doc, new Rect(347, 377, 600, 600));
            line.setStartPoint(new Point(385, 410));
            line.setEndPoint(new Point(540, 555));
            line.setStartStyle(Line.e_Circle);
            line.setEndStyle(Line.e_OpenArrow);
            line.setColor(new ColorPt(1, 0, 0), 3);
            line.setInteriorColor(new ColorPt(0, 1, 0), 3);
            line.setContents("Inline Caption");
            line.setShowCaption(true);
            line.setCaptionPosition(Line.e_Inline);
            line.setLeaderLineExtensionLength(-4.);
            line.setLeaderLineLength(-12.);
            line.setLeaderLineOffset(2.);
            line.refreshAppearance();
            page.annotPushBack(line);
        }
        {
            Line line = Line.create(doc, new Rect(10, 400, 200, 600));
            line.setStartPoint(new Point(25, 426));
            line.setEndPoint(new Point(180, 555));
            line.setStartStyle(Line.e_Circle);
            line.setEndStyle(Line.e_Square);
            line.setColor(new ColorPt(0, 0, 1), 3);
            line.setInteriorColor(new ColorPt(1, 0, 0), 3);
            line.setContents("Offset Caption");
            line.setShowCaption(true);
            line.setCaptionPosition(Line.e_Top);
            line.setTextHOffset(-60);
            line.setTextVOffset(10);
            line.refreshAppearance();
            page.annotPushBack(line);
        }
        {
            Line line = Line.create(doc, new Rect(200, 10, 400, 70));
            line.setStartPoint(new Point(220, 25));
            line.setEndPoint(new Point(370, 60));
            line.setStartStyle(Line.e_Butt);
            line.setEndStyle(Line.e_OpenArrow);
            line.setColor(new ColorPt(0, 0, 1), 3);
            line.setContents("Regular Caption");
            line.setShowCaption(true);
            line.setCaptionPosition(Line.e_Top);
            line.refreshAppearance();
            page.annotPushBack(line);
        }
        {
            Line line = Line.create(doc, new Rect(200, 70, 400, 130));
            line.setStartPoint(new Point(220, 111));
            line.setEndPoint(new Point(370, 78));
            line.setStartStyle(Line.e_Circle);
            line.setEndStyle(Line.e_Diamond);
            line.setContents("Circle to Diamond");
            line.setColor(new ColorPt(0, 0, 1), 3);
            line.setInteriorColor(new ColorPt(0, 1, 0), 3);
            line.setShowCaption(true);
            line.setCaptionPosition(Line.e_Top);
            line.refreshAppearance();
            page.annotPushBack(line);
        }
        {
            Line line = Line.create(doc, new Rect(10, 100, 160, 200));
            line.setStartPoint(new Point(15, 110));
            line.setEndPoint(new Point(150, 190));
            line.setStartStyle(Line.e_Slash);
            line.setEndStyle(Line.e_ClosedArrow);
            line.setContents("Slash to CArrow");
            line.setColor(new ColorPt(1, 0, 0), 3);
            line.setInteriorColor(new ColorPt(0, 1, 1), 3);
            line.setShowCaption(true);
            line.setCaptionPosition(Line.e_Top);
            line.refreshAppearance();
            page.annotPushBack(line);
        }
        {
            Line line = Line.create(doc, new Rect(270, 270, 570, 433));
            line.setStartPoint(new Point(300, 400));
            line.setEndPoint(new Point(550, 300));
            line.setStartStyle(Line.e_RClosedArrow);
            line.setEndStyle(Line.e_ROpenArrow);
            line.setContents("ROpen & RClosed arrows");
            line.setColor(new ColorPt(0, 0, 1), 3);
            line.setInteriorColor(new ColorPt(0, 1, 0), 3);
            line.setShowCaption(true);
            line.setCaptionPosition(Line.e_Top);
            line.refreshAppearance();
            page.annotPushBack(line);
        }
        {
            Line line = Line.create(doc, new Rect(195, 395, 205, 505));
            line.setStartPoint(new Point(200, 400));
            line.setEndPoint(new Point(200, 500));
            line.refreshAppearance();
            page.annotPushBack(line);
        }
        {
            Line line = Line.create(doc, new Rect(55, 299, 150, 301));
            line.setStartPoint(new Point(55, 300));
            line.setEndPoint(new Point(155, 300));
            line.setStartStyle(Line.e_Circle);
            line.setEndStyle(Line.e_Circle);
            line.setContents("Caption that's longer than its line.");
            line.setColor(new ColorPt(1, 0, 1), 3);
            line.setInteriorColor(new ColorPt(0, 1, 0), 3);
            line.setShowCaption(true);
            line.setCaptionPosition(Line.e_Top);
            line.refreshAppearance();
            page.annotPushBack(line);
        }
        {
            Line line = Line.create(doc, new Rect(300, 200, 390, 234));
            line.setStartPoint(new Point(310, 210));
            line.setEndPoint(new Point(380, 220));
            line.setColor(new ColorPt(0, 0, 0), 3);
            line.refreshAppearance();
            page.annotPushBack(line);
        }

        Page page3 = doc.pageCreate(new Rect(0, 0, 600, 600));
        ew.begin(page3);    // begin writing to the page
        ew.end();  // save changes to the current page
        doc.pagePushBack(page3);
        {
            Circle circle = Circle.create(doc, new Rect(300, 300, 390, 350));
            circle.setColor(new ColorPt(0, 0, 0), 3);
            circle.refreshAppearance();
            page3.annotPushBack(circle);
        }
        {
            Circle circle = Circle.create(doc, new Rect(100, 100, 200, 200));
            circle.setColor(new ColorPt(0, 1, 0), 3);
            circle.setInteriorColor(new ColorPt(0, 0, 1), 3);
            double[] dash = {2, 4};
            circle.setBorderStyle(new Annot.BorderStyle(Annot.BorderStyle.e_dashed, 3, 0, 0, dash));
            circle.setPadding(2);
            circle.refreshAppearance();
            page3.annotPushBack(circle);
        }
        {
            Square sq = Square.create(doc, new Rect(10, 200, 80, 300));
            sq.setColor(new ColorPt(0, 0, 0), 3);
            sq.refreshAppearance();
            page3.annotPushBack(sq);
        }
        {
            Square sq = Square.create(doc, new Rect(500, 200, 580, 300));
            sq.setColor(new ColorPt(1, 0, 0), 3);
            sq.setInteriorColor(new ColorPt(0, 1, 1), 3);
            double[] dash = {4, 2};
            sq.setBorderStyle(new Annot.BorderStyle(Annot.BorderStyle.e_dashed, 6, 0, 0, dash));
            sq.setPadding(4);
            sq.refreshAppearance();
            page3.annotPushBack(sq);
        }
        {
            Polygon poly = Polygon.create(doc, new Rect(5, 500, 125, 590));
            poly.setColor(new ColorPt(1, 0, 0), 3);
            poly.setInteriorColor(new ColorPt(1, 1, 0), 3);
            poly.setVertex(0, new Point(12, 510));
            poly.setVertex(1, new Point(100, 510));
            poly.setVertex(2, new Point(100, 555));
            poly.setVertex(3, new Point(35, 544));
            poly.setBorderStyle(new Annot.BorderStyle(Annot.BorderStyle.e_solid, 4, 0, 0));
            poly.setPadding(4);
            poly.refreshAppearance();
            page3.annotPushBack(poly);
        }
        {
            PolyLine poly = PolyLine.create(doc, new Rect(400, 10, 500, 90));
            poly.setColor(new ColorPt(1, 0, 0), 3);
            poly.setInteriorColor(new ColorPt(0, 1, 0), 3);
            poly.setVertex(0, new Point(405, 20));
            poly.setVertex(1, new Point(440, 40));
            poly.setVertex(2, new Point(410, 60));
            poly.setVertex(3, new Point(470, 80));
            poly.setBorderStyle(new Annot.BorderStyle(Annot.BorderStyle.e_solid, 2, 0, 0));
            poly.setPadding(4);
            poly.setStartStyle(Line.e_RClosedArrow);
            poly.setEndStyle(Line.e_ClosedArrow);
            poly.refreshAppearance();
            page3.annotPushBack(poly);
        }
        {
            Link lk = Link.create(doc, new Rect(5, 5, 55, 24));
            lk.refreshAppearance();
            page3.annotPushBack(lk);
        }


        Page page4 = doc.pageCreate(new Rect(0, 0, 600, 600));
        ew.begin(page4);    // begin writing to the page
        ew.end();  // save changes to the current page
        doc.pagePushBack(page4);

        {
            ew.begin(page4);
            Font font = Font.create(doc, Font.e_helvetica);
            element = eb.createTextBegin(font, 16);
            element.setPathFill(true);
            ew.writeElement(element);
            element = eb.createTextRun("Some random text on the page", font, 16);
            element.setTextMatrix(1, 0, 0, 1, 100, 500);
            ew.writeElement(element);
            ew.writeElement(eb.createTextEnd());
            ew.end();
        }
        {
            Highlight hl = Highlight.create(doc, new Rect(100, 490, 150, 515));
            hl.setColor(new ColorPt(0, 1, 0), 3);
            hl.refreshAppearance();
            page4.annotPushBack(hl);
        }
        {
            Squiggly sq = Squiggly.create(doc, new Rect(100, 450, 250, 600));
            sq.setQuadPoint(0, new QuadPoint(new Point(122, 455), new Point(240, 545), new Point(230, 595), new Point(101, 500)));
            sq.refreshAppearance();
            page4.annotPushBack(sq);
        }
        {
            Caret cr = Caret.create(doc, new Rect(100, 40, 129, 69));
            cr.setColor(new ColorPt(0, 0, 1), 3);
            cr.setSymbol("P");
            cr.refreshAppearance();
            page4.annotPushBack(cr);
        }


        Page page5 = doc.pageCreate(new Rect(0, 0, 600, 600));
        ew.begin(page5);    // begin writing to the page
        ew.end();  // save changes to the current page
        doc.pagePushBack(page5);
        FileSpec fs = FileSpec.create(doc, input_path + "butterfly.png", false);
        Page page6 = doc.pageCreate(new Rect(0, 0, 600, 600));
        ew.begin(page6);    // begin writing to the page
        ew.end();  // save changes to the current page
        doc.pagePushBack(page6);

        {
            Text txt = Text.create(doc, new Point(10, 20));
            txt.setIcon("UserIcon");
            txt.setContents("User defined icon, unrecognized by appearance generator");
            txt.setColor(new ColorPt(0, 1, 0));
            txt.refreshAppearance();
            page6.annotPushBack(txt);
        }
        {
            Ink ink = Ink.create(doc, new Rect(100, 400, 200, 550));
            ink.setColor(new ColorPt(0, 0, 1));
            ink.setPoint(1, 3, new Point(220, 505));
            ink.setPoint(1, 0, new Point(100, 490));
            ink.setPoint(0, 1, new Point(120, 410));
            ink.setPoint(0, 0, new Point(100, 400));
            ink.setPoint(1, 2, new Point(180, 490));
            ink.setPoint(1, 1, new Point(140, 440));
            ink.setBorderStyle(new Annot.BorderStyle(Annot.BorderStyle.e_solid, 3, 0, 0));
            ink.refreshAppearance();
            page6.annotPushBack(ink);
        }


        Page page7 = doc.pageCreate(new Rect(0, 0, 600, 600));
        ew.begin(page7);    // begin writing to the page
        ew.end();  // save changes to the current page
        doc.pagePushBack(page7);

        {
            Sound snd = Sound.create(doc, new Rect(100, 500, 120, 520));
            snd.setColor(new ColorPt(1, 1, 0));
            snd.setIcon(Sound.e_Speaker);
            snd.refreshAppearance();
            page7.annotPushBack(snd);
        }
        {
            Sound snd = Sound.create(doc, new Rect(200, 500, 220, 520));
            snd.setColor(new ColorPt(1, 1, 0));
            snd.setIcon(Sound.e_Mic);
            snd.refreshAppearance();
            page7.annotPushBack(snd);
        }

        Page page8 = doc.pageCreate(new Rect(0, 0, 600, 600));
        ew.begin(page8);    // begin writing to the page
        ew.end();  // save changes to the current page
        doc.pagePushBack(page8);

        for (int ipage = 0; ipage < 2; ++ipage) {
            double px = 5, py = 520;
            for (int istamp = 0; istamp <= RubberStamp.e_Draft; ++istamp) {
                RubberStamp st = RubberStamp.create(doc, new Rect(1, 1, 100, 100));
                st.SetIcon(istamp);
                st.setContents(st.getIconName());
                st.setRect(new Rect(px, py, px + 100, py + 25));
                py -= 100;
                if (py < 0) {
                    py = 520;
                    px += 200;
                }
                if (ipage == 0)
                    //page7.AnnotPushBack( st );
                    ;
                else {
                    page8.annotPushBack(st);
                    st.refreshAppearance();
                }
            }
        }
        RubberStamp st = RubberStamp.create(doc, new Rect(400, 5, 550, 45));
        st.setIcon("UserStamp");
        st.setContents("User defined stamp");
        page8.annotPushBack(st);
        st.refreshAppearance();
    }

    public static void main(String[] args) {
        PDFNet.initialize();

        String output_path = "../../TestFiles/Output/";

        try {
            PDFDoc doc = new PDFDoc((input_path + "numbered.pdf"));
            doc.initSecurityHandler();

            // An example of using SDF/Cos API to add any type of annotations.
            AnnotationLowLevelAPI(doc);
            doc.save(output_path + "annotation_test1.pdf", SDFDoc.SaveMode.LINEARIZED, null);
            // output PDF doc
            System.out.println("Done. Results saved in annotation_test1.pdf");

            // An example of using the high-level PDFNet API to read existing annotations,
            // to edit existing annotations, and to create new annotation from scratch.
            AnnotationHighLevelAPI(doc);
            doc.save(output_path + "annotation_test2.pdf", SDFDoc.SaveMode.LINEARIZED, null);
            // output PDF doc
            System.out.println("Done. Results saved in annotation_test2.pdf");

            // an example of creating various annotations in a brand new document
            PDFDoc doc1 = new PDFDoc();
            CreateTestAnnots(doc1);
            doc1.save(output_path + "new_annot_test_api.pdf", SDFDoc.SaveMode.LINEARIZED, null);
            // output PDF doc1
            System.out.println("Saved new_annot_test_api.pdf");
        } catch (Exception e) {
            System.out.println(e);
        }

        PDFNet.terminate();
    }

}
