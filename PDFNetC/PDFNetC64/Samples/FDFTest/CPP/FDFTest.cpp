//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

#include <PDF/PDFNet.h>
#include <PDF/PDFDoc.h>
#include <PDF/Field.h>
#include <FDF/FDFDoc.h>
#include <FDF/FDFField.h>
#include <iostream>

using namespace std;
using namespace pdftron;
using namespace SDF;
using namespace FDF;
using namespace PDF;

//---------------------------------------------------------------------------------------
// PDFNet includes a full support for FDF (Forms Data Format) and capability to merge/extract 
// forms data (FDF) with/from PDF. This sample illustrates basic FDF merge/extract functionality 
// available in PDFNet.
//---------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	int ret = 0;
	PDFNet::Initialize();

	// Relative path to the folder containing test files.
	string input_path =  "../../TestFiles/";
	string output_path = "../../TestFiles/Output/";


	// Example 1)
	// Iterate over all form fields in the document. Display all field names.
	try  
	{
		PDFDoc doc((input_path + "form1.pdf").c_str());
		doc.InitSecurityHandler();

		for(FieldIterator itr = doc.GetFieldIterator(); itr.HasNext(); itr.Next()) 
		{
			cout << "Field name: " << itr.Current().GetName() << endl;
			cout << "Field partial name: " << itr.Current().GetPartialName() << endl;

			cout << "Field type: ";
			Field::Type type = itr.Current().GetType();
			switch(type)
			{
			case Field::e_button: cout << "Button" << endl; break;
			case Field::e_text: cout << "Text" << endl; break;
			case Field::e_choice: cout << "Choice" << endl; break;
			case Field::e_signature: cout << "Signature" << endl; break;
			}

			cout << "------------------------------" << endl;
		}

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


	// Example 2) Import XFDF into FDF, then merge data from FDF into PDF
	try
	{
		// XFDF to FDF
		// form fields
		cout << "Import form field data from XFDF to FDF." << endl;
		
		FDFDoc fdf_doc1(FDFDoc::CreateFromXFDF((input_path + "form1_data.xfdf").c_str()));
		fdf_doc1.Save((output_path + "form1_data.fdf").c_str());

		// annotations
		cout << "Import annotations from XFDF to FDF." << endl;

		FDFDoc fdf_doc2(FDFDoc::CreateFromXFDF((input_path + "form1_annots.xfdf").c_str()));
		fdf_doc2.Save((output_path + "form1_annots.fdf").c_str());

		// FDF to PDF
		// form fields
		cout << "Merge form field data from FDF." << endl;

		PDFDoc doc((input_path + "form1.pdf").c_str());
		doc.InitSecurityHandler();
		doc.FDFMerge(fdf_doc1);
		
		// Refreshing missing appearances is not required here, but is recommended to make them 
		// visible in PDF viewers with incomplete annotation viewing support. (such as Chrome)
		doc.RefreshAnnotAppearances();

		doc.Save((output_path + "form1_filled.pdf").c_str(), SDFDoc::e_linearized, 0);

		// annotations
		cout << "Merge annotations from FDF." << endl;

		doc.FDFMerge(fdf_doc2);
		// Refreshing missing appearances is not required here, but is recommended to make them 
		// visible in PDF viewers with incomplete annotation viewing support. (such as Chrome)
		doc.RefreshAnnotAppearances();
		doc.Save((output_path + "form1_filled_with_annots.pdf").c_str(), SDFDoc::e_linearized, 0);
		cout << "Done." << endl;
	}
	catch (Common::Exception& e)
	{
		cout << e << endl;
		ret = 1;
	}
	catch (...)
	{
		cout << "Unknown Exception" << endl;
		ret = 1;
	}


	// Example 3) Extract data from PDF to FDF, then export FDF as XFDF
	try  
	{
		// PDF to FDF
		PDFDoc in_doc((output_path + "form1_filled_with_annots.pdf").c_str());
		in_doc.InitSecurityHandler();
		
		// form fields only
		cout << "Extract form fields data to FDF." << endl;

		FDFDoc doc_fields = in_doc.FDFExtract(PDFDoc::e_forms_only);
		doc_fields.SetPDFFileName("../form1_filled_with_annots.pdf");
		doc_fields.Save((output_path + "form1_filled_data.fdf").c_str());

		// annotations only
		cout << "Extract annotations to FDF." << endl;

		FDFDoc doc_annots = in_doc.FDFExtract(PDFDoc::e_annots_only);
		doc_annots.SetPDFFileName("../form1_filled_with_annots.pdf");
		doc_annots.Save((output_path + "form1_filled_annot.fdf").c_str());

		// both form fields and annotations
		cout << "Extract both form fields and annotations to FDF." << endl;

		FDFDoc doc_both = in_doc.FDFExtract(PDFDoc::e_both);
		doc_both.SetPDFFileName("../form1_filled_with_annots.pdf");
		doc_both.Save((output_path + "form1_filled_both.fdf").c_str());

		// FDF to XFDF
		// form fields
		cout << "Export form field data from FDF to XFDF." << endl;

		doc_fields.SaveAsXFDF((output_path + "form1_filled_data.xfdf").c_str());

		// annotations
		cout << "Export annotations from FDF to XFDF." << endl;

		doc_annots.SaveAsXFDF((output_path + "form1_filled_annot.xfdf").c_str());

		// both form fields and annotations
		cout << "Export both form fields and annotations from FDF to XFDF." << endl;

		doc_both.SaveAsXFDF((output_path + "form1_filled_both.xfdf").c_str());

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

	// Example 4) Merge/Extract XFDF into/from PDF
	try
	{
		// Merge XFDF from string
		PDFDoc in_doc((input_path + "numbered.pdf").c_str());
		in_doc.InitSecurityHandler();

		cout << "Merge XFDF string into PDF." << endl;

		string str = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><xfdf xmlns=\"http://ns.adobe.com/xfdf\" xml:space=\"preserve\"><square subject=\"Rectangle\" page=\"0\" name=\"cf4d2e58-e9c5-2a58-5b4d-9b4b1a330e45\" title=\"user\" creationdate=\"D:20120827112326-07'00'\" date=\"D:20120827112326-07'00'\" rect=\"227.7814207650273,597.6174863387978,437.07103825136608,705.0491803278688\" color=\"#000000\" interior-color=\"#FFFF00\" flags=\"print\" width=\"1\"><popup flags=\"print,nozoom,norotate\" open=\"no\" page=\"0\" rect=\"0,792,0,792\" /></square></xfdf>";

		FDFDoc fdoc(FDFDoc::CreateFromXFDF(str));
		in_doc.FDFMerge(fdoc);
		in_doc.Save((output_path + "numbered_modified.pdf").c_str(), SDFDoc::e_linearized, 0);
		cout << "Merge complete." << endl;

		// Extract XFDF as string
		cout << "Extract XFDF as a string." << endl;

		FDFDoc fdoc_new = in_doc.FDFExtract(PDFDoc::e_both);
		UString XFDF_str = fdoc_new.SaveAsXFDF();
		cout << "Extracted XFDF: " << endl;
		cout << XFDF_str << endl;
		cout << "Extract complete." << endl;
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

	// Example 5) Read FDF files directly
	try  
	{
		FDFDoc doc((output_path + "form1_filled_data.fdf").c_str());

		for(FDFFieldIterator itr = doc.GetFieldIterator(); itr.HasNext(); itr.Next()) 
		{
			cout << "Field name: " << itr.Current().GetName() << endl;
			cout << "Field partial name: " << itr.Current().GetPartialName() << endl;

			cout << "------------------------------" << endl;
		}

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

	// Example 6) Direct generation of FDF.
	try  
	{
		FDFDoc doc;
		// Create new fields (i.e. key/value pairs).
		doc.FieldCreate("Company", PDF::Field::e_text, "PDFTron Systems");
		doc.FieldCreate("First Name", PDF::Field::e_text, "John");
		doc.FieldCreate("Last Name", PDF::Field::e_text, "Doe");
		// ...		

		// doc.SetPdfFileName("mydoc.pdf");

		doc.Save((output_path + "sample_output.fdf").c_str());
		cout << "Done. Results saved in sample_output.fdf" << endl;
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


