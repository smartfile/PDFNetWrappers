//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

import com.pdftron.pdf.*;
import com.pdftron.sdf.Obj;
import com.pdftron.common.Matrix2D;
import com.pdftron.common.PDFNetException;
import com.pdftron.sdf.UndoManager;
import com.pdftron.sdf.ResultSnapshot;
import com.pdftron.sdf.DocSnapshot;
import com.pdftron.sdf.SDFDoc;

import java.io.File;
import java.io.IOException;
import java.io.FileNotFoundException;

//---------------------------------------------------------------------------------------
// The following sample illustrates how to use the UndoRedo API.
//---------------------------------------------------------------------------------------
public class UndoRedoTest 
{
    public static void main(String[] args) 
	{
        try 
		{
            // The first step in every application using PDFNet is to initialize the
            // library and set the path to common PDF resources. The library is usually
            // initialized only once, but calling Initialize() multiple times is also fine.
            PDFNet.initialize();

            // Relative path to the folder containing test files.
            String input_path = "../../TestFiles/";
            String output_path = "../../TestFiles/Output/";

			// Open the PDF document.
			PDFDoc doc = new PDFDoc(input_path + "newsletter.pdf");

			UndoManager undo_manager = doc.getUndoManager();

			// Take a snapshot to which we can undo after making changes.
			ResultSnapshot snap0 = undo_manager.takeSnapshot();

			DocSnapshot snap0_state = snap0.currentState();
			
			Page page = doc.pageCreate();	// Start a new page

			ElementBuilder bld = new ElementBuilder();		// Used to build new Element objects
			ElementWriter writer = new ElementWriter();		// Used to write Elements to the page	
			writer.begin(page);		// Begin writing to this page

			// ----------------------------------------------------------
			// Add JPEG image to the file
			Image img = Image.create(doc, input_path + "peppers.jpg");
			Element element = bld.createImage(img, new Matrix2D(200, 0, 0, 250, 50, 500));
			writer.writePlacedElement(element);

			writer.end();	// Finish writing to the page
			doc.pagePushFront(page);

			// Take a snapshot after making changes, so that we can redo later (after undoing first).
			ResultSnapshot snap1 = undo_manager.takeSnapshot();

			if (snap1.previousState().equals(snap0_state))
			{
				System.out.println("snap1 previous state equals snap0_state; previous state is correct");
			}
			
			DocSnapshot snap1_state = snap1.currentState();

			doc.save(output_path + "addimage.pdf", SDFDoc.SaveMode.INCREMENTAL, null);

			if (undo_manager.canUndo())
			{
				ResultSnapshot undo_snap;
				undo_snap = undo_manager.undo();

				doc.save(output_path + "addimage_undone.pdf", SDFDoc.SaveMode.INCREMENTAL, null);

				DocSnapshot undo_snap_state = undo_snap.currentState();

				if (undo_snap_state.equals(snap0_state))
				{
					System.out.println("undo_snap_state equals snap0_state; undo was successful");
				}
				
				if (undo_manager.canRedo())
				{
					ResultSnapshot redo_snap = undo_manager.redo();

					doc.save(output_path + "addimage_redone.pdf", SDFDoc.SaveMode.INCREMENTAL, null);

					if (redo_snap.previousState().equals(undo_snap_state))
					{
						System.out.println("redo_snap previous state equals undo_snap_state; previous state is correct");
					}
					
					DocSnapshot redo_snap_state = redo_snap.currentState();
					
					if (redo_snap_state.equals(snap1_state))
					{
						System.out.println("Snap1 and redo_snap are equal; redo was successful");
					}
				}
				else
				{
					System.out.println("Problem encountered - cannot redo.");
				}
			}
			else
			{
				System.out.println("Problem encountered - cannot undo.");
			}

            // Calling Terminate when PDFNet is no longer in use is a good practice, but
            // is not required.
            PDFNet.terminate();
        }
		catch (Exception e) 
		{
            e.printStackTrace();
        }
    }
}