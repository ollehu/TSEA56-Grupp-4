package model;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Observable;
import java.util.Observer;

import javax.swing.JFrame;
import javax.swing.JOptionPane;

public class Log {

	/**
	 * Log file
	 */
	private File lastLog;

	/**
	 * File name variables
	 */
	private String filePath = "logs/";
	private String fileName = "Log ";
	private String fileExtension = ".txt";

	private boolean logEmpty;
	
	/**
	 * Constructor
	 */
	public Log() {
		createNewLog();
	}

	/**
	 * Creates a new log with a timestamp
	 */
	public void createNewLog() {
		logEmpty = true;

		// get time and date
		DateFormat fileDateFormat = new SimpleDateFormat("yyyy-MM-dd HH.mm.ss");
		DateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");

		Date date = new Date();

		String fileTimeDate = fileDateFormat.format(date); //2014/08/06 15:59:48
		String timeDate = dateFormat.format(date); //2014/08/06 15:59:48

		// create file if needed
		lastLog = new File(filePath + fileName + fileTimeDate + fileExtension);

		if(!lastLog.exists()) {
			try {
				lastLog.createNewFile();

				// write log header
				FileWriter fileWriter = new FileWriter(lastLog.getAbsoluteFile(), true);

				PrintWriter printWriter = new PrintWriter(fileWriter);

				printWriter.println("===================");
				printWriter.println(timeDate);
				printWriter.println("===================\n");

				printWriter.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}

	/**
	 * Appends a string to the log
	 * @param textToAppend
	 */
	public void appendToLog(String textToAppend) {
		if(lastLog != null) {
			logEmpty = false;
			
			// get current time
			DateFormat dateFormat = new SimpleDateFormat("HH:mm:ss");
			Date date = new Date();
			String time = dateFormat.format(date); //15:59:48

			// write timestamp and message to log
			try {
				FileWriter fileWriter = new FileWriter(lastLog.getAbsoluteFile(), true);
				PrintWriter printWriter = new PrintWriter(fileWriter);

				printWriter.println(time + " - " + textToAppend);

				printWriter.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}

	/**
	 * Closes the log and creates a new one depending on mode
	 * @param mode
	 */
	public void closeLog(boolean openNewLog) {
		if(isLogEmpty()) {
			deleteLog();
		} else {
			// append final comment
			String comment = JOptionPane.showInputDialog("Write a final log comment");

			// delete if comment == ""
			if(comment.equals("")) {
				deleteLog();
			} else {
				appendToLog(comment);
			}

			// create new log if openNewLog
			if(openNewLog) {
				createNewLog();
			}
		}
	}

	private void deleteLog() {
		if (lastLog != null) {
			lastLog.delete();
		}
	}
	
	public boolean isLogEmpty() {
		return logEmpty;
	}
}
