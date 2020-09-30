//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

import java.util.Map;
import java.util.TreeMap;

import com.pdftron.common.PDFNetException;
import com.pdftron.pdf.struct.*;
import com.pdftron.pdf.*;
import com.pdftron.sdf.Obj;

//---------------------------------------------------------------------------------------
// This sample explores the structure and content of a tagged PDF document and dumps 
// the structure information to the console window.
//
// In tagged PDF documents StructTree acts as a central repository for information 
// related to a PDF document's logical structure. The tree consists of StructElement-s
// and ContentItem-s which are leaf nodes of the structure tree.
//
// The sample can be extended to access and extract the marked-content elements such 
// as text and images.
//---------------------------------------------------------------------------------------
public class LogicalStructureTest {
    static void PrintIndent(int indent) {
        System.out.println();
        for (int i = 0; i < indent; ++i) System.out.print("  ");
    }

    // Used in code snippet 1.
    static void ProcessStructElement(SElement element, int indent) throws PDFNetException {
        if (!element.isValid()) {
            return;
        }

        // Print out the type and title info, if any.
        PrintIndent(indent++);
        System.out.print("Type: " + element.getType());
        if (element.hasTitle()) {
            System.out.print(". Title: " + element.getTitle());
        }

        int num = element.getNumKids();
        for (int i = 0; i < num; ++i) {
            // Check is the kid is a leaf node (i.e. it is a ContentItem).
            if (element.isContentItem(i)) {
                ContentItem cont = element.getAsContentItem(i);
                int type = cont.getType();

                Page page = cont.getPage();

                PrintIndent(indent);
                System.out.print("Content Item. Part of page #" + page.getIndex());

                PrintIndent(indent);
                switch (type) {
                    case ContentItem.e_MCID:
                    case ContentItem.e_MCR:
                        System.out.print("MCID: " + cont.getMCID());
                        break;
                    case ContentItem.e_OBJR: {
                        System.out.print("OBJR ");
                        Obj ref_obj = cont.getRefObj();
                        if (ref_obj != null)
                            System.out.print("- Referenced Object#: " + ref_obj.getObjNum());
                    }
                    break;
                    default:
                        break;
                }
            } else {  // the kid is another StructElement node.
                ProcessStructElement(element.getAsStructElem(i), indent);
            }
        }
    }

    // Used in code snippet 2.
    static void ProcessElements(ElementReader reader) throws PDFNetException {
        Element element;
        while ((element = reader.next()) != null)    // Read page contents
        {
            // In this sample we process only paths & text, but the code can be
            // extended to handle any element type.
            int type = element.getType();
            if (type == Element.e_path || type == Element.e_text || type == Element.e_path) {
                switch (type) {
                    case Element.e_path:                // Process path ...
                        System.out.print("\nPATH: ");
                        break;
                    case Element.e_text:                // Process text ...
                        System.out.print("\nTEXT: " + element.getTextString() + "\n  ");
                        break;
                    case Element.e_form:                // Process form XObjects
                        System.out.print("\nFORM XObject: ");
                        //reader.FormBegin();
                        //ProcessElements(reader);
                        //reader.End();
                        break;
                }

                // Check if the element is associated with any structural element.
                // Content items are leaf nodes of the structure tree.
                SElement struct_parent = element.getParentStructElement();
                if (struct_parent.isValid()) {
                    // Print out the parent structural element's type, title, and object number.
                    System.out.print(" Type: " + struct_parent.getType()
                            + ", MCID: " + element.getStructMCID());
                    if (struct_parent.hasTitle()) {
                        System.out.print(". Title: " + struct_parent.getTitle());
                    }
                    System.out.print(", Obj#: " + struct_parent.getSDFObj().getObjNum());
                }
            }
        }
    }

    // Used in code snippet 3.
    //typedef map<int, string> MCIDPageMap;
    //typedef map<int, MCIDPageMap> MCIDDocMap;

    // Used in code snippet 3.
    static void ProcessElements2(ElementReader reader, Map<Integer, String> mcid_page_map) throws PDFNetException {
        Element element;
        while ((element = reader.next()) != null) // Read page contents
        {
            // In this sample we process only text, but the code can be extended
            // to handle paths, images, or any other Element type.
            int mcid = element.getStructMCID();
            Integer key_mcid = new Integer(mcid);
            if (mcid >= 0 && element.getType() == Element.e_text) {
                String val = element.getTextString();
                if (mcid_page_map.containsKey(key_mcid))
                    mcid_page_map.put(key_mcid, ((String) (mcid_page_map.get(key_mcid)) + val));
                else mcid_page_map.put(key_mcid, val);
            }
        }
    }

    // Used in code snippet 3.
    static void ProcessStructElement2(SElement element, Map<Integer, Map<Integer, String>> mcid_doc_map, int indent) throws PDFNetException {
        if (!element.isValid()) {
            return;
        }

        // Print out the type and title info, if any.
        PrintIndent(indent);
        System.out.print("<" + element.getType());
        if (element.hasTitle()) {
            System.out.print(" title=\"" + element.getTitle() + "\"");
        }
        System.out.print(">");

        int num = element.getNumKids();
        for (int i = 0; i < num; ++i) {
            if (element.isContentItem(i)) {
                ContentItem cont = element.getAsContentItem(i);
                if (cont.getType() == ContentItem.e_MCID) {
                    int page_num = cont.getPage().getIndex();
                    Integer page_num_key = new Integer(page_num);
                    if (mcid_doc_map.containsKey(page_num_key)) {
                        Map<Integer, String> mcid_page_map = mcid_doc_map.get(page_num_key);
                        Integer mcid_key = new Integer(cont.getMCID());
                        if (mcid_page_map.containsKey(mcid_key)) {
                            System.out.println(mcid_page_map.get(mcid_key));
                        }
                    }
                }
            } else {  // the kid is another StructElement node.
                ProcessStructElement2(element.getAsStructElem(i), mcid_doc_map, indent + 1);
            }
        }

        PrintIndent(indent);
        System.out.print("</" + element.getType() + ">");
    }


    /**
     * @param args
     */
    public static void main(String[] args) {
        PDFNet.initialize();

        // Relative path to the folder containing test files.
        String input_path = "../../TestFiles/";
        // string output_path = "../../TestFiles/Output/";

        try    // Extract logical structure from a PDF document
        {
            PDFDoc doc = new PDFDoc((input_path + "tagged.pdf"));
            doc.initSecurityHandler();

            System.out.println("____________________________________________________________");
            System.out.println("Sample 1 - Traverse logical structure tree...");
            {
                STree tree = doc.getStructTree();
                if (tree.isValid()) {
                    System.out.println("Document has a StructTree root.");

                    for (int i = 0; i < tree.getNumKids(); ++i) {
                        // Recursively get structure  info for all all child elements.
                        ProcessStructElement(tree.getKid(i), 0);
                    }
                } else {
                    System.out.println("This document does not contain any logical structure.");
                }
            }
            System.out.println("\nDone 1.");

            System.out.println("____________________________________________________________");
            System.out.println("Sample 2 - Get parent logical structure elements from");
            System.out.println("layout elements.");
            {
                ElementReader reader = new ElementReader();
                for (PageIterator itr = doc.getPageIterator(); itr.hasNext(); ) {
                    reader.begin(itr.next());
                    ProcessElements(reader);
                    reader.end();
                }
            }
            System.out.println("\nDone 2.");

            System.out.println("____________________________________________________________");
            System.out.println("Sample 3 - 'XML style' extraction of PDF logical structure and page content.");
            {
                //A map which maps page numbers(as Integers)
                //to page Maps(which map from struct mcid(as Integers) to
                //text Strings)
                Map<Integer, Map<Integer, String>> mcid_doc_map = new TreeMap<Integer, Map<Integer, String>>();
                ElementReader reader = new ElementReader();
                for (PageIterator itr = doc.getPageIterator(); itr.hasNext(); ) {
                    Page current = itr.next();
                    reader.begin(current);
                    Map<Integer, String> page_mcid_map = new TreeMap<Integer, String>();
                    mcid_doc_map.put(new Integer(current.getIndex()), page_mcid_map);
                    ProcessElements2(reader, page_mcid_map);
                    reader.end();
                }

                STree tree = doc.getStructTree();
                if (tree.isValid()) {
                    for (int i = 0; i < tree.getNumKids(); ++i) {
                        ProcessStructElement2(tree.getKid(i), mcid_doc_map, 0);
                    }
                }
            }
            System.out.println("\nDone 3.");

            doc.close();
        } catch (Exception e) {
            e.printStackTrace();
        }

        PDFNet.terminate();
    }

}
