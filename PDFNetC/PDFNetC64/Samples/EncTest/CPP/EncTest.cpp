//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

#include <PDF/PDFNet.h>
#include <PDF/PDFDoc.h>
#include <SDF/SecurityHandler.h>
#include <Filters/FilterReader.h>
#include <Filters/FlateEncode.h>
#include <Filters/MappedFile.h>
#include <iostream>
#include <string>

using namespace std;

using namespace pdftron;
using namespace SDF;
using namespace PDF;
using namespace Filters;


//---------------------------------------------------------------------------------------
// This sample shows encryption support in PDFNet. The sample reads an encrypted document and 
// sets a new SecurityHandler. The sample also illustrates how password protection can 
// be removed from an existing PDF document.
//---------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	int ret = 0;
	PDFNet::Initialize();

	// Relative path to the folder containing test files.
	string input_path =  "../../TestFiles/";
	string output_path = "../../TestFiles/Output/";

	// Example 1: 
	// secure a PDF document with password protection and adjust permissions 
	
	try
	{
		// Open the test file
		cout << "-------------------------------------------------" << endl << "Securing an existing document..." << endl;
		PDFDoc doc((input_path + "fish.pdf").c_str());
		if (!doc.InitSecurityHandler())
		{
			cout << "Document authentication error..." << endl;
			ret = 1;
		}
		
		
		// Perform some operation on the document. In this case we use low level SDF API
		// to replace the content stream of the first page with contents of file 'my_stream.txt'
		if (true)  // Optional
		{
			cout << "Replacing the content stream, use flate compression..." << endl;

			// Get the page dictionary using the following path: trailer/Root/Pages/Kids/0
			Obj page_dict = doc.GetTrailer().Get("Root").Value()
				.Get("Pages").Value()
				.Get("Kids").Value()
				.GetAt(0);

			// Embed a custom stream (file mystream.txt) using Flate compression.
			MappedFile embed_file((input_path + "my_stream.txt"));
			FilterReader mystm(embed_file);
			page_dict.Put("Contents", 
				doc.CreateIndirectStream(mystm,  
				FlateEncode(Filter())));
		}

		//encrypt the document


		// Apply a new security handler with given security settings. 
		// In order to open saved PDF you will need a user password 'test'.
		SecurityHandler new_handler;

		// Set a new password required to open a document
		const char* user_password="test";
		new_handler.ChangeUserPassword(user_password);

		// Set Permissions
		new_handler.SetPermission (SecurityHandler::e_print, true);
		new_handler.SetPermission (SecurityHandler::e_extract_content, false);

		// Note: document takes the ownership of new_handler.
		doc.SetSecurityHandler(new_handler);

		// Save the changes.
		cout << "Saving modified file..." << endl;
		doc.Save((output_path + "secured.pdf").c_str(), 0, NULL);

		cout << "Done. Result saved in secured.pdf" << endl;
	}
	catch(Common::Exception& e) {
		cout << e << endl;
		ret = 1;
	}
	catch(...) {
		cout << "Unknown Exception" << endl;
		ret = 1;
	}

	// Example 2:
	// Opens an encrypted PDF document and removes its security.

	try
	{
		cout << "-------------------------------------------------" << endl;
		cout << "Open the password protected document from the first example..." << endl;

		// Open the encrypted document that we saved in the first example. 
		PDFDoc doc((output_path + "secured.pdf").c_str());

		cout << "Initializing security handler without any user interaction..." << endl;

		// At this point MySecurityHandler callbacks will be invoked. 
		// MySecurityHandler.GetAuthorizationData() should collect the password and 
		// AuthorizeFailed() is called if user repeatedly enters a wrong password.
		if (!doc.InitStdSecurityHandler("test"))
		{
			cout << "Document authentication error..." << endl << "The password is not valid." << endl;
			ret = 1;
		}
		else
		{
			cout << "The password is correct! Document can now be used for reading and editing" << endl;

			// Remove the password security and save the changes to a new file.
			doc.RemoveSecurity();
			doc.Save(output_path + "secured_nomore1.pdf", 0, NULL);
			cout << "Done. Result saved in secured_nomore1.pdf" << endl;

			/*
			SecurityHandler hdlr = doc.GetSecurityHandler();
			cout << "Document Open Password: " << hdlr.IsUserPasswordRequired() << endl;
			cout << "Permissions Password: " << hdlr.IsMasterPasswordRequired() << endl;
			cout << "Permissions: "
				<< "\n\tHas 'owner' permissions: " << hdlr.GetPermission(SecurityHandler::e_owner)
				<< "\n\tOpen and decrypt the document: " << hdlr.GetPermission(SecurityHandler::e_doc_open)
				<< "\n\tAllow content extraction: " << hdlr.GetPermission(SecurityHandler::e_extract_content)
				<< "\n\tAllow full document editing: " << hdlr.GetPermission(SecurityHandler::e_doc_modify)
				<< "\n\tAllow printing: " << hdlr.GetPermission(SecurityHandler::e_print)
				<< "\n\tAllow high resolution printing: " << hdlr.GetPermission(SecurityHandler::e_print_high)
				<< "\n\tAllow annotation editing: " << hdlr.GetPermission(SecurityHandler::e_mod_annot)
				<< "\n\tAllow form fill: " << hdlr.GetPermission(SecurityHandler::e_fill_forms)
				<< "\n\tAllow content extraction for accessibility: " << hdlr.GetPermission(SecurityHandler::e_access_support)
				<< "\n\tAllow document assembly: " << hdlr.GetPermission(SecurityHandler::e_assemble_doc)
				<< endl;
			*/
		}
	}
	catch(Common::Exception& e) {
		cout << e << endl;
		ret = 1;
	}
	catch(...) {
		cout << "Unknown Exception" << endl;
		ret = 1;
	}

	// An advanced example showing how to work with custom security handlers. 
	// A custom security handler is a class derived from a SecurityHandler.

	// Define a custom security handler used to obtain document password dynamically via user feedback. 
	class MySecurityHandler : public SecurityHandler
	{
	public:
		MySecurityHandler (int key_len, int enc_code) : SecurityHandler("Standard", key_len, enc_code) {}
		MySecurityHandler (const MySecurityHandler& s) : SecurityHandler(s) {}
		virtual ~MySecurityHandler() {
			// cout << "MySecurityHandler Destroy";
		}

		// In this callback ask the user for password/authorization data. 
		// This may involve a dialog box used to collect authorization data or something else.
		virtual bool GetAuthorizationData (Permission p) 
		{
			cout << "The input file requires user password." << endl;
			cout << "Please enter the password:" << endl;

			string password;
			cin >> password;

			InitPassword(password.c_str());
			return true; 
		}

		// This callback could be used to customize security handler preferences.
		virtual bool EditSecurityData(SDF::SDFDoc& doc) { return false; }

		// This callback is used when authorization process fails. 
		virtual void AuthorizeFailed() { cout << "Authorize failed...." << endl; }


		MySecurityHandler(const MySecurityHandler& s, TRN_SecurityHandler base) 
			: SecurityHandler(base, true, s.m_derived_procs) 
		{
		}

		virtual SecurityHandler* Clone(TRN_SecurityHandler base) const 
		{
			return new MySecurityHandler(*this, base);
		}

		// MySecurityHandler's factory method
		static TRN_SecurityHandler Create(const char* name, int key_len, int enc_code, void* custom_data) 
		{ 
			MySecurityHandler* ret = new MySecurityHandler (key_len, enc_code);

			// Explicitly specify which methods are overloaded.
			ret->SetDerived(
				has_CloneProc |   // Clone - must be implemented in every derived class.
				has_AuthFailedProc |
				has_GetAuthDataProc);
			return (TRN_SecurityHandler) ret->mp_handler;
		}
	};

	// Example 3:
	// Read a password protected PDF using a custom security handler.

	try
	{
		// Register standard security. Required only once per application session.
		PDFNet::RegisterSecurityHandler("Standard", "Standard Security", MySecurityHandler::Create);

		cout << "-------------------------------------------------" << endl;
		cout << "Open the password protected document from the first example..." << endl;
		PDFDoc doc((output_path + "secured.pdf").c_str());  // Open the encrypted document that we saved in the first example. 

		cout << "Initializing security handler. The password will now be collected from the user" << endl;
		cout << "Enter 'test' as the password." << endl;

		// this data is just to show how you can pass your own custom data through InitSecurityHandler
		void* custom_data = const_cast<char*>("my custom pointer");

		// At this point MySecurityHandler callbacks will be invoked. 
		// MySecurityHandler.GetAuthorizationData() should collect the password and 
		// AuthorizeFailed() is called if user repeatedly enters a wrong password.
		if (!doc.InitSecurityHandler(custom_data))
		{
			cout << "Document authentication error..." << endl;
			cout << "The password is not valid." << endl;
		}
		else 
		{
			cout << "\nThe password is correct! Document can now be used for reading and editing" << endl;

			// Remove the password security and save the changes to a new file.
			doc.RemoveSecurity();
			doc.Save((output_path + "secured_nomore2.pdf").c_str(), 0, NULL);
			cout << "Done. Result saved in secured_nomore2.pdf" << endl;
		}
	}
	catch(Common::Exception& e) {
		cout << e << endl;
		ret = 1;
	}
	catch(...) {
		cout << "Unknown Exception" << endl;
		ret = 1;
	}

	cout << "Tests completed." << endl;

	PDFNet::Terminate();
	return ret;
}
