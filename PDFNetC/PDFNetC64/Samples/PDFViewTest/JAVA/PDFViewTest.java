//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.print.PrinterException;
import java.awt.print.PrinterJob;
import java.io.File;

import javax.swing.ButtonGroup;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JCheckBoxMenuItem;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JToggleButton;
import javax.swing.JToolBar;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;
import javax.swing.border.BevelBorder;

import com.pdftron.common.PDFNetException;
import com.pdftron.pdf.CurrentPageProc;
import com.pdftron.pdf.PDFNet;
import com.pdftron.pdf.PDFViewCtrl;
import com.pdftron.sdf.SDFDoc;


class PDFStatusBar extends JPanel implements CurrentPageProc{
    
	/**
	 * 
	 */
	private static final long serialVersionUID = 4347425418144912904L;
	JLabel label;
    /** Creates a new instance of StatusBar */
    public PDFStatusBar() 
    {
        super();
        super.setPreferredSize(new Dimension(100, 30));
        label=new JLabel("");
        this.setLayout(new BorderLayout());
        this.add(label, BorderLayout.CENTER);
        this.setBorder(new BevelBorder(BevelBorder.LOWERED));
    }

	public void reportCurrentPage(int current_page, int num_pages, Object data) {
		
		label.setText(" Page " + current_page + " of " + num_pages + ".");
	}        
} 

public class PDFViewTest extends JFrame{
	/**
	 * 
	 */
	private static final long serialVersionUID = -5186302141963453582L;
	PDFViewCtrl current;
	PDFViewer viewer;
	JToolBar toolBar;
	PDFStatusBar statusBar;
	int tool_mode;
	
	public PDFViewTest()
	{
		JPanel cp=new JPanel();
		cp.setPreferredSize(new Dimension(800,600));
		setContentPane(cp);
		//this.setBackground(Color.RED);
        cp.setLayout(new BorderLayout());

        //Create the toolbar.
        toolBar = new JToolBar();
        addButtons(toolBar);
        
        JMenuBar b=new JMenuBar();
        addMenuItems(b);
        this.setJMenuBar(b);

        cp.add(toolBar, BorderLayout.PAGE_START);
		PDFNet.initialize();
		this.setTitle("PDFViewCtrl");
		viewer=new PDFViewer();
        cp.add(viewer, BorderLayout.CENTER);
        statusBar=new PDFStatusBar();
        cp.add(statusBar, BorderLayout.SOUTH);
        
        this.addWindowListener(new WindowAdapter(){
			public void windowClosed(WindowEvent arg0) {
				System.exit(0);				
			}
			public void windowClosing(WindowEvent arg0) {
				System.exit(0);
			}        	
        });
        
        this.setContentPane(cp);
        
        this.pack();
        this.show();
    }

	
	
    private void addMenuItems(JMenuBar b) {
    	
    	//
    	//
    	// File Menu
    	//
    	//
    	
    	JMenu fileMenu=new JMenu("File");
    	JMenuItem openItem=new JMenuItem("Open..");
		openItem.addActionListener(new ActionListener(){

			public void actionPerformed(ActionEvent arg0) {
					open();
			}
			
		});
		fileMenu.add(openItem);
    	
    	
    	JMenuItem closeItem=new JMenuItem("Close");
		closeItem.addActionListener(new ActionListener(){

			public void actionPerformed(ActionEvent arg0) {
					close();
			}
			
		});
    	fileMenu.add(closeItem);
    	
    	fileMenu.addSeparator();
    	
    	JMenuItem printItem=new JMenuItem("Print");
		printItem.addActionListener(new ActionListener(){

			public void actionPerformed(ActionEvent arg0) {
					print();
			}

		
		});
		fileMenu.add(printItem);
    	
    	fileMenu.addSeparator();
		
    	JMenuItem exitItem=new JMenuItem("Exit");
		exitItem.addActionListener(new ActionListener(){

			public void actionPerformed(ActionEvent arg0) {
					System.exit(0);
			}
			
		});
		fileMenu.add(exitItem);
    	
    	b.add(fileMenu);
    	
    	//
    	//
    	// View Menu 
    	//
    	//
    	
    	JMenu viewMenu=new JMenu("View");
    	final JCheckBoxMenuItem toolbarItem=new JCheckBoxMenuItem("Menu Bar", true);
		toolbarItem.addActionListener(new ActionListener(){

			public void actionPerformed(ActionEvent arg0) {
					toolBar.setVisible(toolbarItem.getState());		
			}
			
		});
		
    	viewMenu.add(toolbarItem);
    	final JCheckBoxMenuItem statusItem=new JCheckBoxMenuItem("Status Bar", true);
		statusItem.addActionListener(new ActionListener(){

			public void actionPerformed(ActionEvent arg0) {
					statusBar.setVisible(statusItem.getState());		
			}
			
		});
    	viewMenu.add(statusItem);
    	
    	
    	
    	
    	viewMenu.addSeparator();
    	JMenuItem zoomInItem=new JMenuItem("Zoom In");
		zoomInItem.addActionListener(new ActionListener(){

			public void actionPerformed(ActionEvent arg0) {
				zoomIn();
			}
			
		});
    	viewMenu.add(zoomInItem);
    	
    	
    	JMenuItem zoomOutItem=new JMenuItem("Zoom Out");
		zoomOutItem.addActionListener(new ActionListener(){

			public void actionPerformed(ActionEvent arg0) {
				zoomOut();
			}
			
		});
    	    	
    	viewMenu.add(zoomOutItem);
    	viewMenu.addSeparator();
    	
    	
    	JMenuItem firstItem=new JMenuItem("First Page");
		firstItem.addActionListener(new ActionListener(){

			public void actionPerformed(ActionEvent arg0) {
				firstPage();
			}
			
		});
    	viewMenu.add(firstItem);
    	
    	JMenuItem prevItem=new JMenuItem("Previous Page");
		prevItem.addActionListener(new ActionListener(){

			public void actionPerformed(ActionEvent arg0) {
				prevPage();
			}
			
		});
    	viewMenu.add(prevItem);
    	
    	
    	JMenuItem nextItem=new JMenuItem("Next Page");
		nextItem.addActionListener(new ActionListener(){

			public void actionPerformed(ActionEvent arg0) {
				nextPage();
			}
			
		});
    	viewMenu.add(nextItem);
    	
    	JMenuItem lastItem=new JMenuItem("Last Page");
		lastItem.addActionListener(new ActionListener(){

			public void actionPerformed(ActionEvent arg0) {
				lastPage();
			}
			
		});
    	viewMenu.add(lastItem);
    	
    	b.add(viewMenu);
    	
    	
    	
    	//
    	//
    	// Options Menu
    	//
    	//
	   	JMenu optionMenu=new JMenu("Options");
    	final JCheckBoxMenuItem smoothImages=new JCheckBoxMenuItem("Smooth Images", true),
    	antiAlias=new JCheckBoxMenuItem("Anti-Aliased Rendering", true), 
    	overPrint=new JCheckBoxMenuItem("Overprint", true), 
    	transGrid=new JCheckBoxMenuItem("Show Page Transparency Grid", false), 
    	builtInRasterizer= new JCheckBoxMenuItem("Built-In Rasterizer", true);
    	
		smoothImages.addActionListener(new ActionListener(){

			public void actionPerformed(ActionEvent arg0) {
				if(current!=null)
				{
					current.setImageSmoothing(smoothImages.getState());
				}					
			}
			
		});
		
		antiAlias.addActionListener(new ActionListener(){

			public void actionPerformed(ActionEvent arg0) {
				if(current!=null)
				{
					current.setAntiAliasing(antiAlias.getState());
				}					
			}
			
		});
		
		overPrint.addActionListener(new ActionListener(){

			public void actionPerformed(ActionEvent arg0) {
				if(current!=null)
				{
				    try {
					    if ( overPrint.getState() )
						    current.setOverprint(1);
					    else
						    current.setOverprint(0);
				    } catch (PDFNetException e) {
				        // TODO Auto-generated catch block
				        e.printStackTrace();
			        }
				}					
			}
			
		});
		
		transGrid.addActionListener(new ActionListener(){

			public void actionPerformed(ActionEvent arg0) {
				if(current!=null)
				{
					current.setPageTransparencyGrid(transGrid.getState());
				}					
			}
		});
		
		builtInRasterizer.addActionListener(new ActionListener(){

			public void actionPerformed(ActionEvent arg0) {
				if(current!=null)
				{
					current.setRasterizer(builtInRasterizer.getState());
				}					
			}
			
		});
		
		
		
		optionMenu.add(smoothImages);
		optionMenu.add(antiAlias);
		optionMenu.add(overPrint);
		optionMenu.add(transGrid);
		optionMenu.add(builtInRasterizer);
		b.add(optionMenu);
		
		
		
		
		
		JMenu helpMenu=new JMenu("Help");
		
		
		
		JMenuItem aboutItem=new JMenuItem("About");
		aboutItem.addActionListener(new ActionListener(){

			public void actionPerformed(ActionEvent arg0) {
				about();
			}
			
		});
		helpMenu.add(aboutItem);
			
    	b.add(helpMenu);
    	
    	
	}

	protected void addButtons(JToolBar toolBar) {

    	JButton button = null;

    	button = makeNavigationButton("fileopen", 
    	"Open");
        button.addActionListener(new ActionListener()
        {


			public void actionPerformed(ActionEvent arg0) {
				open();				
			}
        	
        });
    	toolBar.add(button);

    	button = makeNavigationButton("saveas", 
    	"Save As");
    	toolBar.add(button);
        button.addActionListener(new ActionListener()
        {


			public void actionPerformed(ActionEvent arg0) {
				save();				
			}
        	
        });
        toolBar.add(new JToolBar.Separator());
       
        ButtonGroup group=new ButtonGroup();
		final JToggleButton tb1, tb2, tb3;
    	tb1 = makeToolButton("handtool", 
    	"Hand Tool");
    	tb1.addActionListener(new ActionListener()
        {


			public void actionPerformed(ActionEvent arg0) {
				if(tb1.isSelected())
					setTool(tool_mode=PDFViewCtrl.e_pan);			
			}
        	
        });
    	tb1.doClick();
    	toolBar.add(tb1);
        group.add(tb1);

    	tb2 = makeToolButton("text2tool", 
    	"Rectangular Text Selection Tool");
    	tb2.addActionListener(new ActionListener()
        {


			public void actionPerformed(ActionEvent arg0) {
				if(tb2.isSelected())
					setTool(tool_mode=PDFViewCtrl.e_text_rect_select);						
			}
        	
        });
    	toolBar.add(tb2);
        group.add(tb2);
        
    	tb3 = makeToolButton("texttool", 
    	"Structure Text Selection Tool");
    	tb3.addActionListener(new ActionListener()
        {


			public void actionPerformed(ActionEvent arg0) {
				if(tb3.isSelected())
					setTool(tool_mode=PDFViewCtrl.e_text_struct_select);		
			}
        	
        });
    	toolBar.add(tb3);
        group.add(tb3);
    	
    	toolBar.add(new JToolBar.Separator());
        button = makeNavigationButton("fitpage",
                                      "Fit Page");
        button.addActionListener(new ActionListener()
        {


			public void actionPerformed(ActionEvent arg0) {
				fitPage();				
			}
        	
        });
        toolBar.add(button);

        button = makeNavigationButton("fitwidth", 
                                      "Fit Width");
        button.addActionListener(new ActionListener()
        {


			public void actionPerformed(ActionEvent arg0) {
				fitWidth();				
			}
        	
        });
        toolBar.add(button);

        toolBar.add(new JToolBar.Separator());
        
        button = makeNavigationButton("zoomout", 
                                      "Zoom Out");
        button.addActionListener(new ActionListener()
        {


			public void actionPerformed(ActionEvent arg0) {
				zoomOut();				
			}
        	
        });  
        toolBar.add(button);
        
        button = makeNavigationButton("zoomin", 
                                      "Zoom In");
        button.addActionListener(new ActionListener()
        {


			public void actionPerformed(ActionEvent arg0) {
				zoomIn();				
			}
        	
        });      
             
        
        
        toolBar.add(button);
        toolBar.add(new JToolBar.Separator());
        
        button = makeNavigationButton("first", 
                                      "First Page");
        button.addActionListener(new ActionListener()
        {


			public void actionPerformed(ActionEvent arg0) {
				firstPage();				
			}
        	
        });
        toolBar.add(button);
        
        button = makeNavigationButton("prev", 
                                      "Previous Page");
        button.addActionListener(new ActionListener()
        {


			public void actionPerformed(ActionEvent arg0) {
				prevPage();				
			}
        	
        });
        toolBar.add(button);
        
        button = makeNavigationButton("next", 
                                      "Next Page");
        button.addActionListener(new ActionListener()
        {


			public void actionPerformed(ActionEvent arg0) {
				nextPage();				
			}
        	
        });
        toolBar.add(button);
        
        button = makeNavigationButton("last", 
                                      "Last Page");
        button.addActionListener(new ActionListener()
        {


			public void actionPerformed(ActionEvent arg0) {
				lastPage();				
			}
        	
        });
        toolBar.add(button);
        
        
        
        toolBar.add(new JToolBar.Separator());  

        button = makeNavigationButton("single", 
        "Single");
        button.addActionListener(new ActionListener()
        {


        	public void actionPerformed(ActionEvent arg0) {
        		displaySingle();				
        	}

        });
        toolBar.add(button);

        button = makeNavigationButton("single_cont", 
        "Single Continuous");
        button.addActionListener(new ActionListener()
        {


        	public void actionPerformed(ActionEvent arg0) {
        		displaySingleCont();				
        	}

        });
        toolBar.add(button);    

        button = makeNavigationButton("facing", 
        "Facing");
        button.addActionListener(new ActionListener()
        {


        	public void actionPerformed(ActionEvent arg0) {
        		displayFacing();				
        	}

        });
        toolBar.add(button);

        button = makeNavigationButton("facing_cont", 
        "Facing Continuous");
        button.addActionListener(new ActionListener()
        {


        	public void actionPerformed(ActionEvent arg0) {
        		displayFacingCont();				
        	}

        });
        toolBar.add(button);
      
        toolBar.add(new JToolBar.Separator());
        
        button = makeNavigationButton("rotatecw", 
        "Rotate Clockwise");
        button.addActionListener(new ActionListener()
        {


			public void actionPerformed(ActionEvent arg0) {
				rotateClockwise();				
			}
        	
        });
        toolBar.add(button);

        button = makeNavigationButton("rotateccw", 
        "Rotate CounterClockwise");
        button.addActionListener(new ActionListener()
        {


			public void actionPerformed(ActionEvent arg0) {
				rotateCounterClockwise();				
			}
        	
        });
        
        
        toolBar.add(button);
    }
		
		
		
    protected void save() {
    	if(current!=null)
    	{
		JFileChooser fc = new JFileChooser();
		fc.setFileFilter(new PDFFilter());
		int returnVal = fc.showSaveDialog(this);

		if (returnVal == JFileChooser.APPROVE_OPTION) {
			String filepath = fc.getSelectedFile().getAbsolutePath();
			File resFile;
			if(filepath.lastIndexOf('.')<0)
			{
				filepath=filepath+".pdf";
				//if(filepath.substring(filepath.lastIndexOf('.')).toLowerCase().equals(".pdf"));
				resFile=new File(filepath);
			}
			else
				resFile=fc.getSelectedFile();

			if(resFile.exists())
			{
				if(resFile.isDirectory())
				{
					JOptionPane.showMessageDialog(this, "Cannot Overwrite a Directory.", "PDFViewCtrl Error", JOptionPane.ERROR_MESSAGE);		
				}
				else
				{
					int res=JOptionPane.showConfirmDialog(this, "File Exists, Overwrite?", "Confirm Overwrite", JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE);
					if(!(res==JOptionPane.OK_OPTION))
							return;
				}
				
			}
			
			try {
				current.getDoc().save(filepath, SDFDoc.SaveMode.NO_FLAGS, null);
			} catch (PDFNetException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
		}
    	}
		
	}



	private JToggleButton makeToolButton(String imageName, String toolTipText) {
//  	Look for the image.
    	String imgLocation = "res/"
    		+ imageName
    		+ ".gif";


//  	Create and initialize the button.
    	JToggleButton button = new JToggleButton();
    	button.setToolTipText(toolTipText);
    	ImageIcon img=new ImageIcon(imgLocation);
   		button.setIcon(img);
    	return button;
	}

	protected void setTool(int mode) {
    	if(current!=null)
    	{
    		current.setToolMode(mode);
    	}
		
	}

	protected JButton makeNavigationButton(String imageName,
    		String toolTipText) {
//  	Look for the image.
    	String imgLocation = "res/"
    		+ imageName
    		+ ".gif";


//  	Create and initialize the button.
    	JButton button = new JButton();
    	button.setToolTipText(toolTipText);
    	ImageIcon img=new ImageIcon(imgLocation);
   		button.setIcon(img);
    	return button;
    }	
		
	void zoomIn()
	{
		if(current!=null)
		{
		current.setZoom(current.getZoom() * 2);
		}
	}
	
	void zoomOut()
	{
		if(current!=null)
		{
		current.setZoom(current.getZoom()/2);
		}
	}
		
	void rotateClockwise()
	{
		if(current!=null)
		{
		current.rotateClockwise();
		}
	}
	
	void rotateCounterClockwise()
	{
		if(current!=null)
		{
		current.rotateCounterClockwise();
		}		
	}
	
	
	void firstPage()
	{
		if(current!=null)
		{
		current.gotoFirstPage();
		}	
	}
	
	
	void prevPage()
	{
		if(current!=null)
		{
		current.gotoPreviousPage();	
		}	
	}
	
	
	void nextPage()
	{
		if(current!=null)
		{
		current.gotoNextPage();
		}
	}
	
	void lastPage()
	{
		if(current!=null)
		{
		current.gotoLastPage();
		}
	}

	void fitPage()
	{
		if(current!=null)
		{
		current.setPageViewMode(PDFViewCtrl.e_fit_page);
		}	
	}
	
	void fitWidth()
	{
		if(current!=null)
		{
		current.setPageViewMode(PDFViewCtrl.e_fit_width);
		}
	}
		
		
	void displaySingle()
	{
		if(current!=null)
		{
		current.setPagePresentationMode(PDFViewCtrl.e_single_page);
		}
	}
	
	void displaySingleCont()
	{
		if(current!=null)
		{
		current.setPagePresentationMode(PDFViewCtrl.e_single_continuous);
		}
	}
	
	void displayFacing()
	{
		if(current!=null)
		{
		current.setPagePresentationMode(PDFViewCtrl.e_facing);
		}
	}
		
	void displayFacingCont()
	{
		if(current!=null)
		{
		current.setPagePresentationMode(PDFViewCtrl.e_facing_continuous);
		}
	}
		
		
	void open()
	{
		JFileChooser fc = new JFileChooser();
		fc.setFileFilter(new PDFFilter());
		int returnVal = fc.showOpenDialog(this);

		if (returnVal == JFileChooser.APPROVE_OPTION) {
			String filepath = fc.getSelectedFile().getAbsolutePath();
			if(viewer.setDoc(filepath, this))
			{
				current=viewer.view;
				current.setCurrentPageProc(statusBar, null);
				current.setToolMode(tool_mode);
				this.setTitle("PDFViewCtrl - [" + filepath+"]");
			}
		}

	}
		
	
	void close()
	{
		if(current!=null)
		{
			viewer.close();
			current=null;
			this.setTitle("PDFViewCtrl");
			repaint();
		}
	}
	

	void print()
	{
		if(current!=null)
		{
        PrinterJob job = PrinterJob.getPrinterJob();
        job.setPrintable(viewer);
        boolean ok = job.printDialog();
        if (ok) {
            try {
                 job.print();
            } catch (PrinterException ex) {
            	//The Print did not complete successfully
            	ex.printStackTrace();
            }
        }
		}
	}

	void about()
	{
		JOptionPane.showMessageDialog(this, 
				"PDFViewCtrl Sample Application Version 1.0");
	}

	public static void main(String[] args)
	{
	    try {
		    // Set System L&F
	        //UIManager.setLookAndFeel(
	          // UIManager.getSystemLookAndFeelClassName());
	       UIManager.setLookAndFeel("javax.swing.plaf.metal.MetalLookAndFeel");
	       // UIManager.setLookAndFeel("com.sun.java.swing.plaf.motif.MotifLookAndFeel");


	    } 
	    catch (UnsupportedLookAndFeelException e) {
	       // handle exception
	    }
	    catch (ClassNotFoundException e) {
	       // handle exception
	    }
	    catch (InstantiationException e) {
	       // handle exception
	    }
	    catch (IllegalAccessException e) {
	       // handle exception
	    }

		new PDFViewTest();
	}
}
