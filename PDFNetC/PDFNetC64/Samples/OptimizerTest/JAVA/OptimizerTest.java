//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

import com.pdftron.pdf.*;
import com.pdftron.sdf.SDFDoc;

public class OptimizerTest {
    //---------------------------------------------------------------------------------------
    // The following sample illustrates how to reduce PDF file size using 'pdftron.PDF.Optimizer'.
    // The sample also shows how to simplify and optimize PDF documents for viewing on mobile devices
    // and on the Web using 'pdftron.PDF.Flattener'.
    //
    // @note Both 'Optimizer' and 'Flattener' are separately licensable add-on options to the core PDFNet license.
    //
    // ----
    //
    // 'pdftron.PDF.Optimizer' can be used to optimize PDF documents by reducing the file size, removing
    // redundant information, and compressing data streams using the latest in image compression technology.
    //
    // PDF Optimizer can compress and shrink PDF file size with the following operations:
    // - Remove duplicated fonts, images, ICC profiles, and any other data stream.
    // - Optionally convert high-quality or print-ready PDF files to small, efficient and web-ready PDF.
    // - Optionally down-sample large images to a given resolution.
    // - Optionally compress or recompress PDF images using JBIG2 and JPEG2000 compression formats.
    // - Compress uncompressed streams and remove unused PDF objects.
    //
    // 'pdftron.PDF.Flattener' can be used to speed-up PDF rendering on mobile devices and on the Web by
    // simplifying page content (e.g. flattening complex graphics into images) while maintaining vector text
    // whenever possible.
    //
    // Flattener can also be used to simplify process of writing custom converters from PDF to other formats.
    // In this case, Flattener can be used as first step in the conversion pipeline to reduce any PDF to a
    // very simple representation (e.g. vector text on top of a background image).
    //---------------------------------------------------------------------------------------
    public static void main(String[] args) {
        String input_path = "../../TestFiles/";
        String output_path = "../../TestFiles/Output/";
        String input_filename = "newsletter.pdf";
        String input_filename2 = "newsletter_opt1.pdf";
        String input_filename3 = "newsletter_opt2.pdf";
        String input_filename4 = "newsletter_opt3.pdf";
        String input_filename5 = "newsletter_SaveViewerOptimized.pdf";

        PDFNet.initialize();

        //--------------------------------------------------------------------------------
        // Example 1) Optimize a PDF.
        try {
            PDFDoc doc = new PDFDoc(input_path + input_filename);
            doc.initSecurityHandler();
            Optimizer.optimize(doc);
            doc.save(output_path + input_filename2, SDFDoc.SaveMode.LINEARIZED, null);
            // output PDF doc
            doc.close();
        } catch (Exception e) {
            e.printStackTrace();
            return;
        }

        //--------------------------------------------------------------------------------
        // Example 2) Reduce image quality and use jpeg compression for
        // non monochrome images.
        try {
            PDFDoc doc = new PDFDoc(input_path + input_filename);
            doc.initSecurityHandler();

            Optimizer.ImageSettings image_settings = new Optimizer.ImageSettings();
            ;

            // low quality jpeg compression
            image_settings.setCompressionMode(Optimizer.ImageSettings.e_jpeg);
            image_settings.setQuality(1);

            // Set the output dpi to be standard screen resolution
            image_settings.setImageDPI(144, 96);

            // this option will recompress images not compressed with
            // jpeg compression and use the result if the new image
            // is smaller.
            image_settings.forceRecompression(true);


            // this option is not commonly used since it can
            // potentially lead to larger files.  It should be enabled
            // only if the output compression specified should be applied
            // to every image of a given type regardless of the output image size
            //image_settings.forceChanges(true);

            Optimizer.OptimizerSettings opt_settings = new Optimizer.OptimizerSettings();
            opt_settings.setColorImageSettings(image_settings);
            opt_settings.setGrayscaleImageSettings(image_settings);


            Optimizer.optimize(doc, opt_settings);

            doc.save(output_path + input_filename3, SDFDoc.SaveMode.LINEARIZED, null);
            // output PDF doc
            doc.close();
        } catch (Exception e) {
            e.printStackTrace();
            return;
        }

        //--------------------------------------------------------------------------------
        // Example 3) Use monochrome image settings and default settings
        // for color and grayscale images.
        try {
            PDFDoc doc = new PDFDoc(input_path + input_filename);
            doc.initSecurityHandler();

            Optimizer.MonoImageSettings mono_image_settings = new Optimizer.MonoImageSettings();
            mono_image_settings.setCompressionMode(Optimizer.MonoImageSettings.e_jbig2);
            mono_image_settings.forceRecompression(true);
            Optimizer.OptimizerSettings opt_settings = new Optimizer.OptimizerSettings();
            opt_settings.setMonoImageSettings(mono_image_settings);

            Optimizer.optimize(doc, opt_settings);

            doc.save(output_path + input_filename4, SDFDoc.SaveMode.LINEARIZED, null);
            // output PDF doc
            doc.close();
        } catch (Exception e) {
            e.printStackTrace();
            return;
        }

        // ----------------------------------------------------------------------
        // Example 4) Use Flattener to simplify content in this document
        // using default settings
        try {
            PDFDoc doc = new PDFDoc(input_path + "TigerText.pdf");
            doc.initSecurityHandler();

            Flattener fl = new Flattener();

            // The following lines can increase the resolution of background
            // images.
            //fl.setDPI(300);
            //fl.setMaximumImagePixels(5000000);

            // This line can be used to output Flate compressed background
            // images rather than DCTDecode compressed images which is the default
            //fl.setPreferJPG(false);

            // In order to adjust thresholds for when text is Flattened
            // the following function can be used.
            //fl.setThreshold(Flattener.e_keep_most);

            // We use e_fast option here since it is usually preferable
            // to avoid Flattening simple pages in terms of size and
            // rendering speed. If the desire is to simplify the
            // document for processing such that it contains only text and
            // a background image e_simple should be used instead.
            fl.Process(doc, Flattener.e_fast);

            doc.save(output_path + "TigerText_flatten.pdf", SDFDoc.SaveMode.LINEARIZED, null);
            // output PDF doc
            doc.close();
        } catch (Exception e) {
            e.printStackTrace();
            return;
        }
        
        // ----------------------------------------------------------------------
        // Example 5) Optimize a PDF for viewing using SaveViewerOptimized.
        try {
            PDFDoc doc = new PDFDoc(input_path + input_filename);
            doc.initSecurityHandler();

            ViewerOptimizedOptions opts = new ViewerOptimizedOptions();

            // set the maximum dimension (width or height) that thumbnails will have.
            opts.setThumbnailSize(1500);

            // set thumbnail rendering threshold. A number from 0 (include all thumbnails) to 100 (include only the first thumbnail) 
            // representing the complexity at which SaveViewerOptimized would include the thumbnail. 
            // By default it only produces thumbnails on the first and complex pages. 
            // The following line will produce thumbnails on every page.
            // opts.setThumbnailRenderingThreshold(0); 

            doc.saveViewerOptimized(output_path + input_filename5 , opts);
            // output PDF doc
            doc.close();
        } catch (Exception e) {
            e.printStackTrace();
            return;
        }
        PDFNet.terminate();
    }
}
