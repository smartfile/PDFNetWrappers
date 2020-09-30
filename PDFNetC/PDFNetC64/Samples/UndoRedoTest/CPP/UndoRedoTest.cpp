//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

#include <PDF/PDFNet.h>
#include <PDF/PDFDoc.h>
#include <string>
#include <iostream>
#include <PDF/Element.h>
#include <PDF/ElementBuilder.h>
#include <PDF/ElementWriter.h>
#include <PDF/Image.h>
#include <Common/Matrix2D.h>

using namespace std;
using namespace pdftron;
using namespace PDF;
using namespace SDF;
using namespace Common;

//---------------------------------------------------------------------------------------
// The following sample illustrates how to use the UndoRedo API.
//---------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	int ret = 0;
	try 
	{
		// The first step in every application using PDFNet is to initialize the 
		// library and set the path to common PDF resources. The library is usually 
		// initialized only once, but calling Initialize() multiple times is also fine.
		PDFNet::Initialize();
		
		// Relative path to the folder containing test files.
		string input_path =  "../../TestFiles/";
		string output_path = "../../TestFiles/Output/";
		
		// Open the PDF document.
		PDFDoc doc((input_path + "newsletter.pdf").c_str());

		UndoManager undo_manager = doc.GetUndoManager();

		// Take a snapshot to which we can undo after making changes.
		ResultSnapshot snap0 = undo_manager.TakeSnapshot();

		DocSnapshot snap0_state = snap0.CurrentState();
		
		Page page = doc.PageCreate();	// Start a new page

		ElementBuilder bld;		// Used to build new Element objects
		ElementWriter writer;	// Used to write Elements to the page	
		writer.Begin(page);		// Begin writing to this page

		// ----------------------------------------------------------
		// Add JPEG image to the file
		PDF::Image img = PDF::Image::Create(doc, (input_path + "peppers.jpg").c_str());
		Element element = bld.CreateImage(img, Matrix2D(200, 0, 0, 250, 50, 500));
		writer.WritePlacedElement(element);

		writer.End();	// Finish writing to the page
		doc.PagePushFront(page);

		// Take a snapshot after making changes, so that we can redo later (after undoing first).
		ResultSnapshot snap1 = undo_manager.TakeSnapshot();

		if (snap1.PreviousState().Equals(snap0_state))
		{
			puts("snap1 previous state equals snap0_state; previous state is correct");
		}
		
		DocSnapshot snap1_state = snap1.CurrentState();

		doc.Save((output_path + "addimage.pdf").c_str(), SDFDoc::e_incremental, 0);

		if (undo_manager.CanUndo())
		{
			ResultSnapshot undo_snap = undo_manager.Undo();

			doc.Save((output_path + "addimage_undone.pdf").c_str(), SDFDoc::e_incremental, 0);

			DocSnapshot undo_snap_state = undo_snap.CurrentState();

			if (undo_snap_state.Equals(snap0_state))
			{
				puts("undo_snap_state equals snap0_state; undo was successful");
			}

			if (undo_manager.CanRedo())
			{
				ResultSnapshot redo_snap = undo_manager.Redo();

				doc.Save((output_path + "addimage_redone.pdf").c_str(), SDFDoc::e_incremental, 0);

				if (redo_snap.PreviousState().Equals(undo_snap_state))
				{
					puts("redo_snap previous state equals undo_snap_state; previous state is correct");
				}

				DocSnapshot redo_snap_state = redo_snap.CurrentState();

				if (redo_snap_state.Equals(snap1_state))
				{
					puts("Snap1 and redo_snap are equal; redo was successful");
				}
			}
			else
			{
				puts("Problem encountered - cannot redo.");
				ret = 1;
			}
		}
		else
		{
			puts("Problem encountered - cannot undo.");
			ret = 1;
		}
	}
	catch(Common::Exception& e)	
	{
		cout << e << endl;
		ret = 1;
	}
	catch (...) 
	{
		cout << "Unknown Exception" << endl;
		ret = 1;
	}

	PDFNet::Terminate();

	return ret;	
}
