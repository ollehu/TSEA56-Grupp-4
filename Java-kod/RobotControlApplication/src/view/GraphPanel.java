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

		JFreeChart iRXYChart = ChartFactory.createXYLineChart("IR", "Time [s]", "Distance [cm]", createRandomDataset(4, 4, 30));
		JFreeChart lidarXYChart = ChartFactory.createXYLineChart("Lidar", "Time [s]", "Distance [cm]", createRandomDataset(1, 0, 600));
		JFreeChart angVelXYChart = ChartFactory.createXYLineChart("Angular velocity", "Time [s]", "Ang.vel [deg/s]", createRandomDataset(1, 0, 300));
		JFreeChart angXYChart = ChartFactory.createXYLineChart("Angle to wall", "Time [s]", "Angle [deg]", createRandomDataset(1, 0, 15));


		chartList.add(new ChartPanel(iRXYChart));
		chartList.add(new ChartPanel(lidarXYChart));
		//chartList.add(new ChartPanel(angVelXYChart));
		//chartList.add(new ChartPanel(angXYChart));

		for(ChartPanel chartPanel : chartList) {
			chartPanel.setPreferredSize(new Dimension(300, 200));
			changeFontSize(chartPanel.getChart(), 0.8);
			add(chartPanel);
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

	private void changeFontSize(JFreeChart chart, double fontSizePercentage) {
		//		Font oldTitleFont = chart.getTitle().getFont();
		//		Font oldLegendFont = chart.getLegend().getItemFont();
		//		chart.getXYPlot().get
		//		
		//		Font titleFont = new Font(oldTitleFont.getFontName(), oldTitleFont.getStyle(), (int) (oldTitleFont.getSize() * fontSizePercentage)); 
		//		Font legendFont = new Font(oldLegendFont.getFontName(), oldLegendFont.getStyle(), (int) (oldLegendFont.getSize() * fontSizePercentage)); 
		//		
		//		chart.getTitle().setFont(titleFont);
		//		chart.getLegend().setItemFont(legendFont);
		final StandardChartTheme chartTheme = (StandardChartTheme)org.jfree.chart.StandardChartTheme.createJFreeTheme();

		// The default font used by JFreeChart unable to render Chinese properly.
		// We need to provide font which is able to support Chinese rendering.
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
		
	}
	
	private double getTimeStamp() {
		if(firstTime) {
			firstTime = !firstTime;
			startTime = System.currentTimeMillis();
			return 0.0;
		} else {
			return System.currentTimeMillis() - startTime;
		}
	}
}
