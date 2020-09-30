//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

import com.pdftron.common.PDFNetException;
import com.pdftron.pdf.*;
import com.pdftron.sdf.Obj;
import com.pdftron.sdf.SDFDoc;


public class BookmarkTest {

    public static void main(String[] args) {
        PDFNet.initialize();

        // Relative path to the folder containing test files.
        String input_path = "../../TestFiles/";
        String output_path = "../../TestFiles/Output/";

        // The following example illustrates how to create and edit the outline tree
        // using high-level Bookmark methods.
        try {
            PDFDoc doc = new PDFDoc((input_path + "numbered.pdf"));
            doc.initSecurityHandler();

            // Lets first create the root bookmark items.
            Bookmark red = Bookmark.create(doc, "Red");
            Bookmark green = Bookmark.create(doc, "Green");
            Bookmark blue = Bookmark.create(doc, "Blue");

            doc.addRootBookmark(red);
            doc.addRootBookmark(green);
            doc.addRootBookmark(blue);

            // You can also add new root bookmarks using Bookmark.AddNext("...")
            blue.addNext("foo");
            blue.addNext("bar");

            // We can now associate new bookmarks with page destinations:

            // The following example creates an 'explicit' destination (see
            // section '8.2.1 Destinations' in PDF Reference for more details)
            Destination red_dest = Destination.createFit(doc.getPageIterator().next());
            red.setAction(Action.createGoto(red_dest));

            // Create an explicit destination to the first green page in the document
            green.setAction(Action.createGoto(
                    Destination.createFit(doc.getPage(10))));

            // The following example creates a 'named' destination (see
            // section '8.2.1 Destinations' in PDF Reference for more details)
            // Named destinations have certain advantages over explicit destinations.
            byte[] key = {'b', 'l', 'u', 'e', '1'};
            Action blue_action = Action.createGoto(key,
                    Destination.createFit(doc.getPage(19)));

            blue.setAction(blue_action);

            // We can now add children Bookmarks
            Bookmark sub_red1 = red.addChild("Red - Page 1");
            sub_red1.setAction(Action.createGoto(Destination.createFit(doc.getPage(1))));
            Bookmark sub_red2 = red.addChild("Red - Page 2");
            sub_red2.setAction(Action.createGoto(Destination.createFit(doc.getPage(2))));
            Bookmark sub_red3 = red.addChild("Red - Page 3");
            sub_red3.setAction(Action.createGoto(Destination.createFit(doc.getPage(3))));
            Bookmark sub_red4 = sub_red3.addChild("Red - Page 4");
            sub_red4.setAction(Action.createGoto(Destination.createFit(doc.getPage(4))));
            Bookmark sub_red5 = sub_red3.addChild("Red - Page 5");
            sub_red5.setAction(Action.createGoto(Destination.createFit(doc.getPage(5))));
            Bookmark sub_red6 = sub_red3.addChild("Red - Page 6");
            sub_red6.setAction(Action.createGoto(Destination.createFit(doc.getPage(6))));

            // Example of how to find and delete a bookmark by title text.
            Bookmark foo = doc.getFirstBookmark().find("foo");
            if (foo.isValid()) {
                foo.delete();
            } else {
                throw new Exception("Foo is not Valid");
            }

            Bookmark bar = doc.getFirstBookmark().find("bar");
            if (bar.isValid()) {
                bar.delete();
            } else {
                throw new Exception("Bar is not Valid");
            }

            // Adding color to Bookmarks. Color and other formatting can help readers
            // get around more easily in large PDF documents.
            red.setColor(1, 0, 0);
            green.setColor(0, 1, 0);
            green.setFlags(2);            // set bold font
            blue.setColor(0, 0, 1);
            blue.setFlags(3);            // set bold and itallic

            doc.save((output_path + "bookmark.pdf"), SDFDoc.SaveMode.NO_FLAGS, null);
            // output PDF doc
            doc.close();
            System.out.println("Done. Result saved in bookmark.pdf");
        } catch (Exception e) {
            System.out.println(e);
        }

        // The following example illustrates how to traverse the outline tree using
        // Bookmark navigation methods: Bookmark.GetNext(), Bookmark.GetPrev(),
        // Bookmark.GetFirstChild () and Bookmark.GetLastChild ().
        try {
            // Open the document that was saved in the previous code sample
            PDFDoc doc = new PDFDoc((output_path + "bookmark.pdf"));
            doc.initSecurityHandler();

            Bookmark root = doc.getFirstBookmark();
            PrintOutlineTree(root);

            doc.close();
            System.out.println("Done.");
        } catch (Exception e) {
            System.out.println(e);
        }

        // The following example illustrates how to create a Bookmark to a page
        // in a remote document. A remote go-to action is similar to an ordinary
        // go-to action, but jumps to a destination in another PDF file instead
        // of the current file. See Section 8.5.3 'Remote Go-To Actions' in PDF
        // Reference Manual for details.
        try {
            // Open the document that was saved in the previous code sample
            PDFDoc doc = new PDFDoc((output_path + "bookmark.pdf"));
            doc.initSecurityHandler();

            // Create file specification (the file reffered to by the remote bookmark)
            Obj file_spec = doc.createIndirectDict();
            file_spec.putName("Type", "Filespec");
            file_spec.putString("F", "bookmark.pdf");
            FileSpec spec = new FileSpec(file_spec);
            Action goto_remote = Action.createGotoRemote(spec, 5, true);

            Bookmark remoteBookmark1 = Bookmark.create(doc, "REMOTE BOOKMARK 1");
            remoteBookmark1.setAction(goto_remote);
            doc.addRootBookmark(remoteBookmark1);

            // Create another remote bootmark, but this time using the low-level SDF/Cos API.
            // Create a remote action
            Bookmark remoteBookmark2 = Bookmark.create(doc, "REMOTE BOOKMARK 2");
            doc.addRootBookmark(remoteBookmark2);

            Obj gotoR = remoteBookmark2.getSDFObj().putDict("A");
            {
                gotoR.putName("S", "GoToR"); // Set action type
                gotoR.putBool("NewWindow", true);

                // Set the file specification
                gotoR.put("F", file_spec);

                // jump to the first page. Note that pages are indexed from 0.
                Obj dest = gotoR.putArray("D"); // Set the destination
                dest.pushBackNumber(9);
                dest.pushBackName("Fit");
            }

            doc.save((output_path + "bookmark_remote.pdf"), SDFDoc.SaveMode.LINEARIZED, null);
            // output PDF doc
            doc.close();
            System.out.println("Done. Result saved in bookmark_remote.pdf");
        } catch (Exception e) {
            System.out.println(e);
        }

        PDFNet.terminate();
    }

    static void PrintIndent(Bookmark item) throws PDFNetException {
        int ident = item.getIndent() - 1;
        for (int i = 0; i < ident; ++i) System.out.print("  ");
    }

    // Prints out the outline tree to the standard output
    static void PrintOutlineTree(Bookmark item) throws PDFNetException {
        for (; item.isValid(); item = item.getNext()) {
            PrintIndent(item);
            System.out.print((item.isOpen() ? "- " : "+ ") + item.getTitle() + " ACTION -> ");

            // Print Action
            Action action = item.getAction();
            if (action.isValid()) {
                if (action.getType() == Action.e_GoTo) {
                    Destination dest = action.getDest();
                    if (dest.isValid()) {
                        Page page = dest.getPage();
                        System.out.println("GoTo Page #" + page.getIndex());
                    }
                } else {
                    System.out.println("Not a 'GoTo' action");
                }
            } else {
                System.out.println("NULL");
            }

            if (item.hasChildren())     // Recursively print children sub-trees
            {
                PrintOutlineTree(item.getFirstChild());
            }
        }
    }
}
