//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.     
//---------------------------------------------------------------------------------------

using System;
using pdftron;
using pdftron.Common;
using pdftron.SDF;
using pdftron.PDF;

namespace OCRTestCS
{
    
    /// <summary>
    //---------------------------------------------------------------------------------------
    // The following sample illustrates how to use OCR module
    //---------------------------------------------------------------------------------------
    /// </summary>
    class Class1
    {
        private static pdftron.PDFNetLoader pdfNetLoader = pdftron.PDFNetLoader.Instance();
        static Class1() {}
        
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        static void Main(string[] args)
        {
            // The first step in every application using PDFNet is to initialize the 
            // library and set the path to common PDF resources. The library is usually 
            // initialized only once, but calling Initialize() multiple times is also fine.
            PDFNet.Initialize();

            // Can optionally set path to the OCR module
            PDFNet.AddResourceSearchPath("../../../Lib/");
            if( !OCRModule.IsModuleAvailable() )
            {
                Console.WriteLine("");
                Console.WriteLine("Unable to run OCRTest: PDFTron SDK OCR module not available.");
                Console.WriteLine("---------------------------------------------------------------");
                Console.WriteLine("The OCR module is an optional add-on, available for download");
                Console.WriteLine("at http://www.pdftron.com/. If you have already downloaded this");
                Console.WriteLine("module, ensure that the SDK is able to find the required files");
                Console.WriteLine("using the PDFNet.AddResourceSearchPath() function.");
                Console.WriteLine("");
                return;
            }

            // Relative path to the folder containing test files.
            string input_path =  "../../TestFiles/OCR/";
            string output_path = "../../TestFiles/Output/";

            //--------------------------------------------------------------------------------
            // Example 1) Process image
            try
            {

                // A) Setup empty destination doc
                using (PDFDoc doc = new PDFDoc())
                {
                    // B) Set English as the language of choice
                    OCROptions opts = new OCROptions();
                    opts.AddLang("eng");

                    // C) Run OCR on the .png with options            
                    OCRModule.ImageToPDF(doc, input_path + "psychomachia_excerpt.png", opts);

                    // D) check the result
                    doc.Save(output_path + "psychomachia_excerpt.pdf", SDFDoc.SaveOptions.e_remove_unused);

                    Console.WriteLine("Example 1: psychomachia_excerpt.png");
                }

            }
            catch (PDFNetException e)
            {
                Console.WriteLine(e.Message);
            }

            //--------------------------------------------------------------------------------
            // Example 2) Process document using multiple languages
            try
            {

                // A) Setup empty destination doc
                using (PDFDoc doc = new PDFDoc())
                {

                    // B) Setup options with multiple target languages, English will always be considered as secondary language
                    OCROptions opts = new OCROptions();
                    opts.AddLang("rus");
                    opts.AddLang("deu");

                    // C) Run OCR on the .jpg with options        
                    OCRModule.ImageToPDF(doc, input_path + "multi_lang.jpg", opts);

                    // D) check the result
                    doc.Save(output_path + "multi_lang.pdf", SDFDoc.SaveOptions.e_remove_unused);

                    Console.WriteLine("Example 2: multi_lang.jpg");
                }

            }
            catch (PDFNetException e)
            {
                Console.WriteLine(e.Message);
            }

            //--------------------------------------------------------------------------------
            // Example 3) Process a .pdf specifying a language - German - and ignore zone comprising a sidebar image 
            try
            {

                // A) Open the .pdf document
                using (PDFDoc doc = new PDFDoc(input_path + "german_kids_song.pdf"))
                {

                    // B) Setup options with a single language and an ignore zone
                    OCROptions opts = new OCROptions();
                    opts.AddLang("deu");

                    RectCollection ignoreZones = new RectCollection();
                    ignoreZones.AddRect(424, 163, 493, 730);
                    opts.AddIgnoreZonesForPage(ignoreZones, 1);

                    // C) Run OCR on the .pdf with options
                    OCRModule.ProcessPDF(doc, opts);

                    // D) check the result
                    doc.Save(output_path + "german_kids_song.pdf", SDFDoc.SaveOptions.e_remove_unused);

                    Console.WriteLine("Example 3: german_kids_song.pdf");
                }

            }
            catch (PDFNetException e)
            {
                Console.WriteLine(e.Message);
            }

            //--------------------------------------------------------------------------------
            // Example 4) Process multipage tiff with text/ignore zones specified for each page
            try
            {

                // A) Setup empty destination doc
                using (PDFDoc doc = new PDFDoc())
                {

                    // B) Setup options with a single language plus text/ignore zones
                    OCROptions opts = new OCROptions();
                    opts.AddLang("eng");

                    RectCollection zones = new RectCollection();


                    // ignore signature box in the first 2 pages
                    zones.AddRect(1492, 56, 2236, 432);
                    opts.AddIgnoreZonesForPage(zones, 1);
                    zones.Clear();

                    zones.AddRect(1492, 56, 2236, 432);
                    opts.AddIgnoreZonesForPage(zones, 2);
                    zones.Clear();

                    // can use a combination of ignore and text boxes to focus on the page area of interest,
                    // as ignore boxes are applied first, we remove the arrows before selecting part of the diagram
                    zones.AddRect(992, 1276, 1368, 1372);
                    opts.AddIgnoreZonesForPage(zones, 3);
                    zones.Clear();

                    // select horizontal BUFFER ZONE sign
                    zones.AddRect(900, 2384, 1236, 2480);
                    // select right vertical BUFFER ZONE sign
                    zones.AddRect(1960, 1976, 2016, 2296);
                    // select Lot No.
                    zones.AddRect(696, 1028, 1196, 1128);

                    // select part of the plan inside the BUFFER ZONE
                    zones.AddRect(428, 1484, 1784, 2344);
                    zones.AddRect(948, 1288, 1672, 1476);
                    opts.AddTextZonesForPage(zones, 3);

                    // C) Run OCR on the .pdf with options
                    OCRModule.ImageToPDF(doc, input_path + "bc_environment_protection.tif", opts);

                    // D) check the result
                    doc.Save(output_path + "bc_environment_protection.pdf", SDFDoc.SaveOptions.e_remove_unused);

                    Console.WriteLine("Example 4: bc_environment_protection.tif");
                }

            }
            catch (PDFNetException e)
            {
                Console.WriteLine(e.Message);
            }

            //--------------------------------------------------------------------------------
            // Example 5) Alternative workflow for extracting OCR result JSON, postprocessing (e.g., removing words not in the dictionary or filtering special
            // out special characters), and finally applying modified OCR JSON to the source PDF document 
            try
            {

                // A) Open the .pdf document
                using (PDFDoc doc = new PDFDoc(input_path + "zero_value_test_no_text.pdf"))
                {

                    // B) set English language
                    OCROptions opts = new OCROptions();
                    opts.AddLang("eng");


                    // C) Run OCR on the .pdf 
                    string json = OCRModule.GetOCRJsonFromPDF(doc, opts);

                    // D) Post-processing step (whatever it might be), but we just print JSON here
                    Console.WriteLine("Have OCR result JSON, re-applying to PDF");

                    // E) Apply potentially modified OCR JSON to the PDF
                    OCRModule.ApplyOCRJsonToPDF(doc, json);

                    // F) check the result
                    doc.Save(output_path + "zero_value_test_no_text.pdf", SDFDoc.SaveOptions.e_remove_unused);

                    Console.WriteLine("Example 5: extracting and applying OCR JSON from zero_value_test_no_text.pdf");
                }

            }
            catch (PDFNetException e)
            {
                Console.WriteLine(e.Message);
            }

            //--------------------------------------------------------------------------------
            // Example 6) The postprocessing workflow has also an option of extracting OCR results in XML format, similar to the one used by TextExtractor
            try
            {

                // A) Setup empty destination doc
                using (PDFDoc doc = new PDFDoc())
                {

                    // B) set English language
                    OCROptions opts = new OCROptions();
                    opts.AddLang("eng");

                    // C) Run OCR on the .tif with default English language, extracting OCR results in XML format. Note that
                    // in the process we convert the source image into PDF. We reuse this PDF document later to add hidden text layer to it.

                    string xml = OCRModule.GetOCRXmlFromImage(doc, input_path + "physics.tif", opts);

                    // D) Post-processing step (whatever it might be), but we just print XML here
                    Console.WriteLine("Have OCR result XML, re-applying to PDF");

                    // E) Apply potentially modified OCR XML to the PDF
                    OCRModule.ApplyOCRXmlToPDF(doc, xml);

                    // F) check the result
                    doc.Save(output_path + "physics.pdf", SDFDoc.SaveOptions.e_remove_unused);

                    Console.WriteLine("Example 6: extracting and applying OCR XML from physics.tif");
                }

            }
            catch (PDFNetException e)
            {
                Console.WriteLine(e.Message);
            }

            //--------------------------------------------------------------------------------
            // Example 7) Resolution can be manually set, when DPI missing from metadata or is wrong
            try
            {

                // A) Setup empty destination doc
                using (PDFDoc doc = new PDFDoc())
                {

                    // B) Setup options with a text zone                    
                    OCROptions opts = new OCROptions();
                    RectCollection zones = new RectCollection();
                    zones.AddRect(140, 870, 310, 920);
                    opts.AddTextZonesForPage(zones, 1);

                    // C) Manually override DPI
                    opts.AddDPI(100);

                    // D) Run OCR on the .jpg with options
                    OCRModule.ImageToPDF(doc, input_path + "corrupted_dpi.jpg", opts);

                    // E) Check the result
                    doc.Save(output_path + "corrupted_dpi.pdf", SDFDoc.SaveOptions.e_remove_unused);

                    Console.WriteLine("Example 7: converting image with corrupted resolution metadata corrupted_dpi.jpg to pdf with searchable text");
                }

            }
            catch (PDFNetException e)
            {
                Console.WriteLine(e.Message);
            }

        }

    }
}
