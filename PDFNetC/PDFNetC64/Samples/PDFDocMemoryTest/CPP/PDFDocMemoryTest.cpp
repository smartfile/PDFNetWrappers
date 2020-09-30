//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

#include <PDF/PDFNet.h>
#include <PDF/PDFDoc.h>
#include <Filters/MappedFile.h>
#include <Filters/FilterReader.h>
#include <Filters/FilterWriter.h>
#include <PDF/ElementWriter.h>
#include <PDF/ElementReader.h>

#include <iostream>
#include <fstream>

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


	// The following sample illustrates how to read/write a PDF document from/to 
	// a memory buffer.  This is useful for applications that work with dynamic PDF
	// documents that don't need to be saved/read from a disk.
	try  
	{
		// Read a PDF document in a memory buffer.
		MappedFile file((input_path + "tiger.pdf"));
		size_t file_sz = file.FileSize();
        
		FilterReader file_reader(file);

		unsigned char* mem = new unsigned char[file_sz];
		file_reader.Read((unsigned char*)mem, file_sz);
		PDFDoc doc(mem, file_sz);
		delete[] mem;

		doc.InitSecurityHandler();
		int num_pages = doc.GetPageCount();

		ElementWriter writer;
		ElementReader reader;
		Element element;

		// Create a duplicate of every page but copy only path objects
		for(int i=1; i<=num_pages; ++i)
		{
			PageIterator itr = doc.GetPageIterator(2*i-1);

			reader.Begin(itr.Current());
			Page new_page = doc.PageCreate(itr.Current().GetMediaBox());
			PageIterator next_page = itr;
			next_page.Next(); 
			doc.PageInsert(next_page, new_page );

			writer.Begin(new_page);
			while ((element = reader.Next()) !=0) 	// Read page contents
			{
				//if (element.GetType() == Element::e_path)
                writer.WriteElement(element);
			}

			writer.End();
			reader.End();
		}

		doc.Save((output_path + "doc_memory_edit.pdf").c_str(), SDFDoc::e_remove_unused, NULL);
		// doc.Save((output_path + "doc_memory_edit.pdf").c_str(), Doc::e_linearized, NULL);

		// Save the document to a memory buffer.
		const char* buf = 0; 
		size_t buf_sz;

		doc.Save(buf, buf_sz, SDFDoc::e_remove_unused, NULL);
		// doc.Save(buf, buf_sz, Doc::e_linearized, NULL);

		// Write the contents of the buffer to the disk
		{
			ofstream out((output_path + "doc_memory_edit.txt").c_str(), ofstream::binary);
			out.write(buf, buf_sz);
			out.close();
		}

		// Read some data from the file stored in memory
		reader.Begin(doc.GetPage(1));
		while ((element = reader.Next()) !=0) {
			if (element.GetType() == Element::e_path) cout << "Path, ";
		}
		reader.End();

		cout << "\n\nDone. Result saved in doc_memory_edit.pdf and doc_memory_edit.txt ..." << endl;
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
