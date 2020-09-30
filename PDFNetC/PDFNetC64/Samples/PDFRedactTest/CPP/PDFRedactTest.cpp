//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

#include <PDF/PDFNet.h>
#include <PDF/PDFDoc.h>
#include <PDF/Redactor.h>

#include <iostream>
#include <vector>

using namespace std;

using namespace pdftron;
using namespace Common;
using namespace SDF;
using namespace PDF;

// PDF Redactor is a separately licensable Add-on that offers options to remove 
// (not just covering or obscuring) content within a region of PDF. 
// With printed pages, redaction involves blacking-out or cutting-out areas of 
// the printed page. With electronic documents that use formats such as PDF, 
// redaction typically involves removing sensitive content within documents for 
// safe distribution to courts, patent and government institutions, the media, 
// customers, vendors or any other audience with restricted access to the content. 
//
// The redaction process in PDFNet consists of two steps:
// 
//  a) Content identification: A user applies redact annotations that specify the 
// pieces or regions of content that should be removed. The content for redaction 
// can be identified either interactively (e.g. using 'pdftron.PDF.PDFViewCtrl' 
// as shown in PDFView sample) or programmatically (e.g. using 'pdftron.PDF.TextSearch'
// or 'pdftron.PDF.TextExtractor'). Up until the next step is performed, the user 
// can see, move and redefine these annotations.
//  b) Content removal: Using 'pdftron.PDF.Redactor.Redact()' the user instructs 
// PDFNet to apply the redact regions, after which the content in the area specified 
// by the redact annotations is removed. The redaction function includes number of 
// options to control the style of the redaction overlay (including color, text, 
// font, border, transparency, etc.).
// 
// PDFTron Redactor makes sure that if a portion of an image, text, or vector graphics 
// is contained in a redaction region, that portion of the image or path data is 
// destroyed and is not simply hidden with clipping or image masks. PDFNet API can also 
// be used to review and remove metadata and other content that can exist in a PDF 
// document, including XML Forms Architecture (XFA) content and Extensible Metadata 
// Platform (XMP) content.

static void Redact(const string& input, const string& output, const vector<Redactor::Redaction>& vec, Redactor::Appearance app) 
{
	PDFDoc doc(input);
	if (doc.InitSecurityHandler()) {
		Redactor::Redact(doc, vec, app, false, true);
		doc.Save(output, SDFDoc::e_linearized, 0);
	}
}

int main(int argc, char *argv[])
{
	int ret = 0;

	// Relative paths to folders containing test files.
	string input_path =  "../../TestFiles/";
	string output_path = "../../TestFiles/Output/";

	PDFNet::Initialize();

	try  
	{	 
		vector<Redactor::Redaction> vec;
		vec.push_back(Redactor::Redaction(1, Rect(100, 100, 550, 600), false, "Top Secret"));
		vec.push_back(Redactor::Redaction(2, Rect(30, 30, 450, 450), true, "Negative Redaction"));
		vec.push_back(Redactor::Redaction(2, Rect(0, 0, 100, 100), false, "Positive"));
		vec.push_back(Redactor::Redaction(2, Rect(100, 100, 200, 200), false, "Positive"));
		vec.push_back(Redactor::Redaction(2, Rect(300, 300, 400, 400), false, ""));
		vec.push_back(Redactor::Redaction(2, Rect(500, 500, 600, 600), false, ""));
		vec.push_back(Redactor::Redaction(3, Rect(0, 0, 700, 20), false, ""));

		Redactor::Appearance app;
		app.RedactionOverlay = true;
		app.Border = false;
		app.ShowRedactedContentRegions = true;

		Redact(input_path + "newsletter.pdf", output_path + "redacted.pdf", vec, app);

		cout << "Done..." << endl;
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

