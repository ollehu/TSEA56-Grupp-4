package view;

import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.sql.Date;
import java.sql.Time;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.time.LocalDate;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.TimerTask;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ThreadLocalRandom;
import java.util.concurrent.TimeUnit;

import javax.swing.*;

import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.data.category.DefaultCategoryDataset;
import org.jfree.data.category.SlidingCategoryDataset;
import org.jfree.data.general.Dataset;
import org.jfree.data.xy.XYDataset;

public class Animator {

	private JFrame frame;
	
	private ChartPanel chartPanel;
	
	private DefaultCategoryDataset dataset;
	
	private int index = 7;
	
	private int datasetSize = 0;
	
	private int previousValue;
	
	private long startTime;
	
	public Animator() {
		
		startTime = System.currentTimeMillis();
		
		frame = new JFrame("Test monitor"); // insert window name here
		frame.setLayout(new FlowLayout()); // insert preferred layout here
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		//TODO create and add panels
		JFreeChart lineChart = ChartFactory.createLineChart("Test chart", "Index", "Number", createDataset(), 
				PlotOrientation.VERTICAL, true, true, false);
		
		chartPanel = new ChartPanel(lineChart);
		chartPanel.setPreferredSize(new Dimension(560, 367));
		frame.add(chartPanel);
		
		ScheduledExecutorService executorService = Executors.newSingleThreadScheduledExecutor();
		executorService.scheduleAtFixedRate(new Runnable() {
			
			@Override
			public void run() {
				addNumberToSet();
			}
		}, 0, 1, TimeUnit.SECONDS);
		
	}
	
	private void addNumberToSet()
	{
		long currentTime = (System.currentTimeMillis() - startTime) / 1000;
		int value = previousValue + ThreadLocalRandom.current().nextInt(-10, 10);
		previousValue = value;
		
		dataset.addValue(value, "number", Long.toString(currentTime));
		
		if(datasetSize > 10) {
			dataset.removeColumn(0);
			datasetSize--;
		}
		
		index++;
		datasetSize++;
	}
	
	private DefaultCategoryDataset createDataset() 
	{
		dataset = new DefaultCategoryDataset();
		
		dataset.addValue( 15 , "number" , "-5" );
		dataset.addValue( 30 , "number" , "-4" );
		dataset.addValue( 60 , "number" ,  "-3" );
		dataset.addValue( 35 , "number" , "-2" );
		dataset.addValue( 70 , "number" , "-1" );
		dataset.addValue( 80 , "number" , "0" );
		
		previousValue = 80;
		
		datasetSize = 6;
		
		return dataset;
		
	}
	
	public void showFrame() {
		frame.pack();
		frame.setVisible(true);
	}
	
}
