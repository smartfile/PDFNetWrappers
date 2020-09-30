//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

import java.io.File;

import javax.swing.filechooser.FileFilter;

public class PDFFilter extends FileFilter {

    //Accept all directories and all PDF files.
    public boolean accept(File f) {
    	if(f.isDirectory())
    		return true;
        String extension = getExtension(f);
        if (extension != null) {
        	return extension.toLowerCase().equals("pdf");
        }
        return false;
    }

    //The description of this filter
    public String getDescription() {
        return "PDF Files (*.pdf)";
    }

    public static String getExtension(File f) {
        String ext = null;
        String s = f.getName();
        int i = s.lastIndexOf('.');

        if (i > 0 &&  i < s.length() - 1) {
            ext = s.substring(i+1).toLowerCase();
        }
        return ext;
    }
}
