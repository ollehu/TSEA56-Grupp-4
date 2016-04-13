package view;

import java.awt.Dimension;
import java.util.ArrayList;

import javax.swing.JPanel;
import javax.swing.JTextArea;

import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.plot.PlotOrientation;

public class GraphPanel extends JPanel{

	private JTextArea graphPlaceHolder;
	
	private ArrayList<Integer> graphList;

	public GraphPanel() {
		graphList = new ArrayList<>();
		
//		JFreeChart lineChart = ChartFactory.createLineChart("Test chart", "Index", "Number", createDataset(), 
//				PlotOrientation.VERTICAL, true, true, false);
//		
//		chartPanel = new ChartPanel(lineChart);
//		chartPanel.setPreferredSize(new Dimension(560, 367));
//		frame.add(chartPanel);
		
//		graphPlaceHolder = new JTextArea(7,40);
//		graphPlaceHolder.setEditable(false);
//		graphPlaceHolder.setText("This is a placeholder for the graphs");
//
//		add(graphPlaceHolder);
	}

}
