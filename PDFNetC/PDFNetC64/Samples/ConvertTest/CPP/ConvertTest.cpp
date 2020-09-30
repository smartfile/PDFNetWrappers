//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

#include <iostream>
#include <sstream>
#include <PDF/PDFNet.h>
#include <PDF/Convert.h>

//---------------------------------------------------------------------------------------
// The following sample illustrates how to use the PDF::Convert utility class to convert 
// documents and files to PDF, XPS, SVG, or EMF.
//
// Certain file formats such as XPS, EMF, PDF, and raster image formats can be directly 
// converted to PDF or XPS. Other formats are converted using a virtual driver. To check 
// if ToPDF (or ToXPS) require that PDFNet printer is installed use Convert::RequiresPrinter(filename). 
// The installing application must be run as administrator. The manifest for this sample 
// specifies appropriate the UAC elevation.
//
// Note: the PDFNet printer is a virtual XPS printer supported on Vista SP1 and Windows 7.
// For Windows XP SP2 or higher, or Vista SP0 you need to install the XPS Essentials Pack (or 
// equivalent redistributables). You can download the XPS Essentials Pack from:
//		http://www.microsoft.com/downloads/details.aspx?FamilyId=B8DCFFDD-E3A5-44CC-8021-7649FD37FFEE&displaylang=en
// Windows XP Sp2 will also need the Microsoft Core XML Services (MSXML) 6.0:
// 		http://www.microsoft.com/downloads/details.aspx?familyid=993C0BCF-3BCF-4009-BE21-27E85E1857B1&displaylang=en
//
// Note: Convert.FromEmf and Convert.ToEmf will only work on Windows and require GDI+.
//
// Please contact us if you have any questions.	
//---------------------------------------------------------------------------------------

using namespace pdftron;
using namespace PDF;
using namespace std;

UString inputPath("../../TestFiles/");
UString outputPath("../../TestFiles/Output/");

typedef struct  
{
	UString inputFile, outputFile;
	bool requiresWindowsPlatform;
}
Testfile;

Testfile testfiles[] = 
{
	{ "simple-word_2007.docx",		"docx2pdf.pdf",		true	},
	{ "simple-powerpoint_2007.pptx",	"pptx2pdf.pdf",		true	},
	{ "simple-excel_2007.xlsx",		"xlsx2pdf.pdf",		true	},
	{ "simple-publisher.pub",		"pub2pdf.pdf",		true	},
	// { "simple-visio.vsd",		"vsd2pdf.pdf",		true	},	// requires Microsoft Office Visio
	{ "simple-text.txt",			"txt2pdf.pdf",		true	},
	{ "simple-rtf.rtf",			"rtf2pdf.pdf",		true	},
	{ "butterfly.png",			"png2pdf.pdf",		false	},
	{ "simple-emf.emf",			"emf2pdf.pdf",		true	},
	{ "simple-xps.xps",			"xps2pdf.pdf",		false	},
	// { "simple-webpage.mht",		"mht2pdf.pdf",		true	},
	{ "simple-webpage.html",		"html2pdf.pdf",		true	}
};

int ConvertSpecificFormats();  // convert to/from PDF, XPS, EMF, SVG
int ConvertToPdfFromFile();	   // convert from a file to PDF automatically

int main(int argc, char *argv[])
{	
	// The first step in every application using PDFNet is to initialize the 
	// library. The library is usually initialized only once, but calling 
	// Initialize() multiple times is also fine.
	int err = 0;

	PDFNet::Initialize();

	// Demonstrate Convert::ToPdf and Convert::Printer
	err = ConvertToPdfFromFile();
	if (err)
	{
		cout << "ConvertFile failed" << endl;
	}
	else
	{
		cout << "ConvertFile succeeded" << endl;
	}

	// Demonstrate Convert::[FromEmf, FromXps, ToEmf, ToSVG, ToXPS]
	err = ConvertSpecificFormats();
	if (err)
	{
		cout << "ConvertSpecificFormats failed" << endl;
	}
	else
	{
		cout << "ConvertSpecificFormats succeeded" << endl;
	}
#if defined(_WIN32)
	if (Convert::Printer::IsInstalled())
	{
		try 
		{
			cout << "Uninstalling printer (requires Windows platform and administrator)" << endl;
			Convert::Printer::Uninstall();
			cout << "Uninstalled Printer " << Convert::Printer::GetPrinterName().ConvertToAscii().c_str() << endl;;
		}
		catch (Common::Exception)
		{
			cout << "Unable to uninstall printer" << endl;
		}
	}
#endif // defined(_WIN32)
	PDFNet::Terminate();
	cout << "Done.\n";
	return err;
}

int ConvertToPdfFromFile()
{
	int ret = 0;
#if defined(_WIN32)
	if( Convert::Printer::IsInstalled("PDFTron PDFNet") )
	{
		Convert::Printer::SetPrinterName("PDFTron PDFNet");
	}
	else if (!Convert::Printer::IsInstalled())
	{
		try
		{
			// This will fail if not run as administrator. Harmless if PDFNet 
			// printer already installed
			cout << "Installing printer (requires Windows platform and administrator)\n";
			Convert::Printer::Install();
			cout << "Installed printer " << Convert::Printer::GetPrinterName().ConvertToAscii().c_str() << endl;
		}
		catch (Common::Exception)
		{
			cout << "Unable to install printer" << endl;
		}
	}
#endif // defined(_WIN32)

	unsigned int ceTestfiles = sizeof (testfiles) / sizeof (Testfile);

	for (unsigned int i = 0; i < ceTestfiles; i++)
	{
#ifndef _MSC_VER
		if (testfiles[i].requiresWindowsPlatform)
		{
			continue;
		}
#endif
		try
		{
			PDFDoc pdfdoc;
			UString inputFile = inputPath + testfiles[i].inputFile;
			UString outputFile = outputPath + testfiles[i].outputFile;
			if (Convert::RequiresPrinter(inputFile))
			{
				cout << "Using PDFNet printer to convert file " << testfiles[i].inputFile << endl;
			}
			Convert::ToPdf(pdfdoc, inputFile);
			pdfdoc.Save(outputFile, SDF::SDFDoc::e_linearized, NULL);
			cout << "Converted file: " << testfiles[i].inputFile << endl << "to: " << testfiles[i].outputFile << endl;
		}
		catch (Common::Exception& e)
		{
			cout << "Unable to convert file " << testfiles[i].inputFile.ConvertToAscii().c_str() << endl;
			cout << e << endl;
			ret = 1;
		}
		catch (...)
		{
			cout << "Unknown Exception" << endl;
			ret = 1;
		}
	}

	return ret;
}

int ConvertSpecificFormats()
{
	//////////////////////////////////////////////////////////////////////////
	int ret = 0;
	try
	{
		PDFDoc pdfdoc;

		cout << "Converting from XPS" << endl;
		Convert::FromXps(pdfdoc, inputPath + "simple-xps.xps");
		pdfdoc.Save(outputPath + "xps2pdf v2.pdf", SDF::SDFDoc::e_remove_unused, NULL);
		cout << "Saved xps2pdf v2.pdf" << endl;
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

	//////////////////////////////////////////////////////////////////////////
#ifdef _MSC_VER
	try
	{
		PDFDoc pdfdoc;

		cout << "Converting from EMF" << endl;
		Convert::FromEmf(pdfdoc, inputPath + "simple-emf.emf");
		pdfdoc.Save(outputPath + "emf2pdf v2.pdf", SDF::SDFDoc::e_remove_unused, NULL);
		cout << "Saved emf2pdf v2.pdf" << endl;
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
#endif //  _MSC_VER

	//////////////////////////////////////////////////////////////////////////
	try
	{
		PDFDoc pdfdoc;

		// Add a dictionary
		SDF::ObjSet set;
		SDF::Obj options = set.CreateDict();

		// Put options
		options.PutNumber("FontSize", 15);
		options.PutBool("UseSourceCodeFormatting", true);
		options.PutNumber("PageWidth", 12);
		options.PutNumber("PageHeight", 6);

		// Convert from .txt file
		cout << "Converting from txt" << endl;
		Convert::FromText(pdfdoc, inputPath + "simple-text.txt", options);
		pdfdoc.Save(outputPath + "simple-text.pdf", SDF::SDFDoc::e_remove_unused, NULL);
		cout << "Saved simple-text.pdf" << endl;
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

	//////////////////////////////////////////////////////////////////////////
	try
	{
		PDFDoc pdfdoc(inputPath + "newsletter.pdf");

		// Convert PDF document to SVG
		cout << "Converting pdfdoc to SVG" << endl;
		Convert::ToSvg(pdfdoc, outputPath + "pdf2svg v2.svg");
		cout << "Saved pdf2svg v2.svg" << endl;
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

	//////////////////////////////////////////////////////////////////////////
	try
	{
		// Convert PNG image to XPS
		cout << "Converting PNG to XPS" << endl;
		Convert::ToXps(inputPath + "butterfly.png", outputPath + "butterfly.xps");
		cout << "Saved butterfly.xps" << endl;
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

	//////////////////////////////////////////////////////////////////////////
#ifdef _MSC_VER
	try
	{
		// Convert MSWord document to XPS
		cout << "Converting DOCX to XPS" << endl;
		Convert::ToXps(inputPath + "simple-word_2007.docx", outputPath + "simple-word_2007.xps");
		cout << "Saved simple-word_2007.xps" << endl;
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
#endif //  _MSC_VER

	//////////////////////////////////////////////////////////////////////////
	try
	{
		// Convert PDF document to XPS
		cout << "Converting PDF to XPS" << endl;
		Convert::ToXps(inputPath + "newsletter.pdf", outputPath + "newsletter.xps");
		cout << "Saved newsletter.xps" << endl;
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

	//////////////////////////////////////////////////////////////////////////
	try
	{
		// Convert PDF document to HTML
		cout << "Converting PDF to HTML" << endl;
		Convert::ToHtml(inputPath + "newsletter.pdf", outputPath + "newsletter");
		cout << "Saved newsletter as HTML" << endl;
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

	//////////////////////////////////////////////////////////////////////////
	try
	{
		// Convert PDF document to EPUB
		cout << "Converting PDF to EPUB" << endl;
		Convert::ToEpub(inputPath + "newsletter.pdf", outputPath + "newsletter.epub");
		cout << "Saved newsletter.epub" << endl;
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

	//////////////////////////////////////////////////////////////////////////
	try
	{
		// Convert PDF document to multipage TIFF
		cout << "Converting PDF to multipage TIFF" << endl;
		Convert::TiffOutputOptions tiff_options;
		tiff_options.SetDPI(200);
		tiff_options.SetDither(true);
		tiff_options.SetMono(true);
		Convert::ToTiff(inputPath + "newsletter.pdf", outputPath + "newsletter.tiff", tiff_options);
		cout << "Saved newsletter.tiff" << endl;
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

	return ret;
}
