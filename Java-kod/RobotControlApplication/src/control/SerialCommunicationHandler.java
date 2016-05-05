package control;

import java.awt.Robot;

import javax.swing.JOptionPane;

import jssc.*;
import model.*;
import resources.*;

public class SerialCommunicationHandler {

	private Log log;
	
	private RobotData robotData;
	
	private SensorData sensorData;
	
	private static SerialPort serialPort;

	// standard port settings for firefly
	private int baudRate = 115200;
	private int numberOfDataBits = 8;
	private int numberOfStopBits = 1;
	private int numberOfParityBits = 0;

	public SerialCommunicationHandler(Log log, RobotData robotData, SensorData sensorData)  {
		this.log = log;
		this.robotData = robotData;
		this.sensorData = sensorData;
	}

	/**
	 * Connects to the selected port
	 * 
	 * @param portName port selected by user through GUI
	 * @throws SerialPortException
	 */
	public void connectToSerialPort(String portName) {
		serialPort = new SerialPort(portName);

		try {
			// open port for communication
			serialPort.openPort();
			// baundRate, numberOfDataBits, numberOfStopBits, parity
			serialPort.setParams(baudRate, numberOfDataBits, numberOfStopBits, numberOfParityBits);
			// byte data transfer
			serialPort.setFlowControlMode(SerialPort.FLOWCONTROL_RTSCTS_IN | 
					SerialPort.FLOWCONTROL_RTSCTS_OUT);
			// add port listener
			serialPort.addEventListener(new PortReader(), SerialPort.MASK_RXCHAR);
			// ping robot
			sendToRobot(CommunicationID.CONTROL_SETTING, ControlSettingID.PING, 1);
		} catch (SerialPortException e) {
			e.printStackTrace();
		}
		
	}

	/**
	 * Closes port. Called on program exit
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
	public void sendToRobot(int dataID, int commandID, int value) {
		try {
			// send dataID byte
			sendData(convertIntToByte(dataID));
			
			// send command byte
			sendData(convertIntToByte(commandID));

			// send value byte
			sendData(convertIntToByte(value));
		} catch (SerialPortException e) {
			e.printStackTrace();
		}
	}

	//================================================================================
    // Accessors
    //================================================================================
	public SerialPort getSerialPort() {
		return serialPort;
	}
	
	//================================================================================
    // Internal methods
    //================================================================================
	private byte convertIntToByte(int data) {
		byte binaryByte = (byte) (data & (0xFF));
		return binaryByte;
	}

	/**
	 * Sends a single byte of data
	 * @param data
	 * @throws SerialPortException
	 */
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
					byte[] communicationsIDByte = serialPort.readBytes(1);
					int communicationsID = Byte.toUnsignedInt(communicationsIDByte[0]);
					
					// get byte string length
					int byteStringLength = 100;
					
					if(communicationsID == CommunicationID.CONTROL_SETTING) {
						byteStringLength = 2;
					} else if(communicationsID == CommunicationID.SENSOR_DATA) {
						byteStringLength = 14;
					} else if(communicationsID == CommunicationID.MAP_DATA) {
						byteStringLength = 3;
					} else if(communicationsID == CommunicationID.CONTROL_DATA) {
						byteStringLength = 2;
					} else {
						serialPort.purgePort(SerialPort.PURGE_RXCLEAR);
					}
					
					// read data from port
					byte[] receivedData = new byte[byteStringLength];
					
					try {
						receivedData = serialPort.readBytes(byteStringLength, 70);
					} catch (SerialPortTimeoutException e) {
						return;
					}
					
					// respond to received data
					if(communicationsID == CommunicationID.CONTROL_SETTING) {
						updateControlSettings(receivedData);
					} else if(communicationsID == CommunicationID.SENSOR_DATA) {
						updateSensorValues(receivedData);
					} else if(communicationsID == CommunicationID.MAP_DATA) {
						updateMap(receivedData);
					} else if(communicationsID == CommunicationID.CONTROL_DATA) {
						updateControlData(receivedData);
					}
					
				}
				catch (SerialPortException ex) {
					System.out.println("Error in receiving string from COM-port: " + ex);
				}
			}
		}

	}

	private void updateControlSettings(byte[] receivedData) {
		int identifier = Byte.toUnsignedInt(receivedData[0]);
		int value = Byte.toUnsignedInt(receivedData[1]);
		
		// update robot data
		robotData.update(identifier, value);
	}

	private void updateSensorValues(byte[] receivedData) {
		
		// convert byte to unsigned ints
		int[] sensorValues = new int[6];
		int j = 0;
		
		for(int i = 1; i < receivedData.length; i+=2) {
			if(i == 9) {
				sensorValues[j] = Byte.toUnsignedInt(receivedData[i]) * 128;
			} else if(i == 11) {
				sensorValues[j] += Byte.toUnsignedInt(receivedData[i]);
				j++;
			} else if(i == 13){
				sensorValues[j] = Byte.toUnsignedInt(receivedData[i]) - 124;
				j++;
			} else {
				sensorValues[j] =  Byte.toUnsignedInt(receivedData[i]);
				j++;
			}
			
		}
		
		sensorData.update(sensorValues);
	}

	private void updateMap(byte[] receivedData) {
//		// get x- and y-coordinate
//		int xCoordinate = Byte.toUnsignedInt(receivedData[0]);
//		int yCoordinate = Byte.toUnsignedInt(receivedData[1]);
//		
//		int value = Byte.toUnsignedInt(receivedData[2]);
//	
//		// update map
//		handler.getAnimator().getMapPanel().getMap().update(xCoordinate, yCoordinate, value);
//		System.out.println("Uppdaterat kartan! X: " + xCoordinate + ", Y: " + yCoordinate + ", value: " + value);
	}

	private void updateControlData(byte[] receivedData) {
		int identifier = Byte.toUnsignedInt(receivedData[0]);
		int value = Byte.toUnsignedInt(receivedData[1]);
		
		// update robot data
		robotData.update(identifier, value);
	}
	
	//================================================================================
    // Testing
    //================================================================================
	public void simulateReceivedData() {
		byte[] simulatedData = new byte[14];
		
		for(int index = 1; index < 14; index += 2) {
			simulatedData[index] = (byte) (index * 2);
		}
		
		updateSensorValues(simulatedData);
		
//		byte[] simulatedData = new byte[2];
//		
//		simulatedData[0] = (byte) ControlSettingID.DEBUG_MODE;
//		simulatedData[1] = (byte) 1;
//		
//		updateControlSettings(simulatedData);
	}
}
