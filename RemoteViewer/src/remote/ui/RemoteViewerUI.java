package remote.ui;

import javafx.scene.Scene;
import javafx.scene.control.Label;
import javafx.scene.layout.BorderPane;
import javafx.stage.Modality;
import javafx.stage.Stage;

public class RemoteViewerUI
{
	private final Stage mStage;
	private final Scene mScene;
	
	public RemoteViewerUI(Stage stage)
	{
		mStage = stage;
		mScene = createScene();
		
		mStage.setScene(mScene);
		mStage.setTitle("Remote Viewer");
		mStage.show();
	}
	
	public void showModal(Stage s)
	{
		s.initOwner(mStage);
		s.initModality(Modality.APPLICATION_MODAL);
		s.showAndWait();
	}
	
	public void close()
	{
		mStage.close();
	}
	
	private Scene createScene()
	{
		BorderPane pane = new BorderPane();
		
		Label l = new Label("Hier koennte Ihre werbung stehen!");
		
		pane.setCenter(l);
		
		return new Scene(pane, 800, 600);
	}
}
