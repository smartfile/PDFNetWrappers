//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------
#include <PDF/PDFNet.h>
#include <PDF/PDFDoc.h>
#include <PDF/PDFDocInfo.h>
#include <iostream>

using namespace std;

using namespace pdftron;
using namespace SDF;
using namespace PDF;
using namespace Filters;


int main(int argc, char *argv[])
{
	int ret = 0;
	PDFNet::Initialize();

	// Relative path to the folder containing test files.
	string input_path =  "../../TestFiles/";
	string output_path = "../../TestFiles/Output/";

	// Sample 1 - Split a PDF document into multiple pages
	try
	{
		cout << "_______________________________________________" << endl;
		cout << "Sample 1 - Split a PDF document into multiple pages..." << endl;
		cout << "Opening the input pdf..." << endl;
		PDFDoc in_doc((input_path +  "newsletter.pdf").c_str());
		in_doc.InitSecurityHandler();

		int page_num = in_doc.GetPageCount();
		for (int i=1; i<=page_num; ++i)
		{
			PDFDoc new_doc;
			char fname[256];
			sprintf(fname, "newsletter_split_page_%d.pdf", i);
			string output_file(output_path + fname);
			new_doc.InsertPages(0, in_doc, i, i, PDFDoc::e_none);
			new_doc.Save(output_file, SDFDoc::e_remove_unused, 0);
			cout << "Done. Result saved in " << fname << endl;
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

	// Sample 2 - Merge several PDF documents into one
	try
	{
		cout << "_______________________________________________" << endl;
		cout << "Sample 2 - Merge several PDF documents into one..." << endl;
		PDFDoc new_doc;
		new_doc.InitSecurityHandler();

		int page_num = 15;
		for (int i=1; i<=page_num; ++i)
		{
			char fname[256];
			sprintf(fname, "newsletter_split_page_%d.pdf", i);
			string input_file(output_path + fname);
			cout << "Opening " << fname << endl;
			PDFDoc in_doc(input_file);
			new_doc.InsertPages(i, in_doc, 1, in_doc.GetPageCount(), PDFDoc::e_none);
		}
		new_doc.Save(output_path + "newsletter_merge_pages.pdf", SDFDoc::e_remove_unused, 0);
		cout << "Done. Result saved in newsletter_merge_pages.pdf" << endl;
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


	// Sample 3 - Delete every second page
	try	
	{
		cout << "_______________________________________________" << endl;
		cout << "Sample 3 - Delete every second page..." << endl;
		cout << "Opening the input pdf..." << endl;
		PDFDoc in_doc((input_path +  "newsletter.pdf").c_str());
		in_doc.InitSecurityHandler();
		
		int page_num = in_doc.GetPageCount();
		while (page_num>=1)
		{
			PageIterator itr = in_doc.GetPageIterator(page_num);
			in_doc.PageRemove(itr);
			page_num -= 2;
		}		
		
		in_doc.Save((output_path +  "newsletter_page_remove.pdf").c_str(), 0, 0);
		cout << "Done. Result saved in newsletter_page_remove.pdf..." << endl;
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
	
	// Sample 4 - Inserts a page from one document at different 
	// locations within another document
	try
	{	 
		cout << "_______________________________________________" << endl;
		cout << "Sample 4 - Insert a page at different locations..." << endl;
		cout << "Opening the input pdf..." << endl;
		
		PDFDoc in1_doc((input_path +  "newsletter.pdf").c_str());
		in1_doc.InitSecurityHandler();

		PDFDoc in2_doc((input_path + "fish.pdf").c_str());
		in2_doc.InitSecurityHandler(); 
		
		PageIterator src_page = in2_doc.GetPageIterator();
        PageIterator dst_page = in1_doc.GetPageIterator();
        int page_num = 1;
        while (dst_page.HasNext()) {
            if (page_num++ % 3 == 0) {
                in1_doc.PageInsert(dst_page, src_page.Current());
            }
            dst_page.Next();
        }
		in1_doc.Save((output_path +  "newsletter_page_insert.pdf").c_str(), 0 , NULL);
		cout << "Done. Result saved in newsletter_page_insert.pdf..." << endl;
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

	// Sample 5 - Replicate pages within a single document
	try	
	{
		cout << "_______________________________________________" << endl;
		cout << "Sample 5 - Replicate pages within a single document..." << endl;
		cout << "Opening the input pdf..." << endl;
		PDFDoc doc((input_path +  "newsletter.pdf").c_str());
		doc.InitSecurityHandler();
		
		// Replicate the cover page three times (copy page #1 and place it before the 
		// seventh page in the document page sequence)
		Page cover = doc.GetPage(1);
		PageIterator p7 = doc.GetPageIterator(7);
		doc.PageInsert(p7, cover);
		doc.PageInsert(p7, cover);
		doc.PageInsert(p7, cover);
		
		// Replicate the cover page two more times by placing it before and after
		// existing pages.
		doc.PagePushFront(cover);
		doc.PagePushBack(cover);
		
		doc.Save((output_path +  "newsletter_page_clone.pdf").c_str(), 0, NULL);
		cout << "Done. Result saved in newsletter_page_clone.pdf..." << endl;
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
	
	// Sample 6 - Use ImportPages() in order to copy multiple pages at once 
	// in order to preserve shared resources between pages (e.g. images, fonts, 
	// colorspaces, etc.)
	try
	{
		cout << "_______________________________________________" << endl;
		cout << "Sample 6 - Preserving shared resources using ImportPages..." << endl;
		cout << "Opening the input pdf..." << endl;
		PDFDoc in_doc((input_path +  "newsletter.pdf").c_str());
		in_doc.InitSecurityHandler();

		PDFDoc new_doc;
		
		vector<Page> copy_pages; 
		for (PageIterator itr=in_doc.GetPageIterator(); itr.HasNext(); itr.Next())
		{
			copy_pages.push_back(itr.Current());
		}
		
		vector<Page> imported_pages = new_doc.ImportPages(copy_pages);
		vector<Page>::iterator i;
		for (i=imported_pages.begin(); i!=imported_pages.end(); ++i)
		{
			new_doc.PagePushFront(*i); // Order pages in reverse order. 
			// Use PagePushBack() if you would like to preserve the same order.
		}		
		
		new_doc.Save((output_path +  "newsletter_import_pages.pdf").c_str(), 0, NULL);
		cout << "Done. Result saved in newsletter_import_pages.pdf..." << endl << endl
			<<	"Note that the output file size is less than half the size" << endl 
			<< "of the file produced using individual page copy operations" << endl 
			<< "between two documents" << endl;
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
