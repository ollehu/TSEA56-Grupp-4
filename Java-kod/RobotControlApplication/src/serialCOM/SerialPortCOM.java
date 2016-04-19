package serialCOM;

import java.util.ArrayList;
import java.util.regex.Pattern;

import control.Handler;
import jssc.SerialPort;
import jssc.SerialPortEvent;
import jssc.SerialPortEventListener;
import jssc.SerialPortException;
import jssc.SerialPortList;

/**
 * Handles bluetooth communication
 * @author isak
 *
 */
public class SerialPortCOM {

	private Handler handler;

	private static SerialPort serialPort;

	// standard port settings for firefly
	private int baudRate = 115200;
	private int numberOfDataBits = 8;
	private int numberOfStopBits = 1;
	private int numberOfParityBits = 0;

	public SerialPortCOM(Handler handler) {
		this.handler = handler;
	}

	/**
	 * Connects to the selected port
	 * 
	 * @param portName port selected by user through GUI
	 * @throws SerialPortException
	 */
	public void connectToSerialPort(String portName) throws SerialPortException {
		serialPort = new SerialPort(portName);

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

	/**
	 * Closes port on program exit
	 */
	public void closeSerialPort() {
		if(serialPort != null) {
			try {
				serialPort.closePort();
				System.out.println("Closing port");
			} catch (SerialPortException e) {

			}
		}
	}

	/**
	 * Sends a command to the robot
	 * 
	 * @param dataID "Kommunikations-ID"
	 * @param commandID individual ID
	 * @param value value being sent (0-245)
	 * @throws SerialPortException
	 */
	public void sendToRobot(int dataID, int commandID, int value) throws SerialPortException {
		// send dataID byte
		sendData(convertIntToByte(dataID));

		// send command byte
		sendData(convertIntToByte(commandID));

		// send value byte
		sendData(convertIntToByte(value));
	}

	private byte convertIntToByte(int data) {
		byte binaryByte = (byte) (data & (0xFF));
		return binaryByte;
	}

	private void sendData(byte data) throws SerialPortException {
		if(serialPort != null) {
			serialPort.writeByte(data);
		}
		System.out.println("Data sent: " + (data & 0xFF));
	}

	/**
	 * Listens for port events
	 * 
	 * @author Isak
	 *
	 */
	private class PortReader implements SerialPortEventListener {

		@Override
		public void serialEvent(SerialPortEvent event) {
			if(event.isRXCHAR() && event.getEventValue() > 0) {
				try {
					byte[] receivedData = serialPort.readBytes();

					//String receivedDataString = serialPort.readString();

					if(Byte.toUnsignedInt(receivedData[0]) == DataID.CONTROL_SETTING) {
						//TODO handle switch from auto/man
						switchMode(receivedData);
					} else if(Byte.toUnsignedInt(receivedData[0]) == DataID.SENSOR_DATA) {
						//TODO handle sensor array
						updateSensorValues(receivedData);
					} else if(Byte.toUnsignedInt(receivedData[0]) == DataID.MAP_DATA) {
						//TODO handle map data
						updateMap(receivedData);
					}

					for(byte dataByte : receivedData) {
						System.out.println("Received response: " + Byte.toUnsignedInt(dataByte));
					}
				}
				catch (SerialPortException ex) {
					System.out.println("Error in receiving string from COM-port: " + ex);
				}
			}
		}

	}

	private void switchMode(byte[] receivedData) throws CommunicationFormatException{
		// throw error if wrong format
		if(receivedData.length != 3) {
			throw new CommunicationFormatException();
		}
		
		// activate or deactivate autonomous mode
		if(Byte.toUnsignedInt(receivedData[1]) == ControlSettingID.CONTROLLER) {
			if(Byte.toUnsignedInt(receivedData[3]) == 1) {
				// auto mode on
			} else if(Byte.toUnsignedInt(receivedData[3]) == 0) {
				// auto mode off
			} else {
				throw new CommunicationFormatException();
			}
		} else {
			throw new CommunicationFormatException();
		}
	}

	private void updateSensorValues(byte[] receivedData) {
		// throw error if wrong format
		if(receivedData.length != 15) {
			throw new CommunicationFormatException();
		}
		
		// convert byte to unsigned ints
		int[] sensorValues = new int[7];
		int j = 0;
		for(int i = 2; i < receivedData.length; i+=2) {
			sensorValues[j] =  Byte.toUnsignedInt(receivedData[i]);
			j++;
		}
		
		// update graph panel
		handler.getAnimator().getGraphPanel().updateSensorValues(sensorValues);
	}

	private void updateMap(byte[] receivedData) {
		// throw error if wrong format
		if(receivedData.length != 4) {
			throw new CommunicationFormatException();
		}
		
		// get x- and y-coordinate
		int xCoordinate = Byte.toUnsignedInt(receivedData[1]);
		int yCoordinate = Byte.toUnsignedInt(receivedData[2]);
	
		// update map
		handler.getAnimator().getMapPanel().getMap().updateMap(xCoordinate, yCoordinate, receivedData[3]);
		
	}

	public SerialPort getSerialPort() {
		return serialPort;
	}

}
