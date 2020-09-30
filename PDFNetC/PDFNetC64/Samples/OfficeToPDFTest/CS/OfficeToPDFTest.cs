//
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
//

using System;
using System.Drawing;
using System.Drawing.Drawing2D;

using pdftron;
using pdftron.Common;
using pdftron.Filters;
using pdftron.SDF;
using pdftron.PDF;

namespace OfficeToPDFTestCS
{
    /// <summary>
    ///---------------------------------------------------------------------------------------
    /// The following sample illustrates how to use the PDF::Convert utility class to convert 
    /// .docx files to PDF
    ///
    /// This conversion is performed entirely within the PDFNet and has *no* external or
    /// system dependencies dependencies
    ///
    /// Please contact us if you have any questions.    
    ///---------------------------------------------------------------------------------------
    /// </summary>



    class Class1
    {
        private static pdftron.PDFNetLoader pdfNetLoader = pdftron.PDFNetLoader.Instance();
        static Class1() {}

        static String input_path = "../../TestFiles/";
        static String output_path = "../../TestFiles/Output/";

        static void SimpleConvert(String input_filename, String output_filename)
        {
            // Start with a PDFDoc (the conversion destination)
            using (PDFDoc pdfdoc = new PDFDoc())
            {
                // perform the conversion with no optional parameters
                pdftron.PDF.Convert.OfficeToPDF(pdfdoc, input_path + input_filename, null);

                // save the result
                pdfdoc.Save(output_path + output_filename, SDFDoc.SaveOptions.e_linearized);

                // And we're done!
                Console.WriteLine("Saved " + output_filename);
            }   
        }

        static void FlexibleConvert(String input_filename, String output_filename)
        {
            // Start with a PDFDoc (the conversion destination)
            using (PDFDoc pdfdoc = new PDFDoc())
            {
                OfficeToPDFOptions options = new OfficeToPDFOptions();
                options.SetSmartSubstitutionPluginPath(input_path);
                // create a conversion object -- this sets things up but does not yet
                // perform any conversion logic.
                // in a multithreaded environment, this object can be used to monitor
                // the conversion progress and potentially cancel it as well
                DocumentConversion conversion = pdftron.PDF.Convert.StreamingPDFConversion(
                    pdfdoc, input_path + input_filename, options);

                // actually perform the conversion
                // this particular method will not throw on conversion failure, but will
                // return an error status instead
                if (conversion.TryConvert() == DocumentConversionResult.e_document_conversion_success)
                {
                    int num_warnings = conversion.GetNumWarnings();

                    // print information about the conversion 
                    for (int i = 0; i < num_warnings; ++i)
                    {
                        Console.WriteLine("Warning: " + conversion.GetWarningString(i));
                    }

                    // save the result
                    pdfdoc.Save(output_path + output_filename, SDFDoc.SaveOptions.e_linearized);
                    // done
                    Console.WriteLine("Saved " + output_filename);
                }
                else
                {
                    Console.WriteLine("Encountered an error during conversion: " + conversion.GetErrorString());
                }
            }
        }

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        static void Main(string[] args)
        {
            PDFNet.Initialize();

            try
            {
                // first the one-line conversion method
                SimpleConvert("Fishermen.docx", "Fishermen.pdf");

                // then the more flexible line-by-line conversion API
                FlexibleConvert("the_rime_of_the_ancient_mariner.docx", "the_rime_of_the_ancient_mariner.pdf");

                // conversion of RTL content
                FlexibleConvert("factsheet_Arabic.docx", "factsheet_Arabic.pdf");
            }
            catch (pdftron.Common.PDFNetException e)
            {
                Console.WriteLine(e.Message);
            }
            catch (Exception e)
            {
                Console.WriteLine("Unrecognized Exception: " + e.Message );
            }

            PDFNet.Terminate();
            Console.WriteLine("Done.");
        }
    }
}
