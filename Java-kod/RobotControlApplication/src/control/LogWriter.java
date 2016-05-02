package control;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;

import javax.swing.JOptionPane;

public class LogWriter {

	public static final int EXIT = 0;
	public static final int SAVE = 1;
	public static final int DEBUG = 2;

	private File lastLog;

	private String filePath = "logs/";
	private String fileName = "Log ";
	private String fileExtension = ".txt";

	private boolean isLogEmpty;

	public LogWriter() {
		createNewLog();
	}

	public void createNewLog() {
		isLogEmpty = true;

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

			System.out.println("Done");
		}

	}

	public void appendToLog(String textToAppend) {
		if(lastLog != null) {
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

	public void closeLog(int mode) {
		if(mode == DEBUG && isLogEmpty) {
			deleteLog();
		} else {
			// append final comment
			String comment = JOptionPane.showInputDialog("Write a log comment");

			// delete if comment == ""
			if(comment.equals("")) {
				deleteLog();
			} else {
				appendToLog(comment);
			}

			// create new log if save
			if(mode == SAVE) {
				createNewLog();
			}
		}
	}

	public void exit(String comment) {
		// delete if comment == ""
		if(comment.equals("")) {
			deleteLog();
		} else {
			appendToLog(comment);
		}
	}

	private void deleteLog() {
		if (lastLog != null) {
			lastLog.delete();
		}
	}
}
