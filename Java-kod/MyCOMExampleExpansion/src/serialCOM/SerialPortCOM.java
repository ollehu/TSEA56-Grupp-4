package serialCOM;

import java.util.regex.Pattern;

import control.Handler;
import jssc.SerialPort;
import jssc.SerialPortEvent;
import jssc.SerialPortEventListener;
import jssc.SerialPortException;
import jssc.SerialPortList;

public class SerialPortCOM {

	private Handler handler;

	private static SerialPort serialPort;

	// standard port settings for firefly
	private int baudRate = 115200;
	private int numberOfDataBits = 8;
	private int numberOfStopBits = 1;
	private int numberOfParityBits = 0;

	public SerialPortCOM(Handler handler) throws UnknownOperatingSystemException, SerialPortException {
		this.handler = handler;

		String osName = System.getProperty("os.name");
		String[] portNames;

		// get port names
		if(osName.contains("Windows")){
			portNames = SerialPortList.getPortNames();
		} else if(osName.contains("Mac")) {
			portNames = SerialPortList.getPortNames("/dev/", Pattern.compile("tty."));
		} else {
			throw new UnknownOperatingSystemException();
		}

		// get user port choice
		int userPortChoiceIndex = handler.getUserPortChoice(portNames);
		serialPort = new SerialPort(portNames[userPortChoiceIndex]);

		// open port for communication
		serialPort.openPort();
		// baundRate, numberOfDataBits, numberOfStopBits, parity
		serialPort.setParams(baudRate, numberOfDataBits, numberOfStopBits, numberOfParityBits);
		// byte data transfer
		serialPort.setFlowControlMode(SerialPort.FLOWCONTROL_RTSCTS_IN | 
				SerialPort.FLOWCONTROL_RTSCTS_OUT);
		// add port listener
		serialPort.addEventListener(new PortReader(), SerialPort.MASK_RXCHAR);
	}

	public void sendControlCommand(int controlCommand) throws SerialPortException {
		// send control command (1 byte)
		sendData(convertIntToByte(controlCommand));
		
		// send speed (2 bytes)
		// TODO decide how to send 2 bytes
	}
	
	private byte convertIntToByte(int data) {
		// TODO test if this works properly
		return (byte) data;
	}
	
	private void sendData(byte data) throws SerialPortException {
		// TODO test if this works properly
		serialPort.writeByte(data);
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
			// TODO add event handling

			//			if(event.isRXCHAR() && event.getEventValue() > 0) {
			//				try {
			//					String receivedData = serialPort.readString(event.getEventValue());
			//					System.out.println("Received response: " + receivedData);
			//				}
			//				catch (SerialPortException ex) {
			//					System.out.println("Error in receiving string from COM-port: " + ex);
			//				}
			//			}
		}

	}
}
