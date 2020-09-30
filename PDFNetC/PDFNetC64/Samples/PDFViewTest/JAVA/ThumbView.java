//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.geom.Point2D;

import com.pdftron.common.PDFNetException;
import com.pdftron.pdf.*;

	
public class ThumbView extends PDFViewCtrl {

	/**
	 * 
	 */
	private static final long serialVersionUID = 4051717486820217947L;
	private PDFViewCtrl main_view = null;

	ThumbView()
	{
		super(200);
		this.setMinimumSize(new Dimension(200, 100));
		this.setToolMode(PDFViewCtrl.e_custom);
		setImageSmoothing(false);
		this.addMouseListener(new MouseListener(){

			public void mouseClicked(MouseEvent arg0) {
				// TODO Auto-generated method stub
				
			}

			public void mouseEntered(MouseEvent arg0) {
				// TODO Auto-generated method stub
				
			}

			public void mouseExited(MouseEvent arg0) {
				// TODO Auto-generated method stub
				
			}

			public void mousePressed(MouseEvent arg0) {
				handleMousePressed(arg0.getX(), arg0.getY());
				
			}

			public void mouseReleased(MouseEvent arg0) {
				// TODO Auto-generated method stub
				
			}
			
		});
	}
	
	void setDoc(PDFDoc doc, PDFViewCtrl main_view)
	{
		super.setDoc(doc);
		this.main_view=main_view;
	}
	
	protected void paintComponent(Graphics g)
	{
		super.paintComponent(g);
		if (main_view != null)
		{
			int cur_page = main_view.getCurrentPage();
			if (cur_page < 1) return;
			try{
			Page page = getDoc().getPage(cur_page);

			// Draw a red border around the selected page.
			Rect bbox = page.getCropBox();
			bbox.normalize();
			double left=bbox.getX1(), bottom=bbox.getY1(), right=bbox.getX2(), top=bbox.getY2(), tmp;


			Point2D.Double bottom_left=convPagePtToScreenPt(left, bottom, cur_page);
			left=bottom_left.x;
			bottom=bottom_left.y;
			Point2D.Double top_right=convPagePtToScreenPt(right, top, cur_page);
			right=top_right.x;
			top=top_right.y;
			
			if (top>bottom) { tmp=top; top=bottom; bottom=tmp; }
			if (right<left) { tmp=right; right=left; left=tmp; }
			g.setPaintMode();
			g.setColor(Color.red);
			g.drawRect( (int)(left-2)-getX(), (int)(top-1) - getY(), (int)(right-left+4), (int)(bottom-top+3));
			}
			catch(PDFNetException e)
			{
				e.printStackTrace();
			}
		}
	}
	
	protected void handleMousePressed(int x, int y)
	{
		if (main_view != null)
		{
			int cur_page = getPageNumberFromScreenPt(x+(getX()), y + (getY()));
			if (cur_page < 1) return;
			else main_view.setCurrentPage(cur_page);
			repaint();
		}
		
	}

}
