package remote;

import javafx.stage.Stage;
import remote.ui.PortSelectUI;
import remote.ui.RemoteViewerUI;

public class RemoteViewer
{
	private final RemoteViewerUI mUI;
	
	public RemoteViewer(Stage primary)
	{
		mUI = new RemoteViewerUI(primary);
		
		mUI.showModal(new PortSelectUI(port -> listenOn(port), () -> mUI.close()));
	}
	
	boolean listenOn(int port)
	{
		return false;
	}
}
