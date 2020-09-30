//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

using System;
using pdftron;
using pdftron.Common;
using pdftron.Filters;
using pdftron.SDF;
using pdftron.PDF;

namespace BookmarkTestCS
{
	/// <summary>
	//-----------------------------------------------------------------------------------------
	// The sample code illustrates how to read, write, and edit existing outline items 
	// and create new bookmarks using both the high-level and the SDF/Cos API.
	//-----------------------------------------------------------------------------------------
	/// </summary>
	class Class1
	{
		private static pdftron.PDFNetLoader pdfNetLoader = pdftron.PDFNetLoader.Instance();
		static Class1() {}

		static void PrintIndent(Bookmark item)
		{
			int indent = item.GetIndent() - 1;
			for (int i = 0; i < indent; ++i)
				Console.Write("  ");
		}

		// Prints out the outline tree to the standard output
		static void PrintOutlineTree(Bookmark item)
		{
			for (; item.IsValid(); item = item.GetNext())
			{
				PrintIndent(item);
				Console.Write("{0:s}{1:s} ACTION -> ", (item.IsOpen() ? "- " : "+ "), item.GetTitle());

				// Print Action
				pdftron.PDF.Action action = item.GetAction();
				if (action.IsValid())
				{
					if (action.GetType() == pdftron.PDF.Action.Type.e_GoTo)
					{
						Destination dest = action.GetDest();
						if (dest.IsValid())
						{
							Page page = dest.GetPage();
							Console.WriteLine("GoTo Page #{0:d}", page.GetIndex());
						}
					}
					else
					{
						Console.WriteLine("Not a 'GoTo' action");
					}
				}
				else
				{
					Console.WriteLine("NULL");
				}

				if (item.HasChildren())	 // Recursively print children sub-trees
				{
					PrintOutlineTree(item.GetFirstChild());
				}
			}
		}

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		static void Main(string[] args)
		{
			PDFNet.Initialize();

			// Relative path to the folder containing test files.
			string input_path = "../../TestFiles/";
			string output_path = "../../TestFiles/Output/";


			// The following example illustrates how to create and edit the outline tree 
			// using high-level Bookmark methods.
			try
			{
				using (PDFDoc doc = new PDFDoc(input_path + "numbered.pdf"))
				{
					doc.InitSecurityHandler();

					// Lets first create the root bookmark items. 
					Bookmark red = Bookmark.Create(doc, "Red");
					Bookmark green = Bookmark.Create(doc, "Green");
					Bookmark blue = Bookmark.Create(doc, "Blue");

					doc.AddRootBookmark(red);
					doc.AddRootBookmark(green);
					doc.AddRootBookmark(blue);

					// You can also add new root bookmarks using Bookmark.AddNext("...")
					blue.AddNext("foo");
					blue.AddNext("bar");

					// We can now associate new bookmarks with page destinations:

					// The following example creates an 'explicit' destination (see 
					// section '8.2.1 Destinations' in PDF Reference for more details)
					Destination red_dest = Destination.CreateFit(doc.GetPage(1));
					red.SetAction(pdftron.PDF.Action.CreateGoto(red_dest));

					// Create an explicit destination to the first green page in the document
					green.SetAction(pdftron.PDF.Action.CreateGoto(
						Destination.CreateFit(doc.GetPage(10))));

					// The following example creates a 'named' destination (see 
					// section '8.2.1 Destinations' in PDF Reference for more details)
					// Named destinations have certain advantages over explicit destinations.
					String key = "blue1";
					pdftron.PDF.Action blue_action = pdftron.PDF.Action.CreateGoto(key,
						Destination.CreateFit(doc.GetPage(19)));

					blue.SetAction(blue_action);

					// We can now add children Bookmarks
					Bookmark sub_red1 = red.AddChild("Red - Page 1");
					sub_red1.SetAction(pdftron.PDF.Action.CreateGoto(Destination.CreateFit(doc.GetPage(1))));
					Bookmark sub_red2 = red.AddChild("Red - Page 2");
					sub_red2.SetAction(pdftron.PDF.Action.CreateGoto(Destination.CreateFit(doc.GetPage(2))));
					Bookmark sub_red3 = red.AddChild("Red - Page 3");
					sub_red3.SetAction(pdftron.PDF.Action.CreateGoto(Destination.CreateFit(doc.GetPage(3))));
					Bookmark sub_red4 = sub_red3.AddChild("Red - Page 4");
					sub_red4.SetAction(pdftron.PDF.Action.CreateGoto(Destination.CreateFit(doc.GetPage(4))));
					Bookmark sub_red5 = sub_red3.AddChild("Red - Page 5");
					sub_red5.SetAction(pdftron.PDF.Action.CreateGoto(Destination.CreateFit(doc.GetPage(5))));
					Bookmark sub_red6 = sub_red3.AddChild("Red - Page 6");
					sub_red6.SetAction(pdftron.PDF.Action.CreateGoto(Destination.CreateFit(doc.GetPage(6))));

					// Example of how to find and delete a bookmark by title text.
					Bookmark foo = doc.GetFirstBookmark().Find("foo");
					if (foo.IsValid())
					{
						foo.Delete();
					}

					Bookmark bar = doc.GetFirstBookmark().Find("bar");
					if (bar.IsValid())
					{
						bar.Delete();
					}

					// Adding color to Bookmarks. Color and other formatting can help readers 
					// get around more easily in large PDF documents.
					red.SetColor(1, 0, 0);
					green.SetColor(0, 1, 0);
					green.SetFlags(2);			// set bold font
					blue.SetColor(0, 0, 1);
					blue.SetFlags(3);			// set bold and italic

					doc.Save(output_path + "bookmark.pdf", 0);
					Console.WriteLine("Done. Result saved in bookmark.pdf");
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}


			// The following example illustrates how to traverse the outline tree using 
			// Bookmark navigation methods: Bookmark.GetNext(), Bookmark.GetPrev(), 
			// Bookmark.GetFirstChild () and Bookmark.GetLastChild ().
			try
			{
				// Open the document that was saved in the previous code sample
				using (PDFDoc doc = new PDFDoc(output_path + "bookmark.pdf"))
				{
					doc.InitSecurityHandler();

					Bookmark root = doc.GetFirstBookmark();
					PrintOutlineTree(root);

                    Console.WriteLine("Done.");
                }
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}

			// The following example illustrates how to create a Bookmark to a page 
			// in a remote document. A remote go-to action is similar to an ordinary 
			// go-to action, but jumps to a destination in another PDF file instead 
			// of the current file. See Section 8.5.3 'Remote Go-To Actions' in PDF 
			// Reference Manual for details.
			try
			{
				using (PDFDoc doc = new PDFDoc(output_path + "bookmark.pdf"))
				{
					doc.InitSecurityHandler();

					// Create file specification (the file referred to by the remote bookmark)
					Obj file_spec = doc.CreateIndirectDict();
					file_spec.PutName("Type", "Filespec");
					file_spec.PutString("F", "bookmark.pdf");

					FileSpec spec = new FileSpec(file_spec);
					pdftron.PDF.Action goto_remote = pdftron.PDF.Action.CreateGotoRemote(spec, 5, true);

					Bookmark remoteBookmark1 = Bookmark.Create(doc, "REMOTE BOOKMARK 1");
					remoteBookmark1.SetAction(goto_remote);
					doc.AddRootBookmark(remoteBookmark1);

					// Create another remote bookmark, but this time using the low-level SDF/Cos API.
					Bookmark remoteBookmark2 = Bookmark.Create(doc, "REMOTE BOOKMARK 2");
					doc.AddRootBookmark(remoteBookmark2);
					Obj gotoR = remoteBookmark2.GetSDFObj().PutDict("A");
					{	// Create the 'Action' dictionary.
						gotoR.PutName("S", "GoToR"); // Set action type
						gotoR.PutBool("NewWindow", true);

						// Set the file specification
						gotoR.Put("F", file_spec);

						// Set the destination.
						Obj dest = gotoR.PutArray("D");
						dest.PushBackNumber(9);  // jump to the tenth page. Note that Acrobat indexes pages from 0.
						dest.PushBackName("Fit"); // Fit the page
					}

					doc.Save(output_path + "bookmark_remote.pdf", SDFDoc.SaveOptions.e_linearized);
					Console.WriteLine("Done. Result saved in bookmark_remote.pdf");
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}
		}
	}
}
