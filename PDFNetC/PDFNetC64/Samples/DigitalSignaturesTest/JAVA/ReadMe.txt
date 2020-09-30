In order to run the DigitalSignatureTest for Java with the custom Signature Handler, download the BouncyCastle
libraries here: https://www.bouncycastle.org/.

For Android, there is a repackaged version of BouncyCastle that is intended for the Dalvik VM called SpongyCastle. More
information here: https://github.com/rtyley/spongycastle

Place the following files in the Lib folder where PDFNetC.dll and PDFNet.jar resides:
    - bcpkix-jdk15on-xxx.jar
    - bcprov-jdk15on-xxx.jar

    * where "xxx" is the version number of bouncycastle.

Rename BCSignatureHandler.java.txt to BCSignatureHandler.java and modify the code inside as necessary.

In DigitalSignatureTest.java, replace:
		certification_sig_field.certifyOnNextSave(in_private_key_file_path, in_keyfile_password);
	with:
		BCSignatureHandler sigHandler = new BCSignatureHandler(in_private_key_file_path, in_keyfile_password);
		long sigHandlerId = doc.addSignatureHandler(sigHandler);
		certification_sig_field.certifyOnNextSaveWithCustomHandler(sigHandlerId);
		/* Add to the digital signature dictionary a SubFilter name that uniquely identifies the signature format 
		for verification tools. As an example, the custom handler defined in this file uses the CMS/PKCS #7 detached format, 
		so we embed one of the standard predefined SubFilter values: "adbe.pkcs7.detached". It is not necessary to do this 
		when using the StdSignatureHandler. */
		Obj f_obj = certification_sig_field.getSDFObj();
		f_obj.findObj("V").putName("SubFilter", "adbe.pkcs7.detached");
	...and replace:
		found_approval_signature_digsig_field.signOnNextSave(in_private_key_file_path, in_keyfile_password);
	with:
		BCSignatureHandler sigHandler = new BCSignatureHandler(in_private_key_file_path, in_keyfile_password);
		long sigHandlerId = doc.addSignatureHandler(sigHandler);
		found_approval_signature_digsig_field.signOnNextSaveWithCustomHandler(sigHandlerId);
		/* Add to the digital signature dictionary a SubFilter name that uniquely identifies the signature format 
		for verification tools. As an example, the custom handler defined in this file uses the CMS/PKCS #7 detached format, 
		so we embed one of the standard predefined SubFilter values: "adbe.pkcs7.detached". It is not necessary to do this 
		when using the StdSignatureHandler. */
		Obj f_obj = found_approval_signature_digsig_field.getSDFObj();
		f_obj.findObj("V").putName("SubFilter", "adbe.pkcs7.detached");
	...and modify the SubFilter value as indicated, if necessary.
    
Modify the script files (RunTest.sh or RunTest.bat) to include the library in the build process.

Execute the the script to build and run the sample.
