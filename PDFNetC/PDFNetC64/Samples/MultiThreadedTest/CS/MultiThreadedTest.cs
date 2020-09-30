//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.     
//---------------------------------------------------------------------------------------

using System;
using System.Drawing;

using pdftron;
using pdftron.Common;
using pdftron.PDF;
using pdftron.SDF;
using System.Threading;
using System.Threading.Tasks;
using System.Collections.Generic;

namespace PDFDrawTestCS
{
	/// <summary>
	//---------------------------------------------------------------------------------------
	// The following sample illustrates how to use PDFDoc locking mechanisms to access the document
    // concurrently.  PDFDoc uses a recursive shared lock model.  Multiple threads can read the document 
    // at the same time, but only one thread can write to the document.  A given thread can acquire 
    // as many locks of the same type as it wants, in a recursive fashion.
    //
    // It's important to note that you cannot upgrade a PDFDoc read lock to a write lock.  Because of
    // the nature of the lock, this would cause a deadlock, and is not allowed.
    //
    // If a thread attempts to acquire a write lock while holding a read lock, an exception will be
    // thrown.  As a side effect, this means certain PDFNet API calls which implicitly acquire a write
    // lock on the document will throw an exception if called while holding a read lock.  The API
    // documents which methods acquire a write lock on the document.
	//
	// For more information on document locking in PDFNet, please consult the following knowledge base
	// article: https://groups.google.com/forum/#!topic/pdfnet-sdk/pDZsqrjCEdg
	//---------------------------------------------------------------------------------------

	class Class1
	{
        private static pdftron.PDFNetLoader pdfNetLoader = pdftron.PDFNetLoader.Instance();
		static Class1() {}
        
		/// <summary>
		/// A static method for rasterizing a given page to a file.
		/// </summary>
        static void DoDraw(PDFDoc doc, Page pg, string output_path)
        {
            //acquire a read lock, since we will be accessing the document.
            doc.LockRead();

            //Acquiring a write lock when holding a read lock is illegal,
            //and would cause an exception to be thrown:
            //doc.Lock();

            //draw the page
            using (PDFDraw draw = new PDFDraw())
            {
                draw.SetDPI(92);
                draw.Export(pg, output_path);
                Console.WriteLine("Rendered page: Result saved in {0}", output_path);
            }

            //release the read lock
            doc.UnlockRead();
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
            
			// Relative path to the folder containing test files
			string input_path =  "../../TestFiles/";
			string output_path = "../../TestFiles/Output/";

            //Example - Exporting images of each page in the document in parallel,
            //            and annotating the document in the main thread.
			try  
			{
				// Open the PDF document.
				using (PDFDoc doc = new PDFDoc(input_path + "newsletter.pdf")) 
				{
                    // Lock the document, since we are going to annotate it in this thread.
                    doc.Lock();

					// Initialize the security handler, in case the PDF is encrypted.
					doc.InitSecurityHandler();

                    List<Task> tasks = new List<Task>();

                    // Iterate through each page in the document
					for (PageIterator itr=doc.GetPageIterator(); itr.HasNext(); itr.Next()) 
                    {
                        //note that locking a second time does not cause a deadlock:
                        doc.Lock();

                        // You can acquire a read lock while holding a write lock:
                        doc.LockRead();
                        doc.UnlockRead();

                        // Choose an output path for this page
                        string page_output_path = output_path + "newsletter_" + itr.GetPageNumber() + ".png";

                        // Create an asynchronous task to draw the page
                        Page pg = itr.Current();
                        Task t = new Task(() => DoDraw(doc, pg, page_output_path));
                        // Start the Task (although it won't be able to access the document, since we have a write lock)
                        t.Start();
                        // Add it to our list of Tasks so we can Wait() for it later.
                        tasks.Add(t);

                        Console.WriteLine("Adding stamp to PDFDoc, page " + itr.GetPageNumber());

                        // Create a stamp annotation (See AnnotationTest for more details)
                        pdftron.PDF.Annots.RubberStamp stamp = pdftron.PDF.Annots.RubberStamp.Create(doc.GetSDFDoc(), new Rect(30, 30, 300, 200));
                        stamp.SetIcon("Approved");
                        itr.Current().AnnotPushBack(stamp);

                        // Releasing the lock decrements our lock count, 
                        // but the thread still holds a write lock on the document:
                        doc.Unlock();
                    }
     
                    // Now we release the write lock, and the 
                    // PDFDraw tasks can begin execution
                    doc.Unlock();

                    // Wait for the PDFDraw tasks to complete.
                    foreach (Task t in tasks)
                    {
                        t.Wait();
                    }
				}
			}
			catch (PDFNetException e) 
            {
				Console.WriteLine(e.Message);
			}
		}
	}
}
