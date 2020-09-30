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

namespace PDFDrawTestCS
{
	/// <summary>
	//---------------------------------------------------------------------------------------
	// The following sample illustrates how to convert PDF documents to various raster image 
	// formats (such as PNG, JPEG, BMP, TIFF), as well as how to convert a PDF page to GDI+ Bitmap 
	// for further manipulation and/or display in WinForms applications.
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

			try 
			{
				// Optional: Set ICC color profiles to fine tune color conversion 
				// for PDF 'device' color spaces. You can use your own ICC profiles. 
				// Standard Adobe color profiles can be download from Adobes site: 
				// http://www.adobe.com/support/downloads/iccprofiles/iccprofiles_win.html
				//
				// Simply drop all *.icc files in PDFNet resource folder or you specify 
				// the full pathname.
				//---
				// PDFNet.SetResourcesPath("../../../../../resources");
				// PDFNet.SetColorManagement();
				// PDFNet.SetDefaultDeviceCMYKProfile("USWebCoatedSWOP.icc"); // will search in PDFNet resource folder.
				// PDFNet.SetDefaultDeviceRGBProfile("AdobeRGB1998.icc"); 

				// Optional: Set predefined font mappings to override default font 
				// substitution for documents with missing fonts. For example:
				//---
				// PDFNet.AddFontSubst("StoneSans-Semibold", "C:/WINDOWS/Fonts/comic.ttf");
				// PDFNet.AddFontSubst("StoneSans", "comic.ttf");  // search for 'comic.ttf' in PDFNet resource folder.
				// PDFNet.AddFontSubst(PDFNet.CharacterOrdering.e_Identity, "C:/WINDOWS/Fonts/arialuni.ttf");
				// PDFNet.AddFontSubst(PDFNet.CharacterOrdering.e_Japan1, "C:/Program Files/Adobe/Acrobat 7.0/Resource/CIDFont/KozMinProVI-Regular.otf");
				// PDFNet.AddFontSubst(PDFNet.CharacterOrdering.e_Japan2, "c:/myfonts/KozMinProVI-Regular.otf");
				//
				// If fonts are in PDFNet resource folder, it is not necessary to specify 
				// the full path name. For example,
				//---
				// PDFNet.AddFontSubst(PDFNet.CharacterOrdering.e_Korea1, "AdobeMyungjoStd-Medium.otf");
				// PDFNet.AddFontSubst(PDFNet.CharacterOrdering.e_CNS1, "AdobeSongStd-Light.otf");
				// PDFNet.AddFontSubst(PDFNet.CharacterOrdering.e_GB1, "AdobeMingStd-Light.otf");
			}
			catch (Exception)
			{
				Console.WriteLine("The specified color profile was not found.");
			}

			// Relative path to the folder containing test files.
			string input_path =  "../../TestFiles/";
			string output_path = "../../TestFiles/Output/";

			
			using (PDFDraw draw = new PDFDraw()) 
			{
				//--------------------------------------------------------------------------------
				// Example 1) Convert the first PDF page to PNG at 92 DPI. 
				// A three step tutorial to convert PDF page to an image.
				try  
				{
					// A) Open the PDF document.
					using (PDFDoc doc = new PDFDoc(input_path + "tiger.pdf")) 
					{
						// Initialize the security handler, in case the PDF is encrypted.
						doc.InitSecurityHandler();  

						// B) The output resolution is set to 92 DPI.
						draw.SetDPI(92);

						// C) Rasterize the first page in the document and save the result as PNG.
						Page pg = doc.GetPage(1);
						draw.Export(pg, output_path + "tiger_92dpi.png");

						Console.WriteLine("Example 1: tiger_92dpi.png");
						
						// Export the same page as TIFF
						draw.Export(pg, output_path + "tiger_92dpi.tif", "TIFF");
					}
				}
				catch (PDFNetException e) {
					Console.WriteLine(e.Message);
				}

				//--------------------------------------------------------------------------------
				// Example 2) Convert the all pages in a given document to JPEG at 72 DPI.
				ObjSet hint_set=new ObjSet(); // A collection of rendering 'hits'.
				Console.WriteLine("Example 2:");
				try  
				{
					using (PDFDoc doc = new PDFDoc(input_path + "newsletter.pdf")) 
					{
						// Initialize the security handler, in case the PDF is encrypted.
						doc.InitSecurityHandler();  
						
						draw.SetDPI(72); // Set the output resolution is to 72 DPI.
						
						// Use optional encoder parameter to specify JPEG quality.
						Obj encoder_param = hint_set.CreateDict();
						encoder_param.PutNumber("Quality", 80);
						
						// Traverse all pages in the document.
						for (PageIterator itr=doc.GetPageIterator(); itr.HasNext(); itr.Next()) 
						{
							string output_filename = string.Format("newsletter{0:d}.jpg", itr.GetPageNumber());
							Console.WriteLine("newsletter{0:d}.jpg", itr.GetPageNumber());
							draw.Export(itr.Current(), output_path + output_filename, "JPEG", encoder_param);
						}
					}

                    Console.WriteLine("Done.");
				}
				catch (PDFNetException e) 
				{
					Console.WriteLine(e.Message);
				}

				try  // Examples 3-6
				{				
					// Common code for remaining samples.
					using (PDFDoc tiger_doc = new PDFDoc(input_path + "tiger.pdf")) 
					{
						// Initialize the security handler, in case the PDF is encrypted.
						tiger_doc.InitSecurityHandler();  
						Page page = tiger_doc.GetPage(1);

						//--------------------------------------------------------------------------------
						// Example 3) Convert the first page to GDI+ Bitmap. Also, rotate the 
						// page 90 degrees and save the result as RAW.
						draw.SetDPI(100); // Set the output resolution is to 100 DPI.
						draw.SetRotate(Page.Rotate.e_90);  // Rotate all pages 90 degrees clockwise.

						BitmapInfo buf = draw.GetBitmap(page, PDFDraw.PixelFormat.e_rgb, false);

                        // Save the raw RGB data to disk.
                        string filename = "tiger_100dpi_rot90.raw";

                        System.IO.File.WriteAllBytes(output_path + filename, buf.Buffer);

						Console.WriteLine("Example 3: tiger_100dpi_rot90.raw");
						draw.SetRotate(Page.Rotate.e_0);  // Disable image rotation for remaining samples.

						//--------------------------------------------------------------------------------
						// Example 4) Convert PDF page to a fixed image size. Also illustrates some 
						// other features in PDFDraw class such as rotation, image stretching, exporting 
						// to grayscale, or monochrome.

						// Initialize render 'gray_hint' parameter, that is used to control the 
						// rendering process. In this case we tell the rasterizer to export the image as 
						// 1 Bit Per Component (BPC) image.
						Obj mono_hint=hint_set.CreateDict();  
						mono_hint.PutNumber("BPC", 1);

						// SetImageSize can be used instead of SetDPI() to adjust page  scaling 
						// dynamically so that given image fits into a buffer of given dimensions.
						draw.SetImageSize(1000, 1000);		// Set the output image to be 1000 wide and 1000 pixels tall
						draw.Export(page, output_path + "tiger_1000x1000.png", "PNG", mono_hint);
						Console.WriteLine("Example 4: tiger_1000x1000.png");

						draw.SetImageSize(200, 400);	    // Set the output image to be 200 wide and 300 pixels tall
						draw.SetRotate(Page.Rotate.e_180);  // Rotate all pages 90 degrees clockwise.

						// 'gray_hint' tells the rasterizer to export the image as grayscale.
						Obj gray_hint=hint_set.CreateDict();  
						gray_hint.PutName("ColorSpace", "Gray");

						draw.Export(page, output_path + "tiger_200x400_rot180.png", "PNG", gray_hint);
						Console.WriteLine("Example 4: tiger_200x400_rot180.png");

						draw.SetImageSize(400, 200, false);  // The third parameter sets 'preserve-aspect-ratio' to false.
						draw.SetRotate(Page.Rotate.e_0);    // Disable image rotation.
						draw.Export(page, output_path + "tiger_400x200_stretch.jpg", "JPEG");
						Console.WriteLine("Example 4: tiger_400x200_stretch.jpg");

						//--------------------------------------------------------------------------------
						// Example 5) Zoom into a specific region of the page and rasterize the 
						// area at 200 DPI and as a thumbnail (i.e. a 50x50 pixel image).
						page.SetCropBox(new Rect(216, 522, 330, 600));	// Set the page crop box.

						// Select the crop region to be used for drawing.
						draw.SetPageBox(Page.Box.e_crop); 
						draw.SetDPI(900);  // Set the output image resolution to 900 DPI.
						draw.Export(page, output_path + "tiger_zoom_900dpi.png", "PNG");
						Console.WriteLine("Example 5: tiger_zoom_900dpi.png");

                        // -------------------------------------------------------------------------------
                        // Example 6)
                        draw.SetImageSize(50, 50);	   // Set the thumbnail to be 50x50 pixel image.
						draw.Export(page, output_path + "tiger_zoom_50x50.png", "PNG");
						Console.WriteLine("Example 6: tiger_zoom_50x50.png");
					}
				}
				catch (PDFNetException e) 
				{
					Console.WriteLine(e.Message);
				}

				Obj cmyk_hint = hint_set.CreateDict();
				cmyk_hint.PutName("ColorSpace", "CMYK");

                //--------------------------------------------------------------------------------
                // Example 7) Convert the first PDF page to CMYK TIFF at 92 DPI. 
                // A three step tutorial to convert PDF page to an image.
				try
				{
					// A) Open the PDF document.
					using (PDFDoc doc = new PDFDoc(input_path + "tiger.pdf")) 
					{
						// Initialize the security handler, in case the PDF is encrypted.
						doc.InitSecurityHandler();

						// B) The output resolution is set to 92 DPI.
						draw.SetDPI(92);

						// C) Rasterize the first page in the document and save the result as TIFF.
						Page pg = doc.GetPage(1);
						draw.Export(pg, output_path + "out1.tif", "TIFF", cmyk_hint);
						Console.WriteLine("Example 7: out1.tif");
					}
				}
				catch (PDFNetException e)
				{
					Console.WriteLine(e.Message);
				}

                //--------------------------------------------------------------------------------
                // Example 8) Export raster content to PNG using different image smoothing settings. 
				try
				{
                    // A) Open the PDF document.
                    using (PDFDoc doc = new PDFDoc(input_path + "tiger.pdf")) 
					{
						// Initialize the security handler, in case the PDF is encrypted.
						doc.InitSecurityHandler();

                        // B) Get the page matrix 
                        Page pg = doc.GetPage(1);
                        Page.Box box = Page.Box.e_crop;
                        Matrix2D mtx = pg.GetDefaultMatrix(true, box);
                        // We want to render a quadrant, so use half of width and height
                        double pg_w = pg.GetPageWidth(box) / 2;
                        double pg_h = pg.GetPageHeight(box) / 2;

                        // C) Scale matrix from PDF space to buffer space
                        double dpi = 96.0;
                        double scale = dpi / 72.0; // PDF space is 72 dpi
                        int buf_w = (int) (Math.Floor(scale * pg_w));
                        int buf_h = (int) (Math.Floor(scale * pg_h));
                        int bytes_per_pixel = 4; // BGRA buffer
                        int buf_size = buf_w * buf_h * bytes_per_pixel;
                        mtx.Translate(0, -pg_h); // translate by '-pg_h' since we want south-west quadrant
                        mtx = new Matrix2D(scale, 0, 0, scale, 0, 0) * mtx;

                        // D) Rasterize page into memory buffer, according to our parameters
                        byte[] buf;
                        PDFRasterizer rast = new PDFRasterizer();
                        buf = rast.Rasterize(pg, buf_w, buf_h, buf_w * bytes_per_pixel, bytes_per_pixel, true, mtx);

                        // buf now contains raw BGRA bitmap.
                        Console.WriteLine("Example 8: Successfully rasterized into memory buffer.");
					}
				}
				catch (PDFNetException e)
				{
					Console.WriteLine(e.Message);
				}
                //--------------------------------------------------------------------------------
                // Example 9) Export raster content to PNG using different image smoothing settings. 
                try
                {
                    using (PDFDoc text_doc = new PDFDoc(input_path + "lorem_ipsum.pdf")) 
                    {
                        text_doc.InitSecurityHandler();

                        draw.SetImageSmoothing(false, false);
                        string filename = "raster_text_no_smoothing.png";
                        draw.Export(text_doc.GetPageIterator().Current(), output_path + filename);
                        Console.WriteLine("Example 9 a): " + filename + ". Done.");

                        filename = "raster_text_smoothed.png";
                        draw.SetImageSmoothing(true, false /*default quality bilinear resampling*/);
                        draw.Export(text_doc.GetPageIterator().Current(), output_path + filename);
                        Console.WriteLine("Example 9 b): " + filename + ". Done.");

                        filename = "raster_text_high_quality.png";
                        draw.SetImageSmoothing(true, true /*high quality area resampling*/);
                        draw.Export(text_doc.GetPageIterator().Current(), output_path + filename);
                        Console.WriteLine("Example 9 c): " + filename + ". Done.");
                    }
				}
				catch (Exception e)
	            {
					Console.WriteLine(e.Message);
	            }

                //--------------------------------------------------------------------------------
                // Example 10) Export separations directly, without conversion to an output colorspace
                try
                {
                    using (PDFDoc separation_doc = new PDFDoc(input_path + "op_blend_test.pdf"))
                    {
                        separation_doc.InitSecurityHandler();
                        Obj separation_hint = hint_set.CreateDict();
                        separation_hint.PutName("ColorSpace", "Separation");
                        draw.SetDPI(96);
                        draw.SetImageSmoothing(true, true);
                        draw.SetOverprint(PDFRasterizer.OverprintPreviewMode.e_op_on);

                        string filename = "merged_separations.png";
                        draw.Export(separation_doc.GetPageIterator().Current(), output_path + filename, "PNG");
                        Console.WriteLine("Example 10 a): " + filename + ". Done.");

                        filename = "separation";
                        draw.Export(separation_doc.GetPageIterator().Current(), output_path + filename, "PNG", separation_hint);
                        Console.WriteLine("Example 10 b): " + filename + "_[ink].png. Done.");

                        filename = "separation_NChannel.tif";
                        draw.Export(separation_doc.GetPageIterator().Current(), output_path + filename, "TIFF", separation_hint);
                        Console.WriteLine("Example 10 c): " + filename + ". Done.");
                    }
                }
                catch (PDFNetException e)	
		        {
                    Console.WriteLine(e.Message);
                }

			}  // using PDFDraw
        }

        public static byte[] BitmapToByteArray(Bitmap bitmap)
        {

            BitmapData bmpdata = null;

            try
            {
                bmpdata = bitmap.LockBits(new Rectangle(0, 0, bitmap.Width, bitmap.Height), ImageLockMode.ReadOnly, bitmap.PixelFormat);
                int numbytes = bmpdata.Stride * bitmap.Height;
                byte[] bytedata = new byte[numbytes];
                IntPtr ptr = bmpdata.Scan0;

                Marshal.Copy(ptr, bytedata, 0, numbytes);

                return bytedata;
            }
            finally
            {
                if (bmpdata != null)
                    bitmap.UnlockBits(bmpdata);
            }
        } // end
    }
}
