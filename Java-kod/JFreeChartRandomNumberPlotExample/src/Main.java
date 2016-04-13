import javax.swing.Timer;

import control.Handler;

/**
 * Adds a random number (+-10 compared to last value) to a plot.
 * If the plot has more than 10 numbers the oldest value is removed.
 * Requires the JFreeChart library
 * 
 * @author Isak
 *
 */
public class Main {

	public static void main(String[] args) {
		Handler handler = new Handler();
		
		handler.showFrame();
	}
}
