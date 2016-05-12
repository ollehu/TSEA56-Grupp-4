package view;

import java.awt.Dimension;
import java.awt.Font;
import java.util.ArrayList;
import java.util.Observable;
import java.util.Observer;

import javax.swing.JPanel;
import javax.swing.SwingUtilities;

import org.jfree.chart.*;
import org.jfree.chart.axis.ValueAxis;
import org.jfree.chart.plot.XYPlot;
import org.jfree.data.xy.*;

public class GraphPanel extends JPanel implements Observer{

	/**
	 * List of charts
	 */
	private ArrayList<ChartPanel> chartList;
	
	/**
	 * List of data series
	 */
	private ArrayList<XYSeries> dataSeriesList;

	/**
	 * Constants for instantiation
	 */
	private String[] chartNames = {"IR F/R", "IR F/L", "IR B/R",
			"IR B/L", "Lidar Lite", "Angular velocity",
	"Angle to wall"};
	private int chartNameCounter = 0;
	
	private long startTime;
	private boolean firstTime = true;
	
	/**
	 * Constructor
	 */
	public GraphPanel() {
		chartList = new ArrayList<>();
		dataSeriesList = new ArrayList<>();

		// create data sets
		XYSeriesCollection iRXYDataset = new XYSeriesCollection();
		XYSeriesCollection lidarXYDataset = new XYSeriesCollection();
		XYSeriesCollection angularVelocityXYDataset = new XYSeriesCollection();
		
		XYSeries series;
		for(int i = 0; i < 4; i++) {
			series = new XYSeries(chartNames[chartNameCounter++]);
			series.setMaximumItemCount(100);
			
			dataSeriesList.add(series);
			iRXYDataset.addSeries(series);
		}

		// add lidar chart
		series = new XYSeries(chartNames[chartNameCounter++]);
		series.setMaximumItemCount(100);
		
		dataSeriesList.add(series);
		lidarXYDataset.addSeries(series);
		
		// add ang.vel. chart
		series = new XYSeries(chartNames[chartNameCounter++]);
		series.setMaximumItemCount(100);
		
		dataSeriesList.add(series);
		angularVelocityXYDataset.addSeries(series);
		
		// create JCharts
		JFreeChart iRXYChart = ChartFactory.createXYLineChart("IR", "Time [s]", "Distance [cm]", iRXYDataset);
		JFreeChart lidarXYChart = ChartFactory.createXYLineChart("Lidar", "Time [s]", "Distance [cm]", lidarXYDataset);
		JFreeChart angularVelocityChart = ChartFactory.createXYLineChart("Angular velocity", "Time [s]", "Velocity [deg/s]", angularVelocityXYDataset);

		// adjust angular velocity range axis
		XYPlot angularVelocityPlot = (XYPlot) angularVelocityChart.getPlot();
		ValueAxis yAxis = angularVelocityPlot.getRangeAxis();
		yAxis.setRange(-100.0, 100.0 );
		
		// add charts and set preferred size
		chartList.add(new ChartPanel(iRXYChart));
		chartList.add(new ChartPanel(lidarXYChart));
		chartList.add(new ChartPanel(angularVelocityChart));

		for(ChartPanel chartPanel : chartList) {
			chartPanel.setPreferredSize(new Dimension(300, 200));
			changeFontSize(chartPanel.getChart(), 0.8);
			add(chartPanel);
		}
	}
	
	/**
	 * Sensor data observer
	 */
	@Override
	public void update(Observable o, Object arg) {
		if(o instanceof model.SensorData) {
			if(arg instanceof int[]) {
				double timeStamp = getTimeStamp();
//				SwingUtilities.invokeLater(new Runnable() {
					
//					@Override
//					public void run() {
						int[] sensorValues = (int[]) arg;
						
						// add IR sensor and Lidar values
						for(int index = 0; index < 6; index++) {
							dataSeriesList.get(index).add(timeStamp, sensorValues[index]);
						}
//					}
//				});
			}
		}
	}
	
	//================================================================================
    // Internal methods
    //================================================================================
	/**
	 * Changes font size
	 * @param chart
	 * @param fontSizePercentage
	 */
	private void changeFontSize(JFreeChart chart, double fontSizePercentage) {
		final StandardChartTheme chartTheme = (StandardChartTheme)org.jfree.chart.StandardChartTheme.createJFreeTheme();

		final Font oldExtraLargeFont = chartTheme.getExtraLargeFont();
		final Font oldLargeFont = chartTheme.getLargeFont();
		final Font oldRegularFont = chartTheme.getRegularFont();
		final Font oldSmallFont = chartTheme.getSmallFont();

		final Font extraLargeFont = new Font(oldExtraLargeFont.getFontName(), oldExtraLargeFont.getStyle(), 
				(int) (oldExtraLargeFont.getSize() * fontSizePercentage));
		final Font largeFont = new Font(oldLargeFont.getFontName(), oldLargeFont.getStyle(), 
				(int) (oldLargeFont.getSize() * fontSizePercentage));
		final Font regularFont = new Font(oldRegularFont.getFontName(), oldRegularFont.getStyle(), 
				(int) (oldRegularFont.getSize() * fontSizePercentage));
		final Font smallFont = new Font(oldSmallFont.getFontName(), oldSmallFont.getStyle(), 
				(int) (oldSmallFont.getSize() * fontSizePercentage));

		chartTheme.setExtraLargeFont(extraLargeFont);
		chartTheme.setLargeFont(largeFont);
		chartTheme.setRegularFont(regularFont);
		chartTheme.setSmallFont(smallFont);

		chartTheme.apply(chart);
	}
	
	/**
	 * Gets time stamp
	 * @return
	 */
	private double getTimeStamp() {
		if(firstTime) {
			firstTime = !firstTime;
			startTime = System.currentTimeMillis();
			return 0.0;
		} else {
			double elapsedTimeMillis = System.currentTimeMillis() - startTime;
			
			return elapsedTimeMillis / 1000;
		}
	}
}
