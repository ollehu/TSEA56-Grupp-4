package serialCOM;

import java.util.ArrayList;
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

	public SerialPortCOM(Handler handler) {
		this.handler = handler;
	}
	
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
	
	public void closeSerialPort() {
		if(serialPort != null) {
			try {
				serialPort.closePort();
				System.out.println("Closing port");
			} catch (SerialPortException e) {

			}
		}
	}

	public void sendControlCommand(int controlCommand) throws SerialPortException {
		// send control byte
		sendData(convertIntToByte(0));
		
		// send control command (1 byte)
		sendData(convertIntToByte(controlCommand));
		
		// send speed (1 bytes)
		sendData(convertIntToByte(handler.getSpeed()));
	}
	
	public void setClawOpen(boolean state) throws SerialPortException {
		// send control byte
		sendData(convertIntToByte(0));
				
		// send control command
		sendData(convertIntToByte(ControlID.CLAW_SETTING));
		
		// send state (0 = closed, 1 = open)
		if(state) {
			sendData(convertIntToByte(1));
		} else {
			sendData(convertIntToByte(0));
		}
	}
	
	public void setControlOn(boolean state) throws SerialPortException {
		// send control byte
		sendData(convertIntToByte(0));
		
		// send control command
		//sendData(convertIntToByte(ControlID.CONTROL_SETTING));
		
		// send state (0 = off, 1 = on)
		if(state) {
			sendData(convertIntToByte(1));
		} else {
			sendData(convertIntToByte(0));
		}
	}
	
	private byte convertIntToByte(int data) {
		return (byte) data;
	}
	
	private void sendData(byte data) throws SerialPortException {
		serialPort.writeByte(data);
		System.out.println("Data sent: " + data);
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
//					byte[] receivedData = serialPort.readBytes();

					String receivedData = serialPort.readString();
					
					// TODO handle event depending on if it's sensor or map data
//					if(receivedData[0] == 0xFF) {
//						
//					} else if(receivedData[0] == 0xFE) {
//						
//					}
					
					System.out.println("Received response: " + receivedData);
				}
				catch (SerialPortException ex) {
					System.out.println("Error in receiving string from COM-port: " + ex);
				}
			}
		}

	}

	public SerialPort getSerialPort() {
		return serialPort;
	}
	
}
