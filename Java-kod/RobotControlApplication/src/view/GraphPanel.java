package view;

import java.awt.Dimension;
import java.awt.Font;
import java.util.ArrayList;
import java.util.concurrent.ThreadLocalRandom;

import javax.swing.JPanel;
import javax.swing.JTextArea;

import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.StandardChartTheme;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.data.category.DefaultCategoryDataset;
import org.jfree.data.xy.XYDataset;
import org.jfree.data.xy.XYSeries;
import org.jfree.data.xy.XYSeriesCollection;

/**
 * Panel containing all the graphs
 * @author isak
 *
 */
public class GraphPanel extends JPanel{

	private JTextArea graphPlaceHolder;

	private ArrayList<ChartPanel> chartList;
	
	private ArrayList<XYSeries> dataSeriesList;

	private String[] chartNames = {"IR F/R", "IR F/L", "IR B/R",
			"IR B/L", "Lidar Lite", "Angular velocity",
	"Angle to wall"};
	private int chartNameCounter = 0;

	private String[] chartUnits = {"cm", "cm", "cm",
			"cm", "cm", "deg/s",
	"deg"};

	private long startTime;
	private boolean firstTime = true;
	
	public GraphPanel() {
		chartList = new ArrayList<>();
		dataSeriesList = new ArrayList<>();

		// create data sets
		XYSeriesCollection iRXYDataset = new XYSeriesCollection();
		XYSeriesCollection lidarXYDataset = new XYSeriesCollection();
		
		XYSeries series;
		for(int i = 0; i < 4; i++) {
			series = new XYSeries(chartNames[chartNameCounter++]);
			series.setMaximumItemCount(100);
			
			dataSeriesList.add(series);
			iRXYDataset.addSeries(series);
		}
		
		series = new XYSeries(chartNames[chartNameCounter++]);
		series.setMaximumItemCount(100);

		dataSeriesList.add(series);
		lidarXYDataset.addSeries(series);
		
		// create JCharts
		JFreeChart iRXYChart = ChartFactory.createXYLineChart("IR", "Time [s]", "Distance [cm]", iRXYDataset);
		JFreeChart lidarXYChart = ChartFactory.createXYLineChart("Lidar", "Time [s]", "Distance [cm]", lidarXYDataset);


		chartList.add(new ChartPanel(iRXYChart));
		chartList.add(new ChartPanel(lidarXYChart));

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
		
		for(int index = 0; index < 5; index++) {
			dataSeriesList.get(index).add(timeStamp, sensorValues[index]);
		}
		
		
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
}
