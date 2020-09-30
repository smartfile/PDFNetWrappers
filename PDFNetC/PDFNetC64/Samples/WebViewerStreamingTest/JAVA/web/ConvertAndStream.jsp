<%--
//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------
--%>

<%@ page import="java.io.File" %>
<%@ page import="java.io.OutputStream" %>
<%@ page import="pdftron.Filters.Filter" %>
<%@ page import="pdftron.Filters.FilterReader" %>
<%@ page import="pdftron.PDF.Convert" %>
<%@ page import="pdftron.PDF.PDFNet" %>
<%   
    // path where files are located. This relative to where Tomcat's startup.bat was ran.
    System.out.println("PDFNetC.dll needs to be in under java.library.path: " + System.getProperty("java.library.path"));
    
    // get the file query parameter
    String fileName = request.getParameter("file");
    // if no query parameter was provided, use a default one.
    if (fileName == null || fileName.isEmpty())
        fileName = "newsletter.pdf";
    
    
    try {
        // check if file exists (relative to where Tomcat's startup.bat was ran, usually the Tomcat's bin directory)
        File file = new File(fileName);
        if (!file.exists()) {
            System.err.printf("File not found: \"%s\"\n", file.getAbsolutePath());
            String realFilePath = request.getServletContext().getRealPath(fileName);
            //check if the file exists (relative to this jsp file)
            file = new File(realFilePath); 
        }
        if (!file.exists()) {
            System.err.printf("File not found: \"%s\"\n", file.getAbsolutePath());
            response.sendError(HttpServletResponse.SC_NOT_FOUND, "File not found");
			return;
        }
        System.out.printf("Converting and streaming file: \"%s\"...\n", file.getAbsolutePath());
        
        // reset response headers to indicate an XOD file will be streamed instead of serving HTML file
        response.reset();
        response.setContentType("application/vnd.ms-xpsdocument");
        
        PDFNet.initialize();
        // set the conversion option to not create thumbnails on XOD files because
        // they will not be streamed back to the client.
        Convert.XODOutputOptions xodOptions = new Convert.XODOutputOptions();
        xodOptions.setOutputThumbnails(false);
        
        Filter filter = Convert.toXod(file.getAbsolutePath(), xodOptions);
        FilterReader fReader = new FilterReader(filter);

        OutputStream os = response.getOutputStream();
        
        byte[] buffer = new byte[64 * 1024]; // 64 KB chunks
        long bytesRead = 0;
        int totalBytes = 0;

        System.out.println("Start streaming...");
        while((bytesRead = fReader.read(buffer)) > 0) {
            totalBytes += bytesRead;
            os.write(buffer, 0, (int) bytesRead);
            System.out.printf("Sent total: %d bytes\n", totalBytes);
            os.flush();
        }

        System.out.println("Done.");
        os.close();
    }
    catch (Exception ex) {
        System.err.println(ex.getMessage());
        ex.printStackTrace(System.err);
        response.sendError(HttpServletResponse.SC_INTERNAL_SERVER_ERROR , ex.getMessage());
    }
%>
