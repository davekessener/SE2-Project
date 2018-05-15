package remote;

import java.io.IOException;

import javafx.stage.Stage;
import remote.io.ConnectionManager;
import remote.ui.PortSelectUI;
import remote.ui.RemoteViewerUI;

public class RemoteViewer
{
	private final RemoteViewerUI mUI;
	private ConnectionManager mManager;
	
	public RemoteViewer(Stage primary)
	{
		mUI = new RemoteViewerUI(primary);
		
		mUI.showModal(new PortSelectUI(port -> listenOn(port), () -> mUI.close()));
		
		primary.setOnCloseRequest(e -> {
			if(mManager != null)
			{
				mManager.stop();
			}
		});
	}
	
	private boolean listenOn(int port)
	{
		try
		{
			mManager = new ConnectionManager(port, s -> print(s));
			
			return true;
		}
		catch (IOException e)
		{
			return false;
		}
	}
	
	private void print(String s)
	{
		System.out.println("Read line: " + s);
		mUI.addLine(s);
	}
}
