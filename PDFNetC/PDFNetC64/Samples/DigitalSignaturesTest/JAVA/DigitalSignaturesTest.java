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
//		(a) Call doc.createDigitalSignatureField, optionally providing a name. You receive a DigitalSignatureField.
//		-OR-
//		(b) If you didn't just create the digital signature field that you want to sign/certify, find the existing one within the 
//		document by using PDFDoc.DigitalSignatureFieldIterator or by using PDFDoc.getField to get it by its fully qualified name.
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
//					pdfdoc.addSignatureHandler(). The method returns a SignatureHandlerId.
//			iii)	Call SignOnNextSaveWithCustomHandler/CertifyOnNextSaveWithCustomHandler with the SignatureHandlerId.
//		NOTE: It is only possible to sign/certify one signature per call to the Save function.
//	
//	5.	Call pdfdoc.save(). This will also create the digital signature dictionary and write a cryptographic signature to it.
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

import java.util.Locale;
import com.pdftron.common.PDFNetException;
import com.pdftron.pdf.annots.TextWidget;
import com.pdftron.pdf.annots.SignatureWidget;
import com.pdftron.pdf.FieldIterator;
import com.pdftron.pdf.DigitalSignatureFieldIterator;
import com.pdftron.pdf.Field;
import com.pdftron.pdf.Image;
import com.pdftron.pdf.PDFDoc;
import com.pdftron.pdf.PDFNet;
import com.pdftron.pdf.Page;
import com.pdftron.pdf.Rect;
import com.pdftron.pdf.Date;
import com.pdftron.pdf.DigitalSignatureField;
import com.pdftron.pdf.VerificationOptions;
import com.pdftron.pdf.VerificationResult;
import com.pdftron.pdf.TrustVerificationResult;
import com.pdftron.pdf.DigestAlgorithm;
import com.pdftron.pdf.DisallowedChange;
import com.pdftron.sdf.Obj;
import com.pdftron.sdf.SignatureHandler;
import com.pdftron.sdf.SDFDoc;
import com.pdftron.filters.FilterReader;
import com.pdftron.filters.MappedFile;
import com.pdftron.pdf.X509Certificate;
import com.pdftron.pdf.X501AttributeTypeAndValue;
import com.pdftron.pdf.X501DistinguishedName;
import com.pdftron.pdf.TimestampingConfiguration;
import com.pdftron.pdf.TimestampingTestResult;
import com.pdftron.pdf.EmbeddedTimestampVerificationResult;
public class DigitalSignaturesTest 
{
	public static boolean verifySimple(String in_docpath, String in_public_key_file_path) throws PDFNetException
	{
		PDFDoc doc = new PDFDoc(in_docpath);
		System.out.println("==========");
		VerificationOptions opts = new VerificationOptions(VerificationOptions.SecurityLevel.e_compatibility_and_archiving);

		// Add trust root to store of trusted certificates contained in VerificationOptions.
		opts.addTrustedCertificate(in_public_key_file_path);

		PDFDoc.SignaturesVerificationStatus result = doc.verifySignedDigitalSignatures(opts);
		
		switch (result)
		{
		case e_unsigned:
			System.out.println("Document has no signed signature fields.");
			return false;
			/*e_failure == bad doc status, digest status, or permissions status
			(i.e. does not include trust issues, because those are flaky due to being network/config-related)*/
		case e_failure:
			System.out.println("Hard failure in verification on at least one signature.");
			return false;
		case e_untrusted:
			System.out.println("Could not verify trust for at least one signature.");
			return false;
		case e_unsupported:
			/*If necessary, call GetUnsupportedFeatures on VerificationResult to check which
			unsupported features were encountered (requires verification using 'detailed' APIs) */
			System.out.println("At least one signature contains unsupported features.");
			return false;
			// unsigned sigs skipped; parts of document may be unsigned (check GetByteRanges on signed sigs to find out)
		case e_verified:
			System.out.println("All signed signatures in document verified.");
			return true;
		default:
			System.err.println("unrecognized document verification status");
			assert(false);
		}
		return false;
	}

	public static boolean verifyAllAndPrint(String in_docpath, String in_public_key_file_path) throws PDFNetException
	{
		PDFDoc doc = new PDFDoc(in_docpath);
		System.out.println("==========");
		VerificationOptions opts = new VerificationOptions(VerificationOptions.SecurityLevel.e_compatibility_and_archiving);

		// Add trust root to store of trusted certificates contained in VerificationOptions.
		opts.addTrustedCertificate(in_public_key_file_path);

		// Iterate over the signatures and verify all of them.
		DigitalSignatureFieldIterator digsig_fitr = doc.getDigitalSignatureFieldIterator();
		boolean verification_status = true;
		for (; digsig_fitr.hasNext(); )
		{
			DigitalSignatureField curr = digsig_fitr.next();
			VerificationResult result = curr.verify(opts);
			if (result.getVerificationStatus())
			{
				System.out.print("Signature verified, ");
			}
			else
			{
				System.out.print("Signature verification failed, ");
				verification_status = false;
			}
			System.out.println(String.format(Locale.US, "objnum: %d", curr.getSDFObj().getObjNum()));

			switch (result.getDigestAlgorithm())
			{
				case e_sha1:
					System.out.println("Digest algorithm: SHA-1");
					break;
				case e_sha256:
					System.out.println("Digest algorithm: SHA-256");
					break;
				case e_sha384:
					System.out.println("Digest algorithm: SHA-384");
					break;
				case e_sha512:
					System.out.println("Digest algorithm: SHA-512");
					break;
				case e_ripemd160:
					System.out.println("Digest algorithm: RIPEMD-160");
					break;
				case e_unknown_digest_algorithm:
					System.out.println("Digest algorithm: unknown");
					break;
				default:
					System.err.println("unrecognized digest algorithm");
					assert(false);
			}
			System.out.println(String.format("Detailed verification result: \n\t%s\n\t%s\n\t%s\n\t%s", 
				result.getDocumentStatusAsString(),
				result.getDigestStatusAsString(),
				result.getTrustStatusAsString(),
				result.getPermissionsStatusAsString()));


			DisallowedChange[] changes = result.getDisallowedChanges();
			for (DisallowedChange it2 : changes)
			{
				System.out.println(String.format(Locale.US, "\tDisallowed change: %s, objnum: %d", it2.getTypeAsString(), it2.getObjNum()));
			}

			// Get and print all the detailed trust-related results, if they are available.
			if (result.hasTrustVerificationResult())
			{
				TrustVerificationResult trust_verification_result = result.getTrustVerificationResult();
				System.out.println(trust_verification_result.wasSuccessful() ? "Trust verified." : "Trust not verifiable.");
				System.out.println(trust_verification_result.getResultString());

			   long time_of_verification = trust_verification_result.getTimeOfTrustVerification();
				switch (trust_verification_result.getTimeOfTrustVerificationEnum())
				{
					case e_current:
						System.out.println(String.format(Locale.US, "Trust verification attempted with respect to current time (as epoch time): %d", time_of_verification));
						break;
					case e_signing:
						System.out.println(String.format(Locale.US, "Trust verification attempted with respect to signing time (as epoch time): %d", time_of_verification));
						break;
					case e_timestamp:
						System.out.println(String.format(Locale.US, "Trust verification attempted with respect to secure embedded timestamp (as epoch time): %d", time_of_verification));
						break;
					default:
						System.err.println("unrecognized time enum value");
						assert(false);
				}

				if(trust_verification_result.getCertPath().length == 0 )
				{
					System.out.println("Could not print certificate path.");
				}
				else
				{
					System.out.println("Certificate path:");
					X509Certificate[] cert_path = trust_verification_result.getCertPath();
					for (int j = 0; j < cert_path.length; j++)
					{
						System.out.println("\tCertificate:"); 
						X509Certificate full_cert = cert_path[j];
						System.out.println("\t\tIssuer names:");

						X501AttributeTypeAndValue[] issuer_dn  = full_cert.getIssuerField().getAllAttributesAndValues();
						for (int i = 0; i < issuer_dn.length; i++)
						{
							System.out.println("\t\t\t" + issuer_dn[i].getStringValue());
						}
						System.out.println("\t\tSubject names:");
						X501AttributeTypeAndValue[] subject_dn = full_cert.getSubjectField().getAllAttributesAndValues();
						for (int i = 0; i < subject_dn.length; i++)
						{
							System.out.println("\t\t\t" + subject_dn[i].getStringValue());
						}
						System.out.println("\t\tExtensions:");
						for (int i = 0; i < full_cert.getExtensions().length; i++)
						{
							System.out.println("\t\t\t" + full_cert.getExtensions()[i].toString());
						}
					}
				}
			}
			else
			{
				System.out.println("No detailed trust verification result available.");
			}

			String[] unsupported_features = result.getUnsupportedFeatures();
			if (unsupported_features.length > 0)
			{
				System.out.println("Unsupported features:");

				for (String unsupported_feature : unsupported_features)
				{
					System.out.println("\t" + unsupported_feature);
				}
			}
			System.out.println("==========");
		}

		return verification_status;
	}
	
	public static void certifyPDF(String in_docpath,
		String in_cert_field_name,
		String in_private_key_file_path,
		String in_keyfile_password,
		String in_appearance_image_path,
		String in_outpath) throws PDFNetException
	{
		System.out.println("================================================================================");
		System.out.println("Certifying PDF document");

		// Open an existing PDF
		PDFDoc doc = new PDFDoc(in_docpath);

		if (doc.hasSignatures())
		{
			System.out.println("PDFDoc has signatures");
		}
		else
		{
			System.out.println("PDFDoc has no signatures");
		}

		Page page1 = doc.getPage(1);

		// Create a text field that we can lock using the field permissions feature.
		TextWidget annot1 = TextWidget.create(doc, new Rect(50, 550, 350, 600), "asdf_test_field");
		page1.annotPushBack(annot1);

		/* Create a new signature form field in the PDFDoc. The name argument is optional;
		leaving it empty causes it to be auto-generated. However, you may need the name for later.
		Acrobat doesn't show digsigfield in side panel if it's without a widget. Using a
		Rect with 0 width and 0 height, or setting the NoPrint/Invisible flags makes it invisible. */
		DigitalSignatureField certification_sig_field = doc.createDigitalSignatureField(in_cert_field_name);
		SignatureWidget widgetAnnot = SignatureWidget.create(doc, new Rect(0, 100, 200, 150), certification_sig_field);
		page1.annotPushBack(widgetAnnot);

		// (OPTIONAL) Add an appearance to the signature field.
		Image img = Image.create(doc, in_appearance_image_path);
		widgetAnnot.createSignatureAppearance(img);

		// Prepare the document locking permission level. It will be applied upon document certification.
		System.out.println("Adding document permissions.");
		certification_sig_field.setDocumentPermissions(DigitalSignatureField.DocumentPermissions.e_annotating_formfilling_signing_allowed);
		
		// Prepare to lock the text field that we created earlier.
		System.out.println("Adding field permissions.");
		String[] fields_to_lock = {"asdf_test_field"};
		certification_sig_field.setFieldPermissions(DigitalSignatureField.FieldPermissions.e_include, fields_to_lock);

		certification_sig_field.certifyOnNextSave(in_private_key_file_path, in_keyfile_password);
		
		// (OPTIONAL) Add more information to the signature dictionary.
		certification_sig_field.setLocation("Vancouver, BC");
		certification_sig_field.setReason("Document certification.");
		certification_sig_field.setContactInfo("www.pdftron.com");

		// Save the PDFDoc. Once the method below is called, PDFNet will also sign the document using the information provided.
		doc.save(in_outpath, SDFDoc.SaveMode.NO_FLAGS, null);

		System.out.println("================================================================================");
	}

	public static void signPDF(String in_docpath,
		String in_approval_field_name,
		String in_private_key_file_path,
		String in_keyfile_password,
		String in_appearance_img_path,
		String in_outpath) throws PDFNetException
	{
		System.out.println("================================================================================");
		System.out.println("Signing PDF document");

		// Open an existing PDF
		PDFDoc doc = new PDFDoc(in_docpath);

		// Retrieve the unsigned approval signature field.
		Field found_approval_field = doc.getField(in_approval_field_name);
		DigitalSignatureField found_approval_signature_digsig_field = new DigitalSignatureField(found_approval_field);
		
		// (OPTIONAL) Add an appearance to the signature field.
		Image img = Image.create(doc, in_appearance_img_path);
		SignatureWidget found_approval_signature_widget = new SignatureWidget(found_approval_field.getSDFObj());
		found_approval_signature_widget.createSignatureAppearance(img);

		// Prepare the signature and signature handler for signing.
		found_approval_signature_digsig_field.signOnNextSave(in_private_key_file_path, in_keyfile_password);

		// The actual approval signing will be done during the following incremental save operation.
		doc.save(in_outpath, SDFDoc.SaveMode.INCREMENTAL, null);

		System.out.println("================================================================================");
	}

	public static void clearSignature(String in_docpath,
		String in_digsig_field_name,
		String in_outpath) throws PDFNetException
	{
		System.out.println("================================================================================");
		System.out.println("Clearing certification signature");

		PDFDoc doc = new PDFDoc(in_docpath);

		DigitalSignatureField digsig = new DigitalSignatureField(doc.getField(in_digsig_field_name));
		
		System.out.println("Clearing signature: " + in_digsig_field_name);
		digsig.clearSignature();

		if (!digsig.hasCryptographicSignature())
		{
			System.out.println("Cryptographic signature cleared properly.");
		}

		// Save incrementally so as to not invalidate other signatures from previous saves.
		doc.save(in_outpath, SDFDoc.SaveMode.INCREMENTAL, null);

		System.out.println("================================================================================");
	}

	 public static void printSignaturesInfo(String in_docpath) throws PDFNetException
	{
		System.out.println("================================================================================");
		System.out.println("Reading and printing digital signature information");

		PDFDoc doc = new PDFDoc(in_docpath);
		if (!doc.hasSignatures())
		{
			System.out.println("Doc has no signatures.");
			System.out.println("================================================================================");
			return;
		}
		else
		{
			System.out.println("Doc has signatures.");
		}

		
		for (FieldIterator fitr = doc.getFieldIterator(); fitr.hasNext(); )
		{
			Field current = fitr.next();
			if (current.isLockedByDigitalSignature())
			{
				System.out.println("==========\nField locked by a digital signature");
			}
			else
			{
				System.out.println("==========\nField not locked by a digital signature");
			}

			System.out.println("Field name: " + current.getName());
			System.out.println("==========");
		}

		System.out.println("====================\nNow iterating over digital signatures only.\n====================");

		DigitalSignatureFieldIterator digsig_fitr = doc.getDigitalSignatureFieldIterator();
		for (; digsig_fitr.hasNext(); )
		{
			DigitalSignatureField current = digsig_fitr.next();
			System.out.println("==========");
			System.out.println("Field name of digital signature: " + new Field(current.getSDFObj()).getName());

			DigitalSignatureField digsigfield = current;
			if (!digsigfield.hasCryptographicSignature())
			{
				System.out.println("Either digital signature field lacks a digital signature dictionary, " +
					"or digital signature dictionary lacks a cryptographic Contents entry. " +
					"Digital signature field is not presently considered signed.\n" +
					"==========");
				continue;
			}

			int cert_count = digsigfield.getCertCount();
			System.out.println("Cert count: " + cert_count);
			for (int i = 0; i < cert_count; ++i)
			{
				byte[] cert = digsigfield.getCert(i);
				System.out.println("Cert #" + i + " size: " + cert.length);
			}

			DigitalSignatureField.SubFilterType subfilter = digsigfield.getSubFilter();

			System.out.println("Subfilter type: " + subfilter.ordinal());

			if (subfilter != DigitalSignatureField.SubFilterType.e_ETSI_RFC3161)
			{
				System.out.println("Signature's signer: " + digsigfield.getSignatureName());

				Date signing_time = digsigfield.getSigningTime();
				if (signing_time.isValid())
				{
					System.out.println("Signing time is valid.");
				}

				System.out.println("Location: " + digsigfield.getLocation());
				System.out.println("Reason: " + digsigfield.getReason());
				System.out.println("Contact info: " + digsigfield.getContactInfo());
			}
			else
			{
				System.out.println("SubFilter == e_ETSI_RFC3161 (DocTimeStamp; no signing info)");
			}

			if (digsigfield.hasVisibleAppearance())
			{
				System.out.println("Visible");
			}
			else
			{
				System.out.println("Not visible");
			}
			
			DigitalSignatureField.DocumentPermissions digsig_doc_perms = digsigfield.getDocumentPermissions();
			String[] locked_fields = digsigfield.getLockedFields();
			for (String it : locked_fields)
			{
				System.out.println("This digital signature locks a field named: " + it);
			}

			switch (digsig_doc_perms)
			{
			case e_no_changes_allowed:
				System.out.println("No changes to the document can be made without invalidating this digital signature.");
				break;
			case e_formfilling_signing_allowed:
				System.out.println("Page template instantiation, form filling, and signing digital signatures are allowed without invalidating this digital signature.");
				break;
			case e_annotating_formfilling_signing_allowed:
				System.out.println("Annotating, page template instantiation, form filling, and signing digital signatures are allowed without invalidating this digital signature.");
				break;
			case e_unrestricted:
				System.out.println("Document not restricted by this digital signature.");
				break;
			default:
				System.err.println("Unrecognized digital signature document permission level.");
				assert(false);
			}
			System.out.println("==========");
		}

		System.out.println("================================================================================");
	}

	public static boolean timestampAndEnableLTV(String in_docpath, 
		String in_trusted_cert_path, 
		String in_appearance_img_path,
		String in_outpath) throws PDFNetException
	{
		PDFDoc doc = new PDFDoc(in_docpath); 
		DigitalSignatureField doctimestamp_signature_field = doc.createDigitalSignatureField();  
		TimestampingConfiguration tst_config = new TimestampingConfiguration("http://adobe-timestamp.globalsign.com/?signature=sha2");
		VerificationOptions opts = new VerificationOptions(VerificationOptions.SecurityLevel.e_compatibility_and_archiving); 
		/* It is necessary to add to the VerificationOptions a trusted root certificate corresponding to 
		the chain used by the timestamp authority to sign the timestamp token, in order for the timestamp
		response to be verifiable during DocTimeStamp signing. It is also necessary in the context of this 
		function to do this for the later LTV section, because one needs to be able to verify the DocTimeStamp 
		in order to enable LTV for it, and we re-use the VerificationOptions opts object in that part. */
		opts.addTrustedCertificate(in_trusted_cert_path);
		/* By default, we only check online for revocation of certificates using the newer and lighter 
		OCSP protocol as opposed to CRL, due to lower resource usage and greater reliability. However, 
		it may be necessary to enable online CRL revocation checking in order to verify some timestamps
		(i.e. those that do not have an OCSP responder URL for all non-trusted certificates). */
		opts.enableOnlineCRLRevocationChecking(true);

		SignatureWidget widgetAnnot = SignatureWidget.create(doc, new Rect(0, 100, 200, 150), doctimestamp_signature_field);
		doc.getPage(1).annotPushBack(widgetAnnot);

		// (OPTIONAL) Add an appearance to the signature field.
		Image img = Image.create(doc, in_appearance_img_path);
		widgetAnnot.createSignatureAppearance(img);

		System.out.println("Testing timestamping configuration.");
		TimestampingTestResult config_result = tst_config.testConfiguration(opts);
		if (config_result.getStatus())
		{
			System.out.println("Success: timestamping configuration usable. Attempting to timestamp.");
		}
		else
		{
			// Print details of timestamping failure.
			System.out.println(config_result.getString());
			if (config_result.hasResponseVerificationResult())
			{
				EmbeddedTimestampVerificationResult tst_result = config_result.getResponseVerificationResult();
				System.out.println(String.format("CMS digest status: %s", tst_result.getCMSDigestStatusAsString()));
				System.out.println(String.format("Message digest status: %s", tst_result.getMessageImprintDigestStatusAsString()));
				System.out.println(String.format("Trust status: %s", tst_result.getTrustStatusAsString()));
			}
			return false;
		}

		doctimestamp_signature_field.timestampOnNextSave(tst_config, opts);

		// Save/signing throws if timestamping fails.
		doc.save(in_outpath, SDFDoc.SaveMode.INCREMENTAL, null);

		System.out.println("Timestamping successful. Adding LTV information for DocTimeStamp signature.");

		// Add LTV information for timestamp signature to document.
		VerificationResult timestamp_verification_result = doctimestamp_signature_field.verify(opts);
		if (!doctimestamp_signature_field.enableLTVOfflineVerification(timestamp_verification_result))
		{
			System.out.println("Could not enable LTV for DocTimeStamp.");
			return false;
		}
		doc.save(in_outpath, SDFDoc.SaveMode.INCREMENTAL, null);
		System.out.println("Added LTV information for DocTimeStamp signature successfully.");

		return true;
	}
	
	public static void main(String[] args) 
	{
		// Initialize PDFNet
		PDFNet.initialize();

		boolean result = true;
		String input_path = "../../TestFiles/";
		String output_path = "../../TestFiles/Output/";


		//////////////////// TEST 0: 
		/* Create an approval signature field that we can sign after certifying.
		(Must be done before calling CertifyOnNextSave/SignOnNextSave/WithCustomHandler.) */
		try
		{
			PDFDoc doc = new PDFDoc(input_path + "tiger.pdf");
			DigitalSignatureField approval_signature_field = doc.createDigitalSignatureField("PDFTronApprovalSig");
			SignatureWidget widgetAnnotApproval = SignatureWidget.create(doc, new Rect(300, 300, 500, 200), approval_signature_field);
			Page page1 = doc.getPage(1);
			page1.annotPushBack(widgetAnnotApproval);
			doc.save(output_path + "tiger_withApprovalField_output.pdf", SDFDoc.SaveMode.REMOVE_UNUSED, null);
		}
		catch (Exception e)
		{
			System.err.println(e.getMessage());
			e.printStackTrace(System.err);
			result = false;
		}

		//////////////////// TEST 1: certify a PDF.
		try
		{
			certifyPDF(input_path + "tiger_withApprovalField.pdf",
				"PDFTronCertificationSig",
				input_path + "pdftron.pfx",
				"password",
				input_path + "pdftron.bmp",
				output_path + "tiger_withApprovalField_certified_output.pdf");
			printSignaturesInfo(output_path + "tiger_withApprovalField_certified_output.pdf");
		}
		catch (Exception e)
		{
			System.err.println(e.getMessage());
			e.printStackTrace(System.err);
			result = false;
		}

		//////////////////// TEST 2: sign a PDF with a certification and an unsigned signature field in it.
		try
		{
			signPDF(input_path + "tiger_withApprovalField_certified.pdf",
				"PDFTronApprovalSig",
				input_path + "pdftron.pfx",
				"password",
				input_path + "signature.jpg",
				output_path + "tiger_withApprovalField_certified_approved_output.pdf");
			printSignaturesInfo(output_path + "tiger_withApprovalField_certified_approved_output.pdf");
		}
		catch (Exception e)
		{
			System.err.println(e.getMessage());
			e.printStackTrace(System.err);
			result = false;
		}

		//////////////////// TEST 3: Clear a certification from a document that is certified and has an approval signature.
		try
		{
			clearSignature(input_path + "tiger_withApprovalField_certified_approved.pdf",
				"PDFTronCertificationSig",
				output_path + "tiger_withApprovalField_certified_approved_certcleared_output.pdf");
			printSignaturesInfo(output_path + "tiger_withApprovalField_certified_approved_certcleared_output.pdf");
		}
		catch (Exception e)
		{
			System.err.println(e.getMessage());
			e.printStackTrace(System.err);
			result = false;
		}
		
		//////////////////// TEST 4: Verify a document's digital signatures.
		try
		{
			if (!verifyAllAndPrint(input_path + "tiger_withApprovalField_certified_approved.pdf",
              input_path + "pdftron.cer"))
			{
				result = false;
			}
		}
		catch (Exception e)
		{
			System.err.println(e.getMessage());
			e.printStackTrace(System.err);
			result = false;
		}
		//////////////////// TEST 5: Verify a document's digital signatures in a simple fashion using the document API.
		try
		{
			if (!verifySimple(input_path + "tiger_withApprovalField_certified_approved.pdf",
              input_path + "pdftron.cer"))
			{
				result = false;
			}
		}
		catch (Exception e)
		{
			System.err.println(e.getMessage());
			e.printStackTrace(System.err);
			result = false;

		}
		//////////////////// TEST 6: Timestamp a document, then add Long Term Validation (LTV) information for the DocTimeStamp.
		try
		{
			if (!timestampAndEnableLTV(input_path + "tiger.pdf",
			input_path + "GlobalSignRootForTST.cer",
			input_path + "signature.jpg",
			output_path+ "tiger_DocTimeStamp_LTV.pdf"))
			{
				result = false;
			}
		}
		catch (Exception e)
		{
			System.err.println(e.getMessage());
			e.printStackTrace(System.err);
			result = false;

		}



		//////////////////// End of tests. ////////////////////

		if (result)
		{
			System.out.println("Tests successful.\n==========");
		}
		else
		{
			System.out.println("Tests FAILED!!!\n==========");
		}
	}
}
