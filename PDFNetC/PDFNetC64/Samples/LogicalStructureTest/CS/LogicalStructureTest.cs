//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

using System;
using System.Collections;

using pdftron;
using pdftron.Common;
using pdftron.Filters;
using pdftron.SDF;
using pdftron.PDF;
using pdftron.PDF.Struct;


namespace LogicalStructureTestCS
{
	//---------------------------------------------------------------------------------------
	// This sample explores the structure and content of a tagged PDF document and dumps 
	// the structure information to the console window.
	//
	// In tagged PDF documents StructTree acts as a central repository for information 
	// related to a PDF document's logical structure. The tree consists of StructElement-s
	// and ContentItem-s which are leaf nodes of the structure tree.
	//
	// The sample can be extended to access and extract the marked-content elements such 
	// as text and images.
	//---------------------------------------------------------------------------------------
	class Class1
	{
		static void PrintIndent(int indent) { Console.WriteLine(); for (int i=0; i<indent; ++i) Console.Write("  "); }

		// Used in code snippet 1.
		static void ProcessStructElement(SElement element, int indent)
		{
			if (!element.IsValid()) {
				return;
			}

			// Print out the type and title info, if any.
			PrintIndent(indent++);
			Console.Write("Type: " + element.GetType());
			if (element.HasTitle()) {
				Console.Write(". Title: "+ element.GetTitle());
			}

			int num = element.GetNumKids();
			for (int i=0; i<num; ++i) 
			{
				// Check is the kid is a leaf node (i.e. it is a ContentItem).
				if (element.IsContentItem(i)) { 
					ContentItem cont = element.GetAsContentItem(i); 
					ContentItem.Type type = cont.GetType();

					Page page = cont.GetPage();

					PrintIndent(indent);
					Console.Write("Content Item. Part of page #" + page.GetIndex());

					PrintIndent(indent);
					switch (type) {
						case ContentItem.Type.e_MCID:
						case ContentItem.Type.e_MCR:
							Console.Write("MCID: " + cont.GetMCID());
							break;
						case ContentItem.Type.e_OBJR:
							{
								Console.Write("OBJR ");
								Obj ref_obj = cont.GetRefObj();
								if (ref_obj!=null)
									Console.Write("- Referenced Object#: " + ref_obj.GetObjNum());
							}
							break;
						default: 
							break;
					}
				}
				else {  // the kid is another StructElement node.
					ProcessStructElement(element.GetAsStructElem(i), indent);
				}
			}
		}

		// Used in code snippet 2.
		static void ProcessElements(ElementReader reader)
		{
			Element element;
			while ((element = reader.Next())!=null) 	// Read page contents
			{
				// In this sample we process only paths & text, but the code can be 
				// extended to handle any element type.
				Element.Type type = element.GetType();
				if (type == Element.Type.e_path || type == Element.Type.e_text || type == Element.Type.e_path) 
				{   
					switch (type)	{
					case Element.Type.e_path:               // Process path ...
                        Console.WriteLine();
						Console.Write("PATH: ");
	 					break; 
					case Element.Type.e_text: 				// Process text ...
                        Console.WriteLine();
                        Console.WriteLine("TEXT: " + element.GetTextString());
						break;
					case Element.Type.e_form:				// Process form XObjects
                        Console.WriteLine();
						Console.Write("FORM XObject: ");
						//reader.FormBegin(); 
						//ProcessElements(reader);
						//reader.End(); 
						break; 
					}

					// Check if the element is associated with any structural element.
					// Content items are leaf nodes of the structure tree.
					SElement struct_parent = element.GetParentStructElement();
					if (struct_parent.IsValid()) {
						// Print out the parent structural element's type, title, and object number.
						Console.Write(" Type: " + struct_parent.GetType() 
							+ ", MCID: " + element.GetStructMCID());
						if (struct_parent.HasTitle()) {
							Console.Write(". Title: "+ struct_parent.GetTitle());
						}
						Console.Write(", Obj#: " + struct_parent.GetSDFObj().GetObjNum());
					}
				}
			}
		}

		// Used in code snippet 3.
		//typedef map<int, string> MCIDPageMap;
		//typedef map<int, MCIDPageMap> MCIDDocMap;

		// Used in code snippet 3.
		static void ProcessElements2(ElementReader reader, Hashtable mcid_page_map)
		{
			Element element;
			while ((element = reader.Next())!=null) // Read page contents
			{
				// In this sample we process only text, but the code can be extended 
				// to handle paths, images, or any other Element type.
				int mcid = element.GetStructMCID();
				if (mcid>= 0 && element.GetType() == Element.Type.e_text) {
					String val = element.GetTextString();
					if (mcid_page_map.ContainsKey(mcid)) mcid_page_map[mcid] = ((String)(mcid_page_map[mcid])+ val); 
					else mcid_page_map.Add(mcid, val);
				}
			}
		}

		// Used in code snippet 3.
		static void ProcessStructElement2(SElement element, Hashtable mcid_doc_map, int indent)
		{
			if (!element.IsValid()) {
				return;
			}

			// Print out the type and title info, if any.
			PrintIndent(indent);
			Console.Write("<" + element.GetType());
			if (element.HasTitle()) {
				Console.Write(" title=\""+ element.GetTitle() + "\"");
			}
			Console.Write(">");

			int num = element.GetNumKids();
			for (int i=0; i<num; ++i) 
			{		
				if (element.IsContentItem(i)) { 
					ContentItem cont = element.GetAsContentItem(i); 
					if (cont.GetType() == ContentItem.Type.e_MCID) {
						int page_num = cont.GetPage().GetIndex();
						if (mcid_doc_map.ContainsKey(page_num)) {
							Hashtable mcid_page_map = (Hashtable)(mcid_doc_map[page_num]);
							int mcid = cont.GetMCID();
							if (mcid_page_map.ContainsKey(mcid)) {
								Console.Write(mcid_page_map[mcid]); 
							}                    
						}
					}
				}
				else {  // the kid is another StructElement node.
					ProcessStructElement2(element.GetAsStructElem(i), mcid_doc_map, indent+1);
				}
			}

			PrintIndent(indent);
			Console.Write("</" + element.GetType() + ">");
		}

		private static pdftron.PDFNetLoader pdfNetLoader = pdftron.PDFNetLoader.Instance();
		static Class1() {}

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		static void Main(string[] args)
		{
			PDFNet.Initialize();
			// Relative path to the folder containing test files.
			string input_path =  "../../TestFiles/";
			string output_path = "../../TestFiles/Output/";

			try  // Extract logical structure from a PDF document
			{
				using (PDFDoc doc = new PDFDoc(input_path + "tagged.pdf"))
				{
					doc.InitSecurityHandler();

					bool example1 = true;
					bool example2 = true;
					bool example3 = true;

					if (example1)
					{
						Console.WriteLine("____________________________________________________________");
						Console.WriteLine("Sample 1 - Traverse logical structure tree...");

						STree tree = doc.GetStructTree();
						if (tree.IsValid()) 
						{
							Console.WriteLine("Document has a StructTree root.");
							for (int i=0; i<tree.GetNumKids(); ++i) 
							{
								// Recursively get structure  info for all all child elements.
								ProcessStructElement(tree.GetKid(i), 0);
							}
						}
						else 
						{
							Console.WriteLine("This document does not contain any logical structure.");
						}

                        Console.WriteLine();
                        Console.WriteLine("Done 1.");
					}

					if (example2)
					{
						Console.WriteLine("____________________________________________________________");
						Console.WriteLine("Sample 2 - Get parent logical structure elements from");
						Console.WriteLine("layout elements.");
						
						ElementReader reader=new ElementReader();
						for (PageIterator itr = doc.GetPageIterator(); itr.HasNext(); itr.Next()) 
						{				
							reader.Begin(itr.Current());
							ProcessElements(reader);
							reader.End();
						}
                        Console.WriteLine();
						Console.WriteLine("Done 2.");
					}

					if (example3)
					{
						Console.WriteLine("____________________________________________________________");
						Console.WriteLine("Sample 3 - 'XML style' extraction of PDF logical structure and page content.");
						
						//A map which maps page numbers(as Integers)
						//to page Maps(which map from struct mcid(as Integers) to
						//text Strings)
						Hashtable mcid_doc_map=new Hashtable();
						ElementReader reader=new ElementReader();
						for (PageIterator itr = doc.GetPageIterator(); itr.HasNext(); itr.Next()) 
						{				
							Page pg = itr.Current();
							reader.Begin(pg);
							Hashtable page_mcid_map=new Hashtable();
							mcid_doc_map.Add(pg.GetIndex(), page_mcid_map);
							ProcessElements2(reader, page_mcid_map);
							reader.End();
						}
						
						STree tree = doc.GetStructTree();
						if (tree.IsValid()) 
						{
							for (int i=0; i<tree.GetNumKids(); ++i) 
							{
								ProcessStructElement2(tree.GetKid(i), mcid_doc_map, 0);
							}
						}
                        Console.WriteLine();
                        Console.WriteLine("Done 3.");
					}

					doc.Save(output_path + "bookmark.pdf", 0);
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}
		}
	}
}
