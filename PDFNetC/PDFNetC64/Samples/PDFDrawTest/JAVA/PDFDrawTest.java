//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

import java.awt.image.PixelGrabber;

//import com.pdftron.filters.FilterWriter;
//import com.pdftron.filters.MappedFile;
import com.pdftron.pdf.*;
import com.pdftron.sdf.Obj;
import com.pdftron.sdf.ObjSet;
import com.pdftron.common.Matrix2D;
import com.pdftron.common.PDFNetException;

import java.io.FileOutputStream;
import java.io.File;
import java.nio.ByteBuffer;
import java.nio.IntBuffer;

//---------------------------------------------------------------------------------------
// The following sample illustrates how to convert PDF documents to various raster image 
// formats (such as PNG, JPEG, BMP, TIFF, etc), as well as how to convert a PDF page to 
// GDI+ Bitmap for further manipulation and/or display in WinForms applications.
//---------------------------------------------------------------------------------------
public class PDFDrawTest {
    public static void main(String[] args) {
        try {
            // The first step in every application using PDFNet is to initialize the
            // library and set the path to common PDF resources. The library is usually
            // initialized only once, but calling Initialize() multiple times is also fine.
            PDFNet.initialize();

            // Optional: Set ICC color profiles to fine tune color conversion
            // for PDF 'device' color spaces...

            //PDFNet.setResourcesPath("../../../resources");
            //PDFNet.setColorManagement();
            //PDFNet.setDefaultDeviceCMYKProfile("D:/Misc/ICC/USWebCoatedSWOP.icc");
            //PDFNet.setDefaultDeviceRGBProfile("AdobeRGB1998.icc"); // will search in PDFNet resource folder.

            // ----------------------------------------------------
            // Optional: Set predefined font mappings to override default font
            // substitution for documents with missing fonts...

            // PDFNet.addFontSubst("StoneSans-Semibold", "C:/WINDOWS/Fonts/comic.ttf");
            // PDFNet.addFontSubst("StoneSans", "comic.ttf");  // search for 'comic.ttf' in PDFNet resource folder.
            // PDFNet.addFontSubst(PDFNet.e_Identity, "C:/WINDOWS/Fonts/arialuni.ttf");
            // PDFNet.addFontSubst(PDFNet.e_Japan1, "C:/Program Files/Adobe/Acrobat 7.0/Resource/CIDFont/KozMinProVI-Regular.otf");
            // PDFNet.addFontSubst(PDFNet.e_Japan2, "c:/myfonts/KozMinProVI-Regular.otf");
            // PDFNet.addFontSubst(PDFNet.e_Korea1, "AdobeMyungjoStd-Medium.otf");
            // PDFNet.addFontSubst(PDFNet.e_CNS1, "AdobeSongStd-Light.otf");
            // PDFNet.addFontSubst(PDFNet.e_GB1, "AdobeMingStd-Light.otf");

            // Relative path to the folder containing test files.
            String input_path = "../../TestFiles/";
            String output_path = "../../TestFiles/Output/";

            PDFDraw draw = new PDFDraw();  // PDFDraw class is used to rasterize PDF pages.
            ObjSet hint_set = new ObjSet();

            //--------------------------------------------------------------------------------
            // Example 1) Convert the first page to PNG and TIFF at 92 DPI.
            // A three step tutorial to convert PDF page to an image.
            try {
                // A) Open the PDF document.
                PDFDoc doc = new PDFDoc((input_path + "tiger.pdf"));

                // Initialize the security handler, in case the PDF is encrypted.
                doc.initSecurityHandler();

                // B) The output resolution is set to 92 DPI.
                draw.setDPI(92);

                // C) Rasterize the first page in the document and save the result as PNG.
                Page pg = doc.getPage(1);
                draw.export(pg, (output_path + "tiger_92dpi.png"));
                // output "tiger_92dpi.png"

                System.out.println("Example 1: tiger_92dpi.png");

                // Export the same page as TIFF
                draw.export(pg, (output_path + "tiger_92dpi.tif"), "TIFF");
                // output "tiger_92dpi.tif"
                doc.close();
            } catch (Exception e) {
                e.printStackTrace();
            }

            //--------------------------------------------------------------------------------
            // Example 2) Convert the all pages in a given document to JPEG at 72 DPI.
            try {
                System.out.println("Example 2:");
                PDFDoc doc = new PDFDoc((input_path + "newsletter.pdf"));
                // Initialize the security handler, in case the PDF is encrypted.
                doc.initSecurityHandler();

                draw.setDPI(72); // Set the output resolution is to 72 DPI.

                // Use optional encoder parameter to specify JPEG quality.
                Obj encoder_param = hint_set.createDict();
                encoder_param.putNumber("Quality", 80);

                // Traverse all pages in the document.
                for (PageIterator itr = doc.getPageIterator(); itr.hasNext(); ) {
                    Page current = itr.next();
                    String filename = "newsletter" + current.getIndex() + ".jpg";
                    System.out.println(filename);
                    draw.export(current, output_path + filename, "JPEG", encoder_param);
                }

                doc.close();
                System.out.println("Done.");
            } catch (Exception e) {
                e.printStackTrace();
            }

            // Examples 3-5
            try {
                // Common code for remaining samples.
                PDFDoc tiger_doc = new PDFDoc((input_path + "tiger.pdf"));
                // Initialize the security handler, in case the PDF is encrypted.
                tiger_doc.initSecurityHandler();
                Page page = tiger_doc.getPageIterator().next();

                //--------------------------------------------------------------------------------
                // Example 3) Convert the first page to raw bitmap. Also, rotate the
                // page 90 degrees and save the result as RAW.
                draw.setDPI(100); // Set the output resolution is to 100 DPI.
                draw.setRotate(Page.e_90);  // Rotate all pages 90 degrees clockwise.

                // create a Java image
                java.awt.Image image = draw.getBitmap(page);

                //
                int width = image.getWidth(null), height = image.getHeight(null);
                int[] arr = new int[width * height];
                PixelGrabber pg = new PixelGrabber(image, 0, 0, width, height, arr, 0, width);
                pg.grabPixels();

                // convert to byte array
                ByteBuffer byteBuffer = ByteBuffer.allocate(arr.length * 4);
                IntBuffer intBuffer = byteBuffer.asIntBuffer();
                intBuffer.put(arr);
                byte[] rawByteArray = byteBuffer.array();
                // finally write the file
                FileOutputStream fos = new FileOutputStream(output_path + "tiger_100dpi_rot90.raw");
                fos.write(rawByteArray);
                System.out.println("Example 3: tiger_100dpi_rot90.raw");

                draw.setRotate(Page.e_0);  // Disable image rotation for remaining samples.

                //--------------------------------------------------------------------------------
                // Example 4) Convert PDF page to a fixed image size. Also illustrates some
                // other features in PDFDraw class such as rotation, image stretching, exporting
                // to grayscale, or monochrome.

                // Initialize render 'gray_hint' parameter, that is used to control the
                // rendering process. In this case we tell the rasterizer to export the image as
                // 1 Bit Per Component (BPC) image.
                Obj mono_hint = hint_set.createDict();
                mono_hint.putNumber("BPC", 1);

                // SetImageSize can be used instead of SetDPI() to adjust page  scaling
                // dynamically so that given image fits into a buffer of given dimensions.
                draw.setImageSize(1000, 1000);        // Set the output image to be 1000 wide and 1000 pixels tall

                draw.export(page, (output_path + "tiger_1000x1000.png"), "PNG", mono_hint);
                System.out.println("Example 4: tiger_1000x1000.png");

                draw.setImageSize(200, 400); // Set the output image to be 200 wide and 300 pixels tall
                draw.setRotate(Page.e_180); // Rotate all pages 90 degrees clockwise.

                // 'gray_hint' tells the rasterizer to export the image as grayscale.
                Obj gray_hint = hint_set.createDict();
                gray_hint.putName("ColorSpace", "Gray");

                draw.export(page, (output_path + "tiger_200x400_rot180.png"), "PNG", gray_hint);
                System.out.println("Example 4: tiger_200x400_rot180.png");

                draw.setImageSize(400, 200, false);  // The third parameter sets 'preserve-aspect-ratio' to false.
                draw.setRotate(Page.e_0);    // Disable image rotation.
                draw.export(page, (output_path + "tiger_400x200_stretch.jpg"), "JPEG");
                // output "tiger_400x200_stretch.jpg"
                System.out.println("Example 4: tiger_400x200_stretch.jpg");

                //--------------------------------------------------------------------------------
                // Example 5) Zoom into a specific region of the page and rasterize the
                // area at 200 DPI and as a thumbnail (i.e. a 50x50 pixel image).
                Rect zoom_rect = new Rect(216, 522, 330, 600);
                page.setCropBox(zoom_rect);    // Set the page crop box.

                // Select the crop region to be used for drawing.
                draw.setPageBox(Page.e_crop);
                draw.setDPI(900);  // Set the output image resolution to 900 DPI.
                draw.export(page, (output_path + "tiger_zoom_900dpi.png"), "PNG");
                // output "tiger_zoom_900dpi.png"
                System.out.println("Example 5: tiger_zoom_900dpi.png");

                // -------------------------------------------------------------------------------
                // Example 6)
                draw.setImageSize(50, 50);       // Set the thumbnail to be 50x50 pixel image.
                draw.export(page, (output_path + "tiger_zoom_50x50.png"), "PNG");
                // output "tiger_zoom_50x50.png"
                System.out.println("Example 6: tiger_zoom_50x50.png");

                tiger_doc.close();
            } catch (Exception e) {
                e.printStackTrace();
            }

            Obj cmyk_hint = hint_set.createDict();
            cmyk_hint.putName("ColorSpace", "CMYK");

            //--------------------------------------------------------------------------------
            // Example 7) Convert the first PDF page to CMYK TIFF at 92 DPI.
            // A three step tutorial to convert PDF page to an image
            try {
                // A) Open the PDF document.
                PDFDoc doc = new PDFDoc(input_path + "tiger.pdf");
                // Initialize the security handler, in case the PDF is encrypted.
                doc.initSecurityHandler();

                // B) The output resolution is set to 92 DPI.
                draw.setDPI(92);

                // C) Rasterize the first page in the document and save the result as TIFF.
                Page pg = doc.getPage(1);
                draw.export(pg, output_path + "out1.tif", "TIFF", cmyk_hint);
                // output "out1.tif"
                System.out.println("Example 7: out1.tif");
                doc.close();
            } catch (Exception e) {
                e.printStackTrace();
            }

            //--------------------------------------------------------------------------------
            // Example 8) PDFRasterizer can be used for more complex rendering tasks, such as 
            // strip by strip or tiled document rendering. In particular, it is useful for 
            // cases where you cannot simply modify the page crop box (interactive viewing,
            // parallel rendering).  This example shows how you can rasterize the south-west
            // quadrant of a page.
            try {
                // A) Open the PDF document.
                PDFDoc doc = new PDFDoc(input_path + "tiger.pdf");
                // Initialize the security handler, in case the PDF is encrypted.
                doc.initSecurityHandler();

                // B) Get the page matrix 
                Page pg = doc.getPage(1);
                int box = Page.e_crop;
                Matrix2D mtx = pg.getDefaultMatrix(true, box, 0);
                // We want to render a quadrant, so use half of width and height
                double pg_w = pg.getPageWidth(box) / 2;
                double pg_h = pg.getPageHeight(box) / 2;

                // C) Scale matrix from PDF space to buffer space
                double dpi = 96.0;
                double scale = dpi / 72.0; // PDF space is 72 dpi
                double buf_w = Math.floor(scale * pg_w);
                double buf_h = Math.floor(scale * pg_h);
                int bytes_per_pixel = 4; // BGRA buffer
                mtx.translate(0, -pg_h); // translate by '-pg_h' since we want south-west quadrant
                mtx = (new Matrix2D(scale, 0, 0, scale, 0, 0)).multiply(mtx);

                // D) Rasterize page into memory buffer, according to our parameters
                PDFRasterizer rast = new PDFRasterizer();
                byte[] buf = rast.rasterize(pg, (int) buf_w, (int) buf_h, (int) buf_w * bytes_per_pixel, bytes_per_pixel, true, mtx, null);

                System.out.println("Example 8: Successfully rasterized into memory buffer.");
                doc.close();
            } catch (Exception e) {
                e.printStackTrace();
            }

            //--------------------------------------------------------------------------------
            // Example 9) Export raster content to PNG using different image smoothing settings.
            try {
                PDFDoc text_doc = new PDFDoc(input_path + "lorem_ipsum.pdf");
                text_doc.initSecurityHandler();

                draw.setImageSmoothing(false, false);
                String filename = "raster_text_no_smoothing.png";
                draw.export(text_doc.getPageIterator().next(), output_path + filename);
                System.out.println("Example 9 a): " + filename + ". Done.");

                filename = "raster_text_smoothed.png";
                draw.setImageSmoothing(true, false /*default quality bilinear resampling*/);
                draw.export(text_doc.getPageIterator().next(), output_path + filename);
                System.out.println("Example 9 b): " + filename + ". Done.");

                filename = "raster_text_high_quality.png";
                draw.setImageSmoothing(true, true /*high quality area resampling*/);
                draw.export(text_doc.getPageIterator().next(), output_path + filename);
                System.out.println("Example 9 c): " + filename + ". Done.");
            } catch (Exception e) {
                e.printStackTrace();
            }


            //--------------------------------------------------------------------------------
            // Example 10) Export separations directly, without conversion to an output colorspace
            try {
                PDFDoc separation_doc = new PDFDoc(input_path + "op_blend_test.pdf");
                separation_doc.initSecurityHandler();

                Obj separation_hint = hint_set.createDict();
                separation_hint.putName("ColorSpace", "Separation");
                draw.setDPI(96);
                draw.setImageSmoothing(true, true);
                // set overprint preview to always on
                draw.setOverprint(1);

                String filename = new String("merged_separations.png");
                draw.export(separation_doc.getPage(1), output_path + filename, "PNG");
                System.out.println("Example 10 a): " + filename + ". Done.");

                filename = new String("separation");
                draw.export(separation_doc.getPage(1), output_path + filename, "PNG", separation_hint);
                System.out.println("Example 10 b): " + filename + "_[ink].png. Done.");

                filename = new String("separation_NChannel.tif");
                draw.export(separation_doc.getPage(1), output_path + filename, "TIFF", separation_hint);
                System.out.println("Example 10 c): " + filename + ". Done.");
            } catch (Exception e) {
                e.printStackTrace();
            }

            // Calling Terminate when PDFNet is no longer in use is a good practice, but
            // is not required.
            PDFNet.terminate();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}