//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

#include <PDF/PDFNet.h>
#include <PDF/PDFDoc.h>
#include <PDF/PageLabel.h>

#include <iostream>

using namespace std;
using namespace pdftron;
using namespace PDF;

//-----------------------------------------------------------------------------------
// The sample illustrates how to work with PDF page labels.
//
// PDF page labels can be used to describe a page. This is used to 
// allow for non-sequential page numbering or the addition of arbitrary 
// labels for a page (such as the inclusion of Roman numerals at the 
// beginning of a book). PDFNet PageLabel object can be used to specify 
// the numbering style to use (for example, upper- or lower-case Roman, 
// decimal, and so forth), the starting number for the first page,
// and an arbitrary prefix to be pre-appended to each number (for 
// example, "A-" to generate "A-1", "A-2", "A-3", and so forth.)
//-----------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	int ret = 0;
	PDFNet::Initialize();

	// Relative path to the folder containing test files.
	string input_path =  "../../TestFiles/";
	string output_path = "../../TestFiles/Output/";

	try  
	{	
		//-----------------------------------------------------------
		// Example 1: Add page labels to an existing or newly created PDF
		// document.
		//-----------------------------------------------------------
		{
			PDFDoc doc((input_path + "newsletter.pdf").c_str());
			doc.InitSecurityHandler();

			// Create a page labeling scheme that starts with the first page in 
			// the document (page 1) and is using uppercase roman numbering 
			// style. 
			PageLabel L1 = PageLabel::Create(doc, PageLabel::e_roman_uppercase, "My Prefix ", 1);
			doc.SetPageLabel(1, L1);

			// Create a page labeling scheme that starts with the fourth page in 
			// the document and is using decimal Arabic numbering style. 
			// Also the numeric portion of the first label should start with number 
			// 4 (otherwise the first label would be "My Prefix 1"). 
			PageLabel L2 = PageLabel::Create(doc, PageLabel::e_decimal, "My Prefix ", 4);
			doc.SetPageLabel(4, L2);

			// Create a page labeling scheme that starts with the seventh page in 
			// the document and is using alphabetic numbering style. The numeric 
			// portion of the first label should start with number 1. 
			PageLabel L3 = PageLabel::Create(doc, PageLabel::e_alphabetic_uppercase, "My Prefix ", 1);
			doc.SetPageLabel(7, L3);

			doc.Save((output_path + "newsletter_with_pagelabels.pdf").c_str(), SDF::SDFDoc::e_linearized, 0);
			cout << "Done. Result saved in newsletter_with_pagelabels.pdf..." << endl;
		}

		//-----------------------------------------------------------
		// Example 2: Read page labels from an existing PDF document.
		//-----------------------------------------------------------
		{
			PDFDoc doc((output_path + "newsletter_with_pagelabels.pdf").c_str());
			doc.InitSecurityHandler();

			PageLabel label;
			int page_num = doc.GetPageCount();
			for (int i=1; i<=page_num; ++i) 
			{
				cout << "Page number: " << i; 
				label = doc.GetPageLabel(i);
				if (label.IsValid()) {
					cout << " Label: " << label.GetLabelTitle(i) << endl; 
				}
				else {
					cout << " No Label." << endl; 
				}
			}
		}

		//-----------------------------------------------------------
		// Example 3: Modify page labels from an existing PDF document.
		//-----------------------------------------------------------
		{
			PDFDoc doc((output_path + "newsletter_with_pagelabels.pdf").c_str());
			doc.InitSecurityHandler();

			// Remove the alphabetic labels from example 1.
			doc.RemovePageLabel(7); 

			// Replace the Prefix in the decimal labels (from example 1).
			PageLabel label = doc.GetPageLabel(4);
			if (label.IsValid()) {
				label.SetPrefix("A");
				label.SetStart(1);
			}

			// Add a new label
			PageLabel new_label = PageLabel::Create(doc, PageLabel::e_decimal, "B", 1);
			doc.SetPageLabel(10, new_label);  // starting from page 10.

			doc.Save((output_path + "newsletter_with_pagelabels_modified.pdf").c_str(), SDF::SDFDoc::e_linearized, 0);
			cout << "Done. Result saved in newsletter_with_pagelabels_modified.pdf..." << endl;

			int page_num = doc.GetPageCount();
			for (int i=1; i<=page_num; ++i) 
			{
				cout << "Page number: " << i; 
				label = doc.GetPageLabel(i);
				if (label.IsValid()) {
					cout << " Label: " << label.GetLabelTitle(i) << endl; 
				}
				else {
					cout << " No Label." << endl; 
				}
			}
		}

		//-----------------------------------------------------------
		// Example 4: Delete all page labels in an existing PDF document.
		//-----------------------------------------------------------
		{
			PDFDoc doc((output_path + "newsletter_with_pagelabels.pdf").c_str());
			doc.GetRoot().Erase("PageLabels");
			// ...
		}
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
