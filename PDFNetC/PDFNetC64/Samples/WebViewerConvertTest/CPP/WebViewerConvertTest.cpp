//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

#include <iostream>
#include <sstream>
#include <PDF/PDFNet.h>
#include <PDF/Convert.h>

void BulkConvertRandomFilesToXod();

//---------------------------------------------------------------------------------------
// The following sample illustrates how to convert PDF, XPS, image, MS Office, and 
// other image document formats to XOD.
//
// Certain file formats such as PDF, generic XPS, EMF, and raster image formats can 
// be directly converted to XOD. Other formats such as MS Office 
// (Word, Excel, Publisher, Powerpoint, etc) can be directly converted via interop. 
// These types of conversions guarantee optimal output, while preserving important 
// information such as document metadata, intra document links and hyper-links, 
// bookmarks etc. 
//
// In case there is no direct conversion available, PDFNet can still convert from 
// any printable document to XOD using a virtual printer driver. To check 
// if a virtual printer is required use Convert::RequiresPrinter(filename). In this 
// case the installing application must be run as administrator. The manifest for this 
// sample specifies appropriate the UAC elevation. The administrator privileges are 
// not required for direct or interop conversions. 
//
// Please note that PDFNet Publisher (i.e. 'pdftron.PDF.Convert.ToXod') is an
// optionally licensable add-on to PDFNet Core SDK. For details, please see
// http://www.pdftron.com/webviewer/licensing.html.
//---------------------------------------------------------------------------------------

using namespace pdftron;
using namespace PDF;
using namespace std;

UString inputPath("../../TestFiles/");
UString outputPath("../../TestFiles/Output/");

int main(int argc, char *argv[])
{	
	int err = 0;
	try 
	{
		PDFNet::Initialize();
		{
			// Sample 1:
			// Directly convert from PDF to XOD.
			Convert::ToXod(inputPath + "newsletter.pdf", outputPath + "from_pdf.xod");

			// Sample 2:
			// Directly convert from generic XPS to XOD.
			Convert::ToXod(inputPath + "simple-xps.xps", outputPath + "from_xps.xod");

			// Sample 3:
			// Convert from MS Office (does not require printer driver for Office 2007+)
			// and other document formats to XOD.
			BulkConvertRandomFilesToXod();
		}
		PDFNet::Terminate();
	}
	catch(Common::Exception& e)
	{
		std::cout << e << std::endl;
		err = 1;
	}
	catch(...)
	{
		std::cout << "Unknown Exception" << std::endl;
		err = 1;
	}

	cout << "Done.\n";
	return err;
}



typedef struct  
{
	UString inputFile, ouputFile;
	bool requiresWindowsPlatform;
} Testfile;

Testfile testfiles[] = 
{
	{"simple-powerpoint_2007.pptx",	"simple-powerpoint_2007.xod", true},
	{"simple-word_2007.docx",	"simple-word_2007.xod",	true},	
	{"butterfly.png",		"butterfly.xod",	false},
	{"numbered.pdf",		"numbered.xod",		false},
	{"dice.jpg",			"dice.xod",		false},
	{"simple-xps.xps",		"simple-xps.xod",		false}
};

void BulkConvertRandomFilesToXod()
{
	int err = 0;
	if( Convert::Printer::IsInstalled("PDFTron PDFNet") )
	{
		Convert::Printer::SetPrinterName("PDFTron PDFNet");
	}
	else if( !Convert::Printer::IsInstalled() )
	{
		try
		{
			// This will fail if not run as administrator. Harmless if PDFNet 
			// printer already installed
			cout << "Installing printer (requires Windows platform and administrator)\n";
			Convert::Printer::Install();
			cout << "Installed printer " << Convert::Printer::GetPrinterName().ConvertToAscii().c_str() << "\n";
		}
		catch(Common::Exception)
		{
			cout << "Unable to install printer\n";
		}
	}

	unsigned int ceTestfiles = sizeof( testfiles ) / sizeof ( Testfile );

	for( unsigned int i = 0; i < ceTestfiles; i++ )
	{
#ifndef _MSC_VER
		if( testfiles[i].requiresWindowsPlatform)
		{
			continue;
		}
#endif
		try
		{
			UString inputFile = inputPath + testfiles[i].inputFile;
			UString outputFile = outputPath + testfiles[i].ouputFile;
			if( Convert::RequiresPrinter(inputFile) )
			{
				cout << "Using PDFNet printer to convert file " << inputFile << "\n";
			}			
			Convert::ToXod(inputFile, outputFile);			
			cout << "Converted file: " << inputFile << "\n            to: " << outputFile << "\n";
		}
		catch(Common::Exception& e)
		{
			cout << "Unable to convert file " << testfiles[i].inputFile.ConvertToAscii().c_str() << "\n";
			cout << e << "\n";
			err = 1;
		}
		catch(...)
		{
			cout << "Unknown Exception" << "\n";
			err = 1;
		}
	}

	if( err ) {
		cout << "ConvertFile failed\n";
	}
	else {
		cout << "ConvertFile succeeded\n";
	}

	if( Convert::Printer::IsInstalled() )
	{
		try 
		{
			cout << "Uninstalling printer (requires Windows platform and administrator)\n";
			Convert::Printer::Uninstall();
			cout << "Uninstalled Printer " << Convert::Printer::GetPrinterName().ConvertToAscii().c_str() << "\n";
		}
		catch (Common::Exception)
		{
			cout << "Unable to uninstall printer\n";
		}
	}
}
