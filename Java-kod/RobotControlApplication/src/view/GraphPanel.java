package view;

import java.awt.Dimension;
import java.util.ArrayList;
import java.util.concurrent.ThreadLocalRandom;

import javax.swing.JPanel;
import javax.swing.JTextArea;

import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.data.category.DefaultCategoryDataset;
import org.jfree.data.xy.XYDataset;
import org.jfree.data.xy.XYSeries;
import org.jfree.data.xy.XYSeriesCollection;

public class GraphPanel extends JPanel{

	private JTextArea graphPlaceHolder;

	private ArrayList<ChartPanel> chartList;

	private String[] chartNames = {"IR F/R", "IR F/L", "IR B/R",
									"IR B/L", "Lidar Lite", "Angular velocity",
									"Angle to wall"};
	private int chartNameCounter = 0;

	private String[] chartUnits = {"cm", "cm", "cm",
									"cm", "cm", "deg/s",
									"deg"};

	public GraphPanel() {
		chartList = new ArrayList<>();

		JFreeChart iRXYChart = ChartFactory.createXYLineChart("IR", "Time [s]", "Distance [cm]", createRandomDataset(4, 4, 30));
		JFreeChart lidarXYChart = ChartFactory.createXYLineChart("Lidar", "Time [s]", "Distance [cm]", createRandomDataset(1, 0, 600));
		JFreeChart angVelXYChart = ChartFactory.createXYLineChart("Angular velocity", "Time [s]", "Ang.vel [deg/s]", createRandomDataset(1, 0, 300));
		JFreeChart angXYChart = ChartFactory.createXYLineChart("Angle to wall", "Time [s]", "Angle [deg]", createRandomDataset(1, 0, 15));

		chartList.add(new ChartPanel(iRXYChart));
		chartList.add(new ChartPanel(lidarXYChart));
		chartList.add(new ChartPanel(angVelXYChart));
		chartList.add(new ChartPanel(angXYChart));

		for(ChartPanel chart : chartList) {
			chart.setPreferredSize(new Dimension(300, 200));
			add(chart);
		}
	}

	// dummy procedure for creating dataset
	private XYDataset createRandomDataset(int numberOfSets, double min, double max) {
		XYSeriesCollection dataset = new XYSeriesCollection();
		
		for(int i = 0; i < numberOfSets; i++) {
			XYSeries series = new XYSeries(chartNames[chartNameCounter++]);
			
			for(int j = 0; j < 10; j++) {
				series.add(j, ThreadLocalRandom.current().nextDouble(min, max));
			}
			
			dataset.addSeries(series);
		}

		return dataset;

	}
}
