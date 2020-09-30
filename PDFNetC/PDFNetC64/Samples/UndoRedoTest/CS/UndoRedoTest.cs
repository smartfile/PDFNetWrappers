//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.     
//---------------------------------------------------------------------------------------

using System;
using pdftron;
using pdftron.Common;
using pdftron.PDF;
using pdftron.SDF;

namespace UndoRedoTestCS
{
	/// <summary>
	//---------------------------------------------------------------------------------------
	// The following sample illustrates how to use the UndoRedo API.
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

			// Relative path to the folder containing test files.
			string input_path =  "../../TestFiles/";
			string output_path = "../../TestFiles/Output/";

			try  
			{
				// Open the PDF document.
                using (PDFDoc doc = new PDFDoc(input_path + "newsletter.pdf"))
                using (ElementBuilder bld = new ElementBuilder())	// Used to build new Element objects
                using (ElementWriter writer = new ElementWriter())	// Used to write Elements to the page	
                {
                    UndoManager undo_manager = doc.GetUndoManager();

					// Take a snapshot to which we can undo after making changes.
                    ResultSnapshot snap0 = undo_manager.TakeSnapshot();

                    DocSnapshot snap0_state = snap0.CurrentState();

                    Page page = doc.PageCreate();	// Start a new page

                        writer.Begin(page);		// Begin writing to this page

                    // ----------------------------------------------------------
                    // Add JPEG image to the file
                    Image img = Image.Create(doc, input_path + "peppers.jpg");
                    Element element = bld.CreateImage(img, new Matrix2D(200, 0, 0, 250, 50, 500));
                    writer.WritePlacedElement(element);

                    writer.End();	// Finish writing to the page
                    doc.PagePushFront(page);

					// Take a snapshot after making changes, so that we can redo later (after undoing first).
                    ResultSnapshot snap1 = undo_manager.TakeSnapshot();

                    if (snap1.PreviousState().Equals(snap0_state))
                    {
                        Console.WriteLine("snap1 previous state equals snap0_state; previous state is correct");
                    }

                    DocSnapshot snap1_state = snap1.CurrentState();

                    doc.Save(output_path + "addimage.pdf", SDFDoc.SaveOptions.e_incremental);

					if (undo_manager.CanUndo())
					{
						ResultSnapshot undo_snap = undo_manager.Undo();

						doc.Save(output_path + "addimage_undone.pdf", SDFDoc.SaveOptions.e_incremental);

						DocSnapshot undo_snap_state = undo_snap.CurrentState();

						if (undo_snap_state.Equals(snap0_state))
						{
							Console.WriteLine("undo_snap_state equals snap0_state; undo was successful");
						}

						if (undo_manager.CanRedo())
						{
							ResultSnapshot redo_snap = undo_manager.Redo();
							
							doc.Save(output_path + "addimage_redone.pdf", SDFDoc.SaveOptions.e_incremental);

							if (redo_snap.PreviousState().Equals(undo_snap_state))
							{
								Console.WriteLine("redo_snap previous state equals undo_snap_state; previous state is correct");
							}

							DocSnapshot redo_snap_state = redo_snap.CurrentState();

							if (redo_snap_state.Equals(snap1_state))
							{
								Console.WriteLine("Snap1 and redo_snap are equal; redo was successful");
							}
						}
						else
						{
							Console.WriteLine("Problem encountered - cannot redo.");
						}
					}
					else
					{
						Console.WriteLine("Problem encountered - cannot undo.");
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
