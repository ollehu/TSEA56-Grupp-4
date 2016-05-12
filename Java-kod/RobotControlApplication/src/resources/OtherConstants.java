package resources;

/**
 * Constants used by several parts of the program
 * @author isak
 *
 */
public class OtherConstants {

	/**
	 * Robot status names
	 */
	public static final String[] ROBOT_STATUS_NAMES = {"Autonomous mode", 
														"Debug mode",
														"Claw",
														"Speed",
														"Last command",
														"Current heat"
	};
	
	public static final String[][] ROBOT_STATUS_STATES = {{"off", "on"},
															{"off", "on"},
															{"closed", "open"},
															null,
															{"none", "forward", "backward", "left", 
																"right", "forwards right", "forwards left", 
																"backwards right", "backwards left", 
																"none", "stop"},
															null
	};
	
	public static final boolean[] ROBOT_STATUS_VISIBILITY = {true,
															true,
															true,
															true,
															true,
															false
	};
	
	/**
	 * Table data 
	 */
	public static final  String[] SENSOR_COLUMNS = {"Sensor name", "Value", "Unit"};
	
	public static final  Object[][] SENSOR_DATA = {
			{"IR F/R", new Integer(-1), "mm"},
			{"IR F/L", new Integer(-1), "mm"}, 
			{"IR B/R", new Integer(-1), "mm"},
			{"IR B/L", new Integer(-1), "mm"},
			{"Lidar Lite", new Integer(-1), "cm"},
			{"Angular velocity", new Integer(-1), "deg/s"},
			{"Revolutions traveled", new Integer(-1), "1/16 revs"}
		};

	

	public static final  String[] CONTROL_COLUMNS = {"Constant", "Value"};
	
	public static final  Object[][] CONTROL_DATA = {
			{"P", new Double(0.45)},
			{"D", new Double(1.2)},
			{"K", new Double(0.7)},
			{"90", new Integer(720)},
			{"180", new Integer(1740)},
			{"Speed", new Integer(50)},
			{"Rotation speed", new Integer(70)}
		};
	
	/**
	 * Keybindings info text
	 */
	public static final String[] KEYBINDINGS_INFO_TEXT = 
			{"W - move robot forward",
			"A - rotate robot left",
			"S - move robot backward",
			"D - rotate robot right",
			"",
			"C - open/close claw",
			"",
			"X - increase speed",
			"Z - decrease speed"};
	
}

