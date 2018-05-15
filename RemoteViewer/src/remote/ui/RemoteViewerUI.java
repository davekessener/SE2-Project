package remote.ui;

import javafx.application.Platform;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.TextArea;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.Priority;
import javafx.scene.text.Font;
import javafx.stage.Modality;
import javafx.stage.Stage;

public class RemoteViewerUI
{
	private final Stage mStage;
	private final Scene mScene;
	private final TextArea mText;
	
	public RemoteViewerUI(Stage stage)
	{
		mStage = stage;
		mText = new TextArea();
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
	
	public void addLine(String s)
	{
		Platform.runLater(() -> mText.appendText(s + "\n"));
	}
	
	private Scene createScene()
	{
		GridPane pane = new GridPane();
		Button clear = new Button("Clear");
		
		pane.add(mText, 0, 0);
		pane.add(clear, 0, 1);

		GridPane.setHgrow(mText, Priority.ALWAYS);
		GridPane.setVgrow(mText, Priority.ALWAYS);
		
		mText.setEditable(false);
		mText.setFont(Font.font("Courier new", 12));
		
		clear.setOnAction(e -> mText.setText(""));
		
		return new Scene(pane, 800, 600);
	}
}
