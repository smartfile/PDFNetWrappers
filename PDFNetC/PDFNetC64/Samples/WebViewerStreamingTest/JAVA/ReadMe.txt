# Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
# Consult legal.txt regarding legal and license information.

ConvertAndStream.jsp is a java servlet that will convert a source file to .xod, and stream it as the conversion is taking place. This example requires Java SE and Tomcat to run.

===========================================================================
METHOD 1: Running with Tomcat or Glassfish with NetBeans IDE
===========================================================================
1. Install Netbeans IDE (https://netbeans.org/downloads/) with a bundled server (Tomcat or Glassfish)

2. From Netbeans, File > Open Project and select this current directory.

3. Download the latest WebViewer SDK Redistributable package (http://www.pdftron.com/webviewer/download.html), and extract it under the "web" folder

4. Make sure that PDFNetC.dll exists in your java library path (java.library.path). There are several ways to do this:
	a. Copy PDFNetC.dll to C:\Windows\System32 (or C:\Windows\SysWow64 if using 32-bit PDFNet on 64-bit OS) for Windows. Copy libPDFNetC.so to /usr/lib for Linux or Mac
	b. Specify VM Options when running your Java application: 
		-Djava.library.path="/path/to/PDFNetC_LL"

5. Include PDFNet.jar in your project. (This is already done in the project)

6. Copy the samples file "PDFNetC\Samples\TestFiles\newsletter.xod" and "PDFNetC\Samples\TestFiles\newsletter.pdf"  to the "web" folder. These files are used by the sample.

7. Run the project and browse to your application.



===========================================================================
METHOD 2: Running With Tomcat via command-line
===========================================================================

1. Install Java (JRE or JDK depending on Tomcat version you will install. See here: http://wiki.apache.org/tomcat/FAQ/Miscellaneous#Q20). Make sure to also check the required Java versions here:
    http://tomcat.apache.org/whichversion.html

2. Add Java's root directory to JAVA_HOME environment variable, and add $JAVA_HOME/bin (%JAVA_HOME%\bin on Windows) to the PATH environment variable.

3. Download Tomcat from http://tomcat.apache.org/

4. Once Tomcat is extracted or installed, copy the following files to their respective directories:
    - Assume that $TOMCAT_DIR is the root directory of the Tomcat installation where bin, lib, webapps, etc. folders reside.
        a. Copy PDFNetC.dll to C:\Windows\System32 (or C:\Windows\SysWow64 if using 32-bit PDFNet on 64-bit OS) for Windows. Copy libPDFNetC.so to /usr/lib for Linux or Mac
        b. Copy some test files to the $TOMCAT_DIR (i.e. newsletter.pdf -the sample uses this document by default).
        c. Copy PDFNet.jar to $TOMCAT_DIR/lib so that the file $TOMCAT_DIR/lib/PDFNet.jar exists.
        d. Copy web\ConvertAndStream.jsp, web\index.html and newsletter.xod (the sample uses this document by default) to $TOMCAT_DIR/webapps/ROOT
        e. Download the WebViewer Redistributable (WebViewer.zip) from here: http://www.pdftron.com/webviewer/download.html
        f. Extract the contents of WebViewer.zip on $TOMCAT_DIR/webapps/ROOT (make sure you get a folder in the path of: $TOMCAT_DIR/webapps/ROOT/WebViewer).

5. Start Tomcat with the following command (open terminal then navigate to $TOMCAT_DIR)
    $ ./bin/startup.sh (for Linux or Mac)
    > .\bin\startup.bat (for Windows)

6. Open your web browser, then browse to http://localhost:8080/index.html.
