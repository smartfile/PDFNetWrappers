//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// This sample demonstrates the basic usage of the high-level digital signatures API in PDFNet.
//
// The following steps reflect typical intended usage of the digital signatures API:
//
//	0.	Start with a PDF with or without form fields in it that one would like to lock (or, one can add a field, see (1)).
//	
//	1.	EITHER: 
//		(a) Call doc.CreateDigitalSignatureField, optionally providing a name. You receive a DigitalSignatureField.
//		-OR-
//		(b) If you didn't just create the digital signature field that you want to sign/certify, find the existing one within the 
//		document by using PDFDoc.DigitalSignatureFieldIterator or by using PDFDoc.GetField to get it by its fully qualified name.
//	
//	2.	Create a signature widget annotation, and pass the DigitalSignatureField that you just created or found. 
//		If you want it to be visible, provide a Rect argument with a non-zero width or height, and don't set the
//		NoView and Hidden flags. [Optionally, add an appearance to the annotation when you wish to sign/certify.]
//		
//	[3. (OPTIONAL) Add digital signature restrictions to the document using the field modification permissions (SetFieldPermissions) 
//		or document modification permissions functions (SetDocumentPermissions) of DigitalSignatureField. These features disallow 
//		certain types of changes to be made to the document without invalidating the cryptographic digital signature once it
//		is signed.]
//		
//	4. 	Call either CertifyOnNextSave or SignOnNextSave. There are three overloads for each one (six total):
//		a.	Taking a PKCS #12 keyfile path and its password
//		b.	Taking a buffer containing a PKCS #12 private keyfile and its password
//		c.	Taking a unique identifier of a signature handler registered with the PDFDoc. This overload is to be used
//			in the following fashion: 
//			i)		Extend and implement a new SignatureHandler. The SignatureHandler will be used to add or 
//					validate/check a digital signature.
//			ii)		Create an instance of the implemented SignatureHandler and register it with PDFDoc with 
//					pdfdoc.AddSignatureHandler(). The method returns a SignatureHandlerId.
//			iii)	Call SignOnNextSaveWithCustomHandler/CertifyOnNextSaveWithCustomHandler with the SignatureHandlerId.
//		NOTE: It is only possible to sign/certify one signature per call to the Save function.
//	
//	5.	Call pdfdoc.Save(). This will also create the digital signature dictionary and write a cryptographic signature to it.
//		IMPORTANT: If there are already signed/certified digital signature(s) in the document, you must save incrementally
//		so as to not invalidate the other signature(s). 
//
// Additional processing can be done before document is signed. For example, UseSignatureHandler() returns an instance
// of SDF dictionary which represents the signature dictionary (or the /V entry of the form field). This can be used to
// add additional information to the signature dictionary (e.g. Name, Reason, Location, etc.).
//
// Although the steps above describes extending the SignatureHandler class, this sample demonstrates the use of
// StdSignatureHandler (a built-in SignatureHandler in PDFNet) to sign a PDF file.
//----------------------------------------------------------------------------------------------------------------------

// To build and run this sample with OpenSSL, please specify OpenSSL include & lib paths to project settings.
//
// In MSVC, this can be done by opening the DigitalSignatureTest project's properties. Go to Configuration Properties ->
// C/C++ -> General -> Additional Include Directories. Add the path to the OpenSSL headers here. Next, go to
// Configuration Properties -> Linker -> General -> Additional Library Directories. Add the path to the OpenSSL libraries
// here. Finally, under Configuration Properties -> Linker -> Input -> Additional Dependencies, add libeay32.lib,
// crypt32.lib, and advapi32.lib in the list.
//
// For GCC, modify the Makefile, add -lcrypto to the $(LIBS) variable. If OpenSSL is installed elsewhere, it may be
// necessary to add the path to the headers in the $(INCLUDE) variable as well as the location of either libcrypto.a or
// libcrypto.so/libcrypto.dylib.
//

#define USE_STD_SIGNATURE_HANDLER 1 // Comment out this line if you intend to use OpenSSLSignatureHandler rather than StdSignatureHandler.

// standard library includes
#include <cstdio>
#include <iostream>
#include <vector>

// PDFNetC includes
#include <Common/Exception.h>
#include <Common/UString.h>
#include <PDF/Page.h>
#include <PDF/Annot.h>
#include <PDF/Annots/TextWidget.h>
#include <PDF/Date.h>
#include <PDF/Element.h>
#include <PDF/ElementBuilder.h>
#include <PDF/ElementWriter.h>
#include <PDF/Field.h>
#include <PDF/Image.h>
#include <PDF/PDFDoc.h>
#include <PDF/PDFNet.h>
#include <SDF/SignatureHandler.h>
#include <PDF/Annots/SignatureWidget.h>
#include <PDF/VerificationResult.h>
#include <PDF/TrustVerificationResult.h>
#include <PDF/DisallowedChange.h>
#include <Filters/MappedFile.h>
#include <PDF/X501AttributeTypeAndValue.h>

#if (!USE_STD_SIGNATURE_HANDLER)
// OpenSSL includes
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pkcs12.h>
#include <openssl/pkcs7.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>
#endif // (!USE_STD_SIGNATURE_HANDLER)

using namespace std;
using namespace pdftron;
using namespace pdftron::SDF;
using namespace pdftron::PDF::Annots;
using namespace pdftron::PDF;

//////////////////// Here follows an example of how to implement a custom signature handler. //////////
#if (!USE_STD_SIGNATURE_HANDLER)
//
// Extend SignatureHandler by using OpenSSL signing utilities.
//
class OpenSSLSignatureHandler : public SignatureHandler
{
public:
	OpenSSLSignatureHandler(const char* in_pfxfile, const char* in_password) : m_pfxfile(in_pfxfile), m_password(in_password)
	{
		// Please note: this code would require changes to support non-US-ASCII paths on platforms where UTF-8 is not usable within fopen (e.g. Windows).
		FILE* fp = fopen(in_pfxfile, "rb");
		if (fp == NULL)
			throw (Common::Exception("Cannot open private key.", __LINE__, __FILE__, "PKCS7Signature::PKCS7Signature", "Cannot open private key."));

		PKCS12* p12 = d2i_PKCS12_fp(fp, NULL);
		fclose(fp);

		if (p12 == NULL)
			throw (Common::Exception("Cannot parse private key.", __LINE__, __FILE__, "PKCS7Signature::PKCS7Signature", "Cannot parse private key."));

		mp_pkey = NULL;
		mp_x509 = NULL;
		mp_ca = NULL;
		int parseResult = PKCS12_parse(p12, in_password, &mp_pkey, &mp_x509, &mp_ca);
		PKCS12_free(p12);

		if (parseResult == 0)
			throw (Common::Exception("Cannot parse private key.", __LINE__, __FILE__, "PKCS7Signature::PKCS7Signature", "Cannot parse private key."));

		Reset();
	}

	virtual UString GetName() const
	{
		return "Adobe.PPKLite";
	}

	virtual void AppendData(const std::vector<pdftron::UInt8>& in_data)
	{
		SHA256_Update(&m_sha_ctx, (const void*) &(in_data[0]), in_data.size());
		return;
	}

	virtual bool Reset()
	{
		m_digest.resize(0);
		m_digest.clear();
		SHA256_Init(&m_sha_ctx);
		return (true);
	}

	virtual std::vector<pdftron::UInt8> CreateSignature()
	{
		if (m_digest.size() == 0) {
			m_digest.resize(SHA256_DIGEST_LENGTH);
			SHA256_Final(&(m_digest[0]), &m_sha_ctx);
		}

		PKCS7* p7 = PKCS7_new();
		PKCS7_set_type(p7, NID_pkcs7_signed);

		PKCS7_SIGNER_INFO* p7Si = PKCS7_add_signature(p7, mp_x509, mp_pkey, EVP_sha256());
		PKCS7_add_attrib_content_type(p7Si, OBJ_nid2obj(NID_pkcs7_data));
		PKCS7_add0_attrib_signing_time(p7Si, NULL);
		PKCS7_add1_attrib_digest(p7Si, &(m_digest[0]), (int)m_digest.size());
		PKCS7_add_certificate(p7, mp_x509);

		for (int c = 0; c < sk_X509_num(mp_ca); c++) {
			X509* cert = sk_X509_value(mp_ca, c);
			PKCS7_add_certificate(p7, cert);
		}
		PKCS7_set_detached(p7, 1);
		PKCS7_content_new(p7, NID_pkcs7_data);

		PKCS7_SIGNER_INFO_sign(p7Si);

		int p7Len = i2d_PKCS7(p7, NULL);
		std::vector<unsigned char> result(p7Len);
		UInt8* pP7Buf = &(result[0]);
		i2d_PKCS7(p7, &pP7Buf);

		PKCS7_free(p7);

		return (result);
	}

	virtual OpenSSLSignatureHandler* Clone() const
	{
		return (new OpenSSLSignatureHandler(m_pfxfile.c_str(), m_password.c_str()));
	}

	virtual ~OpenSSLSignatureHandler()
	{
		sk_X509_free(mp_ca);
		X509_free(mp_x509);
		EVP_PKEY_free(mp_pkey);
	}

private:
	std::vector<UInt8> m_digest;
	std::string m_pfxfile;
	std::string m_password;

	SHA256_CTX m_sha_ctx;
	EVP_PKEY* mp_pkey;      // private key
	X509* mp_x509;          // signing certificate
	STACK_OF(X509)* mp_ca;  // certificate chain up to the CA
}; // class OpenSSLSignatureHandler
#endif // (!USE_STD_SIGNATURE_HANDLER)
////////// End of the OpenSSLSignatureHandler custom handler code. ////////////////////


string input_path = "../../TestFiles/";
string output_path = "../../TestFiles/Output/";

bool VerifySimple(const UString& in_docpath, const UString& in_public_key_file_path)
{
	PDFDoc doc(in_docpath);
	puts("==========");
	VerificationOptions opts(VerificationOptions::e_compatibility_and_archiving);

	// Add trust root to store of trusted certificates contained in VerificationOptions.
	opts.AddTrustedCertificate(in_public_key_file_path);

	enum PDFDoc::SignaturesVerificationStatus result = doc.VerifySignedDigitalSignatures(opts);
	switch (result)
	{
	case PDFDoc::e_unsigned:
		puts("Document has no signed signature fields.");
		return false;
		/* e_failure == bad doc status, digest status, or permissions status
		(i.e. does not include trust issues, because those are flaky due to being network/config-related) */
	case PDFDoc::e_failure:
		puts("Hard failure in verification on at least one signature.");
		return false;
	case PDFDoc::e_untrusted:
		puts("Could not verify trust for at least one signature.");
		return false;
	case PDFDoc::e_unsupported:
		/* If necessary, call GetUnsupportedFeatures on VerificationResult to check which
		unsupported features were encountered (requires verification using 'detailed' APIs) */
		puts("At least one signature contains unsupported features.");
		return false;
		// unsigned sigs skipped; parts of document may be unsigned (check GetByteRanges on signed sigs to find out)
	case PDFDoc::e_verified:
		puts("All signed signatures in document verified.");
		return true;
	default:
		BASE_ASSERT(false, "unrecognized document verification status");
	}
}

bool VerifyAllAndPrint(const UString& in_docpath, const UString& in_public_key_file_path)
{
	PDFDoc doc(in_docpath);
	cout << "==========" << endl;
	PDF::VerificationOptions opts(PDF::VerificationOptions::e_compatibility_and_archiving);
	
	// Add trust root to store of trusted certificates contained in VerificationOptions.
	opts.AddTrustedCertificate(in_public_key_file_path);

	// Iterate over the signatures and verify all of them.
	DigitalSignatureFieldIterator digsig_fitr = doc.GetDigitalSignatureFieldIterator();
	bool verification_status = true;
	for (; digsig_fitr.HasNext(); digsig_fitr.Next())
	{
		DigitalSignatureField curr = digsig_fitr.Current();
		VerificationResult result = curr.Verify(opts);
		if (result.GetVerificationStatus())
		{
			cout << "Signature verified, ";
		}
		else
		{
			cout << "Signature verification failed, ";
			verification_status = false;
		}
		cout << "objnum: " << curr.GetSDFObj().GetObjNum() << endl;
		
		switch (result.GetDigestAlgorithm())
		{
		case DigestAlgorithm::e_SHA1:
			cout << "Digest algorithm: SHA-1" << endl;
			break;
		case DigestAlgorithm::e_SHA256:
			cout << "Digest algorithm: SHA-256" << endl;
			break;
		case DigestAlgorithm::e_SHA384:
			cout << "Digest algorithm: SHA-384" << endl;
			break;
		case DigestAlgorithm::e_SHA512:
			cout << "Digest algorithm: SHA-512" << endl;
			break;
		case DigestAlgorithm::e_RIPEMD160:
			cout << "Digest algorithm: RIPEMD-160" << endl;
			break;
		case DigestAlgorithm::e_unknown_digest_algorithm:
			cout << "Digest algorithm: unknown" << endl;
			break;
		default:
			BASE_ASSERT(false, "unrecognized digest algorithm");
		}
		printf("Detailed verification result: \n\t%s\n\t%s\n\t%s\n\t%s\n", 
			result.GetDocumentStatusAsString().ConvertToUtf8().c_str(),
			result.GetDigestStatusAsString().ConvertToUtf8().c_str(),
			result.GetTrustStatusAsString().ConvertToUtf8().c_str(),
			result.GetPermissionsStatusAsString().ConvertToUtf8().c_str());


		std::vector<DisallowedChange> changes = result.GetDisallowedChanges();
		for (std::vector<DisallowedChange>::iterator it2 = changes.begin(); it2 != changes.end(); ++it2)
		{
			cout << "\tDisallowed change: " << it2->GetTypeAsString().ConvertToUtf8() << ", objnum: " << it2->GetObjNum() << endl;
		}
		
		// Get and print all the detailed trust-related results, if they are available.
		if (result.HasTrustVerificationResult())
		{
			const TrustVerificationResult trust_verification_result = result.GetTrustVerificationResult();
			trust_verification_result.WasSuccessful()? cout << "Trust verified." << endl : cout << "Trust not verifiable." << endl;
			cout << trust_verification_result.GetResultString().ConvertToUtf8() << endl;

			Int64 tmp_time_t = trust_verification_result.GetTimeOfTrustVerification();
			switch (trust_verification_result.GetTimeOfTrustVerificationEnum())
			{
			case VerificationOptions::e_current:
				cout << "Trust verification attempted with respect to current time (as epoch time): " << tmp_time_t << endl;
				break;
			case VerificationOptions::e_signing:
				cout << "Trust verification attempted with respect to signing time (as epoch time): " << tmp_time_t << endl;
				break;
			case VerificationOptions::e_timestamp:
				cout << "Trust verification attempted with respect to secure embedded timestamp (as epoch time): " << tmp_time_t << endl;
				break;
			default:
				BASE_ASSERT(false, "unrecognized time enum value");
			}

			if (trust_verification_result.GetCertPath().empty())
			{
				cout << "Could not print certificate path.\n";
			}
			else
			{
				cout << "Certificate path:\n";
				std::vector<X509Certificate>  cert_path(trust_verification_result.GetCertPath());
				for (std::vector<X509Certificate>::iterator cert_path_begin = cert_path.begin();
					cert_path_begin != cert_path.end(); ++cert_path_begin)
				{
					cout << "\tCertificate:\n";
					X509Certificate& full_cert(*cert_path_begin);
					cout << "\t\tIssuer names:\n";
					std::vector<X501AttributeTypeAndValue> issuer_dn (full_cert.GetIssuerField().GetAllAttributesAndValues());
					for (size_t i = 0; i < issuer_dn.size(); i++)
					{
						cout << "\t\t\t" << issuer_dn[i].GetStringValue().ConvertToUtf8() << "\n";
					}
					cout << "\t\tSubject names:\n";
					std::vector<X501AttributeTypeAndValue > subject_dn(full_cert.GetSubjectField().GetAllAttributesAndValues());
					for (size_t i = 0; i < subject_dn.size(); i++)
					{
						cout << "\t\t\t" << subject_dn[i].GetStringValue().ConvertToUtf8() << "\n";
					}
					cout << "\t\tExtensions:\n";
					for (size_t i = 0; i < full_cert.GetExtensions().size(); i++)
					{
						cout << "\t\t\t" << full_cert.GetExtensions()[i].ToString() << "\n";
					}
				}
			}
		}
		else
		{
			cout << "No detailed trust verification result available." << endl;
		}

		const std::vector<UString> unsupported_features(result.GetUnsupportedFeatures());
		if (unsupported_features.size() > 0)
		{
			cout << "Unsupported features:" << "\n";
			for (size_t i = 0; i < unsupported_features.size(); i++)
			{
				cout << "\t" << unsupported_features[i].ConvertToUtf8() << "\n";
			}
		}
		cout << "==========" << endl;
	}

	return verification_status;
}

void CertifyPDF(const UString& in_docpath,
	const UString& in_cert_field_name,
	const UString& in_private_key_file_path,
	const UString& in_keyfile_password,
	const UString& in_appearance_image_path,
	const UString& in_outpath)
{
	cout << "================================================================================" << endl;
	cout << "Certifying PDF document" << endl;

	// Open an existing PDF
	PDFDoc doc(in_docpath);

	cout << "PDFDoc has " << (doc.HasSignatures() ? "signatures" : "no signatures") << endl;

	Page page1 = doc.GetPage(1);

	// Create a text field that we can lock using the field permissions feature.
	Annots::TextWidget annot1 = Annots::TextWidget::Create(doc, Rect(50, 550, 350, 600), "asdf_test_field");
	page1.AnnotPushBack(annot1);

	/* Create a new signature form field in the PDFDoc. The name argument is optional;
	leaving it empty causes it to be auto-generated. However, you may need the name for later.
	Acrobat doesn't show digsigfield in side panel if it's without a widget. Using a
	Rect with 0 width and 0 height, or setting the NoPrint/Invisible flags makes it invisible. */
	PDF::DigitalSignatureField certification_sig_field = doc.CreateDigitalSignatureField(in_cert_field_name);
	Annots::SignatureWidget widgetAnnot = Annots::SignatureWidget::Create(doc, Rect(0, 100, 200, 150), certification_sig_field);
	page1.AnnotPushBack(widgetAnnot);

	// (OPTIONAL) Add an appearance to the signature field.
	PDF::Image img = PDF::Image::Create(doc, in_appearance_image_path);
	widgetAnnot.CreateSignatureAppearance(img);

	// Prepare the document locking permission level. It will be applied upon document certification.
	cout << "Adding document permissions." << endl;
	certification_sig_field.SetDocumentPermissions(DigitalSignatureField::e_annotating_formfilling_signing_allowed);
	
	// Prepare to lock the text field that we created earlier.
	cout << "Adding field permissions." << endl;
	vector<UString> fields_to_lock;
	fields_to_lock.push_back("asdf_test_field");
	certification_sig_field.SetFieldPermissions(DigitalSignatureField::e_include, fields_to_lock);

#ifdef USE_STD_SIGNATURE_HANDLER
	certification_sig_field.CertifyOnNextSave(in_private_key_file_path, in_keyfile_password);
#else
	OpenSSLSignatureHandler sigHandler(in_private_key_file_path.ConvertToUtf8().c_str(), in_keyfile_password.ConvertToUtf8().c_str());
	SignatureHandlerId sigHandlerId = doc.AddSignatureHandler(sigHandler);
	certification_sig_field.CertifyOnNextSaveWithCustomHandler(sigHandlerId);
	/* Add to the digital signature dictionary a SubFilter name that uniquely identifies the signature format 
	for verification tools. As an example, the custom handler defined in this file uses the CMS/PKCS #7 detached format, 
	so we embed one of the standard predefined SubFilter values: "adbe.pkcs7.detached". It is not necessary to do this 
	when using the StdSignatureHandler. */
	Obj f_obj = certification_sig_field.GetSDFObj();
	f_obj.FindObj("V").PutName("SubFilter", "adbe.pkcs7.detached");
#endif

	// (OPTIONAL) Add more information to the signature dictionary.
	certification_sig_field.SetLocation("Vancouver, BC");
	certification_sig_field.SetReason("Document certification.");
	certification_sig_field.SetContactInfo("www.pdftron.com");

	// Save the PDFDoc. Once the method below is called, PDFNet will also sign the document using the information provided.
	doc.Save(in_outpath, 0, NULL);

	cout << "================================================================================" << endl;
}

void SignPDF(const UString& in_docpath,
	const UString& in_approval_field_name,
	const UString& in_private_key_file_path,
	const UString& in_keyfile_password,
	const UString& in_appearance_img_path,
	const UString& in_outpath)
{
	cout << "================================================================================" << endl;
	cout << "Signing PDF document" << endl;

	// Open an existing PDF
	PDFDoc doc(in_docpath);

	// Retrieve the unsigned approval signature field.
	Field found_approval_field(doc.GetField(in_approval_field_name));
	PDF::DigitalSignatureField found_approval_signature_digsig_field(found_approval_field);
	
	// (OPTIONAL) Add an appearance to the signature field.
	PDF::Image img = PDF::Image::Create(doc, in_appearance_img_path);
	Annots::SignatureWidget found_approval_signature_widget(found_approval_field.GetSDFObj());
	found_approval_signature_widget.CreateSignatureAppearance(img);

	// Prepare the signature and signature handler for signing.
#ifdef USE_STD_SIGNATURE_HANDLER
	found_approval_signature_digsig_field.SignOnNextSave(in_private_key_file_path, in_keyfile_password);
#else
	OpenSSLSignatureHandler sigHandler(in_private_key_file_path.ConvertToUtf8().c_str(), in_keyfile_password.ConvertToUtf8().c_str());
	SignatureHandlerId sigHandlerId = doc.AddSignatureHandler(sigHandler);
	found_approval_signature_digsig_field.SignOnNextSaveWithCustomHandler(sigHandlerId);
	/* Add to the digital signature dictionary a SubFilter name that uniquely identifies the signature format 
	for verification tools. As an example, the custom handler defined in this file uses the CMS/PKCS #7 detached format, 
	so we embed one of the standard predefined SubFilter values: "adbe.pkcs7.detached". It is not necessary to do this 
	when using the StdSignatureHandler. */
	Obj f_obj = found_approval_signature_digsig_field.GetSDFObj();
	f_obj.FindObj("V").PutName("SubFilter", "adbe.pkcs7.detached");
#endif

	// The actual approval signing will be done during the following incremental save operation.
	doc.Save(in_outpath, SDFDoc::e_incremental, NULL);

	cout << "================================================================================" << endl;
}

void ClearSignature(const UString& in_docpath,
	const UString& in_digsig_field_name,
	const UString& in_outpath)
{
	cout << "================================================================================" << endl;
	cout << "Clearing certification signature" << endl;

	PDFDoc doc(in_docpath);

	DigitalSignatureField digsig(doc.GetField(in_digsig_field_name));
	
	cout << "Clearing signature: " << in_digsig_field_name << endl;
	digsig.ClearSignature();

	if (!digsig.HasCryptographicSignature())
	{
		cout << "Cryptographic signature cleared properly." << endl;
	}

	// Save incrementally so as to not invalidate other signatures from previous saves.
	doc.Save(in_outpath, SDFDoc::e_incremental, NULL);

	cout << "================================================================================" << endl;
}

void PrintSignaturesInfo(const UString& in_docpath)
{
	cout << "================================================================================" << endl;
	cout << "Reading and printing digital signature information" << endl;

	PDFDoc doc(in_docpath);
	if (!doc.HasSignatures())
	{
		cout << "Doc has no signatures." << endl;
		cout << "================================================================================" << endl;
		return;
	}
	else
	{
		cout << "Doc has signatures." << endl;
	}

	
	for (FieldIterator fitr = doc.GetFieldIterator(); fitr.HasNext(); fitr.Next())
	{
		fitr.Current().IsLockedByDigitalSignature() ? cout << "==========" << endl << "Field locked by a digital signature" << endl :
			cout << "==========" << endl << "Field not locked by a digital signature" << endl;

		cout << "Field name: " << fitr.Current().GetName() << endl;
		cout << "==========" << endl;
	}

	cout << "====================" << endl << "Now iterating over digital signatures only." << endl << "====================" << endl;

	DigitalSignatureFieldIterator digsig_fitr = doc.GetDigitalSignatureFieldIterator();
	for (; digsig_fitr.HasNext(); digsig_fitr.Next())
	{
		cout << "==========" << endl;
		cout << "Field name of digital signature: " << Field(digsig_fitr.Current().GetSDFObj()).GetName() << endl;

		DigitalSignatureField digsigfield(digsig_fitr.Current());
		if (!digsigfield.HasCryptographicSignature())
		{
			cout << "Either digital signature field lacks a digital signature dictionary, "
				"or digital signature dictionary lacks a cryptographic Contents entry. "
				"Digital signature field is not presently considered signed." << endl 
				<< "==========" << endl;
			continue;
		}

		UInt32 cert_count = digsigfield.GetCertCount();
		cout << "Cert count: " << cert_count << endl;
		for (UInt32 i = 0; i < cert_count; ++i)
		{
			std::vector<unsigned char> cert = digsigfield.GetCert(i);
			cout << "Cert #" << i << " size: " << cert.size() << endl;
		}

		DigitalSignatureField::SubFilterType subfilter = digsigfield.GetSubFilter();

		cout << "Subfilter type: " << (int)subfilter << endl;

		if (subfilter != DigitalSignatureField::e_ETSI_RFC3161)
		{
			cout << "Signature's signer: " << digsigfield.GetSignatureName() << endl;

			Date signing_time(digsigfield.GetSigningTime());
			if (signing_time.IsValid())
			{
				cout << "Signing time is valid." << endl;
			}

			cout << "Location: " << digsigfield.GetLocation() << endl;
			cout << "Reason: " << digsigfield.GetReason() << endl;
			cout << "Contact info: " << digsigfield.GetContactInfo() << endl;
		}
		else
		{
			cout << "SubFilter == e_ETSI_RFC3161 (DocTimeStamp; no signing info)" << endl;
		}

		cout << ((digsigfield.HasVisibleAppearance()) ? "Visible" : "Not visible") << endl;

		DigitalSignatureField::DocumentPermissions digsig_doc_perms = digsigfield.GetDocumentPermissions();
		vector<UString> locked_fields(digsigfield.GetLockedFields());
		for (vector<UString>::iterator it = locked_fields.begin(); it != locked_fields.end(); ++it)
		{
			cout << "This digital signature locks a field named: " << it->ConvertToAscii() << endl;
		}

		switch (digsig_doc_perms)
		{
		case DigitalSignatureField::e_no_changes_allowed:
			cout << "No changes to the document can be made without invalidating this digital signature." << endl;
			break;
		case DigitalSignatureField::e_formfilling_signing_allowed:
			cout << "Page template instantiation, form filling, and signing digital signatures are allowed without invalidating this digital signature." << endl;
			break;
		case DigitalSignatureField::e_annotating_formfilling_signing_allowed:
			cout << "Annotating, page template instantiation, form filling, and signing digital signatures are allowed without invalidating this digital signature." << endl;
			break;
		case DigitalSignatureField::e_unrestricted:
			cout << "Document not restricted by this digital signature." << endl;
			break;
		default:
			BASE_ASSERT(false, "Unrecognized digital signature document permission level.");
		}
		cout << "==========" << endl;
	}

	cout << "================================================================================" << endl;
}

bool TimestampAndEnableLTV(const UString& in_docpath, 
	const UString& in_trusted_cert_path, 
	const UString& in_appearance_img_path,
	const UString& in_outpath)
{
	PDFDoc doc(in_docpath);
	DigitalSignatureField doctimestamp_signature_field = doc.CreateDigitalSignatureField();
	TimestampingConfiguration tst_config("http://adobe-timestamp.globalsign.com/?signature=sha2");
	VerificationOptions opts(VerificationOptions::e_compatibility_and_archiving);
	/* It is necessary to add to the VerificationOptions a trusted root certificate corresponding to 
	the chain used by the timestamp authority to sign the timestamp token, in order for the timestamp
	response to be verifiable during DocTimeStamp signing. It is also necessary in the context of this 
	function to do this for the later LTV section, because one needs to be able to verify the DocTimeStamp 
	in order to enable LTV for it, and we re-use the VerificationOptions opts object in that part. */
	opts.AddTrustedCertificate(in_trusted_cert_path);
	/* By default, we only check online for revocation of certificates using the newer and lighter 
	OCSP protocol as opposed to CRL, due to lower resource usage and greater reliability. However, 
	it may be necessary to enable online CRL revocation checking in order to verify some timestamps
	(i.e. those that do not have an OCSP responder URL for all non-trusted certificates). */
	opts.EnableOnlineCRLRevocationChecking(true);

	Annots::SignatureWidget widgetAnnot = Annots::SignatureWidget::Create(doc, Rect(0, 100, 200, 150), doctimestamp_signature_field);
	doc.GetPage(1).AnnotPushBack(widgetAnnot);

	// (OPTIONAL) Add an appearance to the signature field.
	PDF::Image img = PDF::Image::Create(doc, in_appearance_img_path);
	widgetAnnot.CreateSignatureAppearance(img);

	puts("Testing timestamping configuration.");
	const TimestampingTestResult config_result(tst_config.TestConfiguration(opts));
	if (config_result.GetStatus())
	{
		puts("Success: timestamping configuration usable. Attempting to timestamp.");
	}
	else
	{
		// Print details of timestamping failure.
		puts(config_result.GetString().ConvertToUtf8().c_str());
		if (config_result.HasResponseVerificationResult())
		{
			EmbeddedTimestampVerificationResult tst_result(config_result.GetResponseVerificationResult());
			printf("CMS digest status: %s\n", tst_result.GetCMSDigestStatusAsString().ConvertToUtf8().c_str());
			printf("Message digest status: %s\n", tst_result.GetMessageImprintDigestStatusAsString().ConvertToUtf8().c_str());
			printf("Trust status: %s\n", tst_result.GetTrustStatusAsString().ConvertToUtf8().c_str());
		}
		return false;
	}

	doctimestamp_signature_field.TimestampOnNextSave(tst_config, opts);

	// Save/signing throws if timestamping fails.
	doc.Save(in_outpath, SDFDoc::e_incremental, 0);

	puts("Timestamping successful. Adding LTV information for DocTimeStamp signature.");

	// Add LTV information for timestamp signature to document.
	VerificationResult timestamp_verification_result = doctimestamp_signature_field.Verify(opts);
	if (!doctimestamp_signature_field.EnableLTVOfflineVerification(timestamp_verification_result))
	{
		puts("Could not enable LTV for DocTimeStamp.");
		return false;
	}
	doc.Save(in_outpath, SDFDoc::e_incremental, 0);
	puts("Added LTV information for DocTimeStamp signature successfully.");

	return true;
}
int main(void)
{
	// Initialize PDFNetC
	PDFNet::Initialize();

#if (!USE_STD_SIGNATURE_HANDLER)
	// Initialize OpenSSL library
	CRYPTO_malloc_init();
	ERR_load_crypto_strings();
	OpenSSL_add_all_algorithms();
#endif // (!USE_STD_SIGNATURE_HANDLER)

	int ret = 0;

	//////////////////// TEST 0: 
	/* Create an approval signature field that we can sign after certifying.
	(Must be done before calling CertifyOnNextSave/SignOnNextSave/WithCustomHandler.) */
	try
	{
		PDFDoc doc(input_path + "tiger.pdf");
		DigitalSignatureField approval_signature_field = doc.CreateDigitalSignatureField("PDFTronApprovalSig");
		Annots::SignatureWidget widgetAnnotApproval = Annots::SignatureWidget::Create(doc, Rect(300, 300, 500, 200), approval_signature_field);
		Page page1 = doc.GetPage(1);
		page1.AnnotPushBack(widgetAnnotApproval);
		doc.Save(output_path + "tiger_withApprovalField_output.pdf", SDFDoc::e_remove_unused, 0);
	}
	catch (Common::Exception& e)
	{
		cerr << e << endl;
		ret = 1;
	}
	catch (exception& e)
	{
		cerr << e.what() << endl;
		ret = 1;
	}
	catch (...)
	{
		cerr << "Unknown exception." << endl;
		ret = 1;
	}

	//////////////////// TEST 1: certify a PDF.
	try
	{
		CertifyPDF(input_path + "tiger_withApprovalField.pdf",
			"PDFTronCertificationSig",
			input_path + "pdftron.pfx",
			"password",
			input_path + "pdftron.bmp",
			output_path + "tiger_withApprovalField_certified_output.pdf");
		PrintSignaturesInfo(output_path + "tiger_withApprovalField_certified_output.pdf");
	}
	catch (Common::Exception& e)
	{
		cerr << e << endl;
		ret = 1;
	}
	catch (exception& e)
	{
		cerr << e.what() << endl;
		ret = 1;
	}
	catch (...)
	{
		cerr << "Unknown exception." << endl;
		ret = 1;
	}

	//////////////////// TEST 2: sign a PDF with a certification and an unsigned signature field in it.
	try
	{
		SignPDF(input_path + "tiger_withApprovalField_certified.pdf",
			"PDFTronApprovalSig",
			input_path + "pdftron.pfx",
			"password",
			input_path + "signature.jpg",
			output_path + "tiger_withApprovalField_certified_approved_output.pdf");
		PrintSignaturesInfo(output_path + "tiger_withApprovalField_certified_approved_output.pdf");
	}
	catch (Common::Exception& e)
	{
		cerr << e << endl;
		ret = 1;
	}
	catch (exception& e)
	{
		cerr << e.what() << endl;
		ret = 1;
	}
	catch (...)
	{
		cerr << "Unknown exception." << endl;
		ret = 1;
	}

	//////////////////// TEST 3: Clear a certification from a document that is certified and has an approval signature.
	try
	{
		ClearSignature(input_path + "tiger_withApprovalField_certified_approved.pdf",
			"PDFTronCertificationSig",
			output_path + "tiger_withApprovalField_certified_approved_certcleared_output.pdf");
		PrintSignaturesInfo(output_path + "tiger_withApprovalField_certified_approved_certcleared_output.pdf");
	}
	catch (Common::Exception& e)
	{
		cerr << e << endl;
		ret = 1;
	}
	catch (exception& e)
	{
		cerr << e.what() << endl;
		ret = 1;
	}
	catch (...)
	{
		cerr << "Unknown exception." << endl;
		ret = 1;
	}

	//////////////////// TEST 4: Verify a document's digital signatures.
	try
	{
		if (!VerifyAllAndPrint(input_path + "tiger_withApprovalField_certified_approved.pdf", input_path + "pdftron.cer"))
		{
			ret = 1;
		}
	}
	catch (Common::Exception& e)
	{
		cerr << e << endl;
		ret = 1;
	}
	catch (exception& e)
	{
		cerr << e.what() << endl;
		ret = 1;
	}
	catch (...)
	{
		cerr << "Unknown exception." << endl;
		ret = 1;
	}

	//////////////////// TEST 5: Verify a document's digital signatures in a simple fashion using the document API.
	try
	{
		if (!VerifySimple(input_path + "tiger_withApprovalField_certified_approved.pdf", input_path + "pdftron.cer"))
		{
			ret = 1;
		}
	}
	catch (exception& e)
	{
		cerr << e.what() << "\n";
		ret = 1;
	}
	catch (...)
	{
		cerr << "Unknown exception.\n";
		ret = 1;
	}

	//////////////////// TEST 6: Timestamp a document, then add Long Term Validation (LTV) information for the DocTimeStamp.
	try
	{
		if (!TimestampAndEnableLTV(input_path + "tiger.pdf",
			input_path + "GlobalSignRootForTST.cer",
			input_path + "signature.jpg",
			output_path+ "tiger_DocTimeStamp_LTV.pdf"))
		{
			ret = 1;
		}
	}
	catch (exception& e)
	{
		cerr << e.what() << "\n";
		ret = 1;
	}
	catch (...)
	{
		cerr << "Unknown exception.\n";
		ret = 1;
	}

	//////////////////// End of tests. ////////////////////

	if (!ret)
	{
		cout << "Tests successful." << endl  << "==========" << endl;
	}
	else
	{
		cout << "Tests FAILED!!!" << endl << "==========" << endl;
	}

	PDFNet::Terminate();

#if (!USE_STD_SIGNATURE_HANDLER)
	ERR_free_strings();
	EVP_cleanup();
#endif // (!USE_STD_SIGNATURE_HANDLER)

	return ret;
}
