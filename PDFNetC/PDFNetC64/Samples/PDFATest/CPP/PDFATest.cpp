//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

#include <PDF/PDFNet.h>
//#include <PDF/PDFDoc.h>
#include <PDF/PDFA/PDFACompliance.h>
#include <string>
#include <iostream>

using namespace std;
using namespace pdftron;
using namespace pdftron::PDF;
using namespace pdftron::PDF::PDFA;

void PrintResults(PDFACompliance& pdf_a, UString filename) 
{
	int err_cnt = static_cast<int>(pdf_a.GetErrorCount());
	if (err_cnt == 0) 
	{
		cout << filename << ": OK.\n";
	}
	else 
	{
		cout << filename << " is NOT a valid PDFA.\n";
		for (int i=0; i<err_cnt; ++i) 
		{
			PDFACompliance::ErrorCode c = pdf_a.GetError(i);
			cout << " - e_PDFA " << c << ": " << PDFACompliance::GetPDFAErrorMessage(c) << ".\n";
			if (true) 
			{
				int num_refs = static_cast<int>(pdf_a.GetRefObjCount(c));
				if (num_refs > 0)  
				{
					cout << "   Objects: ";
					for (int j=0; j<num_refs; ++j) 
					{
						cout << pdf_a.GetRefObj(c, j);
						if (j<num_refs-1) 
							cout << ", ";
					}
					cout << endl;
				}
			}
		}
		cout << endl;
	}
}



//---------------------------------------------------------------------------------------
// The following sample illustrates how to parse and check if a PDF document meets the
//	PDFA standard, using the PDFACompliance class object. 
//---------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{	
	int ret = 0;
	UString input_path("../../TestFiles/");
	UString output_path("../../TestFiles/Output/");
	PDFNet::Initialize();
	PDFNet::SetColorManagement();  // Enable color management (required for PDFA validation).

	//-----------------------------------------------------------
	// Example 1: PDF/A Validation
	//-----------------------------------------------------------
	try
	{
		UString filename("newsletter.pdf");
		/* The max_ref_objs parameter to the PDFACompliance constructor controls the maximum number 
		of object numbers that are collected for particular error codes. The default value is 10 
		in order to prevent spam. If you need all the object numbers, pass 0 for max_ref_objs. */
		PDFACompliance pdf_a(false, input_path+filename, 0, PDFACompliance::e_Level2B, 0, 0, 10);
		PrintResults(pdf_a, filename);
	}
	catch (Common::Exception& e)
	{
		cout << e << endl;
		ret = 1;
	}
	catch (...) {
		cout << "Unknown Exception" << endl;
		ret = 1;
	}

	//-----------------------------------------------------------
	// Example 2: PDF/A Conversion
	//-----------------------------------------------------------
	try
	{
		UString filename("fish.pdf");
		PDFACompliance pdf_a(true, input_path+filename, 0, PDFACompliance::e_Level2B, 0, 0, 10);
		filename = "pdfa.pdf";
		pdf_a.SaveAs(output_path + filename);

		// Re-validate the document after the conversion...
		PDFACompliance comp(false, output_path + filename, 0, PDFACompliance::e_Level2B, 0, 0, 10);
		PrintResults(comp, filename);				
	}
	catch (Common::Exception& e)
	{
		cout << e << endl;
		ret = 1;
	}
	catch (...) {
		cout << "Unknown Exception" << endl;
		ret = 1;
	}

	cout << "PDFACompliance test completed." << endl;
	PDFNet::Terminate();
	return ret;
}
