//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.     
//---------------------------------------------------------------------------------------

using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;

using pdftron;
using pdftron.Common;
using pdftron.PDF;
using pdftron.SDF;

namespace CAD2PDFTestCS
{
    /// <summary>
    //---------------------------------------------------------------------------------------
    // The following sample illustrates how to convert CAD documents (such as dwg, dgn, rvt, 
    // dxf, dwf) to pdf 
    //---------------------------------------------------------------------------------------
    /// </summary>
    class Class1
	{
		private static pdftron.PDFNetLoader pdfNetLoader = pdftron.PDFNetLoader.Instance();
		static Class1() {}

        /// <summary>
        /// Check file extension
        /// </summary>
        static bool IsRVTFile(string input_file_name)
        {
            bool rvt_input = false;
            if (input_file_name.Length > 2)
            {
                if (input_file_name.Substring(input_file_name.Length - 3, 3) == "rvt")
                {
                    rvt_input = true;
                }
            }
            return rvt_input;
        }

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        static void Main(string[] args)
		{
			// The first step in every application using PDFNet is to initialize the 
			// library and set the path to common PDF resources. The library is usually 
			// initialized only once, but calling Initialize() multiple times is also fine.
			PDFNet.Initialize();
            PDFNet.AddResourceSearchPath("../../../Lib/");
            if (!CADModule.IsModuleAvailable())
            {
                Console.WriteLine();
                Console.WriteLine("Unable to run CAD2PDFTest: PDFTron SDK CAD module not available.");
                Console.WriteLine("---------------------------------------------------------------");
                Console.WriteLine("The CAD module is an optional add-on, available for download");
                Console.WriteLine("at http://www.pdftron.com/. If you have already downloaded this");
                Console.WriteLine("module, ensure that the SDK is able to find the required files");
                Console.WriteLine("using the PDFNet::AddResourceSearchPath() function.");
                Console.WriteLine();
            }

            // Relative path to the folder containing test files.
            string input_path =  "../../TestFiles/CAD/";
			string output_path = "../../TestFiles/Output/";

            string input_file_name = "construction drawings color-28.05.18.dwg";
            string output_file_name = "construction drawings color-28.05.18.pdf";

            if (args.Length != 0)
            {
                input_file_name = args[0];
                output_file_name = input_file_name + ".pdf";
            }

            Console.WriteLine("Example cad:");
            try
            {
                using (PDFDoc pdfdoc = new PDFDoc())
                {
                    if (IsRVTFile(input_file_name))
                    {
                        CADConvertOptions opts = new CADConvertOptions();
                        opts.SetPageWidth(800);
                        opts.SetPageHeight(600);
                        opts.SetRasterDPI(150);

                        pdftron.PDF.Convert.FromCAD(pdfdoc, input_path + input_file_name, opts);
                    }
                    else
                    {
                        pdftron.PDF.Convert.FromCAD(pdfdoc, input_path + input_file_name, null);
                    }
                    pdfdoc.Save(output_path + output_file_name, SDFDoc.SaveOptions.e_remove_unused);
                }

                Console.WriteLine("Done.");
            }
            catch (PDFNetException e)
            {
                Console.WriteLine(e.Message);
            }
        }
    }
}
