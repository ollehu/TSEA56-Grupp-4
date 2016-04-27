package view;

import java.awt.Dimension;
import java.awt.Font;
import java.nio.ByteBuffer;
import java.sql.Time;
import java.util.ArrayList;
import java.util.concurrent.ThreadLocalRandom;

import javax.swing.JPanel;
import javax.swing.JTextArea;

import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.StandardChartTheme;
import org.jfree.chart.axis.ValueAxis;
import org.jfree.chart.labels.IntervalCategoryItemLabelGenerator;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.plot.XYPlot;
import org.jfree.data.category.DefaultCategoryDataset;
import org.jfree.data.xy.XYDataset;
import org.jfree.data.xy.XYSeries;
import org.jfree.data.xy.XYSeriesCollection;

import control.Handler;

/**
 * Panel containing all the graphs
 * @author isak
 *
 */
public class GraphPanel extends JPanel{

	private JTextArea graphPlaceHolder;
	
	private Animator animator;
	

	private ArrayList<ChartPanel> chartList;
	
	private ArrayList<XYSeries> dataSeriesList;

	private String[] chartNames = {"IR F/R", "IR F/L", "IR B/R",
			"IR B/L", "Lidar Lite", "Angular velocity",
	"Angle to wall"};
	private int chartNameCounter = 0;

	private String[] chartUnits = {"cm", "cm", "cm",
			"cm", "cm", "deg/s",
	"deg"};
	
	private String[] controllerNames = {"P", "D", "K"};

	private long startTime;
	private boolean firstTime = true;
	
	public GraphPanel(Animator animator) {
		this.animator = animator;
		
		chartList = new ArrayList<>();
		dataSeriesList = new ArrayList<>();

		// create data sets
		XYSeriesCollection iRXYDataset = new XYSeriesCollection();
		XYSeriesCollection lidarXYDataset = new XYSeriesCollection();
		XYSeriesCollection angularVelocityXYDataset = new XYSeriesCollection();
		XYSeriesCollection controllerXYDataset = new XYSeriesCollection();
		
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
		
		// add controller chart
		for(String controllerName : controllerNames) {
			series = new XYSeries(controllerName);
			series.setMaximumItemCount(100);
			
			dataSeriesList.add(series);
			controllerXYDataset.addSeries(series);
		}
		
		// create JCharts
		JFreeChart iRXYChart = ChartFactory.createXYLineChart("IR", "Time [s]", "Distance [cm]", iRXYDataset);
		JFreeChart lidarXYChart = ChartFactory.createXYLineChart("Lidar", "Time [s]", "Distance [cm]", lidarXYDataset);
		JFreeChart angularVelocityChart = ChartFactory.createXYLineChart("Angular velocity", "Time [s]", "Velocity [deg/s]", angularVelocityXYDataset);
//		JFreeChart controllerXYChart = ChartFactory.createXYLineChart("Controller", "Time [s]", "Value", controllerXYDataset);

		// adjust angular velocity range axis
		XYPlot angularVelocityPlot = (XYPlot) angularVelocityChart.getPlot();
		ValueAxis yAxis = angularVelocityPlot.getRangeAxis();
		yAxis.setRange(-100.0, 100.0 );
		
		chartList.add(new ChartPanel(iRXYChart));
		chartList.add(new ChartPanel(lidarXYChart));
		chartList.add(new ChartPanel(angularVelocityChart));
//		chartList.add(new ChartPanel(controllerXYChart));

		for(ChartPanel chartPanel : chartList) {
			chartPanel.setPreferredSize(new Dimension(300, 200));
			changeFontSize(chartPanel.getChart(), 0.8);
			add(chartPanel);
		}
	}

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
	
	public void updateSensorValues(int[] sensorValues) {
		double timeStamp = getTimeStamp();
		
		// add IR sensor and Lidar values
		for(int index = 0; index < 6; index++) {
			dataSeriesList.get(index).add(timeStamp, sensorValues[index]);
		}
		
		// add controller values
//		int[] controllerValues = calculateControllerValue(sensorValues);
//		int index = 5;
//		for(int controllerValue : controllerValues) {
//			dataSeriesList.get(index).add(timeStamp, controllerValue);
//			index++;
//		}
	}
	
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
	
	private int[] calculateControllerValue(int[] sensorValues) {
		int[] controllerValues = new int [3];

		int[] oldDistance = new int[20];
		int preferredDistance = 100;

		double period = 0.2;
		int index = 5;
		
		// calculate distance to wall
		int distance = 10;
		
		if(sensorValues[0] < 245 && sensorValues[1] < 245) {
			distance = (sensorValues[0] + sensorValues[1]) / 2;
		} else if (sensorValues[0] < 245 && sensorValues[1] < 245) {
			distance = (sensorValues[2] + sensorValues[3]) / 2;
		} else {
			//TODO handle crossroads
		}
		
		// get controller coefficients P (*100), D (*100), K (*10)
		int[] controllerCoefficients = animator.getTablePanel().getControllerCoefficients();
		
		// calculate P, D, K
		double P = ((double) controllerCoefficients[0]) / 100;
		double D = ((double) controllerCoefficients[1]) / 100;
		double K = ((double) controllerCoefficients[2]) / 10;
		
		double p_out = P * (distance - preferredDistance);
		double d_out = D * (distance - oldDistance[index]) / (index * period);
		
		int y_out = (int) (K *(p_out + d_out));
		
		controllerValues[0] = (int) p_out;
		controllerValues[1] = (int) d_out;
		controllerValues[2] = y_out;
		
		return controllerValues;
	}
}
