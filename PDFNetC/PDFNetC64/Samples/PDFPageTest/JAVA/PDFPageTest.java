//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

import com.pdftron.pdf.*;
import com.pdftron.sdf.*;
import com.pdftron.common.PDFNetException;

public class PDFPageTest {

    public static void main(String[] args) {
        PDFNet.initialize();

        // Relative path to the folder containing test files.
        String input_path = "../../TestFiles/";
        String output_path = "../../TestFiles/Output/";

        // Sample 1 - Split a PDF document into multiple pages
        try {
            System.out.println("_______________________________________________");
            System.out.println("Sample 1 - Split a PDF document into multiple pages...");
            System.out.println("Opening the input pdf...");
            PDFDoc in_doc = new PDFDoc(input_path + "newsletter.pdf");
            in_doc.initSecurityHandler();

            int page_num = in_doc.getPageCount();
            for (int i = 1; i <= page_num; ++i) {
                PDFDoc new_doc = new PDFDoc();
                new_doc.insertPages(0, in_doc, i, i, PDFDoc.InsertBookmarkMode.NONE, null);
                String fname = "newsletter_split_page_" + i + ".pdf";
                new_doc.save(output_path + fname, SDFDoc.SaveMode.REMOVE_UNUSED, null);
                // output PDF new_doc
                System.out.println("Done. Result saved in newsletter_split_page_" + i + ".pdf");
                new_doc.close();
            }
            in_doc.close();
        } catch (Exception e2) {
            System.out.println(e2);
        }

        // Sample 2 - Merge several PDF documents into one
        try {
            System.out.println("_______________________________________________");
            System.out.println("Sample 2 - Merge several PDF documents into one...");
            PDFDoc new_doc = new PDFDoc();
            new_doc.initSecurityHandler();

            int page_num = 15;
            for (int i = 1; i <= page_num; ++i) {
                System.out.println("Opening newsletter_split_page_" + i + ".pdf");
                String fname = "newsletter_split_page_" + i + ".pdf";
                PDFDoc in_doc = new PDFDoc(output_path + fname);
                new_doc.insertPages(i, in_doc, 1, in_doc.getPageCount(), PDFDoc.InsertBookmarkMode.NONE, null);
                in_doc.close();
            }
            new_doc.save(output_path + "newsletter_merge_pages.pdf", SDFDoc.SaveMode.REMOVE_UNUSED, null);
            // output PDF new_doc
            System.out.println("Done. Result saved in newsletter_merge_pages.pdf");
            new_doc.close();
        } catch (Exception e2) {
            System.out.println(e2);
        }

        // Sample 3 - Delete every second page
        try {
            System.out.println("_______________________________________________");
            System.out.println("Sample 3 - Delete every second page...");
            System.out.println("Opening the input pdf...");
            PDFDoc in_doc = new PDFDoc(input_path + "newsletter.pdf");
            in_doc.initSecurityHandler();

            int page_num = in_doc.getPageCount();
            while (page_num >= 1) {
                PageIterator itr = in_doc.getPageIterator(page_num);
                in_doc.pageRemove(itr);
                page_num -= 2;
            }

            in_doc.save(output_path + "newsletter_page_remove.pdf", SDFDoc.SaveMode.NO_FLAGS, null);
            // output PDF in_doc
            System.out.println("Done. Result saved in newsletter_page_remove.pdf...");

            //Close the open document to free up document
            //memory sooner than waiting for the
            //garbage collector
            in_doc.close();
        } catch (Exception e2) {
            System.out.println(e2);
        }

        // Sample 4 - Inserts a page from one document at different
        // locations within another document
        try {
            System.out.println("_______________________________________________");
            System.out.println("Sample 4 - Insert a page at different locations...");
            System.out.println("Opening the input pdf...");

            PDFDoc in1_doc = new PDFDoc((input_path + "newsletter.pdf"));
            in1_doc.initSecurityHandler();

            PDFDoc in2_doc = new PDFDoc((input_path + "fish.pdf"));
            in2_doc.initSecurityHandler();

            PageIterator src_page_itr = in2_doc.getPageIterator();
            Page src_page = src_page_itr.next();
            PageIterator dst_page_itr = in1_doc.getPageIterator();
            int page_num = 1;
            while (dst_page_itr.hasNext()) {
                if (page_num++ % 3 == 0) {
                    in1_doc.pageInsert(dst_page_itr, src_page);
                }
                dst_page_itr.next();
            }

            in1_doc.save(output_path + "newsletter_page_insert.pdf", SDFDoc.SaveMode.NO_FLAGS, null);
            // output PDF in1_doc
            System.out.println("Done. Result saved in newsletter_page_insert.pdf...");

            //Close the open documents to free up document
            //memory sooner than waiting for the
            //garbage collector
            in1_doc.close();
            in2_doc.close();
        } catch (Exception e2) {
            System.out.println(e2);
        }

        // Sample 5 - Replicate pages within a single document
        try {
            System.out.println("_______________________________________________");
            System.out.println("Sample 5 - Replicate pages within a single document...");
            System.out.println("Opening the input pdf...");
            PDFDoc doc = new PDFDoc((input_path + "newsletter.pdf"));
            doc.initSecurityHandler();

            // Replicate the cover page three times (copy page #1 and place it before the
            // seventh page in the document page sequence)
            Page cover = doc.getPage(1);
            PageIterator p7 = doc.getPageIterator(7);
            doc.pageInsert(p7, cover);
            doc.pageInsert(p7, cover);
            doc.pageInsert(p7, cover);

            // Replicate the cover page two more times by placing it before and after
            // existing pages.
            doc.pagePushFront(cover);
            doc.pagePushBack(cover);

            doc.save(output_path + "newsletter_page_clone.pdf", SDFDoc.SaveMode.NO_FLAGS, null);
            // output PDF doc
            System.out.println("Done. Result saved in newsletter_page_clone.pdf...");


            //Close the open document to free up document
            //memory sooner than waiting for the
            //garbage collector
            doc.close();
        } catch (Exception e2) {
            System.out.println(e2);
        }

        // Sample 6 - Use ImportPages() in order to copy multiple pages at once
        // in order to preserve shared resources between pages (e.g. images, fonts,
        // colorspaces, etc.)
        try {
            System.out.println("_______________________________________________");
            System.out.println("Sample 6 - Preserving shared resources using ImportPages...");
            System.out.println("Opening the input pdf...");
            PDFDoc in_doc = new PDFDoc((input_path + "newsletter.pdf"));
            in_doc.initSecurityHandler();

            PDFDoc new_doc = new PDFDoc();

            Page[] copy_pages = new Page[in_doc.getPageCount()];
            int j = 0;
            for (PageIterator itr = in_doc.getPageIterator(); itr.hasNext(); j++) {
                copy_pages[j] = itr.next();
            }

            Page[] imported_pages = new_doc.importPages(copy_pages);
            for (int i = 0; i < imported_pages.length; ++i) {
                new_doc.pagePushFront(imported_pages[i]); // Order pages in reverse order.
                // Use pushBackPage() if you would like to preserve the same order.
            }

            new_doc.save(output_path + "newsletter_import_pages.pdf", SDFDoc.SaveMode.NO_FLAGS, null);
            // output PDF new_doc

            //Close the open documents to free up document
            //memory sooner than waiting for the
            //garbage collector
            in_doc.close();
            new_doc.close();

            System.out.println("Done. Result saved in newsletter_import_pages.pdf...");
            System.out.println();
            System.out.println("Note that the output file size is less than half the size");
            System.out.println("of the file produced using individual page copy operations");
            System.out.println("between two documents");
        } catch (Exception e1) {
            System.out.println(e1);
        }

        PDFNet.terminate();
    }
}
