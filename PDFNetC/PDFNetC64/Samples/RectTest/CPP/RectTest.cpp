//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

#include <PDF/PDFNet.h>
#include <PDF/PDFDoc.h>
#include <iostream>

using namespace std;

using namespace pdftron;
using namespace PDF;
using namespace Common;


int main(int argc, char *argv[])
{
	int ret = 0;
	PDFNet::Initialize();

	// Relative path to the folder containing test files.
	string input_path =  "../../TestFiles/";
	string output_path = "../../TestFiles/Output/";

	try // Test  - Adjust the position of content within the page.
	{
		cout << "_______________________________________________" << endl;
		cout << "Opening the input pdf..." << endl;

		PDFDoc input_doc((input_path + "tiger.pdf").c_str());
		input_doc.InitSecurityHandler();

		PageIterator pg_itr1 = input_doc.GetPageIterator();

		Rect media_box(pg_itr1.Current().GetMediaBox()); 

		media_box.x1 -= 200;	// translate the page 200 units (1 uint = 1/72 inch)
		media_box.x2 -= 200;

		media_box.Update();	

		input_doc.Save((output_path + "tiger_shift.pdf").c_str(), 0 , NULL);

		cout << "Done. Result saved in tiger_shift..." << endl;
	}
	catch(Exception& e)
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
