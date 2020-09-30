//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import com.pdftron.common.PDFNetException;
import com.pdftron.filters.FilterReader;
import com.pdftron.filters.FilterWriter;
import com.pdftron.filters.MappedFile;
import com.pdftron.pdf.*;
import com.pdftron.sdf.SDFDoc;

public class PDFDocMemoryTest {

    public static void main(String[] args) {

        PDFNet.initialize();

        // Relative path to the folder containing test files.
        String input_path = "../../TestFiles/";
        String output_path = "../../TestFiles/Output/";

        // The following sample illustrates how to read/write a PDF document from/to
        // a memory buffer.  This is useful for applications that work with dynamic PDF
        // documents that don't need to be saved/read from a disk.
        try {
            // Read a PDF document in a memory buffer.
            MappedFile file = new MappedFile((input_path + "tiger.pdf"));
            long file_sz = file.fileSize();

            FilterReader file_reader = new FilterReader(file);

            byte[] mem = new byte[(int) file_sz];

            long bytes_read = file_reader.read(mem);
            PDFDoc doc = new PDFDoc(mem);

            doc.initSecurityHandler();
            int num_pages = doc.getPageCount();

            ElementWriter writer = new ElementWriter();
            ElementReader reader = new ElementReader();
            Element element;

            // Create a duplicate of every page but copy only path objects

            for (int i = 1; i <= num_pages; ++i) {
                PageIterator itr = doc.getPageIterator(2 * i - 1);
                Page current = itr.next();
                reader.begin(current);
                Page new_page = doc.pageCreate(current.getMediaBox());
                doc.pageInsert(itr, new_page);

                writer.begin(new_page);
                while ((element = reader.next()) != null)    // Read page contents
                {
                    //if (element.getType() == Element.e_path)
                    writer.writeElement(element);
                }

                writer.end();
                reader.end();
            }

            doc.save(output_path + "doc_memory_edit.pdf", SDFDoc.SaveMode.REMOVE_UNUSED, null);
            // output PDF doc

            // Save the document to a memory buffer.


            byte[] buf = doc.save(SDFDoc.SaveMode.REMOVE_UNUSED, null);
            // doc.Save(buf, buf_sz, Doc::e_linearized, NULL);

            // Write the contents of the buffer to the disk
            {
                File outfile = new File(output_path + "doc_memory_edit.txt");
                // output "doc_memory_edit.txt"
                FileOutputStream fop = new FileOutputStream(outfile);
                if (!outfile.exists()) {
                    outfile.createNewFile();
                }
                fop.write(buf);
                fop.flush();
                fop.close();
            }

            // Read some data from the file stored in memory
            reader.begin(doc.getPage(1));
            while ((element = reader.next()) != null) {
                if (element.getType() == Element.e_path) System.out.print("Path, ");
            }
            reader.end();

            doc.close();
            System.out.println("\n\nDone. Result saved in doc_memory_edit.pdf and doc_memory_edit.txt ...");
        } catch (PDFNetException e) {
            e.printStackTrace();
        } catch (FileNotFoundException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        //This sample illustrates how to open a PDF document
        //from an Java InputStream and how to save to an OutputStream.
        try {
            PDFDoc doc = new PDFDoc(new FileInputStream(input_path + "newsletter.pdf"));
            doc.save(new FileOutputStream(output_path + "StreamTest.pdf"), SDFDoc.SaveMode.NO_FLAGS, null);
            // output PDF doc
            doc.close();
            System.out.println("\n\nDone. Result saved in StreamTest.pdf ...");
        } catch (PDFNetException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (FileNotFoundException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        PDFNet.terminate();
    }
}
