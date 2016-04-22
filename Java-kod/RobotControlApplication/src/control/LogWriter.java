package control;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;

public class LogWriter {

	private File lastLog;
	
	private String filePath = "logs/";
	private String fileName = "Log ";
	private String fileExtension = ".txt";
	
	public LogWriter() {
		createNewLog();
	}
	
	public void createNewLog() {
		// get time and date
		DateFormat fileDateFormat = new SimpleDateFormat("yyyy-MM-dd HH.mm.ss");
		DateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");

		Date date = new Date();
		
		String fileTimeDate = fileDateFormat.format(date); //2014/08/06 15:59:48
		String timeDate = dateFormat.format(date); //2014/08/06 15:59:48

		
		lastLog = new File(filePath + fileName + fileTimeDate + fileExtension);
		
		// create file if needed
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
	
	public void closeLog(String comment, boolean createNewFile) {
		appendToLog(comment);
		if(createNewFile) {
			createNewLog();
		}
	}
	
	public void deleteLog() {
		if (lastLog != null) {
			lastLog.delete();
		}
	}
}
