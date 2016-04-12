import java.awt.event.KeyEvent;
import java.util.ArrayList;

import javax.swing.JFrame;
import javax.swing.JTextArea;

public class Animator {

	private JFrame frame;
	
	private JTextArea jTextArea;
	
	public Animator() {
		frame = new JFrame("Keys pressed demo");
		
		jTextArea = new JTextArea(5,20);
		jTextArea.addKeyListener(new MyKeyListener(this));
		jTextArea.setEditable(false);
		frame.add(jTextArea);
	}
	
	public void showFrame() {
		frame.pack();
		frame.setVisible(true);
	}
	
	public void print(boolean[] keysCurrentlyPressed) {
		jTextArea.setText("Keys currently pressed:\n");
		boolean noKeys = true;
		
		int index = 0;
		for(boolean key : keysCurrentlyPressed) {
			if(key) {
				jTextArea.append(index + " ");
				noKeys = false;
			}
			index++;
		}
		
		if(noKeys) {
			jTextArea.setText("No keys pressed");
		}
	}
}
