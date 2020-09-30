//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

#include <PDF/PDFNet.h>
#include <PDF/PDFDoc.h>
#include <PDF/ElementReader.h>
#include <iostream>
#include <map>

using namespace pdftron;
using namespace PDF;
using namespace std;

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


void PrintIndent(int indent) { cout << '\n'; for (int i=0; i<indent; ++i) cout << "  "; }

// Used in code snippet 1.
void ProcessStructElement(Struct::SElement element, int ident) 
{
	if (!element.IsValid()) {
		return;
	}

	// Print out the type and title info, if any.
	PrintIndent(ident++);
	cout << "Type: "<< element.GetType();
	if (element.HasTitle()) {
		cout << ". Title: "<< element.GetTitle();
	}

	int num = element.GetNumKids();
	for (int i=0; i<num; ++i) 
	{
		// Check is the kid is a leaf node (i.e. it is a ContentItem).
		if (element.IsContentItem(i)) { 
			Struct::ContentItem cont = element.GetAsContentItem(i); 
			Struct::ContentItem::Type type = cont.GetType();

			Page page = cont.GetPage();

			PrintIndent(ident);
			cout << "Content Item. Part of page #" << page.GetIndex();

			PrintIndent(ident);
			switch (type) {
				case Struct::ContentItem::e_MCID:
				case Struct::ContentItem::e_MCR:
					cout << "MCID: " << cont.GetMCID();
					break;
				case Struct::ContentItem::e_OBJR:
					{
						cout << "OBJR ";
						if (SDF::Obj ref_obj = cont.GetRefObj())
							cout << "- Referenced Object#: " << ref_obj.GetObjNum();
					}
					break;
				default: 
					break;
			}
		}
		else {  // the kid is another StructElement node.
			ProcessStructElement(element.GetAsStructElem(i), ident);
		}
	}
}

// Used in code snippet 2.
void ProcessElements(ElementReader& reader) 
{
   Element element;
	while (element = reader.Next()) 	// Read page contents
	{
		// In this sample we process only paths & text, but the code can be 
		// extended to handle any element type.
		Element::Type type = element.GetType();
		if (type == Element::e_path || type == Element::e_text || type == Element::e_path) 
		{   
			switch (type)	{
			case Element::e_path:				// Process path ...
				cout << "\nPATH: ";
				break; 
			case Element::e_text: 				// Process text ...
				cout << "\nTEXT: " << element.GetTextString() << endl;
				break;
			case Element::e_form:				// Process form XObjects
				cout << "\nFORM XObject: ";
				//reader.FormBegin(); 
				//ProcessElements(reader);
				//reader.End(); 
				break; 
			}

			// Check if the element is associated with any structural element.
			// Content items are leaf nodes of the structure tree.
			Struct::SElement struct_parent = element.GetParentStructElement();
			if (struct_parent.IsValid()) {
				// Print out the parent structural element's type, title, and object number.
				cout << " Type: " << struct_parent.GetType() 
					<< ", MCID: " << element.GetStructMCID();
				if (struct_parent.HasTitle()) {
					cout << ". Title: "<< struct_parent.GetTitle();
				}
				cout << ", Obj#: " << struct_parent.GetSDFObj().GetObjNum();
			}
		}
	}
}

// Used in code snippet 3.
typedef map<int, string> MCIDPageMap;
typedef map<int, MCIDPageMap> MCIDDocMap;

// Used in code snippet 3.
void ProcessElements2(ElementReader& reader, MCIDPageMap& mcid_page_map) 
{
   Element element;
	while (element = reader.Next()) // Read page contents
	{
		// In this sample we process only text, but the code can be extended 
		// to handle paths, images, or any other Element type.
		int mcid = element.GetStructMCID();
		if (mcid>= 0 && element.GetType() == Element::e_text) {
			string val = element.GetTextString().ConvertToAscii();
			MCIDPageMap::iterator itr = mcid_page_map.find(mcid);
			if (itr != mcid_page_map.end()) itr->second += val; 
			else mcid_page_map.insert(MCIDPageMap::value_type(mcid, val));
		}
	}
}

// Used in code snippet 3.
void ProcessStructElement2(Struct::SElement element, MCIDDocMap& mcid_doc_map, int ident) 
{
	if (!element.IsValid()) {
		return;
	}

	// Print out the type and title info, if any.
	PrintIndent(ident);
	cout << "<" << element.GetType();
	if (element.HasTitle()) {
		cout << " title=\""<< element.GetTitle() << "\"";
	}
	cout << ">";

	int num = element.GetNumKids();
	for (int i=0; i<num; ++i) 
	{		
		if (element.IsContentItem(i)) { 
			Struct::ContentItem cont = element.GetAsContentItem(i); 
			if (cont.GetType() == Struct::ContentItem::e_MCID) {
				int page_num = cont.GetPage().GetIndex();
				MCIDDocMap::iterator itr = mcid_doc_map.find(page_num);
				if (itr!=mcid_doc_map.end()) {
					MCIDPageMap& mcid_page_map = itr->second;
					MCIDPageMap::iterator itr2 = mcid_page_map.find(cont.GetMCID());
					if (itr2 != mcid_page_map.end()) {
						cout << itr2->second; 
					}                    
				}
			}
		}
		else {  // the kid is another StructElement node.
			ProcessStructElement2(element.GetAsStructElem(i), mcid_doc_map, ident+1);
		}
	}

	PrintIndent(ident);
	cout << "</" << element.GetType() << ">";
}


int main(int argc, char *argv[])
{
	int ret = 0;
	PDFNet::Initialize();

	// Relative path to the folder containing test files.
	string input_path =  "../../TestFiles/";
	string output_path = "../../TestFiles/Output/";

	try	// Extract logical structure from a PDF document
	{
		PDFDoc doc((input_path + "tagged.pdf").c_str());
		doc.InitSecurityHandler();

		cout << "____________________________________________________________" << endl;
		cout << "Sample 1 - Traverse logical structure tree..." << endl;
		{
			Struct::STree tree = doc.GetStructTree();
			if (tree.IsValid()) {
				cout << "Document has a StructTree root." << endl;				

				for (int i=0; i<tree.GetNumKids(); ++i) {
					// Recursively get structure info for all child elements.
					ProcessStructElement(tree.GetKid(i), 0);
				}
			}
			else {
				cout << "This document does not contain any logical structure." << endl;
			}
		}
		cout << "\nDone 1." << endl;

		cout << "____________________________________________________________" << endl;
		cout << "Sample 2 - Get parent logical structure elements from" << endl;
		cout << "layout elements." << endl;
		{
			ElementReader reader;
			for (PageIterator itr = doc.GetPageIterator(); itr.HasNext(); itr.Next()) {				
				reader.Begin(itr.Current());
				ProcessElements(reader);
				reader.End();
			}
		}
		cout << "\nDone 2." << endl;

		cout << "____________________________________________________________" << endl;
		cout << "Sample 3 - 'XML style' extraction of PDF logical structure and page content." << endl;
		{
			MCIDDocMap mcid_doc_map;
			ElementReader reader;
			for (PageIterator itr = doc.GetPageIterator(); itr.HasNext(); itr.Next()) {				
				reader.Begin(itr.Current());
				pair<MCIDDocMap::iterator, bool> r = mcid_doc_map.insert(MCIDDocMap::value_type(itr.Current().GetIndex(), MCIDPageMap()));
				MCIDPageMap& page_mcid_map = (r.first)->second;
				ProcessElements2(reader, page_mcid_map);
				reader.End();
			}

			Struct::STree tree = doc.GetStructTree();
			if (tree.IsValid()) {
				for (int i=0; i<tree.GetNumKids(); ++i) {
					ProcessStructElement2(tree.GetKid(i), mcid_doc_map, 0);
				}
			}
		}
		cout << "\nDone 3." << endl;

		doc.Save(output_path + "bookmark.pdf", 0);
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

