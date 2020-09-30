//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

#include <PDF/PDFNet.h>
#include <PDF/PDFDoc.h>
#include <PDF/Convert.h>
#include <PDF/CADModule.h>
#include <PDF/CADConvertOptions.h>
#include <string>
#include <iostream>
#include <stdio.h>

using namespace pdftron;
using namespace pdftron::SDF;
using namespace PDF;
using namespace std;


//---------------------------------------------------------------------------------------
// The following sample illustrates how to convert CAD documents to PDF format 
// 
// The CAD module is an optional PDFNet Add-on that can be used to convert CAD
// documents into PDF documents
//
// The PDFTron SDK CAD module can be downloaded from http://www.pdftron.com/
//---------------------------------------------------------------------------------------

UString inputPath("../../TestFiles/CAD/");
UString outputPath("../../TestFiles/Output/");

bool IsRVTFile(UString inputFile)
{
	bool rvt_input = false;
	if (inputFile.GetLength() > 2)
	{
		if (inputFile.SubStr(inputFile.GetLength() - 3, 3) == "rvt")
		{
			rvt_input = true;
		}
	}
	return rvt_input;
}

int main(int argc, char *argv[])
{
	int ret = 0;
	
	try
	{
		// The first step in every application using PDFNet is to initialize the 
		// library and set the path to common PDF resources. The library is usually 
		// initialized only once, but calling Initialize() multiple times is also fine.
		PDFNet::Initialize();
		PDFNet::AddResourceSearchPath("../../../Lib/");
		if(!CADModule::IsModuleAvailable())
		{
			cout << endl;
			cout << "Unable to run CAD2PDFTest: PDFTron SDK CAD module not available." << endl;
			cout << "---------------------------------------------------------------" << endl;
			cout << "The CAD module is an optional add-on, available for download" << endl;
			cout << "at http://www.pdftron.com/. If you have already downloaded this" << endl;
			cout << "module, ensure that the SDK is able to find the required files" << endl;
			cout << "using the PDFNet::AddResourceSearchPath() function." << endl << endl;
			return 0;
		}

		typedef struct
		{
			UString inputFile, ouputFile;
		} TestFile;
		
		UString inputFileName, outputFileName;
		if (argv[1])
		{
			inputFileName = argv[1];
		}
		else
		{
			inputFileName = "construction drawings color-28.05.18.dwg";
		}
		outputFileName = inputFileName + ".pdf";
		
		TestFile testFiles[] =
		{
			{ inputFileName,	outputFileName},
		};

		unsigned int ceTestFiles = sizeof(testFiles) / sizeof(TestFile);
		for (unsigned int i = 0; i < ceTestFiles; i++)
		{

			// Convert the rest of the samples
			if (IsRVTFile(testFiles[i].inputFile))
			{
				try
				{
					PDFDoc pdfdoc;
					// Convert rvt file with some user options
					CADConvertOptions opts;
					opts.SetPageWidth(800);
					opts.SetPageHeight(600);
					opts.SetRasterDPI(150);

					Convert::FromCAD(pdfdoc, inputPath + testFiles[i].inputFile, &opts);
					pdfdoc.Save(outputPath + testFiles[i].ouputFile, SDF::SDFDoc::e_linearized, NULL);
				}
				catch (Common::Exception& e)
				{
					cout << "Unable to convert file " << testFiles[i].inputFile << endl;
					cout << e << endl;
					ret = 1;
				}
				catch (...)
				{
					cout << "Unknown Exception" << endl;
					ret = 1;
				}
			}
			else
			{
				try
				{
					PDFDoc pdfdoc;
					Convert::FromCAD(pdfdoc, inputPath + testFiles[i].inputFile, NULL);
					pdfdoc.Save(outputPath + testFiles[i].ouputFile, SDF::SDFDoc::e_linearized, NULL);
				}
				catch (Common::Exception& e)
				{
					cout << "Unable to convert file " << testFiles[i].inputFile << endl;
					cout << e << endl;
					ret = 1;
				}
				catch (...)
				{
					cout << "Unknown Exception" << endl;
					ret = 1;
				}
			}
		}

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
