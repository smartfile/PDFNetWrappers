//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

import java.awt.BorderLayout;
import java.awt.Desktop;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.print.PageFormat;
import java.awt.print.Printable;
import java.awt.print.PrinterException;
import java.net.URI;
import java.util.HashMap;

import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JSplitPane;
import javax.swing.JTabbedPane;
import javax.swing.JTree;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.TreePath;

import com.pdftron.common.PDFNetException;
import com.pdftron.pdf.Action;
import com.pdftron.pdf.Bookmark;
import com.pdftron.pdf.Destination;
import com.pdftron.pdf.ErrorReportProc;
import com.pdftron.pdf.PDFDoc;
import com.pdftron.pdf.PDFDraw;
import com.pdftron.pdf.PDFViewCtrl;
import com.pdftron.pdf.PDFViewScrollPane;
import com.pdftron.pdf.Page;

class PDFErrorProc implements ErrorReportProc {

    public void reportError(String message, Object data) {
        JOptionPane.showMessageDialog((JFrame) data, message, "PDFViewCtrl Error", JOptionPane.ERROR_MESSAGE);
    }
}

public class PDFViewer extends JPanel implements TreeSelectionListener, Printable {

    /**
     *
     */
    private static final long serialVersionUID = 8854227293093804978L;
    PDFViewCtrl view;
    PDFDraw print_renderer = null;
    HashMap action_map = new HashMap();
    JTabbedPane tabPane;
    JSplitPane splitPane;
    JTree bookTree = null;
    JTree layerTree = null;
    PDFViewScrollPane pdfviewPane;
    int selection_status;
    int TEXT_SELECT_RECT = 0;
    int TEXT_SELECT = 1;

    PDFViewer() {
        this.setLayout(new BorderLayout(10, 10));
    }

    public void setSelectionStatus(int type) {
        selection_status = type;
    }

    public boolean setDoc(String filepath, JFrame mainFrame) {
        boolean initialize = (view == null);

        try {
            PDFDoc doc = new PDFDoc(filepath);
            if (!doc.initSecurityHandler()) {
                boolean success = PasswordDialog.isSuccessful(this.getParent(), doc);
                if (!success) {
                    JOptionPane.showMessageDialog(this.getParent(), "Document authentication error");
                    return false;
                }
            }

            if (!initialize) {
                splitPane.remove(tabPane);
                splitPane.remove(pdfviewPane);
            } else {
                splitPane = new JSplitPane();
            }

            view = new PDFViewCtrl();

            tabPane = new JTabbedPane();

            // Add bookmarks tab (if any)
            if (doc.getFirstBookmark().isValid()) {
                DefaultMutableTreeNode root = new DefaultMutableTreeNode("Dummy");
                bookTree = new JTree(root);
                setUpBookmarkTree(bookTree, doc.getFirstBookmark(), root);
                bookTree.setRootVisible(false);
                bookTree.expandPath(new TreePath(root));
                bookTree.addTreeSelectionListener(this);
                tabPane.addTab("Bookmarks", new JScrollPane(bookTree));
                tabPane.setMinimumSize(new Dimension(0, 0));
                splitPane.setDividerLocation(200);
            }

            // Add layers tab (if any).
            if (doc.hasOC()) {
                DefaultMutableTreeNode root = new DefaultMutableTreeNode("Dummy");
                layerTree = new JTree(root);
                com.pdftron.pdf.ocg.Config cfg = doc.getOCGConfig();
                setUpLayerTree(layerTree, cfg.getOrder(), cfg, root);
                layerTree.setRootVisible(false);
                layerTree.expandPath(new TreePath(root));
                layerTree.addTreeSelectionListener(this);
                tabPane.addTab("Layers", new JScrollPane(layerTree));
                tabPane.setMinimumSize(new Dimension(0, 0));
                splitPane.setDividerLocation(200);
            }

            splitPane.setLeftComponent(tabPane);

            view.setErrorReportProc(new PDFErrorProc(), mainFrame);
            // Enable automatic URL recognition
            boolean enableUrlExtraction = false;
            view.setUrlExtraction(enableUrlExtraction);
            view.setDoc(doc);

            // Add thumbnails (page preview) tab.
            ThumbView v = new ThumbView();
            v.setDoc(doc, view);
            tabPane.addTab("Pages", v.getViewPane());
            
            if (bookTree == null && layerTree == null) {
                tabPane.setMinimumSize(new Dimension(0, 0));
                splitPane.setDividerLocation(0);
            }
            
            this.add(splitPane, BorderLayout.CENTER);

            pdfviewPane = view.getViewPane();
            splitPane.setRightComponent(pdfviewPane);
            view.addMouseListener(new MouseAdapter() {
                public void mouseClicked(MouseEvent e) {
                    try {
                        PDFViewCtrl.LinkInfo link = view.getLinkAt((int) (e.getX() - view.getHScrollPos()), (int) (e.getY() - view.getVScrollPos()));
                        if (link != null) {
                            Desktop.getDesktop().browse(new URI(link.getURL()));
                        }
                    } catch (Exception ex) {
                        System.out.println(ex.getMessage());
                    }
                }
            });

        } catch (PDFNetException e) {
            e.printStackTrace();
        }
        //view.getParent().setBackground(Color.WHITE);
        //((JViewport)view.getParent()).setBackground(Color.RED);
        //((JViewport)view.getParent()).getParent().setBackground(Color.YELLOW);

        this.validate();
        return true;
    }

    public void close() {
        this.remove(splitPane);
        view = null;
    }

    void setUpBookmarkTree(JTree t, Bookmark item, DefaultMutableTreeNode m) throws PDFNetException {
        for (int i = 0; item.isValid(); item = item.getNext(), ++i) {
            DefaultMutableTreeNode new_node = new DefaultMutableTreeNode(item.getTitle());
            if (item.getAction().isValid() && item.getAction().getType() == Action.e_GoTo) {
                Destination dest = item.getAction().getDest();
                if (dest.isValid()) {
                    action_map.put(new_node, dest.getPage());
                }
            }
            m.add(new_node);
            if (item.isOpen()) {
                t.expandPath(new TreePath(new_node.getPath()));
            }
            if (item.hasChildren()) // Recursively add children sub-trees
            {
                setUpBookmarkTree(t, item.getFirstChild(), new_node);
            }
        }
    }

    // Populate the tree control with OCG (Optional Content Group) layers.
    void setUpLayerTree(JTree t, com.pdftron.sdf.Obj layer_arr, com.pdftron.pdf.ocg.Config init_cfg, DefaultMutableTreeNode m) throws PDFNetException {
        if (layer_arr == null || layer_arr.isArray() == false) {
            return; // must be an array.
        }

        com.pdftron.sdf.Obj lobj;
        int i, sz = (int) layer_arr.size();
        for (i = 0; i < sz; ++i) {
            lobj = layer_arr.getAt(i);
            if (lobj.isArray() && lobj.size() > 0) {
                DefaultMutableTreeNode new_node = new DefaultMutableTreeNode();
                // new_node.SetCheck(true);
                m.add(new_node);
                t.expandPath(new TreePath(new_node.getPath()));
                // Recursively add children sub-trees
                setUpLayerTree(t, lobj, init_cfg, new_node);
            } else if (i == 0 && lobj.isString()) {
                m.setUserObject(lobj.getAsPDFText());
            } else {
                com.pdftron.pdf.ocg.Group grp = new com.pdftron.pdf.ocg.Group(lobj);
                if (grp.isValid()) {
                    DefaultMutableTreeNode new_node = new DefaultMutableTreeNode(grp.getName());
                    //new_node.SetCheck(grp.getInitialState(init_cfg));
                    m.add(new_node);
                    action_map.put(new_node, grp);
                }
            }
        }
    }

    public void valueChanged(TreeSelectionEvent event) {
        try {
            Object src = event.getSource();
            if (src == bookTree) // Handle Bookmark events
            {
                Page page;
                view.getDoc().lock();
                if ((page = (Page) action_map.get(((DefaultMutableTreeNode) bookTree.getLastSelectedPathComponent()))) != null) {
                    view.setCurrentPage(page.getIndex());
                }
                view.getDoc().unlock();
            } else if (src == layerTree) // Handle Layer selection events.
            {
                com.pdftron.pdf.ocg.Context ctx = view.getOCGContext();
                if (ctx == null) {
                    return;
                }

                com.pdftron.pdf.ocg.Group grp = (com.pdftron.pdf.ocg.Group) action_map.get((DefaultMutableTreeNode) layerTree.getLastSelectedPathComponent());
                if (grp != null) {
                    //BOOL checked = tree.GetCheck(hnode);
                    ctx.setState(grp, !ctx.getState(grp)); // Toggle the state.
                    view.update();
                }
            }
        } catch (PDFNetException e) {
            JOptionPane.showMessageDialog(PDFViewer.this.getParent(), e, "Error", JOptionPane.ERROR_MESSAGE);
        }
    }

    public int print(Graphics g, PageFormat format, int page_num)
            throws PrinterException {
        try {
            PDFDoc doc = this.view.getDoc();
            if (print_renderer == null) {
                print_renderer = new PDFDraw();
                print_renderer.setDPI(200);
            }
            if (page_num < 0 || page_num >= doc.getPageCount()) {
                return Printable.NO_SUCH_PAGE;
            }

            print_renderer.drawInRect(g, doc.getPage(page_num + 1), 0, 0,
                    (int) (format.getWidth()), (int) format.getHeight());

            return Printable.PAGE_EXISTS;
        } catch (PDFNetException e) {
            e.printStackTrace();
        }
        return Printable.NO_SUCH_PAGE;
    }
}
