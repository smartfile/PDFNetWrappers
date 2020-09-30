//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

#include <PDF/PDFNet.h>
#include <PDF/PDFDoc.h>
#include <iostream>
#include <assert.h>

using namespace std;
using namespace pdftron;
using namespace SDF;
using namespace PDF;

//-----------------------------------------------------------------------------------------
// The sample code illustrates how to read and edit existing outline items and create 
// new bookmarks using the high-level API.
//-----------------------------------------------------------------------------------------

void PrintIndent(Bookmark item) 
{
	int ident = item.GetIndent() - 1;
	for (int i=0; i<ident; ++i) cout << "  ";
}

// Prints out the outline tree to the standard output
void PrintOutlineTree(Bookmark item)
{
	for (; item.IsValid(); item=item.GetNext())
	{
		PrintIndent(item);
		cout << (item.IsOpen() ? "- " : "+ ") << item.GetTitle() << " ACTION -> ";

		// Print Action
		Action action = item.GetAction();
		if (action.IsValid()) {
			if (action.GetType() == Action::e_GoTo) {
				Destination dest = action.GetDest();
				if (dest.IsValid()) {
					Page page = dest.GetPage();
					cout << "GoTo Page #" << page.GetIndex() << endl;
				}
			}
			else {
				cout << "Not a 'GoTo' action" << endl;
			}
		} else {
			cout << "NULL" << endl;
		}

		if (item.HasChildren())	 // Recursively print children sub-trees
		{
			PrintOutlineTree(item.GetFirstChild());
		}
	}
}

int main(int argc, char *argv[])
{
	int ret = 0;
	PDFNet::Initialize();

	// Relative path to the folder containing test files.
	string input_path =  "../../TestFiles/";
	string output_path = "../../TestFiles/Output/";

	// The following example illustrates how to create and edit the outline tree 
	// using high-level Bookmark methods.
	try  
	{
		PDFDoc doc((input_path + "numbered.pdf").c_str());
		doc.InitSecurityHandler();
		
		// Lets first create the root bookmark items. 
		Bookmark red = Bookmark::Create(doc, "Red");
		Bookmark green = Bookmark::Create(doc, "Green");
		Bookmark blue = Bookmark::Create(doc, "Blue");

		doc.AddRootBookmark(red);
		doc.AddRootBookmark(green);
		doc.AddRootBookmark(blue);

		// You can also add new root bookmarks using Bookmark.AddNext("...")
		blue.AddNext("foo");
		blue.AddNext("bar");

		// We can now associate new bookmarks with page destinations:

		// The following example creates an 'explicit' destination (see 
		// section '8.2.1 Destinations' in PDF Reference for more details)
		Destination red_dest = Destination::CreateFit(doc.GetPageIterator().Current());
		red.SetAction(Action::CreateGoto(red_dest));

		// Create an explicit destination to the first green page in the document
		green.SetAction(Action::CreateGoto( 
			Destination::CreateFit(doc.GetPage(10)) ));

		// The following example creates a 'named' destination (see 
		// section '8.2.1 Destinations' in PDF Reference for more details)
		// Named destinations have certain advantages over explicit destinations.
		const char* key = "blue1";
		Action blue_action = Action::CreateGoto((UChar*) key, UInt32(strlen(key)),
			Destination::CreateFit(doc.GetPage(19)) );
		
		blue.SetAction(blue_action);

		// We can now add children Bookmarks
		Bookmark sub_red1 = red.AddChild("Red - Page 1");
		sub_red1.SetAction(Action::CreateGoto(Destination::CreateFit(doc.GetPage(1))));
		Bookmark sub_red2 = red.AddChild("Red - Page 2");
		sub_red2.SetAction(Action::CreateGoto(Destination::CreateFit(doc.GetPage(2))));
		Bookmark sub_red3 = red.AddChild("Red - Page 3");
		sub_red3.SetAction(Action::CreateGoto(Destination::CreateFit(doc.GetPage(3))));
		Bookmark sub_red4 = sub_red3.AddChild("Red - Page 4");
		sub_red4.SetAction(Action::CreateGoto(Destination::CreateFit(doc.GetPage(4))));
		Bookmark sub_red5 = sub_red3.AddChild("Red - Page 5");
		sub_red5.SetAction(Action::CreateGoto(Destination::CreateFit(doc.GetPage(5))));
		Bookmark sub_red6 = sub_red3.AddChild("Red - Page 6");
		sub_red6.SetAction(Action::CreateGoto(Destination::CreateFit(doc.GetPage(6))));
		
		// Example of how to find and delete a bookmark by title text.
		Bookmark foo = doc.GetFirstBookmark().Find("foo");
		if (foo.IsValid()) 
		{
			foo.Delete();
		}
		else 
		{
			assert(false);
		}

		Bookmark bar = doc.GetFirstBookmark().Find("bar");
		if (bar.IsValid()) 
		{
			bar.Delete();
		}
		else 
		{
			assert(false);
		}

		// Adding color to Bookmarks. Color and other formatting can help readers 
		// get around more easily in large PDF documents.
		red.SetColor(1, 0, 0);
		green.SetColor(0, 1, 0);
		green.SetFlags(2);			// set bold font
		blue.SetColor(0, 0, 1);
		blue.SetFlags(3);			// set bold and italic

		doc.Save((output_path + "bookmark.pdf").c_str(), 0, 0);
		cout << "Done. Result saved in bookmark.pdf" << endl;
	}
	catch(Common::Exception& e)
	{
		cout << e << endl;
		ret = 1;
	}
	catch(...)
	{
		cout << "Unknown Exception" << endl;
		ret = 1;
	}

	
	// The following example illustrates how to traverse the outline tree using 
	// Bookmark navigation methods: Bookmark.GetNext(), Bookmark.GetPrev(), 
	// Bookmark.GetFirstChild () and Bookmark.GetLastChild ().
	try  
	{
		// Open the document that was saved in the previous code sample
		PDFDoc doc((output_path + "bookmark.pdf").c_str());
		doc.InitSecurityHandler();
		
		Bookmark root = doc.GetFirstBookmark();
		PrintOutlineTree(root);

		cout << "Done." << endl;
	}
	catch(Common::Exception& e)
	{
		cout << e << endl;
		ret = 1;
	}
	catch(...)
	{
		cout << "Unknown Exception" << endl;
		ret = 1;
	}

	// The following example illustrates how to create a Bookmark to a page 
	// in a remote document. A remote go-to action is similar to an ordinary 
	// go-to action, but jumps to a destination in another PDF file instead 
	// of the current file. See Section 8.5.3 'Remote Go-To Actions' in PDF 
	// Reference Manual for details.
	try  
	{
		// Open the document that was saved in the previous code sample
		PDFDoc doc((output_path + "bookmark.pdf").c_str());
		doc.InitSecurityHandler();

		// Create file specification (the file referred to by the remote bookmark)
		Obj file_spec = doc.CreateIndirectDict(); 
		file_spec.PutName("Type", "Filespec");
		file_spec.PutString("F", "bookmark.pdf");
		FileSpec spec(file_spec);
		Action goto_remote = Action::CreateGotoRemote(spec, 5, true);

		Bookmark remoteBookmark1 = Bookmark::Create(doc, "REMOTE BOOKMARK 1");
		remoteBookmark1.SetAction(goto_remote);
		doc.AddRootBookmark(remoteBookmark1);

		// Create another remote bookmark, but this time using the low-level SDF/Cos API.
		// Create a remote action
		Bookmark remoteBookmark2 = Bookmark::Create(doc, "REMOTE BOOKMARK 2");
		doc.AddRootBookmark(remoteBookmark2);
		
		Obj gotoR = remoteBookmark2.GetSDFObj().PutDict("A");
		{
			gotoR.PutName("S","GoToR"); // Set action type
			gotoR.PutBool("NewWindow", true);

			// Set the file specification
			gotoR.Put("F", file_spec);

			// jump to the first page. Note that pages are indexed from 0.
			Obj dest = gotoR.PutArray("D");  // Set the destination
			dest.PushBackNumber(9); 
			dest.PushBackName("Fit");
		}

		doc.Save((output_path + "bookmark_remote.pdf").c_str(), SDFDoc::e_linearized, 0);

		cout << "Done. Result saved in bookmark_remote.pdf" << endl;
	}
	catch(Common::Exception& e)
	{
		cout << e << endl;
		ret = 1;
	}
	catch(...)
	{
		cout << "Unknown Exception" << endl;
		ret = 1;
	}

	PDFNet::Terminate();
	return ret;
}


