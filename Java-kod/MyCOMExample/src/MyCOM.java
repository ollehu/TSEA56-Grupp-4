import java.util.Scanner;
import java.util.regex.Pattern;

import jssc.SerialPort;
import jssc.SerialPortEvent;
import jssc.SerialPortEventListener;
import jssc.SerialPortException;
import jssc.SerialPortList;

/**
 * Exempelkod som låter användaren välja en serieport 
 * och sedan skickar strängen "Test" till porten.
 * Baserad på lånad kod
 * 
 * OBS! Ej testkörd. Anpassad för OSX. Kräver biblioteket jSSC.
 * 
 * @author Isak
 *
 */
public class MyCOM {

	private static SerialPort serialPort;

	public static void main(String[] args) {
		// get computer serial ports names
		// arguments "/dev/" and Patern... required for OSX
		String[] portNames = SerialPortList.getPortNames("/dev/", Pattern.compile("tty."));
		System.out.println("Available ports:");

		// display available ports
		int index = 0;
		for (String port : portNames) {
			System.out.println(index  + ". " + port);
			index++;
		}

		// get user input
		Scanner reader = new Scanner(System.in);
		System.out.print("\nSelect port: ");
		index = reader.nextInt();
		reader.close();

		// inicialization with selecting port for communication
		serialPort = new SerialPort(portNames[index]);

		try {
			// open port for communication
			serialPort.openPort();
			// baundRate, numberOfDataBits, numberOfStopBits, parity
			serialPort.setParams(9600, 8, 1, 0);
			// byte data transfer
			serialPort.setFlowControlMode(SerialPort.FLOWCONTROL_RTSCTS_IN | 
					SerialPort.FLOWCONTROL_RTSCTS_OUT);

			serialPort.addEventListener(new PortReader(), SerialPort.MASK_RXCHAR);

			serialPort.writeString("Test");
		}
		catch (SerialPortException ex) {
			System.out.println("There are an error on writing string to port т: " + ex);
		}
	}

	/**
	 * Lyssnar efter event på serieporten och skriver, vid event, ut det mottagna data.
	 * 
	 * @author Isak
	 *
	 */
	private static class PortReader implements SerialPortEventListener {

		@Override
		public void serialEvent(SerialPortEvent event) {
			if(event.isRXCHAR() && event.getEventValue() > 0) {
				try {
					String receivedData = serialPort.readString(event.getEventValue());
					System.out.println("Received response: " + receivedData);
				}
				catch (SerialPortException ex) {
					System.out.println("Error in receiving string from COM-port: " + ex);
				}
			}
		}

	}
}